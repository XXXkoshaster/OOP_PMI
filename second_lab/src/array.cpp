#include "../include/array.h"

Decimal::Decimal() : _sz(1), _arr(new unsigned char[1]{0}) {}

Decimal::Decimal(const size_t& n, unsigned char t) {
    if (isInvalidDigit(t)) {
        throw std::invalid_argument("Invslid decimal digit");
    }
    _sz = n;
    if (_sz == 0) {
        _arr = nullptr;
        return;
    }
    _arr = new unsigned char[_sz];
    
    std::memset(_arr, t, _sz);
}

Decimal::Decimal(const std::initializer_list<unsigned char>& lst) {
    if (!isValidDecimalInitList(lst)) {
        throw std::invalid_argument("List include invalid digits");
    }
    _sz = lst.size();
    if (_sz == 0) {
        _arr = nullptr;
        return;
    }
    _arr = new unsigned char[_sz];
    std::copy(lst.begin(), lst.end(), _arr);
}

Decimal::Decimal(const std::string& str) {
    if (!isValidDecimalString(str)) {
        throw std::invalid_argument("String include invalid symbols");
    }
    _sz = str.size();
    if (_sz == 0) {
        _arr = nullptr;
        return;
    }
    _arr = new unsigned char[_sz];
    
    for (size_t i = 0; i < _sz; ++i) {
        _arr[i] = static_cast<unsigned char>(str[_sz - 1 - i] - '0');
    }
}

Decimal::Decimal(const Decimal& other) : _sz(other._sz), _arr(nullptr) {
    if (_sz == 0) { _arr = nullptr; return; }
    _arr = new unsigned char[_sz];
    std::memcpy(_arr, other._arr, _sz * sizeof(unsigned char));
}

Decimal::Decimal(Decimal&& other) noexcept : _sz(other._sz), _arr(other._arr) {
    other._sz = 0;
    other._arr = nullptr;
}

Decimal::~Decimal() noexcept {
    delete[] _arr;
}


Decimal& Decimal::operator=(const Decimal& other) {
    if (this != &other) {
        delete[] _arr;
        _sz = other._sz;
        if (_sz == 0) {
            _arr = nullptr;
        } else {
            _arr = new unsigned char[_sz];
            std::memcpy(_arr, other._arr, _sz * sizeof(unsigned char));
        }
    }
    return *this;
}

Decimal& Decimal::operator=(Decimal&& other) noexcept {
    if (this != &other) {
        delete[] _arr;
        _sz = other._sz;
        _arr = other._arr;
        other._sz = 0;
        other._arr = nullptr;
    }
    return *this;
}


Decimal& Decimal::operator+=(const Decimal& other) {
    *this = *this + other;
    return *this;
}

Decimal& Decimal::operator-=(const Decimal& other) {
    *this = *this - other;
    return *this;
}


Decimal operator+(const Decimal& a, const Decimal& b) {
    size_t max_sz = std::max(a._sz, b._sz);
    
    unsigned char *result = new unsigned char[max_sz + 1];
    std::memset(result, 0, (max_sz + 1) * sizeof(unsigned char));

    unsigned int carry = 0;
    size_t i = 0;
        
    for (; i < max_sz; ++i) {
        unsigned int digit_sum = carry;
        if (i < a._sz) digit_sum += a._arr[i];
        if (i < b._sz) digit_sum += b._arr[i];
        result[i] = static_cast<unsigned char>(digit_sum % 10);
        carry = digit_sum / 10;
    }
    
    if (carry) {
        result[i++] = static_cast<unsigned char>(carry);
    }

    size_t result_sz = i > 0 ? i : 1;
    
    Decimal sum;
    
    delete[] sum._arr;
    sum._arr = result;
    sum._sz = result_sz;
    
    return sum;
}

Decimal operator-(const Decimal& a, const Decimal& b) {
    if (a < b) {
        throw std::underflow_error("Result would be negative.");
    }

    size_t maxSize = std::max(a._sz, b._sz);
    unsigned char *resultArr = new unsigned char[maxSize];
    std::memset(resultArr, 0, maxSize * sizeof(unsigned char));

    int borrow = 0;
    for (size_t i = 0; i < maxSize; ++i) {
        int aDigit = (i < a._sz) ? a._arr[i] : 0;
        int bDigit = (i < b._sz) ? b._arr[i] : 0;
        int diff = aDigit - bDigit - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        resultArr[i] = static_cast<unsigned char>(diff);
    }

    Decimal result(maxSize, 0);
        
    if (maxSize > 0 && result._arr != nullptr) {
        std::memcpy(result._arr, resultArr, maxSize * sizeof(unsigned char));
    }
            
    delete[] resultArr;
    return result;
}



bool operator==(const Decimal& a, const Decimal& b) {
    if (a._sz != b._sz) return false;
    if (a._sz == 0) return true;
    return std::memcmp(a._arr, b._arr, a._sz * sizeof(unsigned char)) == 0;
}

bool operator!=(const Decimal& a, const Decimal& b) {
    return !(a == b);
}

bool operator<(const Decimal& a, const Decimal& b) {
    if (a._sz != b._sz) return a._sz < b._sz;

    for (size_t i = a._sz; i > 0; --i) {
        size_t idx = i - 1;
        if (a._arr[idx] != b._arr[idx]) return a._arr[idx] < b._arr[idx];
    }
    return false;
}

bool operator>(const Decimal& a, const Decimal& b) {
    return b < a;
}

bool operator<=(const Decimal& a, const Decimal& b) {
    return !(a > b);
}

bool operator>=(const Decimal& a, const Decimal& b) {
    return !(a < b);
}



std::ostream& operator<<(std::ostream& os, const Decimal& num) {
    if (num._sz == 0) {
        os << '0';
        return os;
    }
    for (size_t i = num._sz; i > 0; --i) {
        os << static_cast<int>(num._arr[i - 1]);
    }
    return os;
}

size_t Decimal::getSize() const {
    return _sz;
}

std::string Decimal::toString() const {
    if (_sz == 0) return std::string("0");

    std::string str;
    str.reserve(_sz);
    for (size_t i = _sz; i > 0; --i) {
        str.push_back(static_cast<char>(_arr[i - 1] + '0'));
    }

    size_t first_non_zero = 0;
    while (first_non_zero < str.size() && str[first_non_zero] == '0') {
        ++first_non_zero;
    }
    if (first_non_zero == str.size()) {
        return std::string("0");
    }
    return str.substr(first_non_zero);
}


bool Decimal::isInvalidDigit(unsigned char c) {
    return c > 9;
}

bool Decimal::isValidDecimalInitList(const std::initializer_list<unsigned char>& lst) {
    for (unsigned char c : lst) {
        if (isInvalidDigit(c)) return false;
    }
    return true;
}

bool Decimal::isValidDecimalString(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}
