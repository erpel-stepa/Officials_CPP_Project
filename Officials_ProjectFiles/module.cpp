#include "module.h"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <limits>

// проверка, что строка целиком является целым числом
static bool parseIntegerToken(const std::string& token, int& value) {
    if (token == "") {
        return false;
    }

    int start = 0;
    int sign = 1;
    if (token[0] == '-' || token[0] == '+') {
        if (token.length() == 1) {
            return false;
        }
        if (token[0] == '-') {
            sign = -1;
        }
        start = 1;
    }

    long long result = 0;
    for (int i = start; i < static_cast<int>(token.length()); i++) {
        if (token[i] < '0' || token[i] > '9') {
            return false;
        }

        result = result * 10 + (token[i] - '0');
        if (result > 2147483647LL) {
            return false;
        }
    }

    value = static_cast<int>(result) * sign;
    return true;
}

// безопасный ввод целого числа в заданном диапазоне
static int readIntegerInRange(const std::string& prompt, int minValue, int maxValue) {
    int value;
    std::string token;

    while (true) {
        std::cout << prompt;
        if (!(std::cin >> token)) {
            std::cout << "Ошибка: нужно ввести целое число.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (!parseIntegerToken(token, value)) {
            std::cout << "Ошибка: значение должно быть целым числом без букв и лишних символов.\n";
            continue;
        }

        if (value < minValue || value > maxValue) {
            std::cout << "Ошибка: допустимый диапазон от " << minValue
                      << " до " << maxValue << ".\n";
            continue;
        }

        return value;
    }
}

int readMenuChoice() {
    return readIntegerInRange("Выберите действие (0-4): ", 0, 4);
}

// чтение целого числа из файла без частичного принятия значения
static int readIntegerFromFile(std::ifstream& file, const std::string& errorMessage) {
    std::string token;
    int value;

    if (!(file >> token)) {
        throw MinistryException(errorMessage);
    }

    if (!parseIntegerToken(token, value)) {
        throw MinistryException(errorMessage);
    }

    return value;
}

// проверка цикла после ввода очередного начальника
static bool hasCycleFromOfficial(int officialNumber, const int* bosses, const bool* bossEntered, int count) {
    bool* visited = new bool[count + 1];
    for (int i = 1; i <= count; i++) {
        visited[i] = false;
    }

    int current = officialNumber;
    while (current != 0 && bossEntered[current]) {
        if (visited[current]) {
            delete[] visited;
            return true;
        }

        visited[current] = true;
        current = bosses[current];
    }

    delete[] visited;
    return false;
}

// проверка начальника прямо во время ручного ввода
static bool checkBossInputNow(int officialNumber,
                              int boss,
                              int count,
                              const int* bosses,
                              const bool* bossEntered,
                              int currentRoot) {
    if (boss == officialNumber) {
        std::cout << "Ошибка: чиновник не может быть начальником самому себе.\n";
        return false;
    }

    if (boss == 0 && currentRoot != 0) {
        std::cout << "Ошибка: главный чиновник уже указан: " << currentRoot << ".\n";
        std::cout << "Введите номер обычного начальника от 1 до " << count << ".\n";
        return false;
    }

    if (officialNumber == count && currentRoot == 0 && boss != 0) {
        std::cout << "Ошибка: главный чиновник еще не указан.\n";
        std::cout << "Так как это последний чиновник, для него нужно ввести 0.\n";
        return false;
    }

    if (hasCycleFromOfficial(officialNumber, bosses, bossEntered, count)) {
        std::cout << "Ошибка: такая связь создает цикл начальников.\n";
        std::cout << "Выберите другого начальника для чиновника " << officialNumber << ".\n";
        return false;
    }

    return true;
}

// ручное заполнение массивов с проверками во время ввода
static void inputArraysManually(int& newCount, int*& bribes, int*& bosses) {
    std::cout << "Нумерация чиновников начинается с 1.\n";
    std::cout << "Начальник 0 означает главного чиновника.\n\n";

    newCount = readIntegerInRange(
        "Количество чиновников N (целое число от 1 до 1000): ",
        1,
        1000
    );

    bribes = new int[newCount + 1];
    bosses = new int[newCount + 1];
    bool* bossEntered = new bool[newCount + 1];

    for (int i = 1; i <= newCount; i++) {
        bosses[i] = -1;
        bossEntered[i] = false;
    }

    try {
        std::cout << "\nВвод взяток\n";
        for (int i = 1; i <= newCount; i++) {
            bribes[i] = readIntegerInRange(
                "Чиновник " + std::to_string(i) + ", взятка (от 0 до 1000000): ",
                0,
                1000000
            );
        }

        std::cout << "\nВвод начальников\n";
        std::cout << "Введите 0 только для одного главного чиновника.\n";
        std::cout << "При ошибке значение нужно будет ввести повторно.\n";
        int currentRoot = 0;

        for (int i = 1; i <= newCount; i++) {
            bool accepted = false;
            while (!accepted) {
                int candidateBoss = readIntegerInRange(
                    "Чиновник " + std::to_string(i) + ", начальник (от 0 до " + std::to_string(newCount) + "): ",
                    0,
                    newCount
                );

                bosses[i] = candidateBoss;
                bossEntered[i] = true;

                if (checkBossInputNow(i, candidateBoss, newCount, bosses, bossEntered, currentRoot)) {
                    if (candidateBoss == 0) {
                        currentRoot = i;
                    }
                    accepted = true;
                } else {
                    bosses[i] = -1;
                    bossEntered[i] = false;
                }
            }
        }
    } catch (...) {
        delete[] bribes;
        delete[] bosses;
        delete[] bossEntered;
        bribes = nullptr;
        bosses = nullptr;
        throw;
    }

    delete[] bossEntered;
}

// случайное заполнение массивов корректным деревом
static void generateRandomArrays(int newCount, int*& bribes, int*& bosses) {
    if (newCount <= 0) {
        throw MinistryException("Ошибка: N должно быть больше 0.");
    }

    static bool randomInitialized = false;
    if (!randomInitialized) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        randomInitialized = true;
    }

    bribes = new int[newCount + 1];
    bosses = new int[newCount + 1];

    bosses[1] = 0;
    for (int i = 1; i <= newCount; i++) {
        bribes[i] = 1 + std::rand() % 50;
        if (i > 1) {
            bosses[i] = 1 + std::rand() % (i - 1);
        }
    }
}

// запись данных в файл по формату задачи
static void writeArraysToFile(const std::string& fileName, int count, const int* bribes, const int* bosses) {
    std::ofstream file(fileName.c_str());
    if (!file.is_open()) {
        throw MinistryException("Ошибка: не удалось создать файл.");
    }

    file << count << "\n";
    for (int i = 1; i <= count; i++) {
        if (i > 1) {
            file << " ";
        }
        file << bribes[i];
    }
    file << "\n";

    for (int i = 1; i <= count; i++) {
        if (i > 1) {
            file << " ";
        }
        file << bosses[i];
    }
    file << "\n";

    if (!file) {
        throw MinistryException("Ошибка: данные не удалось записать в файл.");
    }
}

MinistryException::MinistryException(const std::string& message)
    : std::runtime_error(message) {
}

IntNode::IntNode(int newValue) {
    value = newValue;
    next = nullptr;
}

int IntNode::getValue() const {
    return value;
}

IntNode* IntNode::getNext() const {
    return next;
}

void IntNode::setNext(IntNode* newNext) {
    next = newNext;
}

IntList::IntList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

IntList::IntList(const IntList& other) {
    head = nullptr;
    tail = nullptr;
    size = 0;
    copyFrom(other);
}

IntList::~IntList() {
    clear();
}

IntList& IntList::operator=(const IntList& other) {
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

void IntList::copyFrom(const IntList& other) {
    IntNode* current = other.head;
    while (current != nullptr) {
        pushBack(current->getValue());
        current = current->getNext();
    }
}

void IntList::pushBack(int value) {
    IntNode* newNode = new IntNode(value);
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    } else {
        tail->setNext(newNode);
        tail = newNode;
    }
    size++;
}

void IntList::clear() {
    IntNode* current = head;
    while (current != nullptr) {
        IntNode* nextNode = current->getNext();
        delete current;
        current = nextNode;
    }

    head = nullptr;
    tail = nullptr;
    size = 0;
}

int IntList::getSize() const {
    return size;
}

int IntList::get(int index) const {
    if (index < 0 || index >= size) {
        throw MinistryException("Ошибка: выход за границы списка.");
    }

    IntNode* current = head;
    for (int i = 0; i < index; i++) {
        current = current->getNext();
    }

    return current->getValue();
}

Official::Official() {
    number = 0;
    bribe = 0;
    boss = 0;
}

void Official::setNumber(int value) {
    number = value;
}

void Official::setBribe(int value) {
    bribe = value;
}

void Official::setBoss(int value) {
    boss = value;
}

void Official::addSubordinate(int value) {
    subordinates.pushBack(value);
}

void Official::clearSubordinates() {
    subordinates.clear();
}

int Official::getNumber() const {
    return number;
}

int Official::getBribe() const {
    return bribe;
}

int Official::getBoss() const {
    return boss;
}

int Official::getSubordinateCount() const {
    return subordinates.getSize();
}

int Official::getSubordinate(int index) const {
    return subordinates.get(index);
}

Ministry::Ministry() {
    officials = nullptr;
    count = 0;
    root = 0;
    filled = false;
}

Ministry::~Ministry() {
    clear();
}

void Ministry::clear() {
    delete[] officials;
    officials = nullptr;
    count = 0;
    root = 0;
    filled = false;
}

void Ministry::allocate(int newCount) {
    clear();
    if (newCount <= 0) {
        throw MinistryException("Ошибка: количество чиновников N должно быть больше 0.");
    }

    count = newCount;
    officials = new Official[count + 1];
    for (int i = 1; i <= count; i++) {
        officials[i].setNumber(i);
    }
}

void Ministry::loadFromArrays(int newCount, const int* bribes, const int* bosses) {
    allocate(newCount);

    try {
        for (int i = 1; i <= count; i++) {
            officials[i].setBribe(bribes[i]);
            officials[i].setBoss(bosses[i]);
        }

        validate();
        buildSubordinates();
        filled = true;
    } catch (...) {
        clear();
        throw;
    }
}

void Ministry::inputFromKeyboard() {
    std::cout << "\nВвод с клавиатуры\n";
    std::cout << "Введите данные о чиновниках и их начальниках.\n";

    int newCount = 0;
    int* bribes = nullptr;
    int* bosses = nullptr;

    try {
        inputArraysManually(newCount, bribes, bosses);
        loadFromArrays(newCount, bribes, bosses);
    } catch (...) {
        delete[] bribes;
        delete[] bosses;
        throw;
    }

    delete[] bribes;
    delete[] bosses;
}

void Ministry::inputFromFile(const std::string& fileName) {
    std::ifstream file(fileName.c_str());
    if (!file.is_open()) {
        throw MinistryException("Ошибка: файл не найден или не может быть открыт.");
    }

    int newCount = readIntegerFromFile(file, "Ошибка: в первой строке файла должно быть целое число N.");

    if (newCount <= 0) {
        throw MinistryException("Ошибка: N должно быть больше 0.");
    }
    if (newCount > 1000) {
        throw MinistryException("Ошибка: N в файле должно быть не больше 1000.");
    }

    int* bribes = new int[newCount + 1];
    int* bosses = new int[newCount + 1];

    try {
        for (int i = 1; i <= newCount; i++) {
            bribes[i] = readIntegerFromFile(file, "Ошибка: во второй строке файла должно быть N целых взяток.");
        }

        for (int i = 1; i <= newCount; i++) {
            bosses[i] = readIntegerFromFile(file, "Ошибка: в третьей строке файла должно быть N целых номеров начальников.");
        }

        loadFromArrays(newCount, bribes, bosses);
    } catch (...) {
        delete[] bribes;
        delete[] bosses;
        throw;
    }

    delete[] bribes;
    delete[] bosses;
}

void Ministry::fillRandom(int newCount) {
    if (newCount <= 0) {
        throw MinistryException("Ошибка: N должно быть больше 0.");
    }

    int* bribes = nullptr;
    int* bosses = nullptr;
    generateRandomArrays(newCount, bribes, bosses);

    loadFromArrays(newCount, bribes, bosses);

    delete[] bribes;
    delete[] bosses;
}

void Ministry::validate() {
    if (count <= 0 || officials == nullptr) {
        throw MinistryException("Ошибка: данные не загружены.");
    }

    int rootCount = 0;
    int foundRoot = 0;

    for (int i = 1; i <= count; i++) {
        if (officials[i].getBribe() < 0) {
            throw MinistryException("Ошибка: взятка не может быть отрицательной.");
        }

        int boss = officials[i].getBoss();
        if (boss < 0 || boss > count) {
            throw MinistryException("Ошибка: номер начальника должен быть от 0 до N.");
        }

        if (boss == i) {
            throw MinistryException("Ошибка: чиновник не может быть начальником самому себе.");
        }

        if (boss == 0) {
            rootCount++;
            foundRoot = i;
        }
    }

    if (rootCount != 1) {
        throw MinistryException("Ошибка: главный чиновник должен быть ровно один.");
    }

    root = foundRoot;
    checkCycles();

    bool* visited = new bool[count + 1];
    for (int i = 1; i <= count; i++) {
        visited[i] = false;
    }

    markReachable(root, visited);
    for (int i = 1; i <= count; i++) {
        if (!visited[i]) {
            delete[] visited;
            throw MinistryException("Ошибка: не все чиновники связаны с главным чиновником.");
        }
    }

    delete[] visited;
}

void Ministry::buildSubordinates() {
    for (int i = 1; i <= count; i++) {
        officials[i].clearSubordinates();
    }

    for (int i = 1; i <= count; i++) {
        int boss = officials[i].getBoss();
        if (boss != 0) {
            // добавляем подчиненного
            officials[boss].addSubordinate(i);
        }
    }
}

void Ministry::checkCycles() const {
    int* colors = new int[count + 1];
    for (int i = 1; i <= count; i++) {
        colors[i] = 0;
    }

    try {
        for (int i = 1; i <= count; i++) {
            if (colors[i] == 0) {
                dfsCycle(i, colors);
            }
        }
    } catch (...) {
        delete[] colors;
        throw;
    }

    delete[] colors;
}

void Ministry::dfsCycle(int officialNumber, int* colors) const {
    colors[officialNumber] = 1;
    int boss = officials[officialNumber].getBoss();

    // поиск цикла по ссылкам на начальников
    if (boss != 0) {
        if (colors[boss] == 1) {
            throw MinistryException("Ошибка: в структуре министерства есть цикл.");
        }
        if (colors[boss] == 0) {
            dfsCycle(boss, colors);
        }
    }

    colors[officialNumber] = 2;
}

void Ministry::markReachable(int officialNumber, bool* visited) const {
    visited[officialNumber] = true;

    for (int i = 1; i <= count; i++) {
        if (officials[i].getBoss() == officialNumber && !visited[i]) {
            markReachable(i, visited);
        }
    }
}

bool Ministry::isFilled() const {
    return filled;
}

int Ministry::calculateMinCost(int officialNumber, int* costs, int* nextOfficials) const {
    if (costs[officialNumber] != -1) {
        return costs[officialNumber];
    }

    int bestChild = 0;
    int bestChildCost = 0;

    // считаем минимум среди непосредственных подчиненных
    for (int i = 0; i < officials[officialNumber].getSubordinateCount(); i++) {
        int child = officials[officialNumber].getSubordinate(i);
        int childCost = calculateMinCost(child, costs, nextOfficials);

        if (bestChild == 0 || childCost < bestChildCost) {
            bestChild = child;
            bestChildCost = childCost;
        }
    }

    nextOfficials[officialNumber] = bestChild;
    costs[officialNumber] = officials[officialNumber].getBribe() + bestChildCost;
    return costs[officialNumber];
}

void Ministry::savePath(int officialNumber, int* nextOfficials, IntList& path) const {
    int child = nextOfficials[officialNumber];
    if (child != 0) {
        savePath(child, nextOfficials, path);
    }

    // сохраняем путь
    path.pushBack(officialNumber);
}

void Ministry::printAll() const {
    if (!filled) {
        throw MinistryException("Ошибка: данные еще не загружены. Сначала выберите пункт 1, 2 или 3.");
    }

    std::cout << "\nДанные министерства\n";
    std::cout << "Всего чиновников: " << count << "\n";
    std::cout << "Главный чиновник: " << root << "\n\n";

    std::cout << "Список чиновников\n";
    std::cout << "Номер | Взятка | Начальник\n";

    for (int i = 1; i <= count; i++) {
        std::cout << std::right << std::setw(5) << officials[i].getNumber()
                  << " | " << std::setw(6) << officials[i].getBribe()
                  << " | " << std::setw(9) << officials[i].getBoss() << "\n";
    }

    std::cout << "\nСтруктура министерства\n";
    printTreeFrom(root, "", true);
}

void Ministry::printTreeFrom(int officialNumber, const std::string& prefix, bool last) const {
    // вывод дерева
    bool rootNode = officialNumber == root;

    std::cout << prefix;
    if (!rootNode) {
        std::cout << (last ? "`-- " : "|-- ");
    }

    std::cout << officialNumber << " [взятка: " << officials[officialNumber].getBribe() << "]\n";

    int subordinateCount = officials[officialNumber].getSubordinateCount();
    for (int i = 0; i < subordinateCount; i++) {
        int child = officials[officialNumber].getSubordinate(i);
        std::string nextPrefix = prefix;
        if (!rootNode) {
            nextPrefix += last ? "    " : "|   ";
        }
        printTreeFrom(child, nextPrefix, i == subordinateCount - 1);
    }
}

void Ministry::solveAndPrint() const {
    if (!filled) {
        throw MinistryException("Ошибка: данные еще не загружены. Сначала выберите пункт 1, 2 или 3.");
    }

    int* costs = new int[count + 1];
    int* nextOfficials = new int[count + 1];

    for (int i = 1; i <= count; i++) {
        costs[i] = -1;
        nextOfficials[i] = 0;
    }

    int minimum = calculateMinCost(root, costs, nextOfficials);

    IntList path;
    savePath(root, nextOfficials, path);

    std::cout << "\nРешение\n";
    std::cout << "Выбранная цепочка подписей: ";
    for (int i = 0; i < path.getSize(); i++) {
        if (i > 0) {
            std::cout << " -> ";
        }
        std::cout << path.get(i);
    }
    std::cout << "\n";

    std::cout << "Минимальная сумма: " << minimum << " у.е.\n";
    std::cout << "Порядок получения подписей: ";
    for (int i = 0; i < path.getSize(); i++) {
        if (i > 0) {
            std::cout << " -> ";
        }
        std::cout << path.get(i);
    }
    std::cout << "\n";

    delete[] costs;
    delete[] nextOfficials;
}

void runKeyboardInput(Ministry& ministry) {
    ministry.inputFromKeyboard();
    ministry.printAll();
    ministry.solveAndPrint();
}

void runFileInput(Ministry& ministry) {
    std::string fileName;

    std::cout << "\nРабота с файлом\n";
    std::cout << "Формат файла:\n";
    std::cout << "1 строка: N\n";
    std::cout << "2 строка: N взяток\n";
    std::cout << "3 строка: N номеров начальников\n";

    std::cout << "\n1) Использовать существующий файл\n";
    std::cout << "2) Создать новый файл\n";
    int fileChoice = readIntegerInRange("Выберите действие (1-2): ", 1, 2);

    if (fileChoice == 1) {
        std::cout << "Имя файла: ";
        std::cin >> fileName;
    } else {
        std::cout << "Имя нового файла: ";
        std::cin >> fileName;

        std::cout << "\nЗаполнение файла\n";
        std::cout << "1) Ввести данные вручную\n";
        std::cout << "2) Заполнить случайно\n";
        int fillChoice = readIntegerInRange("Выберите действие (1-2): ", 1, 2);

        int count = 0;
        int* bribes = nullptr;
        int* bosses = nullptr;

        try {
            if (fillChoice == 1) {
                inputArraysManually(count, bribes, bosses);
            } else {
                count = readIntegerInRange(
                    "Количество чиновников N (целое число от 1 до 1000): ",
                    1,
                    1000
                );
                generateRandomArrays(count, bribes, bosses);
            }

            Ministry checkMinistry;
            checkMinistry.loadFromArrays(count, bribes, bosses);
            writeArraysToFile(fileName, count, bribes, bosses);
        } catch (...) {
            delete[] bribes;
            delete[] bosses;
            throw;
        }

        delete[] bribes;
        delete[] bosses;
    }

    ministry.inputFromFile(fileName);
    ministry.printAll();
    ministry.solveAndPrint();
}

void runRandomInput(Ministry& ministry) {
    std::cout << "\nСлучайное заполнение\n";
    std::cout << "Чиновник 1 будет главным.\n";
    std::cout << "Остальные чиновники получат случайных начальников.\n";
    int count = readIntegerInRange(
        "Количество чиновников N (целое число от 1 до 1000): ",
        1,
        1000
    );

    ministry.fillRandom(count);
    ministry.printAll();
    ministry.solveAndPrint();
}

void runPrintAndSolve(const Ministry& ministry) {
    ministry.printAll();
    ministry.solveAndPrint();
}
