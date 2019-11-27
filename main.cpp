#include <iostream>
#include <fstream>
#include <cstring>

#include "TPatricia.h"

TVector<unsigned char> strToVec(const char* str) {
    TVector<unsigned char> result;
    for (int i = 0; str[i] != '\0'; ++i) {
        result.PushBack(std::tolower(str[i]));
    }
    return result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    TPatricia<unsigned long long> patr;
    char command[257];
    while (std::cin >> command) {
        if (command[0] == '+') {
            std::cin >> command;
            TVector<unsigned char> str = strToVec(command);
            unsigned long long data;
            std::cin >> data;
            if (patr.Insert(std::move(str), data)) {
                std::cout << "OK\n";
            } else {
                std::cout << "Exist\n";
            }
        } else if (command[0] == '-') {
            std::cin >> command;
            TVector<unsigned char> str = strToVec(command);
            if (patr.Erase(str)) {
                std::cout << "OK\n";
            } else {
                std::cout << "NoSuchWord\n";
            }
        } else if (command[0] == '!') {
            std::cin >> command;
            if (strcmp(command,"Save") == 0) {
                std::cin >> command;
                patr.PrintToFile(command);
                std::cout << "OK" << "\n";
            } else if (strcmp(command,"Load") == 0) {
                std::cin >> command;
                patr.ScanFromFile(command);
                std::cout << "OK" << "\n";
            }
        } else {
            TVector<unsigned char> str = strToVec(command);
            TOptional<unsigned long long> opt = patr[str];
            if (opt) {
                std::cout << "OK: " << *opt << "\n";
            } else {
                std::cout << "NoSuchWord\n";
            }
        }
    }
    return 0;
}