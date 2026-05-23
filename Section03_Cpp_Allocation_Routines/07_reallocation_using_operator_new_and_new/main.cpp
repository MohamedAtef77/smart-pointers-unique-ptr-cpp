/**
 * @file    main.cpp
 * @brief   Demonstrates placement new to construct objects in pre-allocated
 *          stack buffers, for both primitive types and user-defined types.
 * @author  Mohamed Atef
 * @date    2026-05-23
 */

/*===========================================================================*
 *                          F I L E  I N C L U S I O N S                     *
 *===========================================================================*/
#include <iostream>
#include <new>
#include <string>
#include <cstdint>

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
 * @brief  Simple employee record used to demonstrate placement new with a
 *         user-defined type that has a constructor and destructor.
 */
class Employee
{
public:
    int    m_id;
    string m_name;
    double m_salary;

    Employee(int id, const string& name, double salary)
        : m_id(id), m_name(name), m_salary(salary)
    {
        cout << "Employee constructor called\n";
    }

    Employee(const Employee& other)
        : m_id(other.m_id), m_name(other.m_name), m_salary(other.m_salary)
    {
        cout << "Employee copy constructor called\n";
    }

    ~Employee()
    {
        cout << "Employee destructor called\n";
    }

    void print(void) const
    {
        cout << "ID: " << m_id
             << " | Name: " << m_name
             << " | Salary: " << m_salary << endl;
    }
};

/*===========================================================================*
 *                      S T A T I C  F U N C T I O N S                       *
 *===========================================================================*/
/* None */

/*===========================================================================*
 *                      G L O B A L  F U N C T I O N S                       *
 *===========================================================================*/

void efficientReallocation(void)
{
    /* First, create a dynamic array of two */
    Employee *emp1 = new Employee[2]{
        {1, "Alice", 75000.0},
        {2, "Bob", 82000.0}
    };
    
    /* Second, create the new buffer  */
    Employee *emp2 = static_cast<Employee *>(operator new(3 * sizeof(Employee)));


    for(size_t i = 0; i < 2; ++i)
    {
        new(emp2 + i) Employee{emp1[i]};
    }


    for(size_t i = 0; i < 2; ++i)
    {
        emp1[i].~Employee();
    }
    operator delete(emp1);
}

/**
 * @brief  Entry point — exercises both placement new examples.
 *
 * @return 0 on success.
 */
int main(void)
{
    efficientReallocation();
}
