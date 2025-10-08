#ifndef NANGLE_H
#define NANGLE_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cmath>
#include <string>

#include "figure.h"

template <Scalar T, size_t N>
class NAngle : public Figure<T> {
public:
    static constexpr size_t NUM_POINTS = N;

    NAngle() noexcept;
    NAngle(const NAngle& other) noexcept;
    NAngle(const std::initializer_list<std::pair<T, T>>& points);

    NAngle& operator=(const NAngle& other);

    double area() const override;
    Point<T> get_barycenter() const override;

private:
    size_t vertices_number_;
    std::vector<Point<T>> points_;
};

template <Scalar T>
using Pentagon = NAngle<T, 5>;

template <Scalar T>
using Hexagon = NAngle<T, 6>;

template <Scalar T>
using Octagon = NAngle<T, 8>;

#endif
