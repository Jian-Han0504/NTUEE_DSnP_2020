#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
using namespace std;
bool myComp (vector<int>* v1, vector<int>* v2) {
    return v1->at(0) < v2->at(0);
}

struct list
{
    vector<int>* _vec;
    bool operator< (const list& l2) const {
        assert (_vec->at(0) != 0 && l2._vec->at(0) != 0);
        return _vec->at(0) < l2._vec->at(0);
    }
} myCompList;

int main() {
    vector<vector<int>*> _vec;
    vector<int>* _inV;
    for (int i = 5; i >= 0; --i) {
        _inV = new vector<int>;
        for (int j = 10; j >= 0; --j)
            _inV->push_back (i*j + 1);
        _vec.push_back (_inV);
        _inV = 0;
    }

    for (size_t i = 0; i < _vec.size(); ++i) {
        for (size_t j = 0; j < _vec[i]->size(); ++j)
            cout << _vec[i]->at(j) << " ";
        cout << endl;
    }
    std::sort (_vec.begin(), _vec.end(), myComp);

    cout << "After sorted\n";
    for (size_t i = 0; i < _vec.size(); ++i) {
        for (size_t j = 0; j < _vec[i]->size(); ++j)
            cout << _vec[i]->at(j) << " ";
        cout << endl;
    }
}
