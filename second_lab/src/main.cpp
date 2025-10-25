#include "../include/array.h"

int main() {
    Decimal num1("12345");
    Decimal num2("678");
    Decimal num3(5, 7);
    
    std::cout << "num1 = " << num1 << "\n";
    std::cout << "num2 = " << num2 << "\n";
    std::cout << "num3 = " << num3 << "\n\n";

    Decimal sum = num1 + num2;
    std::cout << num1 << " + " << num2 << " = " << sum << "\n\n";

    Decimal diff = num1 - num2;
    std::cout << num1 << " - " << num2 << " = " << diff << "\n\n";

    std::cout << num1 << " == " << num2 << ": " << (num1 == num2 ? "true" : "false") << "\n";
    std::cout << num1 << " < " << num2 << ": " << (num1 < num2 ? "true" : "false") << "\n";
    std::cout << num1 << " > " << num2 << ": " << (num1 > num2 ? "true" : "false") << "\n\n";

    Decimal num4 = num1;
    num4 += num2;
    std::cout << "num4 = num1; num4 += num2: " << num4 << "\n";
    
    num4 = num1;
    num4 -= num2;
    std::cout << "num4 = num1; num4 -= num2: " << num4 << "\n\n";

    try {
        Decimal small("10");
        Decimal large("100");
        std::cout << "Попытка: " << small << " - " << large << "\n";
        Decimal invalid = small - large;
    } catch (const std::underflow_error& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
    
    return 0;
}