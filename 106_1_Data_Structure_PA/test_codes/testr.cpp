#include <fstream>
#include <iostream>
#include <locale>
#include <string>
using namespace std;
int main(int argc, char* argv [])
{
    std::locale::global(std::locale("")); // activate user-preferred locale, in my case en_US.utf8
    std::wifstream wf("test.txt"); // test.txt contains utf-8 text
    wstring s;
    wchar_t space = ' ';
    string cs ("");
    while (getline(wf, s)) {
        
        for (size_t i = 0; i < s.length(); ++i) {
            cout <<  "Processed character (" << s[i] << ")\n";
            char c;
            cout << (int) s[i] << endl;
            if ((int) s[i] <= 127) {
                c = char (s[i]);

            }
            else c = ' ';
            cout << "We get char (" << c << ")\n";
            cs += c;
        }
        
        cout << "Final: (";
        for (size_t i = 0; i < cs.length(); ++i) {
            cout << cs[i];
        }
        cout << ")\n";
    }
    
    /*for(wchar_t c; wf.get(c); ) {
        std::wcout << "Processed character (" << c << ")\n";
        if (iswspace(c)) "\t-> THIS IS A WHITE SPACE!\n"; 
    }*/
        
}