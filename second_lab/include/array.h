#ifndef Decimal_H
#define Decimal_H

#include <iostream>
#include <initializer_list>
#include <string>
#include <stdexcept>
#include <cstring>
#include <algorithm>

class Decimal 
{
public: 
    Decimal();
    Decimal(const size_t & n, unsigned char t = 0);
    Decimal(const std::initializer_list<unsigned char> &t);
    Decimal(const std::string &t);
    Decimal(const Decimal& other);
    Decimal(Decimal&& other) noexcept;
    virtual ~Decimal() noexcept;
    
    Decimal& operator=(const Decimal& other);
    Decimal& operator=(Decimal&& other) noexcept;
    
    Decimal& operator+=(const Decimal& other);
    Decimal& operator-=(const Decimal& other);
    
    friend Decimal operator+(const Decimal &a, const Decimal &b);
    friend Decimal operator-(const Decimal &a, const Decimal &b);

    friend bool operator==(const Decimal &a, const Decimal &b);
    friend bool operator!=(const Decimal &a, const Decimal &b);
    friend bool operator<(const Decimal &a, const Decimal &b);
    friend bool operator>(const Decimal &a, const Decimal &b);
    friend bool operator<=(const Decimal &a, const Decimal &b);
    friend bool operator>=(const Decimal &a, const Decimal &b);

    friend std::ostream &operator<<(std::ostream &os, const Decimal &num);

    size_t getSize() const;
    std::string toString() const;

private:
    bool isInvalidDigit(unsigned char c);  
    bool isValidDecimalInitList(const std::initializer_list<unsigned char> &lst);  
    bool isValidDecimalString(const std::string &str); 

private:
    size_t _sz;
    unsigned char *_arr;
};

#endif
