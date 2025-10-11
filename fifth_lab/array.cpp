#include <memory_resource>
#include <type_traits>
#include <cstddef>
#include <iterator>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <new>

template <class T, class allocator_type = std::pmr::polymorphic_allocator<T>>
class DynamicArray {
private:
    allocator_type allocator_;
    T* data_;
    size_t size_;
    size_t capacity_;

public:
    class ForwardIterator {
    public:
        explicit ForwardIterator(T* ptr) : current_(ptr) {}

        inline bool operator==(const ForwardIterator& other) const {
            return current_ == other.current_;
        }

        inline bool operator!=(const ForwardIterator& other) const {
            return !(*this == other);
        }

        inline T& operator*() const {
            return *current_;
        }

        ForwardIterator& operator++() {
            ++current_;
            return *this;
        }

        ForwardIterator operator++(int) {
            ForwardIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        inline T* operator->() const {
            return current_;
        }

    private:
        T* current_;
        friend class DynamicArray;
    };

    explicit DynamicArray(const allocator_type& resource = std::pmr::get_default_resource())
        : allocator_(resource), data_(nullptr), size_(0), capacity_(0) {}

    explicit DynamicArray(size_t sz, std::pmr::memory_resource* resource = std::pmr::get_default_resource())
        : DynamicArray(resource) {
        if (sz > 0) {
            data_ = allocator_.allocate(sz);
            for (size_t i = 0; i < sz; ++i)
                new (data_ + i) T();
    
            size_ = sz;
            capacity_ = sz;
        }
    }

    DynamicArray(const std::initializer_list<T>& ilist, std::pmr::memory_resource* resource = std::pmr::get_default_resource())
        : DynamicArray(resource) {
        size_t n = ilist.size();
        if (n > 0) {
            data_ = allocator_.allocate(n);
            size_t i = 0;
            for (const T& v : ilist) {
                new (data_ + i) T(v);
                ++i;
            }
            size_ = n;
            capacity_ = n;
        }
    }

    DynamicArray(const DynamicArray& other, std::pmr::memory_resource* resource = std::pmr::get_default_resource())
        : DynamicArray(resource) {
        if (other.size_ > 0) {
            data_ = allocator_.allocate(other.size_);
            for (size_t i = 0; i < other.size_; ++i) {
                new (data_ + i) T(other.data_[i]);
            }
            size_ = other.size_;
            capacity_ = other.size_;
        }
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            Clear();
            if (capacity_ < other.size_) {
                if (data_) 
                    allocator_.deallocate(data_, capacity_);
                
                data_ = nullptr;
                capacity_ = 0;
            }
            
            if (other.size_ > capacity_) {
                data_ = allocator_.allocate(other.size_);
                capacity_ = other.size_;
            }
            
            for (size_t i = 0; i < other.size_; ++i) {
                new (data_ + i) T(other.data_[i]);
            }
            size_ = other.size_;
        }
        return *this;
    }

    void PushBack(const T& value) {
        if (size_ == capacity_) {
            Grow(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        new (data_ + size_) T(value);
        ++size_;
    }

    void PopBack() {
        if (IsEmpty()) {
            throw std::runtime_error("Array is empty!");
        }
        
        data_[size_ - 1].~T();
        --size_;
    }

    void Reserve(size_t new_cap) {
        if (new_cap > capacity_) {
            Grow(new_cap);
        }
    }

    void Clear() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    void Swap(DynamicArray& other) {
        std::swap(allocator_, other.allocator_); 
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    T& At(size_t idx) {
        if (idx >= size_) throw std::out_of_range("index out of range");
        return data_[idx];
    }

    const T& At(size_t idx) const {
        if (idx >= size_) throw std::out_of_range("index out of range");
        return data_[idx];
    }

    T& operator[](size_t idx) { return data_[idx]; }
    const T& operator[](size_t idx) const { return data_[idx]; }

    T* Data() { return data_; }
    const T* Data() const { return data_; }

    ForwardIterator Begin() const noexcept { return ForwardIterator(data_); }
    ForwardIterator End() const noexcept { return ForwardIterator(data_ + size_); }

    bool IsEmpty() const noexcept { return size_ == 0; }
    size_t Size() const noexcept { return size_; }
    size_t Capacity() const noexcept { return capacity_; }

    ~DynamicArray() {
        Clear();
        if (data_) {
            allocator_.deallocate(data_, capacity_);
            data_ = nullptr;
            capacity_ = 0;
        }
    }

private:
    void Grow(size_t new_cap) {
        T* new_data = allocator_.allocate(new_cap);

        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(data_[i]);
            data_[i].~T();
        }

        if (data_) {
            allocator_.deallocate(data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_cap;
    }
};
