#include <iostream>
#include <vector>
using namespace std;
class myClass
{
    public:
    myClass (int a) : _a(a) {}
    myClass () { _a = -1;}
    virtual int geta () { return _a;}
    int _a;

    protected:
    int _c;
};

class child : public myClass {
    public:
    child (int a) : _c(a) {}
    int geta () { return _c;}
};

int main ()
{
    myClass* me;
    vector<myClass*> _vec;
    for (int i = 0; i < 5; ++i) {
        me = new child (i);
        _vec.push_back (me);
        me = 0;
    }

    for (vector<myClass*>::iterator it = _vec.begin(); it != _vec.end(); ++it) {
        cout << (*it)->geta() << endl;
    }
}