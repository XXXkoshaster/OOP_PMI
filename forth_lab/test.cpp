#include <gtest/gtest.h>
#include "include/array.h"
#include "include/figure.h"

TEST(ArrayTest, PushBackAndSize) {
    Array<int> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    EXPECT_EQ(arr.getSize(), 3);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(ArrayTest, RemoveElement) {
    Array<int> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    arr.remove(1);
    EXPECT_EQ(arr.getSize(), 2);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 3);
}

TEST(ArrayTest, CopyConstructor) {
    Array<int> arr1;
    arr1.pushBack(1);
    arr1.pushBack(2);
    Array<int> arr2(arr1);
    EXPECT_EQ(arr2.getSize(), 2);
    EXPECT_EQ(arr2[0], 1);
    EXPECT_EQ(arr2[1], 2);
}

TEST(PointTest, EqualityOperator) {
    Point<int> p1(3, 4);
    Point<int> p2(3, 4);
    Point<int> p3(5, 6);
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
}

TEST(PointTest, Getters) {
    Point<double> p(3.5, 4.5);
    EXPECT_DOUBLE_EQ(p.get_x(), 3.5);
    EXPECT_DOUBLE_EQ(p.get_y(), 4.5);
}

TEST(PentagonTest, AreaCalculation) {
    Pentagon<double> pentagon(5.0, Point<double>(0, 0));
    double expected = (5 * 5.0 * 5.0 * std::sin((2 * M_PI) / 5)) / 2;
    EXPECT_NEAR(pentagon.area(), expected, 0.001);
}

TEST(PentagonTest, GeometricCenter) {
    Pentagon<double> pentagon(5.0, Point<double>(3, 4));
    Point<double> center = pentagon.getGeometricCenter();
    EXPECT_DOUBLE_EQ(center.get_x(), 3.0);
    EXPECT_DOUBLE_EQ(center.get_y(), 4.0);
}

TEST(HexagonTest, AreaCalculation) {
    Hexagon<double> hexagon(3.0, Point<double>(0, 0));
    double expected = (6 * 3.0 * 3.0 * std::sin((2 * M_PI) / 6)) / 2;
    EXPECT_NEAR(hexagon.area(), expected, 0.001);
}

TEST(OctagonTest, AreaCalculation) {
    Octagon<double> octagon(4.0, Point<double>(0, 0));
    double expected = (8 * 4.0 * 4.0 * std::sin((2 * M_PI) / 8)) / 2;
    EXPECT_NEAR(octagon.area(), expected, 0.001);
}

TEST(FigureTest, CopyAssignment) {
    Pentagon<double> p1(5.0, Point<double>(0, 0));
    Pentagon<double> p2(3.0, Point<double>(1, 1));
    p2 = p1;
    EXPECT_DOUBLE_EQ(p2.area(), p1.area());
    EXPECT_EQ(p2.getGeometricCenter().get_x(), p1.getGeometricCenter().get_x());
}

TEST(FigureTest, MoveAssignment) {
    Pentagon<double> p1(5.0, Point<double>(0, 0));
    double area = p1.area();
    Pentagon<double> p2(3.0, Point<double>(1, 1));
    p2 = std::move(p1);
    EXPECT_NEAR(p2.area(), area, 0.001);
}

TEST(ArrayTest, ArrayOfFigurePointers) {
    Array<Figure<double>*> figures;
    figures.pushBack(new Pentagon<double>(5.0, Point<double>(0, 0)));
    figures.pushBack(new Hexagon<double>(3.0, Point<double>(1, 1)));
    EXPECT_EQ(figures.getSize(), 2);
    EXPECT_GT(figures[0]->area(), 0);
    EXPECT_GT(figures[1]->area(), 0);
    delete figures[0];
    delete figures[1];
}

TEST(ArrayTest, ArrayOfPentagons) {
    Array<Pentagon<double>> pentagons;
    pentagons.pushBack(Pentagon<double>(3.0, Point<double>(0, 0)));
    pentagons.pushBack(Pentagon<double>(5.0, Point<double>(5, 5)));
    EXPECT_EQ(pentagons.getSize(), 2);
    pentagons.remove(0);
    EXPECT_EQ(pentagons.getSize(), 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
