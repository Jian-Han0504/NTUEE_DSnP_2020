#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int*> _vec;
    for (int i = 0; i < 5; ++i) {
        _vec.push_back (&i);
        cout << (_vec[i]) << endl;
    }

    for (vector<int*>::iterator it = _vec.begin(); it != _vec.end(); ++it) {
        cout << (*it) << endl;
    }
    
}