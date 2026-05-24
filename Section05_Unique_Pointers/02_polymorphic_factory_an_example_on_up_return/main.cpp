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
#include <string>


/*===========================================================================*
 *               M A C R O S  &  F U N C T I O N - L I K E  M A C R O S      *
 *===========================================================================*/
/* None */


/*===========================================================================*
 *                              U S E R  T Y P E S                            *
 *===========================================================================*/

using namespace std;


/**
 * @brief  Abstract base class representing a generic drawable shape.
 *
 * Derived shape types must provide their own drawing, area calculation,
 * and name accessors.
 */
class Shape
{
    public:

    virtual ~Shape()
    {
        cout << "Shape base destructor" << endl;
    }

    virtual void draw() const = 0;
    virtual double area() const = 0;
    virtual string name() const = 0;
};

/**
 * @brief  Represents a circle shape with a radius.
 */
class Circle : public Shape
{
    public:

    explicit Circle(double radius)
        : m_radius(radius)
    {
    }

    ~Circle()
    {
        cout << "Circle destructor" << endl;
    }

    void draw() const override
    {
        cout << "Drawing " << name() << " with radius " << m_radius << endl;
    }

    double area() const override
    {
        return 3.14159265358979323846 * m_radius * m_radius;
    }

    string name() const override
    {
        return "Circle";
    }

    private:

    double m_radius;
};

/**
 * @brief  Represents a rectangle shape with width and height.
 */
class Rectangle : public Shape
{
    public:

    Rectangle(double width, double height)
        : m_width(width)
        , m_height(height)
    {
    }

    ~Rectangle()
    {
        cout << "Rectangle destructor" << endl;
    }

    void draw() const override
    {
        cout << "Drawing " << name() << " with width " << m_width
             << " and height " << m_height << endl;
    }

    double area() const override
    {
        return m_width * m_height;
    }

    string name() const override
    {
        return "Rectangle";
    }

    private:

    double m_width;
    double m_height;
};

/**
 * @brief  Represents a triangle shape with a base and height.
 */
class Triangle : public Shape
{
    public:

    Triangle(double base, double height)
        : m_base(base)
        , m_height(height)
    {
    }

    ~Triangle()
    {
        cout << "Triangle destructor" << endl;
    }

    void draw() const override
    {
        cout << "Drawing " << name() << " with base " << m_base
             << " and height " << m_height << endl;
    }

    double area() const override
    {
        return 0.5 * m_base * m_height;
    }

    string name() const override
    {
        return "Triangle";
    }

    private:

    double m_base;
    double m_height;
};

/**
 * @brief  Enumerates the supported shape types for the factory.
 */
enum class ShapeType {Circle,Rectangle,Triangle};


/*===========================================================================*
 *                          G L O B A L  F U N C T I O N S                    *
 *===========================================================================*/

/**
 * @brief  Factory function that creates a concrete Shape instance.
 *
 * @param  type  The shape type to create.
 * @return A unique_ptr owning a newly created Shape.
 */
unique_ptr<Shape> CreateShape(ShapeType type)
{
    switch (type)
    {
        case ShapeType::Circle:
            return make_unique<Circle>(5.0);
        case ShapeType::Rectangle:
            return make_unique<Rectangle>(4.0, 3.0);
        case ShapeType::Triangle:
            return make_unique<Triangle>(6.0, 2.5);
    }

    return nullptr;
}

/**
 * @brief  Demonstrates creation and use of a single Shape object.
 */
void demoSingleObject(void)
{
    cout << "Demo: Single Object" << endl;

    unique_ptr<Shape> ptr = CreateShape(ShapeType::Circle);
    ptr->draw();

    cout << ptr->area() << endl;


    cout << "Leaving Scope (DemoSingleObject)..." << endl;

}

/**
 * @brief  Demonstrates storing different Shape objects in a container.
 */
void demoContainer(void)
{
    vector<unique_ptr<Shape>> shapes;

    shapes.push_back(CreateShape(ShapeType::Circle));
    shapes.push_back(CreateShape(ShapeType::Rectangle));
    shapes.push_back(CreateShape(ShapeType::Triangle));
    shapes.push_back(CreateShape(ShapeType::Circle));

    for (const auto& shape : shapes)
    {
        shape->draw();
        cout << shape->name() << " area: " << shape->area() << endl;
    }

    cout << "Leaving Scope (DemoContainer)..." << endl;
}


/**
 * @brief  Renders a shape and destroys it when the unique_ptr goes out of scope.
 *
 * Ownership is transferred into this function by value.
 */
void renderAndDiscard(unique_ptr<Shape> shape)
{
    cout << "renderAndDiscard received a " << shape->name() << endl;
    shape->draw();
}

/**
 * @brief  Demonstrates moving unique_ptr ownership to another function.
 */
void demoOwnershipTransfer(void)
{
    cout << "\n=== Demo 3: Ownership Transfer ===" << endl;

    auto s = CreateShape(ShapeType::Rectangle);

    cout << "  Before move: s is ";
    if (s)
    {
        cout << "valid";
    }
    else
    {
        cout << "null";
    }
    cout << endl;

    renderAndDiscard(std::move(s));   // s gives up ownership, if you don't use move, the compilation will fail

    cout << "  After move:  s is ";
    if (s)
    {
        cout << "valid";
    }
    else
    {
        cout << "null";
    }
    cout << endl;
}

/**
 * @brief  Program entry point.
 */
int main(void)
{
    // demoSingleObject();
    // demoContainer();
    demoOwnershipTransfer();
}
