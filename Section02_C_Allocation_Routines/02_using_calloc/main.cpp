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
 * @brief  Allocates a single integer on the heap using malloc(), assigns a
 *         value, prints it, and leaves the memory intentionally unfreed to
 *         illustrate a typical C-style allocation pattern.
 *
 * @note   The allocated memory is not freed — this is intentional for
 *         demonstration purposes; in production code free() must be called.
 */
void usingMalloc(void)
{
    int *p = (int *) malloc(sizeof(int));

    if(p == NULL)
    {
        cout << "The malloc function failed to allocate memory " << endl;
    }
    else
    {
        *p = 5;

        cout << *p << endl;
    }
}

/**
 * @brief  Allocates a contiguous array of integers on the heap using malloc(),
 *         fills each element with (index * 10), and prints all elements.
 *
 * @param  length  Number of integer elements to allocate.
 *
 * @note   The allocated memory is not freed — this is intentional for
 *         demonstration purposes; in production code free() must be called.
 * @warning  Loop indices start at 1 and go up to length inclusive, which
 *           writes and reads one past the last valid element (off-by-one /
 *           out-of-bounds access). This is a known bug kept here for
 *           educational comparison with safer C++ alternatives.
 */
void usingMallocArray(size_t length)
{
    int *p = (int *) malloc(sizeof(int) * length);

    if(p == NULL)
    {
        cout << "The malloc function failed to allocate memory " << endl;
    }
    else
    {
        /* Fill the values */
        for(size_t i = 1; i <= length; ++i)
        {
            p[i] = i * 10;
        }

        /* Display them */
        for(size_t i = 1; i <= length; ++i)
        {
            cout << p[i] << "\t";
        }

        cout << endl;
    }
}
/**
 * @brief  Allocates a zero-initialised contiguous array of integers on the
 *         heap using calloc(), prints the default zero values, fills each
 *         element with (index * 10), and prints the updated values.
 *
 * @param  length  Number of integer elements to allocate.
 *
 * @note   The allocated memory is not freed — this is intentional for
 *         demonstration purposes; in production code free() must be called.
 * @warning  Loop indices start at 1 and go up to length inclusive, which
 *           writes and reads one past the last valid element (off-by-one /
 *           out-of-bounds access). This is a known bug kept here for
 *           educational comparison with safer C++ alternatives.
 */
void usingCallocArray(size_t length)
{
    int *p = (int *) calloc(length,sizeof(int));

    if(p == NULL)
    {
        cout << "The malloc function failed to allocate memory " << endl;
    }
    else
    {

        /* Print the values allocated after using calloc */
        for(size_t i = 1; i <= length; ++i)
        {
            cout << p[i] << "\t";
        }   
        cout << endl;


        /* Fill the values */
        for(size_t i = 1; i <= length; ++i)
        {
            p[i] = i * 10;
        }

        /* Display them */
        for(size_t i = 1; i <= length; ++i)
        {
            cout << p[i] << "\t";
        }

        cout << endl;
    }
}
/**
 * @brief  Entry point — exercises usingMalloc() and usingMallocArray().
 *
 * @return 0 on success.
 */
int main(void)
{
    usingCallocArray(5);
}
