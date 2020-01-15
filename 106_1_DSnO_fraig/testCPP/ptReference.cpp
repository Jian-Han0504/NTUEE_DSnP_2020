#include <iostream>
using namespace std;
int main() {
    int x = 5;
    int* a = &x;
    int* &b = a;

    cout << a << endl;
    cout << b << endl;
}