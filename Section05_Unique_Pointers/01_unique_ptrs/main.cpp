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

/** @brief Represents a vehicle engine with a rated horsepower value. */
class Engine
{
    public:
        double hoursPower;

        Engine() = default;
        Engine(double hp) :hoursPower(hp)
        {
            cout << "Constructor: The Engine is ON, its HP is: " << hoursPower << endl;
        }
        ~Engine()
        {
            cout << "Destructor: The Engine is OFF" << endl;
        }

};

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
    unique_ptr<Engine> ptr{new Engine(450.00)};
}
