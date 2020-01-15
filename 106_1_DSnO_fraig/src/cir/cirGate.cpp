/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "../util/util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
unsigned CirGate::_globalref = 0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   int count = 0;
   string _gateType = getTypeStr();
   string _gateName = getGateName();
   cout << "================================================================================\n";
   cout << "= " << _gateType << "(" << _id << ")";
   count += _gateType.length() + to_string(_id).length() + 4;
   if (_gateName != "") {
      cout << "\"" << _gateName << "\"";
      count += _gateName.length() + 2;
   }
   cout << ", line " << _line << '\n';
   count += 7 + to_string(_line).length();
   
   // print FEC partners
   int columnCount = 1;
   cout << "= FECs:";
   for (size_t i = 0; i < _FECpartners.size(); ++i, ++columnCount) {
      cout << " ";
      if (_FECpartners[i].second) cout << "!";
      cout << _FECpartners[i].first;
      if (columnCount == 80) { cout << '\n'; columnCount = 0;}
   }
   cout << "\n";

   // print value
   int digitCount = 0;
   size_t tmPattern = _simDigit;
   cout << "= Value: ";
   for (; digitCount < 64; ++digitCount) {
      if (digitCount > 0 && digitCount % 8 == 0) cout << "_";
      cout << (tmPattern % 2);
      tmPattern /= 2;
   }
   cout << "\n================================================================================\n";
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   addGlobalRef ();
   reportFanIO (0, level, false, false);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   addGlobalRef ();
   reportFanIO (0, level, true, false);
}

void 
CirGate::reportFanIO (int level, const int& totLevel, bool isFwd, bool isInv) const
{
   // Forward: from PI to PO
   // Backward: from PO to PI
   const CirGate* tmp = this;
   if (isInv) cout << "!";
   this->printGate ();
   if (level == totLevel) { cout << '\n'; return;}
   else if (!isFwd) { // fanin
      if (getTypeStr() == "PI") { cout << endl; return;}
      else if (getTypeStr() == "CONST") { cout << endl; return;}
      else if (isGlobalRef()) { cout << " (*)\n"; return;}
      else { cout << '\n'; this->set2Global();}

      isInv = false;
      if (this->getTypeStr() == "PO") {
         assert (this->_fanins.size() <= 1);
         for (int i = 0; i < level+1; ++i) cout << "  ";
         if (this->getFanin1() % 2 == 1) isInv = true;
         if (this->_fanins.size() == 0) { // floating
            if (isInv) cout << "!";
            cout << "UNDEF " << this->getFanin1() / 2 << endl;
            return;
         }
         else { tmp = this->_fanins[0]; tmp->reportFanIO (level+1, totLevel, false, isInv);}
      }
      else if (getTypeStr() == "AIG") {
         assert (_fanins.size() <= 2);
         if (_fanins.size() == 0) {
            for (int i = 0; i < level+1; ++i) cout << "  ";
            if (this->getFanin1() % 2 == 1) cout << "!";
            cout << "UNDEF " << this->getFanin1 () / 2 << endl;
            for (int i = 0; i < level+1; ++i) cout << "  ";
            if (this->getFanin2() % 2 == 1) cout << "!";
            cout << "UNDEF " << this->getFanin2 () / 2 << endl;
            return;
         }
         else if (_fanins.size() == 1) {
            if (_fanins[0]->_id == (unsigned) getFanin1() / 2) {
               for (int i = 0; i < level+1; ++i) cout << "  ";
               if (this->getFanin1() % 2 == 1) isInv = true;
               tmp = this->_fanins[0]; 
               tmp->reportFanIO (level+1, totLevel, false, isInv);
               for (int i = 0; i < level+1; ++i) cout << "  ";
               if (this->getFanin2() % 2 == 1) cout << "!";
               cout << "UNDEF " << this->getFanin2 () / 2 << endl;
            }
            else {
               for (int i = 0; i < level+1; ++i) cout << "  ";
               if (this->getFanin1() % 2 == 1) cout << "!";
               cout << "UNDEF " << this->getFanin1 () / 2 << endl;
               for (int i = 0; i < level+1; ++i) cout << "  ";
               if (this->getFanin2() % 2 == 1) isInv = true;
               tmp = this->_fanins[0]; 
               tmp->reportFanIO (level+1, totLevel, false, isInv);
            }
         }
         else {
            assert (this->getFanin1() / 2 == (int) _fanins[0]->_id);
            assert (this->getFanin2() / 2 == (int) _fanins[1]->_id);
            isInv = false;
            for (int i = 0; i < level+1; ++i) cout << "  ";
            if (this->getFanin1 () % 2 == 1) isInv = true;
            tmp = this->_fanins[0]; 
            tmp->reportFanIO (level+1, totLevel, false, isInv);

            isInv = false;
            for (int i = 0; i < level+1; ++i) cout << "  ";
            if (this->getFanin2 () % 2 == 1) isInv = true;
            tmp = this->_fanins[1]; 
            tmp->reportFanIO (level+1, totLevel, false, isInv);
         }
      }
   }
   else { // fanout
      if (this->getTypeStr() == "PO") { cout << endl; return; }
      else if (level >= totLevel) { cout << endl; return;}
      else if (this->isGlobalRef()) { cout << " (*)\n"; return; }
      else { cout << endl; this->set2Global(); }
      
      isInv = false;
      if (this->_fanouts.size() == 0) return; // unused
      for (unsigned sus = 0; sus < _fanouts.size(); ++sus) {
         isInv = false;
         for (int i = 0; i < level+1; ++i) cout << "  ";
         tmp = this->_fanouts[sus];
         assert (tmp->getTypeStr() == "AIG" || tmp->getTypeStr() == "PO");
         if ((tmp->getFanin1() / 2 == (int) this->_id && tmp->getFanin1() % 2 == 1) ||
            (tmp->getFanin2() / 2 == (int) this->_id && tmp->getFanin2() % 2 == 1)) 
            isInv = true;
         tmp->reportFanIO (level+1, totLevel, true, isInv);
      }
   }
}
