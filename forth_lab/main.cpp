#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include <unordered_map>
#include <functional>

#include "../include/array.h"
#include "../include/nangle.h"

template <typename ShapeType>
std::shared_ptr<Figure<double>> createShape(const std::string& shapeName) {
    std::cout << "Enter coordinates for " << shapeName << ":\n";
    ShapeType shape;

    for (size_t j = 0; j < ShapeType::NUM_POINTS; ++j) {
        double x, y;
        std::cin >> x >> y;
        shape.add_vertex(Point<double>(x, y));
    }

    return std::make_shared<ShapeType>(shape);
}

int main() {
    Array<std::shared_ptr<Figure<double>>> figures;

    std::cout << "Enter number of shapes: ";
    int shape_count;
    std::cin >> shape_count;

    std::unordered_map<int, std::function<std::shared_ptr<Figure<double>>()>> shapeCreators = {
        {5, []() { return createShape<Pentagon<double>>("Pentagon"); }},
        {6, []() { return createShape<Hexagon<double>>("Hexagon"); }},
        {8, []() { return createShape<Octagon<double>>("Octagon"); }}
    };

    for (int i = 0; i < shape_count; ++i) {
        std::cout << "Enter the type of shape (5 for Pentagon, 6 for Hexagon, 8 for Octagon): ";
        int sides;
        std::cin >> sides;

        if (shapeCreators.find(sides) != shapeCreators.end()) {
            figures.PushBack(shapeCreators[sides]());
        } else {
            std::cout << "Invalid number of sides. Please enter 5, 6, or 8." << std::endl;
            --i;
        }
    }

    std::cout << "\nFigures:\n";
    double total_area = 0.0;
    for (size_t i = 0; i < figures.Size(); ++i) {
        std::cout << *figures[i] << "\n";
        double area = figures[i]->area();
        total_area += area;
        std::cout << "Area: " << std::fixed << std::setprecision(2) << area << "\n";
        Point<double> barycenter = figures[i]->get_barycenter();
        std::cout << "Barycenter: " << barycenter << "\n";
    }

    std::cout << "\nTotal area of all figures: " << std::fixed << std::setprecision(2) << total_area << "\n";

    return 0;
}