# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

Each lab is a single self-contained `main.cpp`. The VSCode default build task compiles the **active file** with MinGW GCC:

```
C:\MinGW\bin\g++.exe -fdiagnostics-color=always -g <file> -o <dir>\<name>.exe
```

To build and run a specific lab from the terminal:

```powershell
g++ -fdiagnostics-color=always -g Section03_Cpp_Allocation_Routines\08_auto_ptr\main.cpp -o Section03_Cpp_Allocation_Routines\08_auto_ptr\main.exe
.\Section03_Cpp_Allocation_Routines\08_auto_ptr\main.exe
```

No standard version flag is set by default. Add `-std=c++17` explicitly when the lab requires it. There are no tests, no build system (CMake/Make), and no lint step.

## Repository structure

This is a course repository progressing from raw C/C++ memory management toward modern smart pointers:

| Section | Topic |
|---|---|
| `Section02_C_Allocation_Routines/` | `malloc`, `calloc`, `realloc` |
| `Section03_Cpp_Allocation_Routines/` | `new`/`delete`, placement new, `operator new`, `auto_ptr` |
| `Section04_SmartPointers_Origin/` | `auto_ptr` as the historical first smart pointer |

Each lab is numbered `NN_description/main.cpp`. Executables (`.exe`) are git-ignored.

## File conventions

Every `main.cpp` follows this exact structure — match it when adding labs:

1. **File-header Doxygen block** — `@file`, `@brief`, `@author`, `@date`
2. **Section banners** — spaced capitals, `/*===...===*/` style (77 chars wide):
   - `F I L E  I N C L U S I O N S`
   - `M A C R O S  &  F U N C T I O N - L I K E  M A C R O S`
   - `U S E R  T Y P E S`
   - `S T A T I C  F U N C T I O N S`
   - `G L O B A L  F U N C T I O N S`
3. `using namespace std;` after includes
4. Empty sections get a `/* None */` placeholder — never omit the banner
5. Member variables use `m_` prefix (`m_id`, `m_name`, `m_salary`)
6. Demo functions are camelCase (`demo_basic_ownership`, `efficientReallocation`)
7. `main()` only calls the demo functions — no logic in `main()`

## Deprecated / removed standard library features

When a lab covers a feature deprecated in C++11 and removed in C++17 (e.g., `auto_ptr`), place the compatibility block **before** any `#include` that pulls in the removed type:

```cpp
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4996)        // MSVC deprecation warning
#elif defined(__GNUC__)
    #define _GLIBCXX_USE_DEPRECATED 1     // GCC/libstdc++
#elif defined(__clang__)
    #define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR  // Clang/libc++
#endif

#include <memory>

// ... all code that uses the deprecated type ...

#ifdef _MSC_VER
    #pragma warning(pop)
#endif
```
