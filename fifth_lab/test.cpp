#include <gtest/gtest.h>
#include <string>
#include "allocator.cpp"
#include "array.cpp"

TEST(VectorTest, DefaultConstructor) {
    Vector<int> v;
    ASSERT_TRUE(v.isEmpty());
    ASSERT_EQ(v.getSize(), 0u);
}

TEST(VectorTest, PushBackInt) {
    Vector<int> v;
    v.pushBack(10);
    v.pushBack(20);
    v.pushBack(30);
    
    ASSERT_EQ(v.getSize(), 3u);
    ASSERT_EQ(v[0], 10);
    ASSERT_EQ(v[1], 20);
    ASSERT_EQ(v[2], 30);
}

TEST(VectorTest, PopBack) {
    Vector<int> v;
    v.pushBack(1);
    v.pushBack(2);
    v.popBack();
    
    ASSERT_EQ(v.getSize(), 1u);
    ASSERT_EQ(v[0], 1);
}

TEST(VectorTest, PopBackEmpty) {
    Vector<int> v;
    ASSERT_THROW(v.popBack(), std::runtime_error);
}

TEST(VectorTest, SizeConstructor) {
    Vector<int> v(5);
    ASSERT_EQ(v.getSize(), 5u);
}

TEST(VectorTest, CopyConstructor) {
    Vector<int> v1;
    v1.pushBack(1);
    v1.pushBack(2);
    
    Vector<int> v2(v1);
    ASSERT_EQ(v2.getSize(), 2u);
    ASSERT_EQ(v2[0], 1);
    ASSERT_EQ(v2[1], 2);
}

TEST(VectorTest, CopyAssignment) {
    Vector<int> v1;
    v1.pushBack(1);
    v1.pushBack(2);
    
    Vector<int> v2;
    v2 = v1;
    ASSERT_EQ(v2.getSize(), 2u);
    ASSERT_EQ(v2[0], 1);
    ASSERT_EQ(v2[1], 2);
}

TEST(VectorTest, At) {
    Vector<int> v;
    v.pushBack(10);
    v.pushBack(20);
    
    ASSERT_EQ(v.at(0), 10);
    ASSERT_EQ(v.at(1), 20);
    ASSERT_THROW(v.at(2), std::out_of_range);
}

TEST(VectorTest, Clear) {
    Vector<int> v;
    v.pushBack(1);
    v.pushBack(2);
    v.clear();
    
    ASSERT_TRUE(v.isEmpty());
    ASSERT_EQ(v.getSize(), 0u);
}

TEST(VectorTest, Iterator) {
    Vector<int> v;
    v.pushBack(1);
    v.pushBack(2);
    v.pushBack(3);
    
    int expected = 1;
    for (auto it = v.begin(); it != v.end(); ++it) {
        ASSERT_EQ(*it, expected++);
    }
}

TEST(VectorTest, WithString) {
    Vector<std::string> v;
    v.pushBack("hello");
    v.pushBack("world");
    
    ASSERT_EQ(v.getSize(), 2u);
    ASSERT_EQ(v[0], "hello");
    ASSERT_EQ(v[1], "world");
}

TEST(AllocatorTest, FixedBlockMemoryResource) {
    FixedBlockMemoryResource resource(4096);
    Vector<int> v(&resource);
    
    v.pushBack(100);
    v.pushBack(200);
    v.pushBack(300);
    
    ASSERT_EQ(v.getSize(), 3u);
    ASSERT_EQ(v[0], 100);
    ASSERT_EQ(v[1], 200);
    ASSERT_EQ(v[2], 300);
}

TEST(AllocatorTest, MultipleAllocations) {
    FixedBlockMemoryResource resource(1024);
    Vector<int> v1(&resource);
    Vector<int> v2(&resource);
    
    for (int i = 0; i < 10; ++i) {
        v1.pushBack(i);
        v2.pushBack(i * 10);
    }
    
    ASSERT_EQ(v1.getSize(), 10u);
    ASSERT_EQ(v2.getSize(), 10u);
}

struct ComplexType {
    int id;
    std::string name;
    double value;
    
    ComplexType() : id(0), name(""), value(0.0) {}
    ComplexType(int i, const std::string& n, double v) : id(i), name(n), value(v) {}
};

TEST(VectorTest, ComplexType) {
    FixedBlockMemoryResource resource(4096);
    Vector<ComplexType> v(&resource);
    
    v.pushBack(ComplexType(1, "Alice", 3.14));
    v.pushBack(ComplexType(2, "Bob", 2.71));
    
    ASSERT_EQ(v.getSize(), 2u);
    ASSERT_EQ(v[0].id, 1);
    ASSERT_EQ(v[0].name, "Alice");
    ASSERT_EQ(v[0].value, 3.14);
    ASSERT_EQ(v[1].id, 2);
    ASSERT_EQ(v[1].name, "Bob");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
