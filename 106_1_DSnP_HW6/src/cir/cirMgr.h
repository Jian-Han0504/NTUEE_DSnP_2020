/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      if (gid >= _vgate.size() || gid < 0) return 0;
      else return _vgate [gid]; 
    }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
    // read files
    int headerInfo [5];
    vector<CirGate*> _vgate; // usage: store ID as index

    // connect circuit
    void connectCircuit ();
    bool DFSvisit (const int&);
    bool _isCONST; // is const push into DFS trace?
    void DFSfloat (const int&);
    
    // for gate report
    void createList ();

    vector<int> _DFSTrace; // store successful DFS gate IDs
    vector<int> _notusedGate; // store gate id whose fanout is not used
    vector<int> _nilfaninGate; // store gate id whose fanin is not used
    int* line2ID; // PI starts from line 2
};

#endif // CIR_MGR_H
