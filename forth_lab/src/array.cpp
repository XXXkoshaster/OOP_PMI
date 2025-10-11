#include "../include/array.h"
#include "../include/figure.h"
#include <memory>

template <typename T, typename Alloc>
Array<T, Alloc>::Array() : data_(nullptr), size_(0), capacity_(0) {}

template <typename T, typename Alloc>
Array<T, Alloc>::Array(size_t count, const T& value) : Array() {
    Reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        std::allocator_traits<Alloc>::construct(allocator_, data_ + i, value);
    }
    
    size_ = count;
}

template <typename T, typename Alloc>
Array<T, Alloc>::Array(const Array& other) : Array() {
    Reserve(other.size_);
    
    for (size_t i = 0; i < other.size_; ++i) {
        std::allocator_traits<Alloc>::construct(allocator_, data_ + i, other.data_[i]);
    }
    
    size_ = other.size_;
}

template <typename T, typename Alloc>
Array<T, Alloc>::Array(Array&& other) noexcept : Array() {
    *this = std::move(other);
}

template <typename T, typename Alloc>
Array<T, Alloc>::Array(std::initializer_list<T> init) : Array() {
    Reserve(init.size());
    size_t index = 0;
    for (const auto& value : init) {
        std::allocator_traits<Alloc>::construct(allocator_, data_ + index, value);
        ++index;
    }
    size_ = init.size();
}

template <typename T, typename Alloc>
Array<T, Alloc>::~Array() {
    Clear();
}

template <typename T, typename Alloc>
Array<T, Alloc>& Array<T, Alloc>::operator=(const Array& other) {
    if (this != &other) {
        Array temp(other);
        *this = std::move(temp);
    }
    return *this;
}

template <typename T, typename Alloc>
Array<T, Alloc>& Array<T, Alloc>::operator=(Array&& other) noexcept {
    if (this != &other) {
        Clear();
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

template <typename T, typename Alloc>
T& Array<T, Alloc>::operator[](size_t pos) {
    if (pos >= size_) {
        throw std::out_of_range("Array access out of range");
    }
    return data_[pos];
}

template <typename T, typename Alloc>
bool Array<T, Alloc>::IsEmpty() const noexcept {
    return size_ == 0;
}

template <typename T, typename Alloc>
const T& Array<T, Alloc>::Front() const noexcept {
    return data_[0];
}

template <typename T, typename Alloc>
T& Array<T, Alloc>::Back() const noexcept {
    return data_[size_ - 1];
}

template <typename T, typename Alloc>
T* Array<T, Alloc>::Data() const noexcept {
    return data_;
}

template <typename T, typename Alloc>
size_t Array<T, Alloc>::Size() const noexcept {
    return size_;
}

template <typename T, typename Alloc>
size_t Array<T, Alloc>::Capacity() const noexcept {
    return capacity_;
}

template <typename T, typename Alloc>
void Array<T, Alloc>::Reserve(size_t new_cap) {
    if (new_cap > capacity_ || capacity_ == 0) {
        if (new_cap < CAPACITY) {
            new_cap = CAPACITY;
        }
        T* new_data = allocator_.allocate(new_cap);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::construct(allocator_, new_data + i, std::move(data_[i]));
            std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
        }
        if (data_) {
            allocator_.deallocate(data_, capacity_);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }
}

template <typename T, typename Alloc>
void Array<T, Alloc>::Clear() noexcept {
    for (size_t i = 0; i < size_; ++i) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
    }
    if (data_) {
        allocator_.deallocate(data_, capacity_);
    }
    data_ = nullptr;
    size_ = 0;
}

template <typename T, typename Alloc>
void Array<T, Alloc>::Insert(size_t pos, T value) {
    if (pos > size_) {
        throw std::out_of_range("Insert position out of range");
    }

    if (size_ >= capacity_) {
        Reserve((capacity_ == 0) ? 1 : 2 * capacity_);
    }

    if (pos < size_) {
        for (size_t i = size_; i > pos; --i) {
            std::allocator_traits<Alloc>::construct(allocator_, data_ + i, std::move(data_[i - 1]));
            std::allocator_traits<Alloc>::destroy(allocator_, data_ + i - 1);
        }
    }
    std::allocator_traits<Alloc>::construct(allocator_, data_ + pos, std::move(value));
    ++size_;
}

template <typename T, typename Alloc>
void Array<T, Alloc>::Erase(size_t begin_pos, size_t end_pos) {
    if (begin_pos >= size_ || begin_pos >= end_pos) {
        return;
    }
    end_pos = std::min(end_pos, size_);
    size_t num_to_remove = end_pos - begin_pos;
    for (size_t i = begin_pos; i < size_ - num_to_remove; ++i) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
        std::allocator_traits<Alloc>::construct(allocator_, data_ + i, std::move(data_[i + num_to_remove]));
    }
    for (size_t i = size_ - num_to_remove; i < size_; ++i) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
    }
    size_ -= num_to_remove;
}

template <typename T, typename Alloc>
void Array<T, Alloc>::PushBack(T value) {
    Insert(size_, std::move(value));
}

template <typename T, typename Alloc>
template <class... Args>
void Array<T, Alloc>::EmplaceBack(Args&&... args) {
    Reserve(size_ + 1);
    std::allocator_traits<Alloc>::construct(allocator_, data_ + size_, std::forward<Args>(args)...);
    ++size_;
}

template <typename T, typename Alloc>
void Array<T, Alloc>::PopBack() {
    if (size_ > 0) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + --size_);
    }
}

template <typename T, typename Alloc>
void Array<T, Alloc>::Resize(size_t count, const T& value) {
    if (count < size_) {
        for (size_t i = count; i < size_; ++i) {
            std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
        }
        size_ = count;
    } else if (count > size_) {
        Reserve(count);
        for (size_t i = size_; i < count; ++i) {
            std::allocator_traits<Alloc>::construct(allocator_, data_ + i, value);
        }
        size_ = count;
    }
}


template class Array<int>;
template class Array<std::shared_ptr<Figure<double>>>;
