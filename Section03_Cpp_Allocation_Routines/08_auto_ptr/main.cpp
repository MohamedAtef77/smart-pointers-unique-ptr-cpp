/**
 * @file    main.cpp
 * @brief   Demonstrates std::auto_ptr introduced in C++98, its ownership
 *          semantics, and why its surprising copy behaviour led to its
 *          deprecation in C++11 and removal in C++17.
 * @author  Mohamed Atef
 * @date    2026-05-24
 */

/*===========================================================================*
 *        C O M P I L E R   C O M P A T I B I L I T Y   P R A G M A S       *
 *===========================================================================*/
/*
 * auto_ptr was deprecated in C++11 and removed from the standard in C++17.
 * The pragmas/defines below re-enable it so this lab compiles under a modern
 * C++17 toolchain without modifying the compiler flags.
 *
 *  - MSVC  : #pragma warning(disable: 4996) suppresses the deprecation warning.
 *  - GCC   : _GLIBCXX_USE_DEPRECATED=1 tells libstdc++ to expose the removed type.
 *  - Clang : _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR exposes it in libc++.
 */
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4996)   /* C4996: 'auto_ptr' was declared deprecated */
#elif defined(__GNUC__)
    #define _GLIBCXX_USE_DEPRECATED 1
#elif defined(__clang__)
    #define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#endif

/*===========================================================================*
 *                          F I L E  I N C L U S I O N S                     *
 *===========================================================================*/
#include <iostream>
#include <memory>    /* auto_ptr lives here */
#include <string>

using namespace std;

/*===========================================================================*
 *                     M A C R O S  &  F U N C T I O N - L I K E             *
 *                              M A C R O S                                  *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                           U S E R  T Y P E S                              *
 *===========================================================================*/

/**
 * @brief  Simple resource type whose constructor/destructor print messages so
 *         we can observe exactly when the object is created and destroyed.
 */
class Resource
{
public:
    int    m_id;
    string m_label;

    Resource(int id, const string& label)
        : m_id(id), m_label(label)
    {
        cout << "[Resource " << m_id << " (" << m_label << ")] constructed\n";
    }

    ~Resource()
    {
        cout << "[Resource " << m_id << " (" << m_label << ")] destroyed\n";
    }

    void use(void) const
    {
        cout << "  Using Resource " << m_id << " (" << m_label << ")\n";
    }
};

/*===========================================================================*
 *                      S T A T I C  F U N C T I O N S                       *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                      G L O B A L  F U N C T I O N S                       *
 *===========================================================================*/

/**
 * @brief  Shows basic auto_ptr ownership: the pointer owns the heap object
 *         and destroys it automatically when it goes out of scope —
 *         no manual delete required.
 */
void demo_basic_ownership(void)
{
    cout << "\n--- demo_basic_ownership ---\n";

    auto_ptr<Resource> ptr(new Resource(1, "basic"));

    /* Access the managed object through operator-> and operator* */
    ptr->use();
    (*ptr).use();

    /* get() returns the raw pointer without releasing ownership */
    cout << "  raw ptr via get(): " << ptr.get() << "\n";

    cout << "  (ptr goes out of scope — destructor called automatically)\n";
    /* Resource 1 is destroyed here; no delete needed */
}

/**
 * @brief  Demonstrates release(): the owner voluntarily gives up the raw
 *         pointer and becomes null.  The caller is now responsible for
 *         the object.
 */
void demo_release(void)
{
    cout << "\n--- demo_release ---\n";

    auto_ptr<Resource> ptr(new Resource(2, "release-demo"));
    ptr->use();

    Resource *raw = ptr.release();     /* ptr is now null; raw owns the object */

    cout << "  ptr.get() after release(): " << ptr.get() << " (null)\n";

    raw->use();
    delete raw;                        /* manual cleanup — we hold the raw ptr now */
}

/**
 * @brief  Demonstrates reset(): discards the currently owned object (destroys
 *         it) and optionally takes ownership of a new one.
 */
void demo_reset(void)
{
    cout << "\n--- demo_reset ---\n";

    auto_ptr<Resource> ptr(new Resource(3, "old"));
    ptr->use();

    cout << "  Calling reset() with a new Resource — old one is destroyed first:\n";
    ptr.reset(new Resource(4, "new"));   /* Resource 3 destroyed here */
    ptr->use();

    cout << "  Calling reset() with nullptr — Resource 4 destroyed:\n";
    ptr.reset();                          /* Resource 4 destroyed here */
    cout << "  ptr.get(): " << ptr.get() << " (null)\n";
}

/**
 * @brief  THE DANGEROUS PART — ownership transfer via copy.
 *
 *  With a normal pointer or value type, copying gives you two independent
 *  copies.  With auto_ptr, copying TRANSFERS ownership: the source becomes
 *  null and the copy becomes the sole owner.  This breaks the fundamental
 *  expectation of copy semantics and is the primary reason auto_ptr was
 *  deprecated.
 *
 *  Concretely:
 *    auto_ptr<T> b = a;   // a is now null!
 *    auto_ptr<T> c(a);    // same — a is now null!
 */
void demo_ownership_transfer_via_copy(void)
{
    cout << "\n--- demo_ownership_transfer_via_copy ---\n";

    auto_ptr<Resource> original(new Resource(5, "owner"));
    cout << "  original.get() before copy: " << original.get() << "\n";

    /* Copy construction — looks innocent, but STEALS ownership */
    auto_ptr<Resource> thief(original);

    cout << "  original.get() after copy:  " << original.get()
         << " (null — ownership stolen!)\n";
    cout << "  thief.get():                " << thief.get() << "\n";

    thief->use();

    /* Dereferencing original here would be undefined behaviour */
}

/**
 * @brief  Ownership transfer via copy assignment — same surprise as the
 *         copy constructor.
 */
void demo_ownership_transfer_via_assignment(void)
{
    cout << "\n--- demo_ownership_transfer_via_assignment ---\n";

    auto_ptr<Resource> a(new Resource(6, "A"));
    auto_ptr<Resource> b(new Resource(7, "B"));

    cout << "  Before assignment — a: " << a.get()
         << "  b: " << b.get() << "\n";

    /*
     * Assignment:
     *   1. b's current object (Resource 7) is destroyed.
     *   2. Ownership of a's object is transferred to b.
     *   3. a becomes null.
     */
    b = a;

    cout << "  After  assignment — a: " << a.get()
         << " (null)  b: " << b.get() << "\n";

    b->use();
    /* Resource 6 is destroyed when b goes out of scope; Resource 7 already gone */
}

/**
 * @brief  Shows why auto_ptr cannot safely be stored in standard containers.
 *
 *  Containers like std::vector copy elements internally (e.g. during resize).
 *  Each copy silently steals ownership, leaving a vector full of null pointers
 *  after an innocent push_back or reallocation.  The standard explicitly
 *  forbids auto_ptr in containers for this reason.
 *
 *  We simulate the hazard manually rather than using a real vector because
 *  the compiler may reject auto_ptr in containers outright.
 */
void demo_container_hazard(void)
{
    cout << "\n--- demo_container_hazard ---\n";

    auto_ptr<Resource> slot_a(new Resource(8, "container-element"));
    cout << "  slot_a before simulated container copy: " << slot_a.get() << "\n";

    /* Simulate what a vector does internally when it copies an element */
    auto_ptr<Resource> slot_b(slot_a);   /* internal copy — slot_a stolen */

    cout << "  slot_a after  simulated container copy: " << slot_a.get()
         << " (null — vector would now hold a null pointer!)\n";

    slot_b->use();
}

/**
 * @brief  Entry point — runs all auto_ptr demonstrations in order.
 *
 * @return 0 on success.
 */
int main(void)
{
    /*
     * Timeline of auto_ptr:
     *   C++98  — introduced as the first standard smart pointer.
     *   C++11  — deprecated; std::unique_ptr introduced as the replacement.
     *   C++17  — removed from the standard library entirely.
     *
     * Key flaw: copy constructor and copy-assignment operator transfer
     * ownership rather than duplicating it, violating the normal semantics
     * of these operations and making auto_ptr unsafe in containers and
     * generic algorithms.
     */

    demo_basic_ownership();
    demo_release();
    demo_reset();
    demo_ownership_transfer_via_copy();
    demo_ownership_transfer_via_assignment();
    demo_container_hazard();

    cout << "\n(all remaining auto_ptrs destroyed on scope exit)\n";
    return 0;
}

#ifdef _MSC_VER
    #pragma warning(pop)   /* restore warning state after all code that uses auto_ptr */
#endif
