#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <stdexcept>
#include <string>

class MinistryException : public std::runtime_error {
public:
    explicit MinistryException(const std::string& message);
};

class IntNode {
private:
    int value;
    IntNode* next;

public:
    explicit IntNode(int newValue);

    int getValue() const;
    IntNode* getNext() const;
    void setNext(IntNode* newNext);
};

class IntList {
private:
    IntNode* head;
    IntNode* tail;
    int size;

    void copyFrom(const IntList& other);

public:
    IntList();
    IntList(const IntList& other);
    ~IntList();

    IntList& operator=(const IntList& other);

    void pushBack(int value);
    void clear();
    int getSize() const;
    int get(int index) const;
};

class Official {
private:
    int number;
    int bribe;
    int boss;
    IntList subordinates;

public:
    Official();

    void setNumber(int value);
    void setBribe(int value);
    void setBoss(int value);
    void addSubordinate(int value);
    void clearSubordinates();

    int getNumber() const;
    int getBribe() const;
    int getBoss() const;
    int getSubordinateCount() const;
    int getSubordinate(int index) const;
};

class Ministry {
private:
    Official* officials;
    int count;
    int root;
    bool filled;

    void clear();
    void allocate(int newCount);
    void buildSubordinates();
    void validate();
    void checkCycles() const;
    void dfsCycle(int officialNumber, int* colors) const;
    void markReachable(int officialNumber, bool* visited) const;
    int calculateMinCost(int officialNumber, int* costs, int* nextOfficials) const;
    void savePath(int officialNumber, int* nextOfficials, IntList& path) const;
    void printTreeFrom(int officialNumber, const std::string& prefix, bool last) const;

public:
    Ministry();
    ~Ministry();

    void loadFromArrays(int newCount, const int* bribes, const int* bosses);
    void inputFromKeyboard();
    void inputFromFile(const std::string& fileName);
    void fillRandom(int newCount);

    bool isFilled() const;
    void printAll() const;
    void solveAndPrint() const;
};

void runKeyboardInput(Ministry& ministry);
void runFileInput(Ministry& ministry);
void runRandomInput(Ministry& ministry);
void runPrintAndSolve(const Ministry& ministry);
int readMenuChoice();

#endif
