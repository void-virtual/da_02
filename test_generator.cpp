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

TVector<unsigned char> convert_string(const string& s) {
    TVector<unsigned char> vector(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        vector[i] = s[i];
    }
    return vector;
}

string convert_string(TVector<unsigned char>& vec) {
    string str;
    str.resize(vec.Size());
    for (size_t i = 0; i < str.size(); ++i) {
        str[i] = vec[i];
    }
    return str;
}

int main() {
    rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    size_t count;
    cin >> count;
    vector<pair<TVector<unsigned char>, unsigned long long>> test_data(count);
    fstream test_data_log("file.test", ios::out);
    {
    LOG_DURATION("Generate")
        for (size_t i = 0; i < count; ++i) {
            test_data[i].first = convert_string(get_string());
            test_data[i].second = get_number(0, numeric_limits<unsigned long long>::max());
            test_data_log << test_data[i].first << " " << test_data[i].second << "\n";
        }
    }

    cout << "Patricia test\n";
    TPatricia<unsigned long long> patricia;
    {
        LOG_DURATION("Insert time")
        for (size_t i = 0; i < count; ++i) {
            patricia.Insert(test_data[i].first, test_data[i].second);
        }
    }

    {
         LOG_DURATION("Save to file")
         patricia.PrintToFile("file");
    }

    std::shuffle(test_data.begin(), test_data.end(), rng);
    {
        LOG_DURATION("Write and erase one element")
        for (size_t i = 0; i < count; ++i) {
            patricia.Erase(test_data[i].first);
            patricia.Insert(test_data[i].first, test_data[i].second);
        }
    }
    {
        LOG_DURATION("Erase time")
        for (size_t i = 0; i < count; ++i) {
            patricia.Erase(test_data[i].first);
        }
    }
    {
        LOG_DURATION("Erase empty time")
        for (size_t i = 0; i < count; ++i) {
            patricia.Erase(test_data[i].first);
        }
    }

    {
        LOG_DURATION("Scan from file")
        patricia.ScanFromFile("file");
    }


    vector<pair<string, unsigned long long>> map_test(test_data.size());
    for (size_t i = 0; i < map_test.size(); ++i) {
        map_test[i] = {convert_string(test_data[i].first), test_data[i].second};
    }
    cout << "std::map test\n";
    map<string, unsigned long long> map;
    {
        LOG_DURATION("Insert time")
        for (size_t i = 0; i < count; ++i) {
            map[map_test[i].first] = map_test[i].second;
        }
    }

    std::shuffle(test_data.begin(), test_data.end(), rng);

    {
        LOG_DURATION("Write and erase one element")
        for (size_t i = 0; i < count; ++i) {
            map.erase(map_test[i].first);
            map[map_test[i].first] = map_test[i].second;
        }
    }
    {
        LOG_DURATION("Erase time")
        for (size_t i = 0; i < count; ++i) {
            map.erase(map_test[i].first);
        }
    }
    {
        LOG_DURATION("Erase empty time")
        for (size_t i = 0; i < count; ++i) {
            patricia.Erase(test_data[i].first);
        }
    }

    return 0;
}

//1000000
//Insert time: 4195 ms
//Erase time: 5034 ms
//Erase empty time: 23 ms
