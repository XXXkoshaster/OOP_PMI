#ifndef POINT_H
#define POINT_H

#include <concepts>
#include <iosfwd>
#include <istream>
#include <ostream>
#include <type_traits>

template <typename T>
concept Scalar = std::is_arithmetic_v<T>;

template <Scalar T>
class Point {
public:
    Point() = default;
    Point(T x, T y);
    Point(const Point& other) = default;
    Point(Point&& other) noexcept = default;
    ~Point() = default;

    T get_x() const;
    T get_y() const;

    Point& operator=(const Point& other) = default;
    Point& operator=(Point&& other) noexcept = default;

    bool operator==(const Point& other) const;

    template <Scalar U>
    friend std::ostream& operator<<(std::ostream& os, const Point<U>& p);

    template <Scalar U>
    friend std::istream& operator>>(std::istream& is, Point<U>& p);

private:
    T _x{0};
    T _y{0};
};

#endif