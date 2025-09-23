#include "bits_counter.h"

int count_bits(int a, int b) {
    int result = 0;
    
    for (int i = a; i <= b; i++) {
        int x = i;
        
        while (x > 0) {
            result += x % 2;
            x /= 2;
        }
    }
    
    return result;
}