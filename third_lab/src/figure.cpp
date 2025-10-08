#include "../include/figure.h"
#include <iostream>

RegularPolygon::RegularPolygon(int sides, double radius, std::pair<double, double> center)
    : sides(sides), radius(radius), center(center) {}

RegularPolygon::RegularPolygon(const RegularPolygon& other)
    : sides(other.sides), radius(other.radius), center(other.center) {}

RegularPolygon::RegularPolygon(RegularPolygon&& other) noexcept
    : sides(other.sides), radius(other.radius), center(std::move(other.center)) {
    other.sides = 0; 
    other.radius = 0;
    other.center = {0, 0};
}

std::pair<double, double> RegularPolygon::GeometricCenter() const {
    return center;
}

void RegularPolygon::Print(std::ostream& os) const {
    os << "Polygon with " << sides << " sides, radius " << radius 
       << ", center (" << center.first << ", " << center.second << ")\n";
}

void RegularPolygon::Read(std::istream& is) {
    std::cout << "Enter center (x y): ";
    is >> center.first >> center.second;
    std::cout << "Enter radius: ";
    is >> radius;
}

double RegularPolygon::Area() const {
    return 0.5 * sides * radius * radius * std::sin(2 * M_PI / sides);
}

double RegularPolygon::GetRadius() const {
    return radius;
}

bool RegularPolygon::operator==(const Figure& other) const {
    const RegularPolygon* o = dynamic_cast<const RegularPolygon*>(&other);
    return o && sides == o->sides && radius == o->radius && center == o->center;
}

Figure& RegularPolygon::operator=(const Figure& other) {
    if (this == &other) 
        return *this;
    
    const RegularPolygon* o = dynamic_cast<const RegularPolygon*>(&other);
    if (o) {
        sides = o->sides;
        radius = o->radius;
        center = o->center;
    }
    
    return *this;
}

Figure& RegularPolygon::operator=(Figure&& other) noexcept {
    if (this == &other) 
        return *this;
    
    RegularPolygon* o = dynamic_cast<RegularPolygon*>(&other);
    if (o) {
        sides = o->sides;
        radius = o->radius;
        center = std::move(o->center);

        o->sides = 0; 
        o->radius = 0;
    }
    return *this;
}


Pentagon::Pentagon(double radius) : RegularPolygon(5, radius) {}

Pentagon::Pentagon(const Pentagon& other) : RegularPolygon(other) {}

Pentagon::Pentagon(Pentagon&& other) noexcept : RegularPolygon(std::move(other)) {}

Hexagon::Hexagon(double radius) : RegularPolygon(6, radius) {}

Hexagon::Hexagon(const Hexagon& other) : RegularPolygon(other) {}

Hexagon::Hexagon(Hexagon&& other) noexcept : RegularPolygon(std::move(other)) {}

Octagon::Octagon(double radius) : RegularPolygon(8, radius) {}

Octagon::Octagon(const Octagon& other) : RegularPolygon(other) {}

Octagon::Octagon(Octagon&& other) noexcept : RegularPolygon(std::move(other)) {}