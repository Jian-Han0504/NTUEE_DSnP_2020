#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include "BST.h"
//#define DEBUG_H

using namespace std;

int main (int argc, char* argv []) {
    BSTree* stdtree = 0;
    BSTree* spltree = 0;
    std::locale::global(std::locale(""));
    wifstream forest(argv[1]);
    if (argc == 1) {
        cerr << "Error: Missing input file!!\n";
        cout << "Usage: ./myBST <Input_Name>\n";
        return 0;
    }
    else if (argc > 2) {
        cerr << "Error: Extra arguments (" << argv[2] << ")!!\n";
        return 0;
    }
    else if (!forest) {
        cerr << "Error: Cannot open file " << argv[1] << "!!\n";
        return 0;
    }
    
    wstring wline;
    string line("");
    size_t count = 0;
    while (getline (forest, wline)) {
        line = "";
        for (size_t i = 0; i < wline.size(); ++i) {
            // cout << "We get utf-8 coded in (" << (int) wline[i] << ")\n";
            if ((int) wline[i] <= 127 && (int) wline[i] > 0) // ascii
                line += char (wline[i]);
            else {
                assert (((int) wline[i]) > 0);
                assert (((int) wline[i]) == 160); // UTF-8 space
                line += ' ';
            }
        }

        #ifdef DEBUG_H
        cout << "================= TREE =================\n";
        cout << "(" << line << ")\n";
        cout << "----------------------------------------\n";
        #endif // DEBUG_H
        
        ++count;
        if (line.length() == 0) {
            cerr << "Error: Tree is empty!!\n";
            return 0;
        }
        else if (line[line.length()-1] == 13) 
            line = line.substr (0, line.length()-1);
        
        stdtree = new BSTstd;
        if (!stdtree->readTree (line, count)) return 0;
        else if (!stdtree->writeTree ("./output/BTree")) return 0;
        delete stdtree;
        
        spltree = new BSTsplay;
        if (!spltree->readTree (line, count)) return 0;
        else if (!spltree->writeTree ("./output/STree")) return 0;
        delete spltree;
    }

    return 0;
}

/* REFERENCE
#include <fstream>
#include <iostream>
#include <locale>
int main()
{
    std::locale::global(std::locale("")); // activate user-preferred locale, in my case en_US.utf8
    std::wifstream wf("test.txt"); // test.txt contains utf-8 text
    for(wchar_t c; wf.get(c); )
        std::wcout << "Processed character " << c << '\n';
}
*/