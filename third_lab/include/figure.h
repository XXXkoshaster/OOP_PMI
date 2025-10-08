#include <utility>
#include <ostream>
#include <istream>

class Figure {
public:
    virtual ~Figure() = default;

    virtual std::pair<double, double> GeometricCenter() const = 0;
    virtual void Print(std::ostream& os) const = 0;
    virtual void Read(std::istream& is) = 0;
    virtual double Area() const = 0;
    virtual double GetRadius() const = 0;

    virtual bool operator==(const Figure& other) const = 0;
    virtual Figure& operator=(const Figure& other) = 0;
    virtual Figure& operator=(Figure&& other) noexcept = 0;

    friend std::ostream& operator<<(std::ostream& os, const Figure& fig) {
        fig.Print(os);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Figure& fig) {
        fig.Read(is);
        return is;
    }

    operator double() const {
        return Area();
    }
};

class RegularPolygon : public Figure {
protected:
    std::pair<double, double> center;  
    double radius;                     
    int sides;                         

public:
    RegularPolygon(int sides = 3, double radius = 1.0, std::pair<double, double> center = {0, 0});
    RegularPolygon(const RegularPolygon& other); 
    RegularPolygon(RegularPolygon&& other) noexcept;
    std::pair<double, double> GeometricCenter() const override;
    void Print(std::ostream& os) const override;
    void Read(std::istream& is) override;
    double Area() const override;
    double GetRadius() const override;

    bool operator==(const Figure& other) const override;
    Figure& operator=(const Figure& other) override;
    Figure& operator=(Figure&& other) noexcept override; 
};

class Pentagon : public RegularPolygon {
public:
    Pentagon(double radius = 1.0);
    Pentagon(const Pentagon& other); 
    Pentagon(Pentagon&& other) noexcept; 
};


class Hexagon : public RegularPolygon {
public:
    Hexagon(double radius = 1.0);
    Hexagon(const Hexagon& other); 
    Hexagon(Hexagon&& other) noexcept; 
};

class Octagon : public RegularPolygon {
public:
    Octagon(double radius = 1.0); 
    Octagon(const Octagon& other); 
    Octagon(Octagon&& other) noexcept; 
};