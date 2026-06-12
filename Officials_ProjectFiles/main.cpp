#include "module.h"

#include <clocale>
#include <iostream>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    setlocale(LC_ALL, ".UTF-8");

#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    Ministry ministry;
    int choice = -1;

    do {
        std::cout << "\n";
        std::cout << "Чиновники\n";
        std::cout << "Минимальная стоимость получения лицензии\n";
        std::cout << "Статус данных: " << (ministry.isFilled() ? "загружены" : "не загружены") << "\n\n";
        std::cout << "1) Ввести данные с клавиатуры\n";
        std::cout << "2) Работа с файлом\n";
        std::cout << "3) Сформировать случайное министерство\n";
        std::cout << "4) Показать текущие данные и решение\n";
        std::cout << "0) Завершить работу\n";
        choice = readMenuChoice();

        try {
            if (choice == 1) {
                runKeyboardInput(ministry);
            } else if (choice == 2) {
                runFileInput(ministry);
            } else if (choice == 3) {
                runRandomInput(ministry);
            } else if (choice == 4) {
                runPrintAndSolve(ministry);
            } else if (choice == 0) {
                std::cout << "\nРабота программы завершена.\n";
            }
        } catch (const MinistryException& error) {
            std::cout << "\n" << error.what() << "\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } catch (const std::exception& error) {
            std::cout << "\nНепредвиденная ошибка: " << error.what() << "\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } while (choice != 0);

    return 0;
}
