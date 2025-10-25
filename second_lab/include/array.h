#include <utility>
#include <string>
#include <iostream>

class Decimal {
    public:
        Decimal();
        Decimal(const size_t& n, unsigned char t);
        Decimal(const std::initializer_list<unsigned char>& t);
        Decimal(const std::string& t);

        Decimal(const Decimal& other);
        Decimal(Decimal&& other) noexcept;

        Decimal& operator=(const Decimal& other);
        Decimal& operator=(Decimal&& other);

        friend Decimal operator+(Decimal lhs, const Decimal& rhs);
        friend Decimal operator-(Decimal lhs, const Decimal& rhs);
        Decimal& operator+=(const Decimal& rhs);
        Decimal& operator-=(const Decimal& rhs);

        friend std::istream& operator>>(std::istream& is, Decimal& obj);
        friend std::ostream& operator<<(std::ostream& os, Decimal& obj);

        friend bool operator<(const Decimal& lhs, const Decimal& rhs);
        friend bool operator>(const Decimal& lhs, const Decimal& rhs);
        friend bool operator<=(const Decimal& lhs, const Decimal& rhs);
        friend bool operator>=(const Decimal& lhs, const Decimal& rhs);
        friend bool operator==(const Decimal& lhs, const Decimal& rhs);
        friend bool operator!=(const Decimal& lhs, const Decimal& rhs);
        
        ~Decimal() noexcept;

        size_t getSize() const;
        std::string toString() const;

    protected:
        unsigned char* arr = nullptr;
        size_t size;

    private:
        bool isInvalidDigit(unsigned char c);  
        bool isValidDecimalInitList(const std::initializer_list<unsigned char> &lst);  
        bool isValidDecimalString(const std::string &str); 

};