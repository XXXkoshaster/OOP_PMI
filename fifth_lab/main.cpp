#include <iostream>
#include <string>
#include <memory_resource>
#include "allocator.cpp"
#include "array.cpp"

struct ComplexType {
    int id;
    std::string name;
    double value;

    ComplexType(int id_, const std::string& name_, double value_)
        : id(id_), name(name_), value(value_) {}
};

int main() {
    FixedBlockMemoryResource resource(4096);
;
    Vector<int> int_vec(&resource);
    int_vec.pushBack(10);
    int_vec.pushBack(20);
    int_vec.pushBack(30);
    
    std::cout << "Size: " << int_vec.getSize() << "\n";
    for (size_t i = 0; i < int_vec.getSize(); ++i) {
        std::cout << "int_vec[" << i << "] = " << int_vec[i] << "\n";
    }

    Vector<ComplexType> complex_vec(&resource);
    complex_vec.pushBack(ComplexType(1, "Alice", 3.14));
    complex_vec.pushBack(ComplexType(2, "Bob", 2.71));
    complex_vec.pushBack(ComplexType(3, "Charlie", 1.41));
    
    std::cout << "Size: " << complex_vec.getSize() << "\n";
    for (size_t i = 0; i < complex_vec.getSize(); ++i) {
        std::cout << "complex_vec[" << i << "]: id=" << complex_vec[i].id 
                  << ", name=" << complex_vec[i].name 
                  << ", value=" << complex_vec[i].value << "\n";
    }

    complex_vec.popBack();
    std::cout << "After popBack, size: " << complex_vec.getSize() << "\n";

    return 0;
}
