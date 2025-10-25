#include "../include/array.h"

Array::Array(const Array& other) {
    size = other.size;
    capacity = other.capacity;

    figures = new Figure*[capacity];
    for (size_t i = 0; i < size; ++i)
        figures[i] = other.figures[i];
}

Array::Array(Array&& other) 
    : figures(std::exchange(other.figures, nullptr)), size(other.size), capacity(other.capacity) {}

Array& Array::operator=(const Array& other) {
    if (this == &other)
        return *this;

    delete[] figures;

    size = other.size;
    capacity = other.capacity;

    figures = new Figure*[capacity];
    for (size_t i = 0; i < size; ++i)
        figures[i] = other.figures[i];

    return *this;
}

Array& Array::operator=(Array&& other) {
    if (this == &other)
        return *this;

    delete[] figures;

    figures = std::exchange(other.figures, nullptr);
    size = std::exchange(other.size, 0);
    capacity = std::exchange(other.capacity, 0);

    return *this;
}

void Array::reserve(size_t new_cap) {
    if (new_cap <= capacity) 
        return;
    
    Figure** new_arr = new Figure*[new_cap];
    for (size_t i = 0; i < size; ++i)
        new_arr[i] = figures[i];

    delete[] figures;

    figures = new_arr;
    capacity = new_cap;
}

void Array::pushBack(Figure* figure) {
    if (size == capacity)
        reserve(capacity * 2);

    figures[size] = figure;
    ++size;
}

void Array::popBack() {
    if (size == 0)
        return;

    figures[size - 1] = nullptr;
    --size;
}

void Array::erase(size_t index) {
    if (index >= size)
        return;

    delete figures[index];

    for (size_t i = index; i < size - 1; ++i)
        figures[i] = figures[i + 1];

    --size;
}

Figure& Array::operator[](size_t index) {
    return *(figures[index]);
}

Array::~Array() {
    for (size_t i = 0; i < size; ++i)
        delete figures[i];

    delete[] figures;
}