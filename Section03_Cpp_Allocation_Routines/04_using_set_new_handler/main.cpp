/**
 * @file    main.cpp
 * @brief   Demonstrates set_new_handler() to install a custom callback invoked
 *          by new when heap exhaustion occurs.
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
 * @brief  Installs a custom new-handler that prints a message on allocation
 *         failure, then repeatedly allocates large int arrays until heap is
 *         exhausted. The handler does not terminate the program, so new will
 *         keep invoking it on each failed attempt.
 */
void newFailure(void)
{
    int *p = NULL;

    set_new_handler([]() { std::cout << "Failed to allocate memory\n"; });
    for(int i = 0; i < 100000000; i++)
    {
        p = new int[1000000];
    }
}
/**
 * @brief  Entry point — calls newFailure() to demonstrate set_new_handler()
 *         behavior when new cannot satisfy an allocation request.
 *
 * @return 0 on success.
 */
int main(void)
{
    newFailure();
}
