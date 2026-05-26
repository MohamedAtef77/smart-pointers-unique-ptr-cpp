<div align="center">

# 🧠 Smart Pointers & `unique_ptr` in Modern C++

**A hands-on lab series journeying from raw memory to zero-overhead ownership**

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Compiler](https://img.shields.io/badge/Compiler-MinGW%20GCC-F16522?style=for-the-badge&logo=gnu&logoColor=white)
![License](https://img.shields.io/badge/License-Educational-green?style=for-the-badge)

---

> *"With great power comes great responsibility — unless you use smart pointers."*

</div>

---

## 📖 Overview

This repository is a **progressive lab series** that teaches C++ memory management from the ground up. Starting with the raw, error-prone world of C-style allocation and climbing all the way to modern RAII-based smart pointers, each section builds on the last to give you a complete mental model of *how* and *why* C++ memory safety works.

```
  C malloc/free  ──►  C++ new/delete  ──►  auto_ptr (legacy)  ──►  unique_ptr  ──►  Deleters & make_unique
      §02               §03                    §03 / §04               §05             §07 / §08
```

---

## 🗂️ Course Structure

| # | Section | Core Concepts | Status |
|---|---------|---------------|--------|
| 02 | [C Allocation Routines](#-section-02--c-allocation-routines) | `malloc`, `calloc`, `realloc`, `free` | ✅ |
| 03 | [C++ Allocation Routines](#-section-03--c-allocation-routines) | `new` / `delete`, placement new, `operator new`, `auto_ptr` | ✅ |
| 04 | [Smart Pointers Origin](#-section-04--smart-pointers-origin) | `auto_ptr` — the first smart pointer (historical) | ✅ |
| 05 | [Unique Pointers](#-section-05--unique-pointers) | `std::unique_ptr`, ownership semantics, move semantics | ✅ |
| 07 | [Deleters](#-section-07--deleters) | Custom deleters, function pointers, lambdas | ✅ |
| 08 | [Make Unique](#-section-08--make_unique) | `std::make_unique`, exception safety, best practices | ✅ |

---

## 🔬 Section 02 — C Allocation Routines

> The bedrock. Raw heap access with no safety net.

C memory management gives you full control — and full responsibility. One forgotten `free()` is a memory leak; one double-`free()` is undefined behaviour.

```c
// malloc — allocate uninitialised memory
int* arr = (int*) malloc(5 * sizeof(int));

// calloc — allocate zero-initialised memory
int* arr = (int*) calloc(5, sizeof(int));

// realloc — resize an existing allocation
arr = (int*) realloc(arr, 10 * sizeof(int));

// free — always pair with every allocation
free(arr);
arr = NULL; // best practice: null-out after free
```

**Key pitfalls explored:**
- Memory leaks (missing `free`)
- Dangling pointers (accessing after `free`)
- Undefined behaviour on double-`free`
- The `realloc` return-value trap

---

## 🔬 Section 03 — C++ Allocation Routines

> Type-safe allocation. Constructors & destructors finally enter the picture.

C++ `new` and `delete` go beyond `malloc` — they call constructors and destructors, making objects proper first-class citizens on the heap.

```cpp
// Single object — constructor is called automatically
Employee* emp = new Employee(42, "Alice", 95000.0);
delete emp;          // destructor called, memory freed

// Array form
Employee* team = new Employee[5];
delete[] team;       // MUST match: [] for []

// Placement new — construct at a pre-allocated address
alignas(Employee) char buf[sizeof(Employee)];
Employee* p = new (buf) Employee(1, "Bob", 80000.0);
p->~Employee();      // must call destructor manually!
```

**Also covered:**
- Overloading `operator new` / `operator delete`
- The historical `auto_ptr` (the segue into smart pointers)

---

## 🔬 Section 04 — Smart Pointers Origin

> *How did we get here?* The story of `auto_ptr`.

Before C++11 introduced `unique_ptr`, the standard library shipped `std::auto_ptr` as its first attempt at automatic heap ownership. It had a fundamental flaw — copy semantics that silently transferred ownership, causing crashes in containers and algorithms.

```cpp
// ⚠️  auto_ptr — DEPRECATED (C++11), REMOVED (C++17)
#include <memory>
std::auto_ptr<Employee> ptr1(new Employee(1, "Alice", 75000));

std::auto_ptr<Employee> ptr2 = ptr1; // DANGER: ptr1 is now NULL!
ptr1->show();                        // 💥 undefined behaviour
```

**Lessons from `auto_ptr`:**

| Behaviour | `auto_ptr` | `unique_ptr` |
|-----------|-----------|--------------|
| Copy semantics | Silently transfers ownership | `= delete` — won't compile |
| Move semantics | Not supported | Explicit `std::move` required |
| STL containers | Breaks horribly | Fully supported |
| Status | ~~Removed in C++17~~ | ✅ Modern standard |

---

## 🔬 Section 05 — Unique Pointers

> Zero-overhead, single-owner RAII. The workhorse of modern C++.

`std::unique_ptr` fixes everything `auto_ptr` got wrong. It expresses *exclusive ownership* — exactly one pointer owns the resource, and when that pointer goes out of scope, the resource is freed automatically.

```
 Stack                    Heap
┌───────────────────┐    ┌─────────────────────┐
│  unique_ptr<Emp>  │───►│  Employee { id=42 } │
│  (owns resource)  │    │  name = "Alice"      │
└───────────────────┘    │  salary = 95000      │
        │                └─────────────────────┘
        │ (scope ends)
        ▼
    destructor called ──► memory freed automatically ✅
```

```cpp
#include <memory>

// Basic creation
std::unique_ptr<Employee> emp(new Employee(42, "Alice", 95000.0));

// Accessing the managed object
emp->show();          // via operator->
(*emp).show();        // via operator*
emp.get();            // raw pointer (non-owning!)

// Transferring ownership — EXPLICIT move required
std::unique_ptr<Employee> emp2 = std::move(emp);
// emp is now nullptr — safe and explicit

// Release without deleting
Employee* raw = emp2.release();   // emp2 = nullptr, you own raw now
delete raw;

// Replace the managed object
emp2.reset(new Employee(99, "Bob", 80000.0));
emp2.reset();         // frees and sets to nullptr

// Arrays
std::unique_ptr<int[]> scores = std::make_unique<int[]>(10);
scores[3] = 100;      // operator[] supported for array form
```

**Ownership diagram — move semantics:**

```
Before std::move:          After std::move:

ptr1 ──► [ Employee ]      ptr1 ──► (nullptr)
ptr2 ──► (nullptr)         ptr2 ──► [ Employee ]
```

---

## 🔬 Section 07 — Deleters

> What if `delete` isn't the right cleanup action?

By default `unique_ptr` calls `delete`. But resources aren't always heap memory — they can be file handles, socket descriptors, GPU buffers, or C library handles that need `fclose`, `CloseHandle`, or a custom `destroy()` call.

```cpp
// Custom deleter via function pointer
auto fileDeleter = [](FILE* f) {
    if (f) { fclose(f); std::cout << "File closed\n"; }
};
std::unique_ptr<FILE, decltype(fileDeleter)>
    fp(fopen("data.txt", "r"), fileDeleter);

// Custom deleter struct (zero-overhead — inlined at compile time)
struct EmployeeDeleter {
    void operator()(Employee* emp) const {
        std::cout << "Custom deleting: " << emp->getName() << '\n';
        delete emp;
    }
};
std::unique_ptr<Employee, EmployeeDeleter> emp(new Employee(1, "Alice", 75000));
```

**When to use custom deleters:**

```
Resource type       │  Cleanup call
────────────────────┼─────────────────────────
Heap object         │  delete (default)
Heap array          │  delete[] (use unique_ptr<T[]>)
C FILE*             │  fclose()
POSIX fd (int)      │  close()
Win32 HANDLE        │  CloseHandle()
OpenGL texture      │  glDeleteTextures()
Custom pool         │  pool.deallocate()
```

---

## 🔬 Section 08 — `make_unique`

> The safest way to create a `unique_ptr`. Always prefer this.

`std::make_unique` (C++14) is the idiomatic way to construct unique pointers. It eliminates a subtle exception-safety hole present when calling `new` directly in function arguments.

```cpp
// ✅ Preferred — exception safe, no raw new visible
auto emp = std::make_unique<Employee>(42, "Alice", 95000.0);

// ✅ Arrays
auto scores = std::make_unique<int[]>(10);

// ❌ Risky in function arguments (pre-C++17 evaluation order issue)
process(std::unique_ptr<Employee>(new Employee(42, "Alice", 95000)),
        computeSalary());   // if computeSalary() throws, leak possible!

// ✅ Safe equivalent
process(std::make_unique<Employee>(42, "Alice", 95000),
        computeSalary());
```

---

## 🧩 Key Concepts Cheat Sheet

```
┌──────────────────────────────────────────────────────────────────────┐
│                   MEMORY MANAGEMENT EVOLUTION                        │
├────────────────┬─────────────────────────────────────────────────────┤
│ malloc/free    │  Manual, type-unsafe, no constructors               │
│ new/delete     │  Type-safe, constructors/destructors, still manual  │
│ auto_ptr       │  First RAII attempt; broken copy semantics (legacy) │
│ unique_ptr     │  Exclusive ownership, move-only, zero overhead      │
│ make_unique    │  Factory for unique_ptr — exception-safe & clean    │
├────────────────┴─────────────────────────────────────────────────────┤
│                   unique_ptr QUICK REFERENCE                         │
├──────────────────────────────┬───────────────────────────────────────┤
│  make_unique<T>(args...)     │  Create (preferred)                   │
│  ptr.get()                   │  Raw pointer (non-owning)             │
│  ptr.reset(new T(...))       │  Replace managed object               │
│  ptr.reset()                 │  Free and nullify                     │
│  ptr.release()               │  Surrender ownership (caller deletes) │
│  std::move(ptr)              │  Transfer ownership                   │
│  ptr->member / (*ptr).member │  Access managed object                │
└──────────────────────────────┴───────────────────────────────────────┘
```

---

## 🛠️ Building the Labs

Each lab is a single, self-contained `main.cpp`. No build system required.

### Using VSCode
The repo ships with a `.vscode/` build configuration. Open any `main.cpp` and press **Ctrl+Shift+B** — it compiles the active file with MinGW GCC.

### From the Terminal

```bash
# Compile
g++ -std=c++17 -fdiagnostics-color=always -g \
    Section05_Unique_Pointers\01_basic\main.cpp \
    -o Section05_Unique_Pointers\01_basic\main.exe

# Run
.\Section05_Unique_Pointers\01_basic\main.exe
```

> 💡 Add `-std=c++17` when the lab uses features removed before that standard (e.g., `auto_ptr` needs `_GLIBCXX_USE_DEPRECATED` on GCC).

---

## 📋 Code Conventions

Every lab follows a consistent style to make diffs and reading easy:

```cpp
/**
 * @file    main.cpp
 * @brief   Lab description here
 * @author  Mohamed Atef
 * @date    YYYY-MM-DD
 */

/*=====================================================================
 *                  F I L E   I N C L U S I O N S
 *====================================================================*/
#include <iostream>
#include <memory>
using namespace std;

/*=====================================================================
 *                     U S E R   T Y P E S
 *====================================================================*/
class Employee {
    int    m_id;       // m_ prefix for member variables
    string m_name;
    double m_salary;
    // ...
};

/*=====================================================================
 *                  G L O B A L   F U N C T I O N S
 *====================================================================*/
void demo_basic_ownership() { /* ... */ }

int main() {
    demo_basic_ownership();   // main() only calls demos — no logic here
    return 0;
}
```

---

## 📚 Further Reading

- [cppreference — `std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr)
- [cppreference — `std::make_unique`](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique)
- [ISO C++ — Smart Pointers](https://isocpp.org/wiki/faq/freestore-mgmt#smart-ptrs)
- *Effective Modern C++* — Scott Meyers, Items 18–22

---

<div align="center">

Made with 💙 by [Mohamed Atef](https://github.com/MohamedAtef77)

*Learn it once. Own memory forever.*

</div>
