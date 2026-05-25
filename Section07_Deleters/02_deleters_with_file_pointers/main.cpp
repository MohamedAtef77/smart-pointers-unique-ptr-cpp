/**
 * @file    main.cpp
 * @brief   Demonstrates using std::unique_ptr with C file pointers and
 *          custom deleters to safely close FILE objects.
 * @author  Mohamed Atef
 * @date    2026-05-23
 */

/*===========================================================================*
 *                          F I L E  I N C L U S I O N S                     *
 *===========================================================================*/
#include <cstdio>
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

/** @brief Functor deleter used by unique_ptr to close a C FILE pointer. */
struct FileCloser
{
    void operator()(FILE *file) const
    {
        if (file)
        {
            cout << "Closing file pointer via FileCloser" << endl;
            fclose(file);
        }
    }
};

/*===========================================================================*
 *                       S T A T I C  F U N C T I O N S                     *
 *===========================================================================*/

/**
 * @brief Demonstrates unique_ptr with a function pointer deleter for FILE*.
 *
 * The file is automatically closed by calling fclose when the unique_ptr goes
 * out of scope.
 */
static void demo_function_pointer_file_deleter(void)
{
    cout << "=== demo_function_pointer_file_deleter ===" << endl;

    unique_ptr<FILE, int (*)(FILE *)> file{
        fopen("file_pointer_deleter.txt", "w+"),
        &fclose
    };

    if (!file)
    {
        cout << "Failed to open file with function pointer deleter." << endl;
        return;
    }

    fputs("Hello from unique_ptr with fclose deleter!\n", file.get());
    fflush(file.get());
}

/**
 * @brief Demonstrates unique_ptr with a custom functor deleter for FILE*.
 *
 * This form allows more control and can embed additional cleanup logic.
 */
static void demo_functor_file_deleter(void)
{
    cout << "=== demo_functor_file_deleter ===" << endl;

    unique_ptr<FILE, FileCloser> file{
        fopen("file_pointer_functor.txt", "w+"),
        FileCloser{}
    };

    if (!file)
    {
        cout << "Failed to open file with functor deleter." << endl;
        return;
    }

    fputs("Hello from unique_ptr with functor deleter!\n", file.get());
    fflush(file.get());
}

/*===========================================================================*
 *                       G L O B A L  F U N C T I O N S                     *
 *===========================================================================*/

/** @brief Entry point — demonstrates std::unique_ptr with FILE* deleters. */
int main(void)
{
    demo_function_pointer_file_deleter();
    demo_functor_file_deleter();

    return 0;
}
