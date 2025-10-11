#ifndef ARRAY_H
#define ARRAY_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T, typename Alloc = std::allocator<T>>
class Array {
public:
    using difference_type	 = std::ptrdiff_t;
    using value_type	= T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;
    
    Array();
    Array(size_t count, const T& value);
    Array(const Array& other);
    Array(Array&& other) noexcept;
    Array(std::initializer_list<T> init);
    ~Array();

    Array& operator=(const Array& other);
    Array& operator=(Array&& other) noexcept;

    T& operator[](size_t pos);

    bool IsEmpty() const noexcept;
    const T& Front() const noexcept;
    T& Back() const noexcept;
    T* Data() const noexcept;
    size_t Size() const noexcept;
    size_t Capacity() const noexcept;

    void Reserve(size_t new_cap);
    void Clear() noexcept;
    void Insert(size_t pos, T value);
    void Erase(size_t begin_pos, size_t end_pos);
    void PushBack(T value);

    template <class... Args>
    void EmplaceBack(Args&&... args);

    void PopBack();
    void Resize(size_t count, const T& value);

private:
    T* data_;
    size_t size_;
    size_t capacity_;
    Alloc allocator_;
    static constexpr size_t CAPACITY = 1;
};

#endif
