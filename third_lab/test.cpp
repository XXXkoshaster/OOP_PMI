#include <gtest/gtest.h>
#include "../include/figure.h"

TEST(FigureTest, PentagonProperties) {
    Pentagon pentagon(3.0);
    Point center = pentagon.geometricCenter();
    EXPECT_EQ(center.x, 0.0);
    EXPECT_EQ(center.y, 0.0);
    EXPECT_NEAR(pentagon.area(), 21.3987, 0.0001);
}

TEST(FigureTest, HexagonProperties) {
    Hexagon hexagon(4.0);
    Point center = hexagon.geometricCenter();
    EXPECT_EQ(center.x, 0.0);
    EXPECT_EQ(center.y, 0.0);
    EXPECT_NEAR(hexagon.area(), 41.5692, 0.0001);
}

TEST(FigureTest, OctagonProperties) {
    Octagon octagon(5.0);
    Point center = octagon.geometricCenter();
    EXPECT_EQ(center.x, 0.0);
    EXPECT_EQ(center.y, 0.0);
    EXPECT_NEAR(octagon.area(), 70.7106, 0.0001);
}

TEST(FigureTest, PentagonInputOutput) {
    Pentagon pentagon(1.0);
    std::stringstream ss;
    ss << pentagon;
    EXPECT_EQ(ss.str(), "Polygon with 5 sides, radius 1, center (0, 0)\n");

    ss.str("");
    ss << "1 1\n2\n";
    ss >> pentagon;
    Point center = pentagon.geometricCenter();
    EXPECT_EQ(center.x, 1.0);
    EXPECT_EQ(center.y, 1.0);
}

TEST(FigureTest, PentagonCopyMove) {
    Pentagon pentagon1(3.0);
    Pentagon pentagon2(pentagon1);
    EXPECT_EQ(pentagon1, pentagon2);

    Pentagon pentagon3(std::move(pentagon1));
    EXPECT_NEAR(pentagon3.area(), 21.3987, 0.0001);
}

TEST(FigureTest, HexagonCopyMove) {
    Hexagon hexagon1(4.0);
    Hexagon hexagon2(hexagon1);
    EXPECT_EQ(hexagon1, hexagon2);

    Hexagon hexagon3(std::move(hexagon1));
    EXPECT_NEAR(hexagon3.area(), 41.5692, 0.0001);
}

TEST(FigureTest, OctagonCopyMove) {
    Octagon octagon1(5.0);
    Octagon octagon2(octagon1);
    EXPECT_EQ(octagon1, octagon2);

    Octagon octagon3(std::move(octagon1));
    EXPECT_NEAR(octagon3.area(), 70.7106, 0.0001);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 