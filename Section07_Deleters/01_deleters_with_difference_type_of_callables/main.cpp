/**
 * @file    main.cpp
 * @brief   Demonstrates basic usage of std::unique_ptr for exclusive-ownership
 *          smart pointer management in modern C++.
 * @author  Mohamed Atef
 * @date    2026-05-23
 */

/*===========================================================================*
 *                          F I L E  I N C L U S I O N S                     *
 *===========================================================================*/
#include <iostream>
#include <memory>
using namespace std;

/*===========================================================================*
 *                     M A C R O S  &  F U N C T I O N - L I K E             *
 *                              M A C R O S                                  *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                           U S E R  T Y P E S                              *
 *===========================================================================*/

struct Free
{
    void operator()(int *p)
    {
        cout << "I'm inside the functor deleter" << endl;
        free(p);
    }
};


void Free2(int *p)
{
    cout << "I'm inside a function deleter " << endl; 
    free(p);
}

/*===========================================================================*
 *                       S T A T I C  F U N C T I O N S                     *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                       G L O B A L  F U N C T I O N S                     *
 *===========================================================================*/

/** @brief Entry point — demonstrates basic unique_ptr ownership of an Engine. */
int main(void)
{
    unique_ptr<int, Free> p1{new int{5},Free{}};
    unique_ptr<int,void (*)(int *p)> p2{new int{14},Free2};

}
