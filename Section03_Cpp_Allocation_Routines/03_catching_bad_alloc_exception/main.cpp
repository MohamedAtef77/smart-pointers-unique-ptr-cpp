/**
 * @file    main.cpp
 * @brief   Demonstrates catching std::bad_alloc thrown by new when heap
 *          exhaustion occurs during repeated large array allocations.
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
 * @brief  Allocates large int arrays repeatedly until heap is exhausted,
 *         causing new to throw std::bad_alloc.
 */
void newFailure(void)
{
    int *p = NULL;
    for(int i = 0; i < 100000000; i++)
    {
        p = new int[1000000];
    }
}
/**
 * @brief  Entry point — calls newFailure() inside a try-catch block to
 *         demonstrate handling std::bad_alloc thrown by new.
 *
 * @return 0 on success.
 */
int main(void)
{
    try
    {
        newFailure();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
