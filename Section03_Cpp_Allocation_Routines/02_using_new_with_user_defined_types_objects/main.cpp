/**
 * @file    main.cpp
 * @brief   Demonstrates dynamic memory allocation using malloc() for single
 *          values and arrays in C-style allocation routines.
 * @author  Mohamed Atef
 * @date    2026-05-23
 */

/*===========================================================================*
 *                          F I L E  I N C L U S I O N S                     *
 *===========================================================================*/
#include <iostream>
#include <malloc.h>

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
 * @brief  Allocates a single int on the heap via new, assigns a value
 *         post-allocation, prints it, then releases the memory with delete.
 */
void usingNewExample1(void)
{
    int *p = new int;

    *p = 5 ;

    cout << *p << endl;

    delete p;
}

/**
 * @brief  Allocates a single int on the heap via new with a brace-initializer,
 *         prints the value, then releases the memory with delete.
 */
void usingNewExample2(void)
{
    int *p = new int{34};

    cout << *p << endl;

    delete p;
}
/**
 * @brief  Allocates an int array of fixed size on the heap via new[],
 *         assigns values manually, prints them, then releases memory with delete[].
 *         Elements are uninitialized and hold indeterminate values before assignment.
 */
void usingNewExample3(void)
{
    const int SIZE = 5;
    int *arr = new int[SIZE];

    for (int i = 0; i < SIZE; i++)
        arr[i] = (i + 1) * 10;

    for (int i = 0; i < SIZE; i++)
        cout << arr[i] << " ";
    cout << endl;

    delete[] arr;
}

/**
 * @brief  Allocates an int array on the heap via new[] with a brace-initializer
 *         list, prints all elements, then releases memory with delete[].
 *         Unspecified trailing elements are zero-initialized.
 */
void usingNewExample4(void)
{
    const int SIZE = 5;
    int *arr = new int[SIZE]{10, 20, 30, 40, 50};

    for (int i = 0; i < SIZE; i++)
        cout << arr[i] << " ";
    cout << endl;

    delete[] arr;
}

/**
 * @brief  Entry point — exercises usingMalloc() and usingMallocArray().
 *
 * @return 0 on success.
 */
int main(void)
{
    usingNewExample1();
    usingNewExample2();
    usingNewExample3();
    usingNewExample4();

}
