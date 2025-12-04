#include <iostream>
#include <stdexcept>
#include <string>

class Array {
private:
    int* data;
    int size;
    
    // Проверка значения на соответствие диапазону
    bool isValidValue(int value) const {
        return (value >= -100 && value <= 100);
    }
    
    // Проверка индекса на валидность
    bool isValidIndex(int index) const {
        return (index >= 0 && index < size);
    }

public:
    // Конструктор
    Array(int size) {
        if (size <= 0) {
            throw std::invalid_argument("Размер массива должен быть положительным числом");
        }
        this->size = size;
        data = new int[size];
        // Инициализация нулями (0 входит в диапазон)
        for (int i = 0; i < size; i++) {
            data[i] = 0;
        }
    }
    
    // Деструктор
    ~Array() {
        delete[] data;
    }
    
    // Конструктор копирования
    Array(const Array& other) {
        size = other.size;
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
    
    // Оператор присваивания
    Array& operator=(const Array& other) {
        if (this != &other) {
            // Проверяем все значения перед копированием
            for (int i = 0; i < other.size; i++) {
                if (!isValidValue(other.data[i])) {
                    throw std::invalid_argument("Невозможно скопировать массив: содержит значение " + 
                                               std::to_string(other.data[i]) + 
                                               " вне диапазона [-100, 100]");
                }
            }
            
            delete[] data;
            size = other.size;
            data = new int[size];
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    // Получение размера массива
    int getSize() const {
        return size;
    }
    
    // Оператор [] для получения элемента (константная версия)
    const int& operator[](int index) const {
        if (!isValidIndex(index)) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива [0, " + 
                                   std::to_string(size - 1) + "]");
        }
        return data[index];
    }
    
    // Класс-прокси для контроля присваивания через оператор []
    class ArrayElement {
    private:
        Array& array;
        int index;
        
    public:
        ArrayElement(Array& arr, int idx) : array(arr), index(idx) {}
        
        // Оператор присваивания для элемента
        ArrayElement& operator=(int value) {
            if (!array.isValidValue(value)) {
                throw std::invalid_argument("Значение " + std::to_string(value) + 
                                           " должно быть в диапазоне [-100, 100]");
            }
            array.data[index] = value;
            return *this;
        }
        
        // Оператор приведения к int (для чтения)
        operator int() const {
            return array.data[index];
        }
    };
    
    // Оператор [] для изменения элемента (возвращает прокси-объект)
    ArrayElement operator[](int index) {
        if (!isValidIndex(index)) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива [0, " + 
                                   std::to_string(size - 1) + "]");
        }
        return ArrayElement(*this, index);
    }
    
    // Метод для установки значения с проверкой
    void setValue(int index, int value) {
        if (!isValidIndex(index)) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива [0, " + 
                                   std::to_string(size - 1) + "]");
        }
        
        if (!isValidValue(value)) {
            throw std::invalid_argument("Значение " + std::to_string(value) + 
                                       " должно быть в диапазоне [-100, 100]");
        }
        
        data[index] = value;
    }
    
    // Получение значения
    int getValue(int index) const {
        if (!isValidIndex(index)) {
            throw std::out_of_range("Индекс " + std::to_string(index) + 
                                   " выходит за границы массива [0, " + 
                                   std::to_string(size - 1) + "]");
        }
        return data[index];
    }
    
    // Вывод массива
    void print() const {
        std::cout << "[";
        for (int i = 0; i < size; i++) {
            std::cout << data[i];
            if (i < size - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
    
    // Заполнение массива значениями с проверкой
    void fill(int value) {
        if (!isValidValue(value)) {
            throw std::invalid_argument("Значение " + std::to_string(value) + 
                                       " должно быть в диапазоне [-100, 100]");
        }
        
        for (int i = 0; i < size; i++) {
            data[i] = value;
        }
    }
};

// Пример использования с обработкой исключений
int main() {
    try {
        // Создание массива
        Array arr(5);
        
        std::cout << "Исходный массив: ";
        arr.print();
        
        // Корректные операции
        std::cout << "\n1. Корректные операции:" << std::endl;
        arr.setValue(0, 10);
        arr.setValue(1, -50);
        arr.setValue(2, 100);
        arr[3] = -100;  // Использование оператора [] с проверкой
        arr[4] = 0;     // Использование оператора [] с проверкой
        
        std::cout << "Массив после заполнения: ";
        arr.print();
        
        std::cout << "Элемент arr[2]: " << arr.getValue(2) << std::endl;
        std::cout << "Элемент arr[3] через оператор []: " << arr[3] << std::endl; // Теперь работает через прокси
        
        // Тестирование исключений
        std::cout << "\n2. Тестирование исключений:" << std::endl;
        
        try {
            // Попытка доступа по несуществующему индексу
            std::cout << "Попытка доступа к arr[10]..." << std::endl;
            int value = arr[10];  // Используем оператор [] для чтения
        } 
        catch (const std::out_of_range& e) {
            std::cout << "Поймано исключение out_of_range: " << e.what() << std::endl;
        }
        
        try {
            // Попытка присвоить значение вне диапазона через setValue
            std::cout << "Попытка установить arr[0] = 150 через setValue..." << std::endl;
            arr.setValue(0, 150);
        } 
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано исключение invalid_argument: " << e.what() << std::endl;
        }
        
        try {
            // Попытка использовать оператор [] для установки значения вне диапазона
            std::cout << "Попытка установить arr[1] = -101 через оператор []..." << std::endl;
            arr[1] = -101;  // Теперь здесь будет проверка!
        } 
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано исключение invalid_argument: " << e.what() << std::endl;
        }
        
        try {
            // Попытка использовать оператор [] для чтения значения вне диапазона
            std::cout << "Попытка использовать arr[1] = 200 в выражении..." << std::endl;
            arr[1] = 200;  // Это выбросит исключение
        } 
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано исключение invalid_argument: " << e.what() << std::endl;
        }
        
        try {
            // Попытка создать массив с недопустимым размером
            std::cout << "Попытка создать массив размером -5..." << std::endl;
            Array invalidArr(-5);
        } 
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано исключение invalid_argument: " << e.what() << std::endl;
        }
        
        try {
            // Попытка использовать fill с недопустимым значением
            std::cout << "Попытка заполнить массив значением 200..." << std::endl;
            arr.fill(200);
        } 
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано исключение invalid_argument: " << e.what() << std::endl;
        }
        
        // Тестирование копирования
        std::cout << "\n3. Тестирование копирования:" << std::endl;
        Array arr2 = arr;  // Конструктор копирования
        std::cout << "Скопированный массив: ";
        arr2.print();
        
        try {
            // Создаем "плохой" массив
            Array badArr(3);
            badArr[0] = 50;
            badArr[1] = 101;  // ОШИБКА: значение вне диапазона!
            badArr[2] = -50;
            
            std::cout << "Попытка скопировать массив с недопустимым значением..." << std::endl;
            Array arr3 = badArr;  // Должно выбросить исключение
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано исключение при копировании: " << e.what() << std::endl;
        }
        
        std::cout << "\n4. Текущее состояние массива: ";
        arr.print();
        
    } 
    catch (const std::exception& e) {
        std::cout << "Непредвиденное исключение: " << e.what() << std::endl;
    }
    
    return 0;
}