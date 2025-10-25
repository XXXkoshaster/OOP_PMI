#include <sstream>
#include <iostream>
#include <cmath>

struct Point {
    double x{0.0}, y{0.0};

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};


class Figure {
    public:
        virtual ~Figure() = default;

        virtual Point geometricCenter() const = 0;
        virtual double area() const = 0;
        virtual void print(std::ostream& os) const = 0;
        virtual void read(std::istream& is) = 0;

        virtual bool operator==(const Figure& other) const = 0;
        virtual Figure& operator=(const Figure& other) = 0;
        virtual Figure& operator=(Figure&& other)  = 0;

        friend std::ostream& operator<<(std::ostream& os, const Figure& f) {
            f.print(os);
            return os;
        }

        friend std::istream& operator>>(std::istream& is, Figure& f) {
            f.read(is);
            return is;
        }

        operator double() const {
            return area();
        }
};

class RegularFigure : public Figure {
    private:
        double radius;
        int sides;
        Point center;
    public:
        RegularFigure(double radius, int sides, Point center = Point{}) 
            : radius(radius), sides(sides), center(center) {};
        
        RegularFigure(const RegularFigure& other)
            : radius(other.radius), sides(other.sides), center(other.center) {};
        
        RegularFigure (RegularFigure&& other)
            : radius(std::exchange(other.radius, 0.0)),
              sides(std::exchange(other.sides, 0)),
              center(std::exchange(other.center, Point{})) {};

        Figure& operator=(const Figure& other) override;

        Figure& operator=(Figure&& other) override;

        bool operator==(const Figure& other) const override;
        
        Point geometricCenter() const override {
            return center;
        }

        double area() const override;

        void read(std::istream& is) override;
        void print(std::ostream& os) const override;
};

class Pentagon : public RegularFigure {
    public:
        Pentagon(double radius, Point center = {})
            : RegularFigure(radius, 5, center) {};

        Pentagon(const Pentagon& other)
            : RegularFigure(other) {};

        Pentagon(Pentagon&& other)
            : RegularFigure(std::move(other)) {};
};

class Hexagon : public RegularFigure {
    public:
        Hexagon(double radius, Point center = {})
            : RegularFigure(radius, 6, center) {};

        Hexagon(const Hexagon& other)
            : RegularFigure(other) {};

        Hexagon(Hexagon&& other)
            : RegularFigure(std::move(other)) {};
};

class Octagon : public RegularFigure {
    public:
        Octagon(double radius, Point center = {})
            : RegularFigure(radius, 8, center) {};

        Octagon(const Octagon& other)
            : RegularFigure(other) {};

        Octagon(Octagon&& other)
            : RegularFigure(std::move(other)) {};
};