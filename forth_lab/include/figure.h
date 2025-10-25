#ifndef FIGURE_H
#define FIGURE_H

#include <sstream>
#include <iostream>
#include <cmath>
#include <concepts>
#include <vector>
#include <memory>

template <typename T>
concept Scalar = std::is_arithmetic_v<T>;

template <Scalar T>
class Point {
    private:
        T x, y;
    public:
        Point() : x(0), y(0) {}
        Point(T x, T y) :  x(x), y(y) {}
        Point(const Point& other) : x(other.x), y(other.y) {}
        Point(Point&& other) : x(std::exchange(other.x, 0)), y(std::exchange(other.y, 0)) {}

        Point& operator=(const Point& other) {
            if (this != &other) {
                x = other.x;
                y = other.y;
            }
            return *this;
        }

        Point& operator=(Point&& other) {
            if (this != &other) {
                x = std::exchange(other.x, 0);
                y = std::exchange(other.y, 0);
            }
            return *this;
        }

        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }

        friend std::ostream& operator<<(std::ostream& os, const Point<T>& p) {
            os << "(" << p.x << ", " << p.y << ")";
            return os;
        }

        friend std::istream& operator>>(std::istream& is, Point<T>& p) {
            is >> p.x >> p.y;
            return is;
        }

        T get_x() const {
            return x;
        }

        T get_y() const {
            return y;
        }

        ~Point() = default;
};

template <Scalar T>
class Figure {
    public:
        virtual ~Figure() = default;

        virtual Point<T> getGeometricCenter() const = 0;
        virtual double area() const = 0;
        virtual void print(std::ostream& os) const = 0;
        virtual void read(std::istream& is) = 0;

        virtual bool operator==(const Figure<T>& other) const = 0;
        virtual Figure<T>& operator=(const Figure<T>& other) = 0;
        virtual Figure<T>& operator=(Figure<T>&& other)  = 0;

        friend std::ostream& operator<<(std::ostream& os, const Figure<T>& f) {
            f.print(os);
            return os;
        }

        friend std::istream& operator>>(std::istream& is, Figure<T>& f) {
            f.read(is);
            return is;
        }

        operator double() const {
            return area();
        }

    protected:
        std::vector<std::unique_ptr<Point<T>>> vertices;
};

template <Scalar T>
class RegularFigure : public Figure<T> {
    private:
        int sides;
        double radius;
        Point<T> center;

        void calculate_vertices() {
            this->vertices.clear();
            for (int i = 0; i < sides; ++i) {
                double angle = 2 * M_PI * i / sides;
                T x = center.get_x() + radius * std::cos(angle);
                T y = center.get_y() + radius * std::sin(angle);
                this->vertices.push_back(std::make_unique<Point<T>>(x, y));
            }
        }
    public:
        RegularFigure() : sides(0), radius(0.0), center((0, 0)) {}

        RegularFigure(double radius, int sides, Point<T> center) 
            : radius(radius), sides(sides), center(center) {
                calculate_vertices();
            };
        
        RegularFigure(const RegularFigure& other)
            : radius(other.radius), sides(other.sides), center(other.center) {
                calculate_vertices();
            };
        
        RegularFigure (RegularFigure&& other)
            : radius(std::exchange(other.radius, 0.0)),
              sides(std::exchange(other.sides, 0)),
              center(std::exchange(other.center, Point<T>(0.0, 0.0))) {
                this->vertices = std::move(other.vertices);
              };

        RegularFigure& operator=(const RegularFigure& other) {
            if (this != &other) {
                radius = other.radius;
                sides = other.sides;
                center = other.center;
                calculate_vertices();
            }
            return *this;
        }

        RegularFigure& operator=(RegularFigure&& other) {
            if (this != &other) {
                radius = std::exchange(other.radius, 0.0);
                sides = std::exchange(other.sides, 0);
                center = std::exchange(other.center, Point<T>(0.0, 0.0));
                this->vertices = std::move(other.vertices);
            }
            return *this;
        }

        Figure<T>& operator=(const Figure<T>& other) override {
            if (this != &other) {
                const RegularFigure<T>& rf_other = dynamic_cast<const RegularFigure<T>&>(other);
                radius = rf_other.radius;
                sides = rf_other.sides;
                center = rf_other.center;
                calculate_vertices();
            }
            return *this;
        }

        Figure<T>& operator=(Figure<T>&& other) override {
            if (this != &other) {
                RegularFigure<T>& rf_other = dynamic_cast<RegularFigure<T>&>(other);
                radius = std::exchange(rf_other.radius, 0.0);
                sides = std::exchange(rf_other.sides, 0);
                center = std::exchange(rf_other.center, Point<T>(0.0, 0.0));
                this->vertices = std::move(rf_other.vertices);
            }

            return *this;
        }

        bool operator==(const Figure<T>& other) const override {
            const RegularFigure<T>& rf_other = dynamic_cast<const RegularFigure<T>&>(other);
            return radius == rf_other.radius && sides == rf_other.sides && center == rf_other.center;
        }
        
        Point<T> getGeometricCenter() const override {
            return center;
        }

        double area() const override {
            return (sides * radius * radius * std::sin((2 * M_PI) / sides)) / 2;
        }

        void read(std::istream& is) override {
            std::cout << "Enter radius: ";
            is >> radius;
            std::cout << "Enter center (x y): ";
            is >> center;
            calculate_vertices();
        }

        void print(std::ostream& os) const override {
            os << "Regular " << sides << "-gon: center=" << center << ", radius=" << radius << "\nVertices: ";
            for (const auto& v : this->vertices) {
                os << *v << " ";
            }
        }
};

template <Scalar T>
class Pentagon : public RegularFigure<T> {
    public:
        Pentagon(double radius = 0.0, Point<T> center = Point<T>(0, 0))
            : RegularFigure<T>(radius, 5, center) {}

        Pentagon(const Pentagon<T>& other)
            : RegularFigure<T>(other) {}

        Pentagon(Pentagon<T>&& other)
            : RegularFigure<T>(std::move(other)) {}

        Pentagon& operator=(const Pentagon& other) {
            RegularFigure<T>::operator=(other);
            return *this;
        }

        Pentagon& operator=(Pentagon&& other) {
            RegularFigure<T>::operator=(std::move(other));
            return *this;
        }
};

template <Scalar T>
class Hexagon : public RegularFigure<T> {
    public:
        Hexagon(double radius = 0.0, Point<T> center = Point<T>(0, 0))
            : RegularFigure<T>(radius, 6, center) {}

        Hexagon(const Hexagon<T>& other)
            : RegularFigure<T>(other) {}

        Hexagon(Hexagon<T>&& other)
            : RegularFigure<T>(std::move(other)) {}

        Hexagon& operator=(const Hexagon& other) {
            RegularFigure<T>::operator=(other);
            return *this;
        }

        Hexagon& operator=(Hexagon&& other) {
            RegularFigure<T>::operator=(std::move(other));
            return *this;
        }
};

template <Scalar T>
class Octagon : public RegularFigure<T> {
    public:
        Octagon(double radius = 0.0, Point<T> center = Point<T>(0, 0))
            : RegularFigure<T>(radius, 8, center) {}

        Octagon(const Octagon<T>& other)
            : RegularFigure<T>(other) {}

        Octagon(Octagon<T>&& other)
            : RegularFigure<T>(std::move(other)) {}

        Octagon& operator=(const Octagon& other) {
            RegularFigure<T>::operator=(other);
            return *this;
        }

        Octagon& operator=(Octagon&& other) {
            RegularFigure<T>::operator=(std::move(other));
            return *this;
        }
};

#endif