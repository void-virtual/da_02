#include <ctime>
#include <random>
#include <map>
#include <limits>
#include <tuple>
#include <string>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "TPatricia.h"
#include "profile.h"

using namespace std;

default_random_engine rng;

uint64_t get_number(uint64_t min = 0,uint64_t max = numeric_limits<unsigned long long>::max()) {
    uniform_int_distribution<unsigned long long> dist_ab(min, max);
    return dist_ab(rng);
}

string get_string() {
    size_t string_size = get_number(1,256);
    string string;
    string.resize(string_size);
    for (size_t i = 0; i < string_size; ++i) {
        string[i] = 'a' + get_number(0,25);
    }
    return string;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        return 0;
    }
    size_t count = stoll(argv[1]);
    rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    vector<pair<string,unsigned long long>> test_data(count);

    {
        LOG_DURATION("Generate")
        for (size_t i = 0; i < count; ++i) {
            test_data[i].first = get_string();
            test_data[i].second = get_number(0, numeric_limits<unsigned long long>::max());
        }
        for (size_t i = 0; i < count; ++i) {
            cout << "+ " << test_data[i].first << " " << test_data[i].second << "\n";
        }
        std::shuffle(test_data.begin(), test_data.end(), rng);
        for (size_t i = 0; i < count; ++i) {
            cout << "+ " << test_data[i].first << " " << test_data[i].second << "\n";
        }
        std::shuffle(test_data.begin(), test_data.end(), rng);
        std::cout << "! Save tree_file\n";
        for (size_t i = 0; i < count; ++i) {
            cout << "- " << test_data[i].first << "\n";
        }
        std::shuffle(test_data.begin(), test_data.end(), rng);
        for (size_t i = 0; i < count; ++i) {
            cout << "- " << test_data[i].first << "\n";
        }
        std::cout << "! Load tree_file\n";
        for (size_t i = 0; i < count; ++i) {
            cout << test_data[i].first << "\n";
        }
    }
    return 0;
}

