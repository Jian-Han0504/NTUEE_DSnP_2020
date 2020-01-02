#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <locale>
#include <iomanip>
using namespace std;

int main()
{
    ofstream file ("out.txt");
    string c = "My\u00A0Wo";
    //wcout << setw(5) << i << endl;
    file << setfill ('\u00A0');
    file << std::left << setw(10) << c;
    file.close();

    std::locale::global(std::locale("")); 
    wstring s;
    wifstream ifile ("out.txt");
    wcout << "gf\n";
    getline (ifile, s);
    for (size_t i = 0; i < s.length(); ++i)
        wcout << "We get (" << (int) s[i] << ") -" << s[i] << "-\n";

}
