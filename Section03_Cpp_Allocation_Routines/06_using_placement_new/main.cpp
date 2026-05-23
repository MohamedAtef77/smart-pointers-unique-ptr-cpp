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

/**
 * @brief  Demonstrates placement new on a stack-allocated memory buffer
 *         with a user-defined type. The destructor is called explicitly
 *         since the buffer was not heap-allocated.
 */
void placementNew1(void)
{
    uint8_t buffer[sizeof(Employee)];

    Employee *obj = new(buffer) Employee(1, "Mohamed", 10000.0);
    obj->print();
    obj->~Employee();
}

/**
 * @brief  Demonstrates placement new for primitive types and a user-defined
 *         type within a shared buffer, placing an int, an int array, and an
 *         Employee at consecutive positions.
 */
void placementNew2(void)
{
    constexpr size_t ARRAY_SIZE = 3;
    constexpr size_t BUFFER_SIZE = sizeof(int) + ARRAY_SIZE * sizeof(int) + sizeof(Employee);
    unsigned char buffer[BUFFER_SIZE]{};

    int *p = new(buffer) int{0x55AA55AA};
    cout << hex << showbase << *p << dec << endl;

    int *q = new(buffer + sizeof(int)) int[ARRAY_SIZE]{0x3, 0x4, 0x5};
    for (size_t i = 0; i < ARRAY_SIZE; i++)
        cout << hex << showbase << q[i] << dec << endl;

    Employee *emp = new(buffer + sizeof(int) + ARRAY_SIZE * sizeof(int)) Employee(2, "Ahmed", 20000.0);
    emp->print();
    emp->~Employee();
}

/**
 * @brief  Entry point — exercises both placement new examples.
 *
 * @return 0 on success.
 */
int main(void)
{
    placementNew1();
    cout << "Trying placement new 2: " << endl;

    placementNew2();
}
