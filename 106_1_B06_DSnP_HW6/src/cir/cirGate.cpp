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

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_globalref = 0;

void
CirGate::reportGate() const
{
   int count = 0;
   cout << "==================================================\n";
   cout << "= " << _gateType << "(" << _gateID << ")";
   count += _gateType.length() + to_string(_gateID).length() + 4;
   if (_gateName != "") {
      cout << "\"" << _gateName << "\"";
      count += _gateName.length() + 2;
   }
   cout << ", line " << _gateLine;
   count += 7 + to_string(_gateLine).length();
   if (count < 50) cout << setw(50-count+1) << "=\n" << setw(0);
   cout << "==================================================\n";
}

void
CirGate::reportFanin (int level) const
{
   assert (level >= 0);
   addGlobalRef ();
   reportGateCone ("IN", 0, level);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   addGlobalRef ();
   reportGateCone ("OUT", 0, level);
}

void CirGate::reportGateCone (const string& dir, const int& cur, const int& tot, bool invert) const
{
   bool fanInv = false;
   const CirGate* tmp = this;

   #ifdef DEBUG_H
   cout << "\n==========================================\n";
   cout << "GATE TYPE: " << this->getTypeStr() << endl;
   cout << "level: " << tot << ", current: " << cur << endl;
   cout << "Pre (" << this->predecess.size() << "):";
   for (unsigned i = 0; i < this->predecess.size(); ++i) cout << " " << predecess[i]->_gateID;
   cout << "\nSus (" << this->successor.size() << "):";
   for (unsigned i = 0; i < this->successor.size(); ++i) cout << " " << successor[i]->_gateID;
   cout << endl;
   cout << "==========================================\n";
   #endif // DEBUG_H

   // print current gate
   if (invert) cout << "!";
   this->printGate();

   // need to call recursive
   if (dir == "IN") { // fanin
      if (this->getTypeStr() == "PI") { cout << endl; return; }
      else if (this->getTypeStr() == "CONST") { cout << endl; return; }
      else if (cur >= tot) { cout << endl; return;}
      else if (this->isGlobalRef()) {cout << " (*)\n"; return; }
      else { cout << endl; this->set2Global(); }

      if (this->getTypeStr() == "PO") {
         assert (this->predecess.size() <= 1);
         for (int i = 0; i < cur+1; ++i) cout << "  ";
         if (this->getFanin1() % 2 == 1) fanInv = true;
         if (this->predecess.size() == 0) { // floating
            if (fanInv) cout << "!";
            cout << "UNDEF " << this->getFanin1() / 2 << endl;
            return;
         }
         else { tmp = this->predecess[0]; tmp->reportGateCone ("IN", cur+1, tot, fanInv);}
      }
      else if (this->getTypeStr() == "AIG") {
         assert (this->predecess.size() <= 2);
         if (this->predecess.size() == 0) { // both floating
            for (int i = 0; i < cur+1; ++i) cout << "  ";
            if (this->getFanin1() % 2 == 1) cout << "!";
            cout << "UNDEF " << this->getFanin1 () / 2 << endl;
            for (int i = 0; i < cur+1; ++i) cout << "  ";
            if (this->getFanin2() % 2 == 1) cout << "!";
            cout << "UNDEF " << this->getFanin2 () / 2 << endl;
            return;
         }
         else if (this->predecess.size() == 1) {
            fanInv = false;
            if (this->predecess[0]->getID() == (unsigned) this->getFanin1()) {
               for (int i = 0; i < cur+1; ++i) cout << "  ";
               if (this->getFanin1() % 2 == 1) fanInv = true;
               tmp = this->predecess[0]; 
               tmp->reportGateCone ("IN", cur+1, tot, fanInv);
               for (int i = 0; i < cur+1; ++i) cout << "  ";
               if (this->getFanin2() % 2 == 1) cout << "!";
               cout << "UNDEF " << this->getFanin2 () / 2 << endl;
            }
            else {
               for (int i = 0; i < cur+1; ++i) cout << "  ";
               if (this->getFanin1 () % 2 == 1) cout << "!";
               cout << "UNDEF " << this->getFanin1 () / 2 << endl;
               for (int i = 0; i < cur+1; ++i) cout << "  ";
               if (this->getFanin2() % 2 == 1) fanInv = true;
               tmp = this->predecess[0]; 
               tmp->reportGateCone ("IN", cur+1, tot, fanInv);
            }
         }
         else if (this->predecess.size() == 2) {
            assert (this->getFanin1() / 2 == (int) predecess[0]->_gateID);
            assert (this->getFanin2() / 2 == (int) predecess[1]->_gateID);
            fanInv = false;
            for (int i = 0; i < cur+1; ++i) cout << "  ";
            if (this->getFanin1 () % 2 == 1) fanInv = true;
            tmp = this->predecess[0]; 
            tmp->reportGateCone ("IN", cur+1, tot, fanInv);

            fanInv = false;
            for (int i = 0; i < cur+1; ++i) cout << "  ";
            if (this->getFanin2 () % 2 == 1) fanInv = true;
            tmp = this->predecess[1]; 
            tmp->reportGateCone ("IN", cur+1, tot, fanInv);
         }
      }
   }
   else if (dir == "OUT")
   {
      if (this->getTypeStr() == "PO") { cout << endl; return; }
      else if (cur >= tot) { cout << endl; return;}
      else if (this->isGlobalRef()) { cout << " (*)\n"; return; }
      else { cout << endl; this->set2Global(); }

      if (this->successor.size() == 0) return; // unused
      for (unsigned sus = 0; sus < successor.size(); ++sus) {
         fanInv = false;
         for (int i = 0; i < cur+1; ++i) cout << "  ";
         tmp = this->successor[sus];
         assert (tmp->getTypeStr() == "AIG" || tmp->getTypeStr() == "PO");
         if ((tmp->getFanin1() / 2 == (int) this->_gateID && tmp->getFanin1() % 2 == 1) ||
            (tmp->getFanin2() / 2 == (int) this->_gateID && tmp->getFanin2() % 2 == 1)) 
            fanInv = true;
         tmp->reportGateCone ("OUT", cur+1, tot, fanInv);
      }
   }
}
