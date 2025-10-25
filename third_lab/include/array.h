#include "figure.h"

class Array {
    private:
        Figure** figures = nullptr;
        size_t capacity;
        size_t size;

        void reserve(size_t new_cap);
    public:
        Array() : size(0), capacity(1) {
            figures = new Figure*[capacity];
        }

        Array(const Array& other);
        Array(Array&& other);

        Array& operator=(const Array& other);
        Array& operator=(Array&& other);

        void pushBack(Figure* figure);
        void popBack();
        void erase(size_t index);

        size_t getSize() const {
            return size;
        }
        
        size_t getCapacity() const {
            return capacity;
        }

        Figure& operator[](size_t index);

        ~Array();
};