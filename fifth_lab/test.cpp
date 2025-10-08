#include <thread>
#include <future>
#include <iterator>
#include <string>
#include <gtest/gtest.h>

#include "allocator.cpp"
#include "array.cpp"

class ArrayTest : public testing::Test {
protected:
    void SetUp() override {
        for (int i = 1; i <= 7; ++i) {
            arr.PushBack(i);
        }
        assert(arr.Size() == sz);
    }
    DynamicArray<int> arr;
    const size_t sz = 7;
};

TEST(EmptyArrayTest, DefaultConstructor) {
    DynamicArray<int> a;
    ASSERT_TRUE(a.IsEmpty()) << "Default array isn't empty!";
}

TEST(EmptyArrayTest, PushBackSimple) {
    DynamicArray<int> a;
    a.PushBack(2);
    ASSERT_EQ(a.Size(), 1u);
    ASSERT_EQ(a[0], 2);
}

TEST(EmptyArrayTest, PopBackSimple) {
    DynamicArray<int> a;
    a.PushBack(1);
    a.PushBack(2);
    a.PopBack();

    ASSERT_EQ(a.Size(), 1u);
    ASSERT_EQ(a[0], 1);
}

TEST(EmptyArrayTest, ConstructorSizeDefaultValues) {
    DynamicArray<std::string> a(5);
    ASSERT_EQ(a.Size(), 5u);
    while (a.Size()) {
        ASSERT_TRUE(a[a.Size() - 1].empty());
        a.PopBack();
    }
}

TEST(EmptyArrayTest, ConstructorWithInitList) {
    DynamicArray<int> a{1,2,3,4,5,6,7,8};
    ASSERT_EQ(a.Size(), 8u);
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(a[i], i + 1);
    }
}

TEST(EmptyArrayTest, Swap) {
    DynamicArray<int> a;
    a.PushBack(5);

    DynamicArray<int> b;
    b.PushBack(14);
    b.PushBack(15);

    size_t old_a_size = a.Size();
    size_t old_b_size = b.Size();

    std::swap(a, b);

    ASSERT_EQ(b.Size(), old_a_size);
    ASSERT_EQ(a.Size(), old_b_size);

    ASSERT_EQ(b[0], 5);
    ASSERT_EQ(a[0], 14);
    a.PopBack();
    ASSERT_EQ(a[0], 14); // после PopBack из [14,15] останется 14
}

TEST_F(ArrayTest, CopyConstructor) {
    DynamicArray<int> b = arr;
    ASSERT_NE(&arr, &b) << "Copy constructor must do copy!\n";
    ASSERT_EQ(arr.Size(), b.Size());
    for (size_t i = 0; i < b.Size(); ++i) {
        ASSERT_EQ(arr[i], b[i]);
    }
}

TEST_F(ArrayTest, CopyAssignment) {
    DynamicArray<int> b;
    b.PushBack(4);
    arr = b;
    ASSERT_NE(&arr, &b) << "Copy assignment must do copy!\n";
    ASSERT_EQ(arr.Size(), b.Size());
    for (size_t i = 0; i < b.Size(); ++i) {
        ASSERT_EQ(arr[i], b[i]);
    }
}

TEST_F(ArrayTest, SelfAssignment) {
    std::thread thread([&](){
        arr = arr;
    });
    auto future = std::async(std::launch::async, &std::thread::join, &thread);
    ASSERT_LT(
        future.wait_for(std::chrono::seconds(1)),
        std::future_status::timeout
    ) << "There is infinity loop!\n";
}

TEST_F(ArrayTest, RangeWithIteratorPreFix) {
    ASSERT_EQ(arr.Size(), static_cast<std::ptrdiff_t>(sz))
        << "Distance between begin and end iterators isn't equal to size";
    int expected = 1;
    for (auto it = arr.Begin(); it != arr.End(); ++it) {
        ASSERT_EQ(*it, expected++);
    }
}

TEST_F(ArrayTest, RangeWithIteratorPostFix) {
    ASSERT_EQ(arr.Size(), static_cast<std::ptrdiff_t>(sz))
        << "Distance between begin and end iterators isn't equal to size";
    int expected = 1;
    for (auto it = arr.Begin(); it != arr.End(); it++) {
        ASSERT_EQ(*it, expected++);
    }
}

TEST_F(ArrayTest, ReserveKeepsDataAndSize) {
    for (size_t i = 0; i < arr.Size(); ++i) arr[i] *= 10;
    size_t old_size = arr.Size();
    size_t old_cap = arr.Capacity();
    arr.Reserve(old_cap + 50);
    ASSERT_EQ(arr.Size(), old_size);
    ASSERT_GE(arr.Capacity(), old_cap + 50);
    int expected = 10;
    for (auto it = arr.Begin(); it != arr.End(); ++it) {
        ASSERT_EQ(*it, expected);
        expected += 10;
    }
}

TEST_F(ArrayTest, Clear) {
    arr.Clear();
    ASSERT_TRUE(arr.IsEmpty());
    ASSERT_EQ(arr.Size(), 0u);
}

TEST(CustomAllocatorTest, WorksWithFixedBlockMemoryResource) {
    FixedBlockMemoryResource mem(1024 * 1024);
    std::pmr::polymorphic_allocator<int> alloc(&mem);
    DynamicArray<int, std::pmr::polymorphic_allocator<int>> a(alloc);

    a.PushBack(100);
    a.PushBack(200);
    a.PushBack(300);

    ASSERT_EQ(a.Size(), 3u);
    ASSERT_EQ(a[0], 100);
    ASSERT_EQ(a[1], 200);
    ASSERT_EQ(a[2], 300);

    a.PopBack();
    ASSERT_EQ(a.Size(), 2u);
    ASSERT_EQ(a[1], 200);
}

TEST(BoundsTest, AtThrows) {
    DynamicArray<int> a{1,2,3};
    ASSERT_NO_THROW(a.At(0));
    ASSERT_NO_THROW(a.At(2));
    ASSERT_THROW(a.At(3), std::out_of_range);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
