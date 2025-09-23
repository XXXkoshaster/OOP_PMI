#include <iostream>
#include <vector>
#include <memory>
#include "../include/figure.h"

int main() {
    std::vector<std::shared_ptr<Figure>> figures;

    std::cout << "Creating a Pentagon with radius 3:\n";
    auto pentagon = std::make_shared<Pentagon>(3.0);
    figures.push_back(pentagon);

    std::cout << "Creating a Hexagon with radius 4:\n";
    auto hexagon = std::make_shared<Hexagon>(4.0);
    figures.push_back(hexagon);

    std::cout << "Creating an Octagon with radius 5:\n";
    auto octagon = std::make_shared<Octagon>(5.0);
    figures.push_back(octagon);

    std::cout << "\nFigures in array:\n";
    for (const auto& fig : figures) {
        std::cout << *fig;
        std::cout << "Geometric center: (" << fig->GeometricCenter().first << ", " << fig->GeometricCenter().second << ")\n";
        std::cout << "Area: " << static_cast<double>(*fig) << "\n\n";
    }

    double totalArea = 0;
    for (const auto& fig : figures) {
        totalArea += static_cast<double>(*fig);
    }
    std::cout << "Total area: " << totalArea << "\n";

    int index;
    std::cout << "Enter index of figure to delete: ";
    std::cin >> index;
    if (index >= 0 && index < figures.size()) {
        figures.erase(figures.begin() + index);
        std::cout << "Figure at index " << index << " deleted.\n";
    } else {
        std::cout << "Invalid index!\n";
    }

    return 0;
}
