#ifndef ARRAY_H
#define ARRAY_H

#include "figure.h"

template<typename T>
class Array {
    private:
        std::shared_ptr<T[]> data;
        size_t capacity;
        size_t size;

        void reserve(size_t new_cap) {
            if (new_cap <= capacity) 
                return;
            
            std::shared_ptr<T[]> new_arr(new T[new_cap]);
            for (size_t i = 0; i < size; ++i)
                new_arr[i] = data[i];

            data = new_arr;
            capacity = new_cap;
        }
    public:
        Array() : data(new T[1]), size(0), capacity(1) {}

        Array(size_t capacity) : data(new T[capacity]()), size(0), capacity(capacity) {}

        Array(const Array& other) {
            size = other.size;
            capacity = other.capacity;
            
            data = std::shared_ptr<T[]>(new T[capacity]);
            for (size_t i = 0; i < size; ++i)
                data[i] = other.data[i];
        }
        Array(Array&& other) 
            : data(std::move(other.data)), size(std::exchange(other.size, 0)), capacity(std::exchange(other.capacity,0)) {}

        Array& operator=(const Array& other) {
            if (this == &other)
                return *this;

            size = other.size;
            capacity = other.capacity;

            data = std::shared_ptr<T[]>(new T[capacity]);
            for (size_t i = 0; i < size; ++i)
                data[i] = other.data[i];

            return *this;
        }
        Array& operator=(Array&& other) {
            if (this == &other)
                return *this;

            data = std::move(other.data);
            size = std::exchange(other.size, 0);
            capacity = std::exchange(other.capacity, 0);

            return *this;           
        }

        void pushBack(const T& t) {
            if (size == capacity)
                reserve(capacity * 2);

            data[size] = t;
            ++size;
        }
    
        void pushBack(T&& t) {
            if (size == capacity)
                reserve(capacity * 2);

            data[size] = std::move(t);
            ++size;
        }

        void popBack() {
            if (size == 0)
                return;
            --size;
        }

        void remove(size_t index) {
            if (index >= size)
                return;

            for (size_t i = index; i < size - 1; ++i)
                data[i] = std::move(data[i + 1]);

            --size;
        }

        size_t getSize() const {
            return size;
        }
        
        size_t getCapacity() const {
            return capacity;
        }

        T& operator[](size_t index) {
            return data[index];
        }
        const T& operator[](size_t index) const {
            return data[index];
        }

        ~Array() = default;
};

#endif