#include "include/array.h"
#include <gtest/gtest.h>
#include <sstream>

// Тестирование конструктора по умолчанию
TEST(DecimalTest, DefaultConstructor) {
    Decimal num;
    EXPECT_EQ(num.getSize(), 1);
    EXPECT_EQ(num.toString(), "0");
}

// Тестирование конструктора с размером и инициализацией цифры
TEST(DecimalTest, ConstructorWithSizeAndInit) {
    Decimal num(5, 7);
    EXPECT_EQ(num.getSize(), 5);
    EXPECT_EQ(num.toString(), "77777");
}

// Тестирование конструктора из строки
TEST(DecimalTest, ConstructorWithString) {
    Decimal num("12345");
    EXPECT_EQ(num.getSize(), 5);
    EXPECT_EQ(num.toString(), "12345");
}

// Тестирование конструктора копирования
TEST(DecimalTest, CopyConstructor) {
    Decimal original("987");
    Decimal copy = original;
    EXPECT_EQ(copy.getSize(), 3);
    EXPECT_EQ(copy.toString(), "987");
}

// Тестирование оператора копирующего присваивания
TEST(DecimalTest, CopyAssignment) {
    Decimal num1("456");
    Decimal num2;
    num2 = num1;
    EXPECT_EQ(num2.getSize(), 3);
    EXPECT_EQ(num2.toString(), "456");
}

// Тестирование оператора сложения
TEST(DecimalTest, AdditionOperator) {
    Decimal num1("11");
    Decimal num2("2");
    Decimal result = num1 + num2;
    EXPECT_EQ(result.toString(), "13");  // 11 + 2 = 13
}

// Тестирование оператора вычитания
TEST(DecimalTest, SubtractionOperator) {
    Decimal num1("15");
    Decimal num2("4");
    Decimal result = num1 - num2;
    EXPECT_EQ(result.toString(), "11");  // 15 - 4 = 11
}

// Тестирование оператора сравнения ==
TEST(DecimalTest, EqualityOperator) {
    Decimal num1("123");
    Decimal num2("123");
    EXPECT_TRUE(num1 == num2);
    EXPECT_FALSE(num1 != num2);
}

// Тестирование оператора сравнения <
TEST(DecimalTest, LessThanOperator) {
    Decimal num1("122");
    Decimal num2("123");
    EXPECT_TRUE(num1 < num2);
    EXPECT_FALSE(num1 > num2);
}

// Тестирование оператора вывода в поток
TEST(DecimalTest, OutputOperator) {
    Decimal num("123");
    std::stringstream ss;
    ss << num;
    EXPECT_EQ(ss.str(), "123");
}

// Тестирование исключений при недопустимой строке
TEST(DecimalTest, InvalidStringConstructor) {
    EXPECT_THROW(Decimal("abcd"), std::invalid_argument);
}

// Тестирование исключений при вычитании, если результат отрицательный
TEST(DecimalTest, InvalidSubtraction) {
    Decimal num1("11");
    Decimal num2("22");
    EXPECT_THROW(num1 - num2, std::underflow_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
