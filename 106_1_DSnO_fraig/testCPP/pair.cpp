#include <iostream>
using namespace std;
int main()
{
    typedef pair<int, int> Human;
    Human Joyce (155, 40);
    Human Hank (178, 72);
    cout << (Joyce.first == Hank.first) << endl;
}