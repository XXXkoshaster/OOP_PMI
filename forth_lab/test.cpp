#include <gtest/gtest.h>
#include "../include/nangle.h"
#include "../include/array.h"

TEST(ArrayTest, ReserveMemory) {
    Array<int> arr;
    arr.Reserve(10);
    EXPECT_EQ(arr.Capacity(), 10);
    EXPECT_EQ(arr.Size(), 0);
}

TEST(ArrayTest, PushBackAndResize) {
    Array<int> arr;
    for (int i = 0; i < 10; ++i) {
        arr.PushBack(i);
    }
    EXPECT_EQ(arr.Size(), 10);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(arr[i], i);
    }
}

TEST(ArrayTest, InsertAtBeginning) {
    Array<int> arr;
    arr.PushBack(2);
    arr.PushBack(3);
    arr.Insert(0, 1);
    EXPECT_EQ(arr.Size(), 3);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(ArrayTest, EraseElements) {
    Array<int> arr{1, 2, 3, 4, 5};
    arr.Erase(1, 4);
    EXPECT_EQ(arr.Size(), 2);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 5);
}

TEST(ArrayTest, ClearArray) {
    Array<int> arr{1, 2, 3};
    arr.Clear();
    EXPECT_TRUE(arr.IsEmpty());
    EXPECT_EQ(arr.Size(), 0);
}

TEST(PointTest, EqualityOperator) {
    Point<int> p1(3, 4);
    Point<int> p2(3, 4);
    Point<int> p3(5, 6);
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
}

TEST(FigureTest, FigureCopyAssignment) {
    NAngle<int, 3> fig1{{0, 0}, {1, 1}, {2, 0}};
    NAngle<int, 3> fig2;
    fig2 = fig1;
    EXPECT_EQ(fig2.get_size(), fig1.get_size());
    EXPECT_EQ(fig2.get_barycenter().get_x(), fig1.get_barycenter().get_x());
}


TEST(PentagonTest, PentagonInitializationAndArea) {
    Pentagon<double> pentagon{{0, 0}, {5, 0}, {6, 4}, {3, 5}, {0, 4}};
    EXPECT_NEAR(pentagon.area(), 43.011, 0.001);
}

TEST(HexagonTest, HexagonBarycenter) {
    Hexagon<double> hexagon{{0, 0}, {2, 0}, {3, 2}, {2, 4}, {0, 4}, {-1, 2}};
    Point<double> barycenter = hexagon.get_barycenter();
    EXPECT_DOUBLE_EQ(barycenter.get_x(), 1.0);
    EXPECT_DOUBLE_EQ(barycenter.get_y(), 2.0);
}

TEST(ArrayTest, InsertOutOfBounds) {
    Array<int> arr{1, 2, 3};
    EXPECT_NO_THROW(arr.Insert(0, 0));
    EXPECT_THROW(arr.Insert(5, 10), std::out_of_range);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}