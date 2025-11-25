#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class DynArray {
protected:
    int* data;
    size_t size;
    size_t capacity;

public:
    DynArray(size_t capacity = 10)
        : size(0), capacity(capacity)
    {
        data = new int[capacity];
    }

    virtual ~DynArray() {
        delete[] data;
    }

    void push_back(int value) {
        if (size >= capacity) {
            // увеличиваем массив
            capacity *= 2;
            int* newdata = new int[capacity];

            for (size_t i = 0; i < size; i++)
                newdata[i] = data[i];

            delete[] data;
            data = newdata;
        }
        data[size++] = value;
    }

    size_t getSize() const { return size; }
    int operator[](size_t i) const { return data[i]; }

    virtual void save() = 0;  // Виртуальный метод
protected:
    static std::string getCurrentDateTime() {
        auto t = std::time(nullptr);
        std::tm* now = std::localtime(&t);

        std::ostringstream oss;
        oss << (now->tm_year + 1900)
            << "-" << std::setw(2) << std::setfill('0') << (now->tm_mon + 1)
            << "-" << std::setw(2) << std::setfill('0') << now->tm_mday
            << "_" << std::setw(2) << std::setfill('0') << now->tm_hour
            << "-" << std::setw(2) << std::setfill('0') << now->tm_min
            << "-" << std::setw(2) << std::setfill('0') << now->tm_sec;

        return oss.str();
    }
};

// TXT Class
class ArrTxt : public DynArray {
public:
    using DynArray::DynArray;

    void save() override {
        std::string filename = getCurrentDateTime() + ".txt";
        std::ofstream out(filename);

        if (!out) {
            std::cerr << "Ошибка открытия файла!\n";
            return;
        }

        for (size_t i = 0; i < size; i++)
            out << data[i] << "\n";

        std::cout << "Файл TXT сохранён: " << filename << "\n";
    }
};

// CSV Class
class ArrCSV : public DynArray {
public:
    using DynArray::DynArray;

    void save() override {
        std::string filename = getCurrentDateTime() + ".csv";
        std::ofstream out(filename);

        if (!out) {
            std::cerr << "Ошибка открытия файла!\n";
            return;
        }

        for (size_t i = 0; i < size; i++)
            out << data[i] << (i + 1 < size ? "," : "");

        std::cout << "Файл CSV сохранён: " << filename << "\n";
    }
};

// MAIN
int main() {
    ArrTxt arrTxt;
    ArrCSV arrCsv;

    // Добавим данные
    for (int i = 1; i <= 10; i++) {
        arrTxt.push_back(i * 2);
        arrCsv.push_back(i * 3);
    }

    // Вызов виртуальных функций (полиморфизм)
    DynArray* arrays[2];
    arrays[0] = &arrTxt;
    arrays[1] = &arrCsv;

    for (int i = 0; i < 2; i++) {
        arrays[i]->save();
    }

    return 0;
}