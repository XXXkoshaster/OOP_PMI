#include "../include/figure.h"

template <Scalar T>
Figure<T>::Figure() = default;

template <Scalar T>
Figure<T>::Figure(const std::initializer_list<Point<T>>& lst) {
    for (const auto& point : lst) {
        _vertices.push_back(std::make_unique<Point<T>>(point));
    }
}

template <Scalar T>
Figure<T>::Figure(const Figure& other) {
    for (const auto& point : other._vertices) {
        _vertices.push_back(std::make_unique<Point<T>>(*point));
    }
}

template <Scalar T>
Figure<T>::Figure(Figure&& other) noexcept = default;

template <Scalar T>
Figure<T>::~Figure() noexcept = default;

template <Scalar T>
Figure<T>& Figure<T>::operator=(const Figure& other) {
    if (this != &other) {
        _vertices.clear();
        for (const auto& point : other._vertices) {
            _vertices.push_back(std::make_unique<Point<T>>(*point));
        }
    }
    return *this;
}

template <Scalar T>
Figure<T>& Figure<T>::operator=(Figure&& other) noexcept = default;

template <Scalar T>
bool Figure<T>::operator==(const Figure& other) const {
    if (_vertices.size() != other._vertices.size()) {
        return false;
    }
    
    for (size_t i = 0; i < _vertices.size(); ++i) {
        if (*_vertices[i] != *other._vertices[i]) {
            return false;
        }
    }
    
    return true;
}

template <Scalar T>
void Figure<T>::add_vertex(const Point<T>& point) {
    _vertices.push_back(std::make_unique<Point<T>>(point));
}

template <Scalar T>
size_t Figure<T>::get_size() const {
    return _vertices.size();
}

template <Scalar T>
std::ostream& operator<<(std::ostream& os, const Figure<T>& fig) {
    os << "Figure with " << fig._vertices.size() << " vertices:\n";
    for (const auto& vertex : fig._vertices) {
        os << *vertex << " ";
    }
    return os;
}

template <Scalar T>
std::istream& operator>>(std::istream& is, Figure<T>& fig) {
    size_t size;
    is >> size;
    fig._vertices.clear();
    for (size_t i = 0; i < size; ++i) {
        Point<T> p;
        is >> p;
        fig._vertices.push_back(std::make_unique<Point<T>>(p));
    }
    return is;
}

template class Figure<double>;
template class Figure<int>;

template std::ostream& operator<< <double>(std::ostream&, const Figure<double>&);
template std::istream& operator>> <double>(std::istream&, Figure<double>&);

template std::ostream& operator<< <int>(std::ostream&, const Figure<int>&);
template std::istream& operator>> <int>(std::istream&, Figure<int>&);
