#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <cmath>
#include <type_traits>

// Шаблонный класс массива
template<typename T>
class Array {
private:
    T* data;
    size_t size;
    
public:
    // Конструктор
    explicit Array(size_t size) : size(size) {
        if (size <= 0) {
            throw std::invalid_argument("Размер массива должен быть положительным числом");
        }
        data = new T[size];
        // Инициализация значениями по умолчанию
        for (size_t i = 0; i < size; i++) {
            data[i] = T();
        }
    }
    
    // Деструктор
    ~Array() {
        delete[] data;
    }
    
    // Конструктор копирования
    Array(const Array& other) : size(other.size) {
        data = new T[size];
        for (size_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
    
    // Оператор присваивания
    Array& operator=(const Array& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new T[size];
            for (size_t i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    // Получение размера
    size_t getSize() const {
        return size;
    }
    
    // Оператор [] для доступа к элементам (без проверки границ для производительности)
    T& operator[](size_t index) {
        return data[index];
    }
    
    const T& operator[](size_t index) const {
        return data[index];
    }
    
    // Безопасный доступ с проверкой границ
    T& at(size_t index) {
        if (index >= size) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива [0, " + 
                                   std::to_string(size - 1) + "]");
        }
        return data[index];
    }
    
    const T& at(size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива [0, " + 
                                   std::to_string(size - 1) + "]");
        }
        return data[index];
    }
    
    // Сеттер с проверкой для числовых типов (специализация через SFINAE)
    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value>::type
    setWithCheck(size_t index, U value) {
        // Для числовых типов можно добавить дополнительную проверку
        // Например, проверку диапазона для целых чисел
        if (index >= size) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива");
        }
        data[index] = value;
    }
    
    // Перегрузка для нечисловых типов (просто сеттер без дополнительной проверки)
    template<typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value>::type
    setWithCheck(size_t index, const U& value) {
        if (index >= size) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива");
        }
        data[index] = value;
    }
    
    // Операция вывода
    friend std::ostream& operator<<(std::ostream& os, const Array<T>& arr) {
        os << "[";
        for (size_t i = 0; i < arr.size; i++) {
            os << arr.data[i];
            if (i < arr.size - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }
    
    // Вычисление евклидова расстояния между массивами
    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, double>::type
    static euclideanDistance(const Array<U>& arr1, const Array<U>& arr2) {
        if (arr1.size != arr2.size) {
            throw std::invalid_argument("Массивы должны иметь одинаковый размер: " + 
                                       std::to_string(arr1.size) + " != " + 
                                       std::to_string(arr2.size));
        }
        
        double sum = 0.0;
        for (size_t i = 0; i < arr1.size; i++) {
            double diff = static_cast<double>(arr1.data[i]) - static_cast<double>(arr2.data[i]);
            sum += diff * diff;
        }
        
        return std::sqrt(sum);
    }
    
    // Перегрузка для нечисловых типов
    template<typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, double>::type
    static euclideanDistance(const Array<U>& arr1, const Array<U>& arr2) {
        // Для нечисловых типов выбрасываем исключение
        throw std::bad_typeid();
    }
};

// Специальный сеттер с проверкой диапазона для числовых значений
template<typename T>
class ArrayRangeChecker {
public:
    // Проверка и установка значения с проверкой диапазона
    template<typename U = T>
    static typename std::enable_if<std::is_arithmetic<U>::value>::type
    setWithRangeCheck(Array<U>& arr, size_t index, U value, U minVal, U maxVal) {
        if (value < minVal || value > maxVal) {
            throw std::invalid_argument("Значение " + std::to_string(value) + 
                                       " должно быть в диапазоне [" + 
                                       std::to_string(minVal) + ", " + 
                                       std::to_string(maxVal) + "]");
        }
        arr.setWithCheck(index, value);
    }
    
    // Для нечисловых типов - просто установка (без проверки диапазона)
    template<typename U = T>
    static typename std::enable_if<!std::is_arithmetic<U>::value>::type
    setWithRangeCheck(Array<U>& arr, size_t index, const U& value, U minVal, U maxVal) {
        // Для нечисловых типов игнорируем minVal и maxVal
        arr.setWithCheck(index, value);
    }
};

// Пример использования
int main() {
    std::cout << "Тестирование массива с int" << std::endl;
    try {
        Array<int> intArr(5);
        
        // Инициализация массива
        for (size_t i = 0; i < intArr.getSize(); i++) {
            intArr.setWithCheck(i, static_cast<int>(i * 10));
        }
        
        std::cout << "intArr: " << intArr << std::endl;
        
        // Использование сеттера с проверкой диапазона
        ArrayRangeChecker<int>::setWithRangeCheck(intArr, 0, 50, 0, 100);
        std::cout << "После setWithRangeCheck(0, 50, 0, 100): " << intArr << std::endl;
        
        try {
            ArrayRangeChecker<int>::setWithRangeCheck(intArr, 1, 150, 0, 100);
        } catch (const std::invalid_argument& e) {
            std::cout << "Ошибка при установке значения 150: " << e.what() << std::endl;
        }
        
        // Создаем второй массив
        Array<int> intArr2(5);
        for (size_t i = 0; i < intArr2.getSize(); i++) {
            intArr2.setWithCheck(i, static_cast<int>(i * 5));
        }
        std::cout << "intArr2: " << intArr2 << std::endl;
        
        // Вычисление евклидова расстояния
        double distance = Array<int>::euclideanDistance(intArr, intArr2);
        std::cout << "Евклидово расстояние между intArr и intArr2: " << distance << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
    
    std::cout << "\nТестирование массива с double" << std::endl;
    try {
        Array<double> doubleArr(3);
        doubleArr.setWithCheck(0, 1.5);
        doubleArr.setWithCheck(1, 2.5);
        doubleArr.setWithCheck(2, 3.5);
        
        Array<double> doubleArr2(3);
        doubleArr2.setWithCheck(0, 0.5);
        doubleArr2.setWithCheck(1, 1.5);
        doubleArr2.setWithCheck(2, 2.5);
        
        std::cout << "doubleArr: " << doubleArr << std::endl;
        std::cout << "doubleArr2: " << doubleArr2 << std::endl;
        
        double distance = Array<double>::euclideanDistance(doubleArr, doubleArr2);
        std::cout << "Евклидово расстояние между doubleArr и doubleArr2: " << distance << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
    
    std::cout << "\nТестирование массива с std::string" << std::endl;
    try {
        Array<std::string> strArr(3);
        strArr.setWithCheck(0, "Hello");
        strArr.setWithCheck(1, "World");
        strArr.setWithCheck(2, "!");
        
        std::cout << "strArr: " << strArr << std::endl;
        
        Array<std::string> strArr2(3);
        strArr2.setWithCheck(0, "Hi");
        strArr2.setWithCheck(1, "There");
        strArr2.setWithCheck(2, "!");
        
        // Попытка вычислить евклидово расстояние для строк - должно выбросить исключение
        try {
            double distance = Array<std::string>::euclideanDistance(strArr, strArr2);
            std::cout << "Евклидово расстояние: " << distance << std::endl;
        } catch (const std::bad_typeid&) {
            std::cout << "Поймано std::bad_typeid: невозможно вычислить расстояние для нечисловых типов" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
    
    std::cout << "\nТестирование исключений" << std::endl;
    try {
        Array<int> arr1(3);
        Array<int> arr2(4);  // Разный размер
        
        try {
            double distance = Array<int>::euclideanDistance(arr1, arr2);
        } catch (const std::invalid_argument& e) {
            std::cout << "Поймано std::invalid_argument: " << e.what() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
    
    std::cout << "\nТестирование разных типов" << std::endl;
    try {
        // Массив с char
        Array<char> charArr(3);
        charArr.setWithCheck(0, 'A');
        charArr.setWithCheck(1, 'B');
        charArr.setWithCheck(2, 'C');
        std::cout << "charArr: " << charArr << std::endl;
        
        // Проверка сеттера с диапазоном для char
        ArrayRangeChecker<char>::setWithRangeCheck(charArr, 0, 'X', 'A', 'Z');
        std::cout << "После установки 'X': " << charArr << std::endl;
        
        // Массив с float
        Array<float> floatArr(2);
        floatArr.setWithCheck(0, 3.14f);
        floatArr.setWithCheck(1, 2.71f);
        std::cout << "floatArr: " << floatArr << std::endl;
        
        // Проверка расстояния для float
        Array<float> floatArr2(2);
        floatArr2.setWithCheck(0, 1.0f);
        floatArr2.setWithCheck(1, 1.0f);
        
        double distance = Array<float>::euclideanDistance(floatArr, floatArr2);
        std::cout << "Евклидово расстояние для float: " << distance << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
    
    return 0;
}