#include "../include/figure.h"

Figure& RegularFigure::operator=(const Figure& other) {
    if (this == &other)
        return *this;

    const RegularFigure* o = dynamic_cast<const RegularFigure*>(&other);
    if (o) {
        radius = o->radius;
        sides = o->sides;
        center = o->center;
    }

    return *this;
}

Figure& RegularFigure::operator=(Figure&& other) {
    if (this == &other)
        return *this;

    RegularFigure* o = dynamic_cast<RegularFigure*>(&other);
    if (o) {
        radius = std::exchange(o->radius, 0.0);
        sides = std::exchange(o->sides, 0);
        center = std::exchange(o->center, Point{});
    }

    return *this;
}

bool RegularFigure::operator==(const Figure& other) const {
    const RegularFigure* o = dynamic_cast<const RegularFigure*>(&other);
    if (!o)
        return false;

    return (radius == o->radius) && (sides == o->sides) && (center == o->center);
}

double RegularFigure::area() const {
    return (sides * radius * radius * std::sin((2 * M_PI) / sides)) / 2.0;
}

void RegularFigure::print(std::ostream& os) const {
    os << "Polygon with " << sides << " sides, radius " << radius 
       << ", center (" << center.x << ", " << center.y << ")\n";
}

void RegularFigure::read(std::istream& is) {
    std::cout << "Enter center (x y): ";
    is >> center.x >> center.y;
    std::cout << "Enter radius: ";
    is >> radius;
}

