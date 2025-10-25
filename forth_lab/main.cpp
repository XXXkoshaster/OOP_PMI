#include "include/array.h"
#include "include/figure.h"

int main() {
    Array<Figure<double>*> figures;
    
    figures.pushBack(new Pentagon<double>(5.0, Point<double>(0, 0)));
    figures.pushBack(new Hexagon<double>(3.0, Point<double>(1, 1)));
    figures.pushBack(new Octagon<double>(4.0, Point<double>(2, 2)));
    
    std::cout << "Total figures: " << figures.getSize() << "\n";
    double totalArea = 0;
    for (size_t i = 0; i < figures.getSize(); ++i) {
        std::cout << *figures[i] << "\n";
        std::cout << "Area: " << figures[i]->area() << "\n";
        std::cout << "Center: " << figures[i]->getGeometricCenter() << "\n\n";
        totalArea += figures[i]->area();
    }
    std::cout << "Total area: " << totalArea << "\n\n";
    
    for (size_t i = 0; i < figures.getSize(); ++i) {
        delete figures[i];
    }
    
    Array<Pentagon<double>> pentagons;
    
    pentagons.pushBack(Pentagon<double>(3.0, Point<double>(0, 0)));
    pentagons.pushBack(Pentagon<double>(5.0, Point<double>(5, 5)));
    
    std::cout << "Total pentagons: " << pentagons.getSize() << "\n";
    for (size_t i = 0; i < pentagons.getSize(); ++i) {
        std::cout << pentagons[i] << "\n";
        std::cout << "Area: " << pentagons[i].area() << "\n\n";
    }
    
    pentagons.remove(0);
    std::cout << "After remove: " << pentagons.getSize() << " pentagons\n";
    
    return 0;
}
