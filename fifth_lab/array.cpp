#include <memory_resource>
#include <type_traits>
#include <cstddef>
#include <iterator>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <new>

template <class T, class allocator_type = std::pmr::polymorphic_allocator<T>>
class Vector {
    private:
        allocator_type allctr;
        T* data;
        size_t size;
        size_t capacity;

        template <bool IsConst>
        class ForwardIterator {
            public:
                using pointer_type = std::conditional_t<IsConst, const T*, T*>;
                using reference_type = std::conditional_t<IsConst, const T&, T&>;
                using value_type = T;

                ForwardIterator(pointer_type ptr) : ptr(ptr) {}
                ForwardIterator(const ForwardIterator& other) : ptr(other.ptr) {}

                
                ForwardIterator& operator++() {
                    ++ptr;
                    return *this;
                }

                ForwardIterator operator++(int) {
                    ForwardIterator tmp = *this;
                    ++(*this);
                    return tmp;
                }  
                
                reference_type operator*() {
                    return *ptr;
                }
                
                pointer_type operator->() const {
                    return ptr;
                }
                
                bool operator==(const ForwardIterator& other) const {
                    return ptr == other.ptr;
                }
                
                bool operator!=(const ForwardIterator& other) const {
                    return ptr != other.ptr;
                }
            private:
                pointer_type ptr;
        };

        void reserve(size_t new_cap) {
            if (new_cap <= capacity) 
                return;

            T* new_data = allctr.allocate(new_cap);

            for (size_t i = 0; i < size; ++i) {
                new (new_data + i) T(data[i]);
            }

            for (size_t i = 0; i < size; ++i) {
                (data + i)->~T();
            }

            allctr.deallocate(data, capacity);

            data = new_data;
            capacity = new_cap;
        }

        void swap_(Vector& other) {
            std::swap(data, other.data);
            std::swap(size, other.size);
            std::swap(capacity, other.capacity);
        }


    public:
        using iterator = ForwardIterator<false>;
        using const_iterator = ForwardIterator<true>;
        
        iterator begin () {
            return iterator(data);
        }

        iterator end () {
            return iterator(data + size);
        }

        const_iterator begin () const {
            return const_iterator(data);
        }

        const_iterator end () const {
            return const_iterator(data + size);
        }

        const_iterator cbegin() const {
            return const_iterator(data);
        }

        const_iterator cend() const {
            return const_iterator(data + size);
        }

        Vector(const allocator_type& resource = std::pmr::get_default_resource())
            : allctr(resource), data(nullptr), size(0), capacity(0) {}

        Vector(size_t sz, std::pmr::memory_resource* resource = std::pmr::get_default_resource())
            : Vector(resource) {
            if (sz > 0) {
                data = allctr.allocate(sz);
                for (size_t i = 0; i < sz; ++i)
                    new (data + i) T();
        
                size = sz;
                capacity = sz;
            }
        }

        Vector(const Vector& other, std::pmr::memory_resource* resource = std::pmr::get_default_resource())
            : Vector(resource) {
            if (other.size > 0) {
                data = allctr.allocate(other.size);
                for (size_t i = 0; i < other.size; ++i) {
                    new (data + i) T(other.data[i]);
                }
                size = other.size;
                capacity = other.size;
            }
        }

        Vector(Vector&& other)
            : size(std::exchange(other.size, 0)),
              capacity(std::exchange(other.capacity, 0)),
              data(std::exchange(other.data, nullptr)),
              allctr(std::move(other.allctr)) {}

        Vector& operator=(const Vector& other) {
            if (this != &other) {
                clear();
                if (capacity < other.size) {
                    if (data) 
                        allctr.deallocate(data, capacity);
                    
                    data = nullptr;
                    capacity = 0;
                }
                
                if (other.size > capacity) {
                    data = allctr.allocate(other.size);
                    capacity = other.size;
                }
                
                for (size_t i = 0; i < other.size; ++i) {
                    new (data + i) T(other.data[i]);
                }
                size = other.size;
            }
            return *this;
        }
        

        Vector operator=(Vector&& other) {
            if (this != &other) {
                allctr.deallocate(data, capacity);
                swap_(*this, other);
                allctr = std::move(other.allctr);
            }
            return *this;
        }

        void swap(Vector& other) {
            swap_(*this, other);
            std::swap(allctr, other.allctr);
        }

        void pushBack(const T& value) {
            if (size == capacity) {
                reserve(capacity == 0 ? 1 : capacity * 2);
            }
            new (data + size) T(value);
            ++size;
        }

        void pushBack(T&& value) {
            if (size == capacity) {
                reserve(capacity == 0 ? 1 : capacity * 2);
            }
            new (data + size) T(std::move(value));
            ++size;
        }

        void popBack() {
            if (isEmpty()) {
                throw std::runtime_error("Array is empty!");
            }
            
            data[size - 1].~T();
            --size;
        }

        void clear() {
            for (size_t i = 0; i < size; ++i) {
                data[i].~T();
            }
            size = 0;
        }


        T& at(size_t idx) {
            if (idx >= size) 
                throw std::out_of_range("index out of range");

            return data[idx];
        }

        const T& at(size_t idx) const {
            if (idx >= size) 
                throw std::out_of_range("index out of range");

            return data[idx];
        }

        T& operator[](size_t idx) { 
            return data[idx]; 
        }

        const T& operator[](size_t idx) const { 
            return data[idx]; 
        }

        bool isEmpty() const noexcept { 
            return size == 0; 
        }
        size_t getSize() const noexcept { 
            return size; 
        }
        size_t getCapacity() const noexcept { 
            return capacity; 
        }

        ~Vector() {
            clear();
            if (data) {
                allctr.deallocate(data, capacity);
                data = nullptr;
                capacity = 0;
            }
        }
};