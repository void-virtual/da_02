#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "TQueue.h"
#include "TVector.h"
#include "HeapSort.h"

using namespace std;

int main() {
    TVector<int> vector;
    vector.PushBack(5);
    vector.PushBack(1);
    vector.PushBack(65);
    vector.PushBack(22);
    vector.PushBack(10);
    vector.PushBack(32);
    vector.PushBack(12);
    vector.PushBack(25);

    HeapSort(vector);
    for (size_t i = 0; i < vector.Size(); ++i) {
        cout << vector[i] << " ";
    }

    return 0;
}