#include "TPatricia.h"

void printChar(unsigned char a) {
    for (int i = BITS_IN_CHAR - 1; i >= 0; --i) {
        std::cout << ((a & (1 << i)) != 0);
    }
}

bool getNthBit(const TVector<unsigned char>& lhs, size_t index) {
    if (index / BITS_IN_CHAR >= lhs.Size()) {
        return false;
    }
    unsigned char cur_char = lhs[index / BITS_IN_CHAR];
    return (cur_char & (MAX_LEFT_ONE >> (index % BITS_IN_CHAR))) != 0;
    //true <=> 1, false <=> 0
}