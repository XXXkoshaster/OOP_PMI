#include <iostream>
#include <memory_resource>
#include <list>
#include <stdexcept>
#include <new>
#include <cstring>
#include <iterator>

class FixedBlockMemoryResource : public std::pmr::memory_resource {
public:
    explicit FixedBlockMemoryResource(size_t total_bytes)
        : buffer_(nullptr), total_(total_bytes)
    {
        buffer_ = static_cast<char*>(::operator new(total_));
        Block b;
        b.ptr = buffer_;
        b.size = total_;
        b.free = true;
        blocks_.push_back(b);
    }

    ~FixedBlockMemoryResource() {
        ::operator delete(buffer_);
        buffer_ = nullptr;
        blocks_.clear();
    }

protected:
    void* do_allocate(size_t bytes, size_t alignment) override {
        if (bytes == 0) bytes = 1;
        if (alignment == 0) alignment = alignof(std::max_align_t);

        for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
            if (!it->free) continue;

            char* block_begin = it->ptr;
            size_t space = it->size;
            void* aligned_ptr = block_begin;
            size_t space_copy = space;

            if (std::align(alignment, bytes, aligned_ptr, space_copy)) {
                char* aligned_char = static_cast<char*>(aligned_ptr);
                size_t head_size = static_cast<size_t>(aligned_char - block_begin);
                size_t tail_size = space_copy >= bytes ? (space_copy - bytes) : 0;

                if (head_size > 0) {
                    Block head;
                    head.ptr = block_begin;
                    head.size = head_size;
                    head.free = true;
                    it = blocks_.insert(it, head);
                    ++it;
                    it->ptr = aligned_char;
                    it->size = bytes + tail_size;
                    it->free = true;
                }

                it->free = false;
                size_t old_size = it->size;
                it->size = bytes;

                size_t remain = old_size - bytes;
                if (remain > 0) {
                    Block tail;
                    tail.ptr = it->ptr + bytes;
                    tail.size = remain;
                    tail.free = true;
                    blocks_.insert(std::next(it), tail);
                }

                return it->ptr;
            }
        }

        throw std::bad_alloc();
    }

    void do_deallocate(void* p, size_t /*bytes*/, size_t /*alignment*/) override {
        if (!p) return;
        char* cp = static_cast<char*>(p);

        for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
            if (it->ptr == cp) {
                it->free = true;

                if (it != blocks_.begin()) {
                    auto prev = std::prev(it);
                    if (prev->free && prev->ptr + prev->size == it->ptr) {
                        prev->size += it->size;
                        it = blocks_.erase(it);
                        it = prev;
                    }
                }

                auto next = std::next(it);
                if (next != blocks_.end()) {
                    if (next->free && it->ptr + it->size == next->ptr) {
                        it->size += next->size;
                        blocks_.erase(next);
                    }
                }

                return;
            }
        }

    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:
    struct Block {
        char* ptr;
        size_t size;
        bool free;
    };

    char* buffer_;
    size_t total_;
    std::list<Block> blocks_;
};