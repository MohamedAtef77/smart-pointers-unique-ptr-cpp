/**
 * @file    main.cpp
 * @brief   Demonstrates the nothrow version of new, which returns nullptr on
 *          allocation failure instead of throwing std::bad_alloc.
 * @author  Mohamed Atef
 * @date    2026-05-23
 */

/*===========================================================================*
 *                          F I L E  I N C L U S I O N S                     *
 *===========================================================================*/
#include <iostream>
#include <new>

using namespace std;

/*===========================================================================*
 *                     M A C R O S  &  F U N C T I O N - L I K E             *
 *                              M A C R O S                                  *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                           U S E R  T Y P E S                              *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                      S T A T I C  F U N C T I O N S                       *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                      G L O B A L  F U N C T I O N S                       *
 *===========================================================================*/

/**
 * @brief  Allocates a single int on the heap via new (std::nothrow), checks
 *         the returned pointer for nullptr, prints the value, then deletes it.
 */
void usingNewNothrowExample1(void)
{
    int *p = new (std::nothrow) int{42};

    if (p == nullptr)
    {
        cout << "Allocation failed\n";
        return;
    }

    cout << *p << endl;

    delete p;
}

/**
 * @brief  Allocates an int array on the heap via new (std::nothrow) with a
 *         brace-initializer list, checks for nullptr, prints all elements,
 *         then releases memory with delete[].
 */
void usingNewNothrowExample2(void)
{
    const int SIZE = 5;
    int *arr = new (std::nothrow) int[SIZE]{10, 20, 30, 40, 50};

    if (arr == nullptr)
    {
        cout << "Allocation failed\n";
        return;
    }

    for (int i = 0; i < SIZE; i++)
        cout << arr[i] << " ";
    cout << endl;

    delete[] arr;
}

/**
 * @brief  Repeatedly allocates large int arrays via new (std::nothrow) until
 *         the heap is exhausted, demonstrating that failure yields nullptr
 *         rather than an exception.
 */
void usingNewNothrowExample3(void)
{
    int *p = NULL;

    for (int i = 0; i < 100000000; i++)
    {
        p = new (std::nothrow) int[1000000];

        if (p == nullptr)
        {
            cout << "Allocation failed at iteration " << i << endl;
            break;
        }
    }
}

/**
 * @brief  Entry point — exercises the three nothrow new examples.
 *
 * @return 0 on success.
 */
int main(void)
{
    usingNewNothrowExample1();
    usingNewNothrowExample2();
    usingNewNothrowExample3();
}
