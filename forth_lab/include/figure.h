#ifndef FIGURE_H
#define FIGURE_H

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <vector>
#include <utility>

#include "point.h"

template <Scalar T>
class Figure {
public:
    Figure();
    Figure(const std::initializer_list<Point<T>>& lst);
    Figure(const Figure& other);
    Figure(Figure&& other) noexcept;
    virtual ~Figure() noexcept;

    Figure& operator=(const Figure& other);
    Figure& operator=(Figure&& other) noexcept;

    bool operator==(const Figure& other) const;

    virtual Point<T> get_barycenter() const = 0;

    void add_vertex(const Point<T>& point);
    size_t get_size() const;

    virtual double area() const = 0;

    template <Scalar U>
    friend std::ostream& operator<<(std::ostream& os, const Figure<U>& fig);

    template <Scalar U>
    friend std::istream& operator>>(std::istream& is, Figure<U>& fig);

protected:
    std::vector<std::unique_ptr<Point<T>>> _vertices;
};

#endif