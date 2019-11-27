#include <iostream>

using namespace std;

class Shit{
public:
    Shit() = default;
    Shit(int new_a) : a(new_a) {}

    int get() const {
        return a;
    }
private:
    int a;
};

int main() {
    Shit a(5),b(10);
    a = b;
    cout << a.get();
}
