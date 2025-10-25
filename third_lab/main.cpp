#include "include/array.h"

int main() {
    Array figures;

    std::cout << "Creating a Pentagon with radius 3:\n";
    figures.pushBack(new Pentagon(3.0));

    std::cout << "Creating a Hexagon with radius 4:\n";
    figures.pushBack(new Hexagon(4.0));

    std::cout << "Creating an Octagon with radius 5:\n";
    figures.pushBack(new Octagon(5.0));

    std::cout << "\nFigures in array:\n";
    for (size_t i = 0; i < figures.getSize(); ++i) {
        std::cout << figures[i];
        Point center = figures[i].geometricCenter();
        std::cout << "Geometric center: (" << center.x << ", " << center.y << ")\n";
        std::cout << "Area: " << static_cast<double>(figures[i]) << "\n\n";
    }

    double totalArea = 0;
    for (size_t i = 0; i < figures.getSize(); ++i)
        totalArea += static_cast<double>(figures[i]);

    std::cout << "Total area: " << totalArea << "\n";

    int index;
    std::cout << "Enter index of figure to delete: ";
    std::cin >> index;
    if (index >= 0 && static_cast<size_t>(index) < figures.getSize()) {
        figures.erase(index);
        std::cout << "Figure at index " << index << " deleted.\n";
    } else {
        std::cout << "Invalid index!\n";
    }

    return 0;
}
