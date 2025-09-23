#include "../include/nangle.h"

template <Scalar T, size_t N>
NAngle<T, N>::NAngle() noexcept : vertices_number_(N) {
    points_.resize(N);
}

template <Scalar T, size_t N>
NAngle<T, N>::NAngle(const NAngle& other) noexcept : vertices_number_(N), points_(other.points_) {}

template <Scalar T, size_t N>
NAngle<T, N>::NAngle(const std::initializer_list<std::pair<T, T>>& points) : NAngle() {
    if (points.size() != N) {
        throw std::runtime_error("Trying to initialize " + std::to_string(N) + "-angle with incorrect count of points.");
    }
    size_t i = 0;
    for (const auto& point : points) {
        points_[i] = Point<T>(point.first, point.second);
        ++i;
    }
}

template <Scalar T, size_t N>
NAngle<T, N>& NAngle<T, N>::operator=(const NAngle& other) {
    if (this != &other) {
        points_ = other.points_;
    }
    return *this;
}

template <Scalar T, size_t N>
double NAngle<T, N>::area() const {
    if (vertices_number_ < 3) {
        return 0.0;
    }

    double a = std::sqrt(
        std::pow(points_[0].get_x() - points_[1].get_x(), 2) +
        std::pow(points_[0].get_y() - points_[1].get_y(), 2)
    );

    double n = static_cast<double>(vertices_number_);
    double area = (n * std::pow(a, 2)) / (4.0 * std::tan(std::acos(-1.0) / n));

    return area;
}

template <Scalar T, size_t N>
Point<T> NAngle<T, N>::get_barycenter() const {
    T sum_x = 0;
    T sum_y = 0;
    for (const auto& point : points_) {
        sum_x += point.get_x();
        sum_y += point.get_y();
    }
    return Point<T>(sum_x / vertices_number_, sum_y / vertices_number_);
}
