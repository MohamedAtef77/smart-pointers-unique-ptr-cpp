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
#include <vector>
using namespace std;

/** @brief Represents a vehicle engine with its type and horsepower. */
class Engine
{
private:
    string type;
    int horsePower;

public:
    Engine(string t, int hp) : type(move(t)), horsePower(hp)
    {
        cout << "Engine Constructor: " << type << " (" << horsePower << " HP)" << endl;
    }

    ~Engine()
    {
        cout << "Engine Destructor: " << type << endl;
    }

    void info() const
    {
        cout << "Engine Info -> Type: " << type << ", HP: " << horsePower << endl;
    }
};

/** @brief Represents a Car with a specific name and an exclusively owned Engine. */
class Car
{
public:
    string name;
    unique_ptr<Engine> engine;

    Car(string n, unique_ptr<Engine> e) : name(move(n)), engine(move(e))
    {
        cout << "Car Constructor: " << name << endl;
    }

    ~Car()
    {
        cout << "Car Destructor: " << name << endl;
    }

    void info() const
    {
        cout << "Car Info -> Name: " << name << endl;
        if (engine)
        {
            engine->info();
        }
    }
};

/** @brief Factory function to create a Car and its Engine. */
unique_ptr<Car> createCar(const string &name,const string &engineType, int HP)
{
    auto engine = make_unique<Engine>(engineType,HP);

    return make_unique<Car>(name, move(engine)); /* The ownership is moved from engine object to the object returned by this function*/
}

/** @brief Takes ownership of a Car and registers it. */
void registerCar(unique_ptr<Car> car) {
    cout << endl << "[Registry] Received ownership of: " << car->name << endl;
    car->info();
    // car is destroyed here — end of function scope
}

/** @brief Prints information about a Car without taking ownership. */
void printCarInfo(const Car& car) {  // raw reference — just borrowing
    cout << endl << "[Print] Borrowing car info:" << endl;
    car.info();
}

int main(void)
{
    cout << "=== 1. Basic make_unique ===" << endl;
    {
        auto engine = make_unique<Engine>("V8", 450);
        engine->info();
        // engine destroyed automatically at end of this block
    }
    cout << "(block ended — engine was destroyed above)" << endl << endl;


    cout << "=== 2. Nested ownership ===" << endl;
    {
        auto car = createCar("Mustang GT", "V8", 450);
        car->info();
    }
    cout << "(block ended — car and engine both destroyed)" << endl << endl;


    cout << "=== 3. Transferring ownership with move ===" << endl;
    {
        auto car1 = make_unique<Car>(
            "Mustang Shelby",
            make_unique<Engine>("Supercharged V8", 760)
        );

        cout << "Before move — car1 is: " << car1->name << endl;

        auto car2 = move(car1);  // ownership transferred

        if (!car1) {
            cout << "After move  — car1 is: nullptr (no longer owns anything)" << endl;
        }
        cout << "After move  — car2 is: " << car2->name << endl;
    }
    cout << "(block ended)" << endl << endl;


    cout << "=== 4. Passing to a function (ownership transfer) ===" << endl;
    {
        auto car = createCar("Mustang Mach-E", "Electric", 480);
        registerCar(move(car));  // car is now nullptr here
        if (!car) {
            cout << "[main] car is nullptr — ownership was transferred" << endl;
        }
    }
    cout << "(block ended)" << endl << endl;

    cout << "=== 5. Borrowing without transferring ownership ===" << endl;
    {
        auto car = createCar("Mustang Dark Horse", "V8", 500);
        printCarInfo(*car);           // dereference — pass as reference, no move
        cout << "[main] car still owned here: " << car->name << endl;
    }
    cout << "(block ended)" << endl << endl;


    cout << "=== 6. unique_ptr in a vector ===" << endl;
    {
        vector<unique_ptr<Car>> garage;

        garage.push_back(createCar("Mustang GT",         "V8",            450));
        garage.push_back(createCar("Mustang Shelby GT500","Supercharged V8",760));
        garage.push_back(createCar("Mustang Mach-E",     "Electric",       480));

        cout << endl << "Garage contents:" << endl;
        for (const auto& car : garage) {
            car->info();
        }
        // all Cars and their Engines destroyed when garage goes out of scope
    }
    cout << "(block ended — all garage cars destroyed)" << endl << endl;


    cout << "=== 7. Resetting and releasing ===" << endl;
    {
        auto car = createCar("Mustang Base", "EcoBoost V4", 310);

        cout << endl << "Calling reset() — destroys the object early:" << endl;
        car.reset();  // explicitly destroy before scope ends
        if (!car) {
            cout << "car is now nullptr after reset()" << endl;
        }
    }
    cout << "(block ended)" << endl << endl;

    cout << "=== Program complete ===" << endl;
    return 0;
}