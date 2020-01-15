#include <iostream>
#include <climits>
using namespace std;
int main () {
    size_t in1;
    size_t in2;
    while (true) {
        cout << "input 1: ";
        cin >> in1;
        cout << "input 2: ";
        cin >> in2;
        if (in1 == in2) cout << "They are the same!\n";
        else if (in1 == (in2^ULLONG_MAX)) cout << "Inverted inputs!\n";
        else {
            cout << "They are different!\n";
            break;
        }
    }
    return 0;
    
}