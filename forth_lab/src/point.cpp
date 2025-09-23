#include "../include/point.h"

#include <istream>
#include <ostream>

template <Scalar T>
Point<T>::Point(T x, T y) : _x(x), _y(y) {}

template <Scalar T>
T Point<T>::get_x() const {
    return _x;
}

template <Scalar T>
T Point<T>::get_y() const {
    return _y;
}

template <Scalar T>
bool Point<T>::operator==(const Point& other) const {
    return _x == other._x && _y == other._y;
}

template <Scalar T>
std::ostream& operator<<(std::ostream& os, const Point<T>& p) {
    os << "(" << p._x << ", " << p._y << ")";
    return os;
}

template <Scalar T>
std::istream& operator>>(std::istream& is, Point<T>& p) {
    is >> p._x >> p._y;
    return is;
}
