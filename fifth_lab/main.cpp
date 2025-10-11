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
    std::cout << "DynamicArray with int" << std::endl;
    DynamicArray<int> intArr;
    intArr.PushBack(10);
    intArr.PushBack(20);
    intArr.PushBack(30);

    std::cout << "Array size: " << intArr.Size() << std::endl;
    for (auto it = intArr.Begin(); it != intArr.End(); ++it) {
        std::cout << *it << " ";
    }
    
    std::cout << std::endl;

    while (!intArr.IsEmpty()) {
        std::cout << "PopBack" << std::endl;
        intArr.PopBack();
    }

    std::cout << "\nDynamicArray with ComplexType" << std::endl;
    DynamicArray<ComplexType> complexArr;
    complexArr.PushBack(ComplexType(1, "First", 100.0));
    complexArr.PushBack(ComplexType(2, "Second", 200.5));
    complexArr.PushBack(ComplexType(3, "Third", 300.75));

    std::cout << "Array size: " << complexArr.Size() << std::endl;
    for (auto it = complexArr.Begin(); it != complexArr.End(); ++it) {
        const ComplexType& v = *it;
        std::cout << "{ id: " << v.id
                  << ", name: " << v.name
                  << ", value: " << v.value << " }" << std::endl;
    }

    std::cout << "\nDynamicArray with custom allocator" << std::endl;
    
    FixedBlockMemoryResource memoryResource(1024 * 1024);
    std::pmr::polymorphic_allocator<int> allocator(&memoryResource);
    
    DynamicArray<int, std::pmr::polymorphic_allocator<int>> allocArr(allocator);

    allocArr.PushBack(100);
    allocArr.PushBack(200);
    allocArr.PushBack(300);

    std::cout << "Array size (with custom allocator): " << allocArr.Size() << std::endl;
    for (auto it = allocArr.Begin(); it != allocArr.End(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    while (!allocArr.IsEmpty()) {
        std::cout << "PopBack: " << allocArr[allocArr.Size() - 1] << std::endl;
        allocArr.PopBack();
    }
    
    return 0;
}
