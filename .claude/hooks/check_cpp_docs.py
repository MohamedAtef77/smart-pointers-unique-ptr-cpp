"""
PostToolUse hook — validates documentation conventions in edited .cpp files.

Checks (per CLAUDE.md):
  1. File-header Doxygen block has @file, @brief, @author, @date.
  2. All five required section banners are present.
  3. Empty sections carry a /* None */ placeholder.
  4. Every top-level class/struct, free function, typedef, using alias, and
     user-defined macro (ALL_CAPS name with a value) has a Doxygen
     /** @brief */ block immediately above it.

Receives the tool call as JSON on stdin; exits non-zero on violations.
"""

import json
import re
import sys
from typing import Optional

# ── Fixed requirements ────────────────────────────────────────────────────────

REQUIRED_DOXYGEN_TAGS = ["@file", "@brief", "@author", "@date"]

REQUIRED_BANNERS = [
    "F I L E  I N C L U S I O N S",
    "M A C R O S",
    "U S E R  T Y P E S",
    "S T A T I C  F U N C T I O N S",
    "G L O B A L  F U N C T I O N S",
]

# ── Patterns ──────────────────────────────────────────────────────────────────

# class/struct definition — not a forward declaration (would end with ;)
_CLASS_RE       = re.compile(r'^(class|struct)\s+\w+(?:\s*:\s*[\w\s,:<>&*]+)?\s*$')
_CLASS_BRACE_RE = re.compile(r'^(class|struct)\s+\w+.*\{')

# User-defined macros: ALL_CAPS name (no leading _) with a non-empty value
_MACRO_RE = re.compile(r'^#define\s+([A-Z][A-Z0-9_]*)\s+\S')

_TYPEDEF_RE     = re.compile(r'^typedef\s+')
_USING_ALIAS_RE = re.compile(r'^using\s+\w+\s*=')

# Keywords that can precede '(' but are not function definitions
_CONTROL_KW = frozenset({
    'if', 'else', 'for', 'while', 'switch', 'do', 'return',
    'case', 'catch', 'throw', 'delete', 'new', 'sizeof',
    'decltype', 'static_assert', 'assert',
})


# ── Helpers ───────────────────────────────────────────────────────────────────

def _is_func_def(line: str) -> bool:
    """Heuristic: does this stripped line start a function definition?"""
    s = line.strip()
    if '(' not in s:
        return False
    if s.startswith('#') or s.startswith('//') or s.startswith('*'):
        return False
    # Declarations/calls end with ;
    if s.rstrip().endswith(';'):
        return False
    # Control-flow keyword immediately before the first '('
    before_paren = s.split('(')[0].strip()
    last_token = before_paren.split()[-1] if before_paren.split() else ''
    if last_token in _CONTROL_KW:
        return False
    # Must look like:  <type> <name>(   or   ~<name>(   (constructor/destructor)
    if re.search(r'\w+\s+\w+\s*\(', s) or re.search(r'^~?\w+\s*\(', s):
        return True
    return False


def _has_doc_block_before(lines: list[str], idx: int) -> bool:
    """Return True when a Doxygen /** … */ block ends immediately before line idx."""
    i = idx - 1
    while i >= 0 and not lines[i].strip():
        i -= 1
    if i < 0:
        return False

    top = lines[i].strip()

    # Single-line /** … */ or ///
    if (top.startswith('/**') and top.endswith('*/')) or top.startswith('///'):
        return True

    # Closing */ of a multi-line block — walk back to find the opening /**
    if top == '*/':
        j = i - 1
        while j >= 0:
            jl = lines[j].strip()
            if jl.startswith('/**') or jl.startswith('/*!'):
                return True
            if jl.startswith('*') or not jl:
                j -= 1
                continue
            break   # hit non-comment content — not a Doxygen block
    return False


# ── Individual checks ─────────────────────────────────────────────────────────

def _check_header_tags(text: str) -> list[str]:
    return [f"Missing Doxygen tag: {t}" for t in REQUIRED_DOXYGEN_TAGS if t not in text]


def _check_banners(text: str) -> list[str]:
    return [f"Missing section banner: {b}" for b in REQUIRED_BANNERS if b not in text]


def _check_empty_sections(text: str) -> list[str]:
    """Every section delimited by /*===…===*/ banners must have content or /* None */."""
    errors: list[str] = []
    positions = [m.start() for m in re.finditer(r'/\*=+\*', text)]

    for i, pos in enumerate(positions):
        end = positions[i + 1] if i + 1 < len(positions) else len(text)
        section_lines = text[pos:end].splitlines()

        # Extract banner label from the middle line of the banner block
        label = section_lines[1].strip().strip('* ') if len(section_lines) > 1 else ''

        # Keep only lines that are actual content (not banner scaffold, not blank)
        content = [
            l for l in section_lines
            if l.strip()
            and not l.strip().startswith('*')   # banner body / closing */
            and not l.strip().startswith('/*=')  # banner opener /*===…
        ]
        if not content:
            errors.append(
                f"Section '{label[:60]}' has no content and no '/* None */' placeholder"
            )
    return errors


def _check_doc_consistency(lines: list[str]) -> list[str]:
    """Every top-level definition must be preceded by a Doxygen /** @brief */ block."""
    errors: list[str] = []
    brace_depth = 0
    in_block_comment = False

    for i, raw in enumerate(lines):
        s = raw.strip()

        # ── Block-comment state machine ───────────────────────────────────────
        if in_block_comment:
            if '*/' in s:
                in_block_comment = False
            continue

        if '/*' in s:
            open_pos = s.index('/*')
            if '*/' not in s[open_pos:]:
                # Multi-line block comment opens here — skip this line
                in_block_comment = True
                continue
            # Single-line /* … */ — fall through and inspect the rest of the line

        if s.startswith('//'):
            continue

        # ── Brace depth ───────────────────────────────────────────────────────
        depth_before = brace_depth
        brace_depth += s.count('{') - s.count('}')

        # ── Definition detection (global scope only) ──────────────────────────
        if depth_before != 0:
            continue

        label: Optional[str] = None

        if _CLASS_RE.match(s) or _CLASS_BRACE_RE.match(s):
            label = f"class/struct '{s[:70]}'"
        elif _TYPEDEF_RE.match(s):
            label = f"typedef '{s[:70]}'"
        elif _USING_ALIAS_RE.match(s):
            label = f"using alias '{s[:70]}'"
        elif _MACRO_RE.match(s):
            m = _MACRO_RE.match(s)
            label = f"macro '{m.group(1)}'"  # type: ignore[union-attr]
        elif _is_func_def(s):
            label = f"function '{s[:70]}'"

        if label and not _has_doc_block_before(lines, i):
            errors.append(f"Line {i + 1}: {label} has no Doxygen /** @brief */ block")

    return errors


# ── Entry point ───────────────────────────────────────────────────────────────

def check_file(path: str) -> list[str]:
    if not path.endswith('.cpp'):
        return []
    try:
        text = open(path, encoding='utf-8').read()
    except OSError as exc:
        return [f'Could not read file: {exc}']

    lines = text.splitlines()
    return (
        _check_header_tags(text)
        + _check_banners(text)
        + _check_empty_sections(text)
        + _check_doc_consistency(lines)
    )


def main() -> None:
    try:
        payload = json.load(sys.stdin)
    except json.JSONDecodeError:
        sys.exit(0)

    file_path: str = payload.get('tool_input', {}).get('file_path', '')
    if not file_path:
        sys.exit(0)

    errors = check_file(file_path)
    if errors:
        print(f'check_cpp_docs: {file_path}', file=sys.stderr)
        for e in errors:
            print(f'  WARN  {e}', file=sys.stderr)
        sys.exit(2)


if __name__ == '__main__':
    main()
