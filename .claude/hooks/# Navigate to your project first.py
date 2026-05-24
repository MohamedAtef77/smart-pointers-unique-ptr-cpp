#!/usr/bin/env python3
"""
Claude Code Hook: C++ Documentation Consistency Checker
Fires on PostToolUse for Write/Edit events targeting .cpp files.
Samples reference .cpp files from the repo, extracts their documentation
patterns, then evaluates the written file and feeds findings back to Claude.
"""

import json
import os
import sys
import glob
import random

# ── tunables ────────────────────────────────────────────────────────────────
MAX_REF_FILES      = 4      # how many reference .cpp files to sample
MAX_REF_LINES      = 80     # lines to read from each reference file
MAX_TARGET_LINES   = 300    # lines to read from the file just written
MAX_OUTPUT_CHARS   = 6000   # hard cap on additionalContext size
# ────────────────────────────────────────────────────────────────────────────


def read_head(path: str, max_lines: int) -> str:
    """Read up to max_lines from a file, return as a single string."""
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            lines = []
            for i, line in enumerate(f):
                if i >= max_lines:
                    lines.append(f"... (truncated at {max_lines} lines)\n")
                    break
                lines.append(line)
        return "".join(lines)
    except OSError:
        return ""


def find_reference_files(project_dir: str, exclude: str) -> list[str]:
    """
    Find .cpp files in the project excluding the file just written.
    Returns a random sample of up to MAX_REF_FILES paths.
    """
    pattern = os.path.join(project_dir, "**", "*.cpp")
    all_cpp = [
        p for p in glob.glob(pattern, recursive=True)
        if os.path.abspath(p) != os.path.abspath(exclude)
        and os.path.getsize(p) > 200          # skip near-empty stubs
    ]
    if not all_cpp:
        return []
    # Prefer files with doxygen-style comments to bias the sample
    scored = []
    for p in all_cpp:
        head = read_head(p, 60)
        score = head.count("/**") + head.count("///") + head.count("@brief")
        scored.append((score, p))
    scored.sort(key=lambda x: x[0], reverse=True)
    # Take top half (well-documented ones) then random-sample from those
    top = scored[: max(len(scored) // 2, MAX_REF_FILES)]
    sample = random.sample(top, min(MAX_REF_FILES, len(top)))
    return [p for _, p in sample]


def build_prompt(target_path: str, target_content: str, refs: dict[str, str]) -> str:
    """Build the evaluation prompt sent to Claude as additionalContext."""

    ref_block = ""
    for path, content in refs.items():
        rel = os.path.relpath(path)
        ref_block += f"\n### Reference: {rel}\n```cpp\n{content}\n```\n"

    if not ref_block:
        ref_block = "(No reference files found — apply general Doxygen best practices.)\n"

    rel_target = os.path.relpath(target_path)

    prompt = f"""
## Documentation Consistency Report for `{rel_target}`

You just wrote or edited the file above. A documentation consistency check has been triggered.

### Reference files from this repo (excerpt):
{ref_block}

### File you just wrote (`{rel_target}`, excerpt):
```cpp
{target_content}
```

### Your task — Documentation Audit:
Compare `{rel_target}` against the reference files and identify **every** documentation gap:

1. **File-level header** — Does the file have a file docblock (`@file`, `@brief`, `@author`, `@date`, copyright notice)?  
   Report: present / missing / incomplete.

2. **Class / struct / enum docs** — Every class/struct/enum must have a `///` or `/** */` block with at least `@brief`.  
   List any that are missing or bare.

3. **Function / method docs** — Every non-trivial function must document:
   - `@brief` (one-line purpose)
   - `@param` for each parameter
   - `@return` / `@retval` if non-void
   - `@note` / `@warning` where relevant  
   List all functions missing any of these.

4. **Inline comments** — Complex logic, magic numbers, and non-obvious branches should have inline comments.  
   Flag sections that lack explanation.

5. **Style consistency** — Does the style (Doxygen tags, comment delimiters `/** */` vs `///`, indentation) match the reference files?  
   List any deviations.

### Output format:
Respond ONLY with a JSON object:
```json
{{
  "has_issues": true,
  "summary": "One sentence overall assessment.",
  "issues": [
    {{
      "severity": "error|warning|info",
      "location": "ClassName::methodName or line ~N",
      "issue": "What is missing or wrong",
      "fix": "Concrete fix — show the exact docblock to add"
    }}
  ],
  "compliant": false
}}
```
If everything looks good, set `has_issues` to false, `issues` to [], and `compliant` to true.
Do NOT include markdown fences or any text outside the JSON object.
""".strip()

    return prompt


def call_claude_api(prompt: str) -> dict:
    """Call the Anthropic API and parse the JSON response."""
    import urllib.request

    payload = {
        "model": "claude-sonnet-4-20250514",
        "max_tokens": 1000,
        "messages": [{"role": "user", "content": prompt}],
        "system": (
            "You are a senior C++ code reviewer specialising in documentation quality. "
            "You respond ONLY with a valid JSON object — no markdown, no extra text."
        ),
    }

    req = urllib.request.Request(
        "https://api.anthropic.com/v1/messages",
        data=json.dumps(payload).encode(),
        headers={
            "Content-Type": "application/json",
            "anthropic-version": "2023-06-01",
        },
        method="POST",
    )

    with urllib.request.urlopen(req, timeout=30) as resp:
        data = json.loads(resp.read())

    raw = ""
    for block in data.get("content", []):
        if block.get("type") == "text":
            raw += block["text"]

    # Strip accidental markdown fences
    raw = raw.strip()
    if raw.startswith("```"):
        raw = raw.split("\n", 1)[-1]
    if raw.endswith("```"):
        raw = raw.rsplit("```", 1)[0]

    return json.loads(raw.strip())


def format_context(report: dict, target_path: str) -> str:
    """Format the API response into a clear additionalContext string."""
    rel = os.path.relpath(target_path)
    lines = [f"## 📋 Documentation Audit: `{rel}`"]

    if report.get("compliant"):
        lines.append("✅ Documentation is consistent and complete. No action needed.")
        return "\n".join(lines)

    lines.append(f"**Summary:** {report.get('summary', '')}")
    lines.append("")

    issues = report.get("issues", [])
    if not issues:
        lines.append("No specific issues identified.")
        return "\n".join(lines)

    errors   = [i for i in issues if i.get("severity") == "error"]
    warnings = [i for i in issues if i.get("severity") == "warning"]
    infos    = [i for i in issues if i.get("severity") == "info"]

    def render_group(label: str, icon: str, group: list) -> None:
        if not group:
            return
        lines.append(f"### {icon} {label} ({len(group)})")
        for item in group:
            lines.append(f"- **{item.get('location', '?')}**: {item.get('issue', '')}")
            fix = item.get("fix", "").strip()
            if fix:
                lines.append(f"  ```cpp\n  {fix}\n  ```")

    render_group("Errors (must fix)", "🔴", errors)
    render_group("Warnings", "🟡", warnings)
    render_group("Suggestions", "🔵", infos)

    lines.append("")
    lines.append(
        "**Action required:** Please apply the fixes above to bring this file's "
        "documentation in line with the rest of the repo before finalising."
    )

    result = "\n".join(lines)
    if len(result) > MAX_OUTPUT_CHARS:
        result = result[:MAX_OUTPUT_CHARS] + "\n... (truncated)"
    return result


def main() -> None:
    # ── Read hook event from stdin ──────────────────────────────────────────
    try:
        event = json.loads(sys.stdin.read())
    except json.JSONDecodeError:
        sys.exit(0)  # Non-blocking: bad input, let Claude continue

    tool_name  = event.get("tool_name", "")
    tool_input = event.get("tool_input", {})
    file_path  = tool_input.get("file_path", "")

    # ── Only care about .cpp files ──────────────────────────────────────────
    if not file_path.endswith(".cpp"):
        sys.exit(0)

    # ── Determine project root ──────────────────────────────────────────────
    project_dir = event.get("cwd", os.path.dirname(file_path))

    # ── Read the target file ────────────────────────────────────────────────
    target_content = read_head(file_path, MAX_TARGET_LINES)
    if not target_content.strip():
        sys.exit(0)  # Empty file, nothing to check

    # ── Find reference files ────────────────────────────────────────────────
    ref_paths = find_reference_files(project_dir, file_path)
    refs = {p: read_head(p, MAX_REF_LINES) for p in ref_paths}

    # ── Build & send prompt ─────────────────────────────────────────────────
    prompt = build_prompt(file_path, target_content, refs)

    try:
        report = call_claude_api(prompt)
    except Exception as e:
        # Non-blocking: API failure should never break Claude Code
        sys.stderr.write(f"[check_cpp_docs] API call failed: {e}\n")
        sys.exit(0)

    # ── Format and return additionalContext ─────────────────────────────────
    context_text = format_context(report, file_path)

    output = {
        "hookSpecificOutput": {
            "hookEventName": "PostToolUse",
            "additionalContext": context_text,
        }
    }

    # If there are errors, also set decision=block so Claude stops and fixes
    issues = report.get("issues", [])
    has_errors = any(i.get("severity") == "error" for i in issues)
    if has_errors:
        output["decision"] = "block"
        output["reason"] = (
            f"Documentation errors found in `{os.path.relpath(file_path)}`. "
            "See the audit report in additionalContext and fix all 🔴 errors before continuing."
        )

    print(json.dumps(output))
    sys.exit(0)


if __name__ == "__main__":
    main()
