#include <iostream>
#include "bits_counter.h"

int main(){
    int a, b;
    std::cin >> a >> b;
    
    if (a < 0 || b > 0 || a > b) {
        std::cout << "Uncorrect inpot" << std::endl;
        return 1;
    }
    
    int result;
    result = count_bits(a, b);
    
    std::cout << result << std::endl;
    
    return 0;
}