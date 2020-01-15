#include <iostream>
#include <climits>
using namespace std;
int main ()
{
    int* a = new int (5);
    cout << a << endl;
    size_t k = (size_t) (size_t*) a;
    cout << k << endl;
    cout << (k << 1) << endl;
    cout << (k << 10) << '\n';
    cout << INT_MAX << endl;

}