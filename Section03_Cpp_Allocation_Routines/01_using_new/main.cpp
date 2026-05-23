/**
 * @file    main.cpp
 * @brief   Demonstrates dynamic memory allocation using new/delete for single
 *          values and arrays in C++ allocation routines.
 * @author  Mohamed Atef
 * @date    2026-05-23
 */

/*===========================================================================*
 *                          F I L E  I N C L U S I O N S                     *
 *===========================================================================*/
#include <iostream>

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
 * @brief  Simple data class representing an employee with a name and ID.
 *         Used to demonstrate new/delete on user-defined types.
 */
class Employee
{
public:
    string name;
    int ID;

    /** @brief Default constructor — members must be set manually after construction. */
    Employee()
    {
        cout << "Default Constructor" << endl;
    }

    /**
     * @brief  Parameterized constructor.
     * @param  name  Employee's full name.
     * @param  id    Unique employee identifier.
     */
    Employee(string name, int id) : name(name), ID(id)
    {
        cout << "Parameterized Constructor" << endl;
    }

    /** @brief Destructor — called automatically when delete is used on the object. */
    ~Employee()
    {
        cout << "Destructor" << endl;
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
 * @brief  Allocates an Employee on the heap via new using the default constructor,
 *         sets its members manually, prints them, then releases memory with delete.
 */
void newExample5(void)
{
    Employee *p = new Employee;
    p->ID = 10;
    p->name = "Mohamed";

    cout << p->ID << "\t" << p->name << endl;

    delete p;
}

/**
 * @brief  Allocates an Employee on the heap via new using the parameterized
 *         constructor, prints its members, then releases memory with delete.
 */
void newExample6(void)
{
    Employee *p = new Employee("Ahmed", 20);

    cout << p->ID << "\t" << p->name << endl;

    delete p;
}

/**
 * @brief  Entry point — exercises newExample5() and newExample6().
 *
 * @return 0 on success.
 */
int main(void)
{
    newExample5();
    newExample6();
}
