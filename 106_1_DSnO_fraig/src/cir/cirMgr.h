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

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() { _vgate.reserve(64);}
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      if (gid >= _vgate.size() || gid < 0) return 0;
      else return _vgate[gid]; 
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
   ofstream           *_simLog;
   int* PILine2ID;

   // read files M I L O A
   int headerInfo [5];
   vector<CirGate*> _vgate; // usage: store ID as index
   mutable vector<CirGate*> _DFSTrace;
   mutable vector<int> _nilfaninGate;
   mutable vector<int> _notusedGate;
   void ConnectGate();
   void DFSTrace (CirGate*, bool);

   // opt used
   void resetFanin (CirGate*, CirGate*);
   void resetFanout (CirGate*, CirGate*);

   // simulate used
   void printSim2File (ofstream*);
   bool parsePatternFile (ifstream&, size_t&);
   bool rndgenPattern (int& patterNum);
   size_t simDFSList (CirGate*);
   vector <vector<CirGate*>*> _FECgrpList;

};

bool myVecomp (vector<CirGate*>*, vector<CirGate*>*);

#endif // CIR_MGR_H
