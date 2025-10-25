#include "../include/array.h"

Decimal::Decimal() : size(1), arr(new unsigned char[1]) {
    arr[0] = 0;
}

Decimal::Decimal(const size_t& n, unsigned char t) {
    if (isInvalidDigit(t))
        throw std::invalid_argument("Invslid decimal digit");
    
    size = n;

    if (!size)
        return;

    arr = new unsigned char[size];

    for (size_t i = 0; i < size; i++)
        arr[i] = t;
}

Decimal::Decimal(const std::initializer_list<unsigned char>& t) {
    if (!isValidDecimalInitList(t))
        throw std::invalid_argument("List include invalid digits");
    
    size = t.size();

    if (!size)
        return;

    arr = new unsigned char[size];
    size_t i = 0;
    for (auto it : t)
        arr[i++] = it;
}

Decimal::Decimal(const std::string& t) {
    if (!isValidDecimalString(t))
        throw std::invalid_argument("String include invalid symbols");
    
    size = t.size();

    if (!size)
        return;

    arr = new unsigned char[size];
    for (size_t i = 0; i < size; i++)
        arr[i] = static_cast<unsigned char>(t[size - 1 - i] - '0');
}

Decimal::Decimal(const Decimal& other) : size(other.size), arr(nullptr) {
    if (!size)
        return;

    arr = new unsigned char[size];

    for (size_t i = 0; i < size; i++)
        arr[i] = other.arr[i];

}

Decimal::Decimal(Decimal&& other) noexcept : size(other.size), arr(other.arr) {
    other.size = 0;
    other.arr = nullptr;
}

Decimal& Decimal::operator=(const Decimal& other) {
    if (this == &other)
        return *this;

    if (size != other.size) {
        unsigned char* tmp = new unsigned char[other.size];
        delete[] arr;
        arr = tmp;
        size = other.size;
    }

    std::copy(other.arr, other.arr + other.size, arr);
    return *this;
}

Decimal& Decimal::operator=(Decimal&& other) {
    if (this == &other)
        return *this;

    delete[] arr;
    arr = std::exchange(other.arr, nullptr);
    size = std::exchange(other.size, 0);

    return *this;
}

Decimal& Decimal::operator+=(const Decimal& rhs) {
    if (rhs.size == 0)
        return *this;

    if (size == 0) {
        *this = rhs;
        return *this;
    }

    size_t max_size = std::max(size, rhs.size);
    unsigned char* newarr = new unsigned char[max_size];
    
    for (size_t i = 0; i < max_size; i++)
        newarr[i] = 0;

    unsigned char carry = 0;
    for (size_t i = 0; i < max_size; i++) {
        unsigned char sum = carry;
        if (i < size)
            sum += arr[i];

        if (i < rhs.size)
            sum += rhs.arr[i];
        
        newarr[i] = sum % 10;
        carry = sum / 10;
    }

    if (carry > 0) {
        unsigned char* tmp = new unsigned char[max_size + 1];
        std::copy(newarr, newarr + max_size, tmp);
        tmp[max_size] = carry;
        delete[] newarr;
        newarr = tmp;
        max_size++;
    }

    delete[] arr;
    arr = newarr;
    size = max_size;

    return *this;
}

Decimal& Decimal::operator-=(const Decimal& rhs) {
    if (rhs.size == 0)
        return *this;

    if (size == 0 || *this < rhs)
        throw std::underflow_error("Result would be negative.");

    size_t max_size = std::max(size, rhs.size);
    unsigned char* newarr = new unsigned char[max_size];
    
    for (size_t i = 0; i < max_size; i++)
        newarr[i] = 0;

    int borrow = 0;
    for (size_t i = 0; i < max_size; i++) {
        int diff = borrow;
        if (i < size)
            diff += arr[i];
        if (i < rhs.size)
            diff -= rhs.arr[i];
            
        if (diff < 0) {
            diff += 10;
            borrow = -1;
        } else {
            borrow = 0;
        }

        newarr[i] = static_cast<unsigned char>(diff);
    }

    while (max_size > 1 && newarr[max_size-1] == 0)
        max_size--;

    delete[] arr;
    arr = new unsigned char[max_size];
    std::copy(newarr, newarr + max_size, arr);
    delete[] newarr;
    size = max_size;

    return *this;
}

Decimal operator+(Decimal lhs, const Decimal& rhs) {
    lhs += rhs;
    return lhs;
}

Decimal operator-(Decimal lhs, const Decimal& rhs) {
    lhs -= rhs;
    return lhs;
}


std::ostream& operator<<(std::ostream& os, Decimal& obj) {
    if (obj.size == 0) {
        os << '0';
        return os;
    }

    for (size_t i = obj.size; i > 0; --i)
        os << static_cast<int>(obj.arr[i - 1]);

    return os;
}

bool operator<(const Decimal& lhs, const Decimal& rhs) {
    if (lhs.size != rhs.size)
        return lhs.size < rhs.size;

    for (size_t i = lhs.size; i > 0; --i) {
        size_t idx = i - 1;
        if (lhs.arr[idx] != rhs.arr[idx])
            return lhs.arr[idx] < rhs.arr[idx];
    }

    return false;
}

bool operator>(const Decimal& lhs, const Decimal& rhs) {
    return rhs < lhs;
}

bool operator<=(const Decimal& lhs, const Decimal& rhs) {
    return !(lhs > rhs);
}
bool operator>=(const Decimal& lhs, const Decimal& rhs) {
    return !(lhs < rhs);
}

bool operator==(const Decimal& lhs, const Decimal& rhs) {
    if (lhs.size != rhs.size)
        return false;

    for (size_t i = 0; i < lhs.size; i++) {
        if (lhs.arr[i] != rhs.arr[i])
            return false;
    }

    return true;
}

bool operator!=(const Decimal& lhs, const Decimal& rhs) {
    return !(lhs == rhs);
}

Decimal::~Decimal() noexcept {
    delete[] arr;
    arr = nullptr;
    size = 0;
}

bool Decimal::isInvalidDigit(unsigned char c) {
    return c > 9;
}

bool Decimal::isValidDecimalInitList(const std::initializer_list<unsigned char>& lst) {
    for (unsigned char c : lst) {
        if (isInvalidDigit(c))
            return false;
    }

    return true;
}

bool Decimal::isValidDecimalString(const std::string& str) {
    if (str.empty())
        return false;
        
    for (char c : str) {
        if (c < '0' || c > '9')
            return false;
    }

    return true;
}

size_t Decimal::getSize() const {
    return size;
}

std::string Decimal::toString() const {
    if (size == 0)
        return std::string("0");

    std::string str;
    
    size_t start = size;
    while (start > 1 && arr[start - 1] == 0)
        --start;
    
    for (size_t i = start; i > 0; --i)
         str += static_cast<char>(arr[i - 1] + '0');
    
    return str.empty() ? "0" : str;
}
