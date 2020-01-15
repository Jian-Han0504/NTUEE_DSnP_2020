/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "../util/util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

#define PRINTALLFEC 0

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream oFile (fileName);
   if (!oFile) {
      cerr << "Cannot open design \"" << fileName << "\"!!\n";
      return false;
   }

   // position for input file
   size_t pos = 0;
   size_t newpos = pos;
   
   // check line
   string fstLine ("");
   if (!getline (oFile, fstLine)) {
      lineNo = 0;
      errMsg = "aag";
      parseError (MISSING_IDENTIFIER);
      return false;
   }
   else fstLine += " ";

   // read header
   // lineNo = 0, colNo = pos
   string token ("");
   lineNo = 0;
   newpos = myStrGetTok (fstLine, token, pos);

   if (token != "aag") { // not aag identifier
      errMsg = token;
      parseError (ILLEGAL_IDENTIFIER);
      return false;
   }
   else if (newpos > 3) { // extra space
      colNo = pos;
      parseError (EXTRA_SPACE);
      return false;
   }
   else {
      for (int i = 0; i < 5; ++i) {
         if (fstLine [newpos+1] == ' ') { // extra space before number
            colNo = newpos+1;
            parseError (EXTRA_SPACE);
            return false;
         }

         // get integer five times
         pos = ++newpos;
         newpos = myStrGetTok (fstLine, token, pos);
         colNo = newpos+1;
         errMsg = token;

         if (i < 4 && (newpos+1 == fstLine.length())) { // missing var num
            errMsg = "number of variables";
            parseError (MISSING_NUM);
            return false;
         }
         else if (!myStr2Int (token, headerInfo [i]) || headerInfo [i] < 0) { // check number legal
            errMsg = token;
            parseError (ILLEGAL_NUM);
            return false;
         }
      }

      if (fstLine [newpos+1] == ' ') { // extra space after the last num
         colNo = newpos+1;
         parseError (EXTRA_SPACE);
         return false;
      }
      else { // if still has token
         myStrGetTok (fstLine, token, newpos+1);
         if (token != "")
         {
            colNo = newpos;
            parseError (MISSING_NEWLINE);
            return false;
         }
      }

      // M >= I + L + S
      errMsg = to_string (headerInfo [0]);
      if (headerInfo[0] < headerInfo[1] + headerInfo[2] + headerInfo[4]) {
         errInt = headerInfo[0];
         errMsg = "Number of varaibles";
         parseError (NUM_TOO_SMALL);
         return false;
      }
   }

   // create const gate
   CirGate* constGate = new CirCONSTGate (0, 0);
   assert (_vgate.empty());
   _vgate.push_back (constGate);

   // read input
   int gateID = 0;
   for (int i = 0; i < headerInfo [1]; ++i) 
   {
      lineNo = 1 + i;
      colNo = 0;
      fstLine = "";
      if (!getline (oFile, fstLine)) {
         parseError (MISSING_NEWLINE);
         return false;
      }
      
      token = "";
      pos = newpos = 0;
      newpos = myStrGetTok (fstLine + " ", token, pos);
   
      if (token.empty()) { // missing gate definition
         errMsg = "PI";
         parseError (MISSING_DEF);
         return false;
      }
      else if (fstLine[pos] == ' ') { // extra space at start
         colNo = pos;
         parseError (EXTRA_SPACE);
         return false;
      }
      else if (newpos != fstLine.length()) { // extra space at end
         colNo = newpos;
         parseError (EXTRA_SPACE);
         return false;
      }
      else if (!myStr2Int (token, gateID) || gateID < 0) { // invalid number
         errMsg = token;
         parseError (ILLEGAL_NUM);
         return false;
      }
      else if (gateID == 0 || gateID == 1) { // redefine const 
         errInt = gateID;
         parseError (REDEF_CONST);
         return false;
      }
      else if (gateID % 2 == 1) { // cannot be inverted
         errMsg = "PI";
         errInt = gateID;
         parseError (CANNOT_INVERTED);
         return false;
      }
      else if (gateID > headerInfo [0]*2 + 1) { // ID too large
         colNo = pos;
         errInt = gateID;
         parseError (MAX_LIT_ID);
         return false;
      }

      gateID /= 2;
      CirGate* tmpGate = new CirPIGate (gateID, lineNo+1);
      if ((unsigned) gateID < _vgate.size() && _vgate[gateID] != 0) { // re-define
         errInt = gateID * 2;
         errGate = _vgate[gateID];
         parseError (REDEF_GATE);
         return false;
      }
      else if ((unsigned) gateID >= _vgate.size()) 
         while ((unsigned) gateID >= _vgate.size()) {_vgate.push_back (0);}
      
      _vgate [gateID] = tmpGate;
   }
   
   // read output
   int fanin = 0;
   for (int i = 0; i < headerInfo[3]; ++i)
   {
      ++lineNo;
      colNo = 0;
      fstLine = "";
      if (!getline (oFile, fstLine)) {
         parseError (MISSING_NEWLINE);
         return false;
      }

      token = "";
      pos = newpos = 0;
      newpos = myStrGetTok (fstLine + " ", token, pos);
      
      if (token.empty()) { // missing gate definition
         errMsg = "PO";
         parseError (MISSING_DEF);
         return false;
      }
      else if (fstLine[pos] == ' ') { //extra space at start
         colNo = pos;
         parseError (EXTRA_SPACE);
         return false;
      }
      else if (newpos != fstLine.length()) { // extra space at end
         colNo = newpos;
         parseError (EXTRA_SPACE);
         return false;
      }
      else if (!myStr2Int (token, fanin) || fanin < 0) { // invalid number
         errMsg = token;
         parseError (ILLEGAL_NUM);
         return false;
      }
      else if (fanin > 2*headerInfo[0] + 1) { // fanin too large
         errInt = fanin;
         parseError (NUM_TOO_BIG);
         return false;
      }

      gateID = headerInfo [0] + 1 + i;
      CirGate* tmpGate = new CirPOGate (gateID, lineNo+1, fanin);
      while ((unsigned) gateID >= _vgate.size()) _vgate.push_back (0);
      _vgate [gateID] = tmpGate;
   }

   // read AND gate
   int fanin1 = 0, fanin2 = 0, fanout = 0;
   for (int i = 0; i < headerInfo[4]; ++i)
   {
      ++lineNo;
      colNo = 0;
      fstLine = "";
      if (!getline (oFile, fstLine)) {
         parseError (MISSING_NEWLINE);
         return false;
      }

      pos = newpos = 0;
      fanout = fanin1 = fanin2 = 0;

      for (int j = 0; j < 3; ++j)
      {
         token = "";
         newpos = myStrGetTok (fstLine + " ", token, pos);

         if (token.empty()) { // missing gate definition
            errMsg = "AIG";
            parseError (MISSING_DEF);
            return false;
         }
         else if (fstLine[pos] == ' ') { // extra space before fan in/out
            colNo = pos;
            parseError (EXTRA_SPACE);
            return false;
         }
         else if (j == 2 && newpos < fstLine.length()) { // extra space at end
            colNo = pos;
            parseError (EXTRA_SPACE);
            return false;
         }
         else if (((j == 0 && !myStr2Int (token, fanout)) || fanout < 0) ||
                  ((j == 1 && !myStr2Int (token, fanin1)) || fanin1 < 0) ||
                  ((j == 2 && !myStr2Int (token, fanin2)) || fanin2 < 0)) { // invalid fan num
            errMsg = token;
            parseError (ILLEGAL_NUM);
            return false;
         }
         else if ((j == 0 && fanout <= 1)) { // redefine const
            colNo = pos;
            errInt = fanout;
            parseError (REDEF_CONST);
            return false;
         } 
         else if (j == 0 && fanout % 2 != 0) { // fanout cannot invert
            errMsg = "AIG";
            errInt = fanout;
            parseError (CANNOT_INVERTED);
            return false;
         }
         else if (j == 0 && fanout > 2 * headerInfo [0] + 1) { // number too large
            errInt = fanout;
            colNo = pos;
            parseError (MAX_LIT_ID);
            return false;
         }
         else if (j == 1 && fanin1 > 2 * headerInfo [0] + 1) { // number too large
            errInt = fanin1;
            colNo = pos;
            parseError (MAX_LIT_ID);
            return false;
         }
         else if (j == 2 && fanin2 > 2 * headerInfo [0] + 1) { // number too large
            errInt = fanin2;
            colNo = pos;
            parseError (MAX_LIT_ID);
            return false;
         }

         pos = newpos+1;
      }

      // change fanout to ID format
      gateID = fanout /= 2;
      CirGate* tmpGate = new CirANDGate (gateID, lineNo+1, fanin1, fanin2);

      if ((unsigned) gateID < _vgate.size() && _vgate [gateID] != 0) { // re-define
         errInt = gateID * 2;
         errGate = _vgate[gateID];
         parseError (REDEF_GATE);
         return false;
      }
      else if ((unsigned) gateID >= _vgate.size())
         while ((unsigned) gateID >= _vgate.size()) _vgate.push_back (0);

      _vgate [gateID] = tmpGate;
   }

   // if some of the gates haven't been specified
   while ((int) _vgate.size() < headerInfo[0]+1) _vgate.push_back(0);
   #ifdef DEBUG_H
   cout << "================ AIG readin ==================\n";
   cout << "_vgate size: " << _vgate.size() << endl;;
   for (unsigned i = 0; i < _vgate.size(); ++i) {
      if (_vgate [i] == 0) cout << "no element\n";
      else 
      {
         cout << "ID: " << i << ", type: " << _vgate[i]->getTypeStr ()
              << ", line number: " << _vgate[i]->getLineNo() << ", ";
         _vgate[i]->BUG_printGate();
      }  
   }
   cout << "===============================================\n";
   cout << "Ready to read symbols...\n";
   #endif // DEBUG_H

   // read symbols
   fstLine = "";
   string stokenNum ("");
   string gateName ("");
   int ntokenNum = 0;
   bool isComment = false;
   while (1) 
   {
      ++lineNo;
      colNo = 0;
      if (!getline (oFile, fstLine)) break;
      else if (fstLine == "") { // null line
         colNo = 0;
         errMsg = fstLine;
         parseError (ILLEGAL_SYMBOL_TYPE);
         return false;
      }

      pos = newpos = 0;
      token = "";
      stokenNum = "";
      gateName = "";
      ntokenNum = 0;
      newpos = myStrGetTok (fstLine + " ", token, pos);
      if (token == "c") { isComment = true; break;}
      else if (token == "") { 
         if (fstLine.length() != 0) { // extra space
            colNo = 0;
            parseError (EXTRA_SPACE);
            return false;
         }
         else { // illegal symbol type
            colNo = 0;
            errMsg = token;
            parseError (ILLEGAL_SYMBOL_TYPE);
            return false;
         }
      }
      else if (fstLine[0] == ' ') { // extra space
         colNo = 0;
         parseError (EXTRA_SPACE);
         return false;
      }
      else if (token[0] != 105 && token[0] != 111) { // illegal symbol type
         colNo = 0;
         errMsg = token;
         parseError (ILLEGAL_SYMBOL_TYPE);
         return false;
      }

      stokenNum = token.substr (1, token.length()-1);
      if (!myStr2Int (stokenNum, ntokenNum) || ntokenNum < 0) { // illegal symbol #
         colNo = 1;
         errMsg = stokenNum;
         parseError (ILLEGAL_SYMBOL_TYPE);
         return false;
      }
      else if ((int) token[0] == 105 && ntokenNum >= headerInfo[1]) { // PI number too big
         colNo = 1;
         errMsg = "PI index";
         errInt = ntokenNum;
         parseError (NUM_TOO_BIG);
         return false;
      }
      else if ((int) token[0] == 111 && ntokenNum >= headerInfo[3]) { // PO number too big 
         colNo = 1;
         errMsg = "PO index";
         errInt = ntokenNum;
         parseError (NUM_TOO_BIG);
         return false;
      }
      else if (fstLine.length() <= 3) { // missing identifier
         errMsg = "symbolic name";
         parseError(MISSING_IDENTIFIER);
         return false;
      }

      gateName = fstLine.substr (newpos + 1, fstLine.length() - 3);
      if (gateName == "") { // invalid symbol for gate name 
         errMsg = "symbolic name";
         parseError(MISSING_IDENTIFIER);
         return false;
      }
      else {
         for (unsigned i = 0; i < gateName.length(); ++i) { // illegal symbol name
            if ((int) gateName[i] < 32 || (int) gateName[i] == 127) {
               colNo = i + 3;
               errInt = (int) gateName[i];
               parseError (ILLEGAL_SYMBOL_NAME);
               return false;
            }  
         }
      }

      if ((int) token[0] == 105) // PIs
      { 
         int count = 0; // count pi
         int index = 0;
         ++ntokenNum; // i0 means symbol for PI 1
         while (count < ntokenNum) 
         {
            ++index;
            if (_vgate [index] == 0) continue;
            else if (_vgate[index]->getTypeStr() != "PI") continue;
            else ++count;
         }

         if (_vgate[index]->getGateName() != "") { // redefine...
            errMsg = "i";
            errInt = ntokenNum-1;
            parseError (REDEF_SYMBOLIC_NAME);
            return false;
         }
         else _vgate[index]->setGateName (gateName);
      }
      else if ((int) token[0] == 111) { // POs
         if (_vgate[headerInfo[0]+1+ntokenNum]->getGateName() != "") { // redefine
            errMsg = "o";
            errInt = ntokenNum;
            parseError (REDEF_SYMBOLIC_NAME);
            return false;
         }
         _vgate[headerInfo[0]+1+ntokenNum]->setGateName (gateName);
      }
   }

   // construct line to ID for PIs -> used in cirw command
   // PI starts from line 2
   int* tmpPILine2ID = new int [headerInfo[0]+2];
   for (int i = 0; i < headerInfo[0]+2; ++i) tmpPILine2ID [i] = 0;
   int PIcount = 0; 
   for (int i = 0; i <= headerInfo[0]; ++i) {
      if (_vgate[i] == 0) continue;
      else if (_vgate[i]->getTypeStr() != "PI") continue;
      else tmpPILine2ID [_vgate[i]->getLineNo()] = i;
      if (++PIcount >= headerInfo[1]) break;
   }
   assert (PIcount == headerInfo[1]);

   PIcount = 0;
   PILine2ID = new int [headerInfo[1]];
   for (int i = 0; i < headerInfo[0]+2; ++i) {
      if (tmpPILine2ID[i] == 0) continue;
      else {
         PILine2ID [PIcount] = tmpPILine2ID[i];
         ++PIcount;
      }
   }
   assert (PIcount == headerInfo[1]);
   delete [] tmpPILine2ID;

   // comment format
   if (isComment) {
      if (fstLine[0] == ' ') { // extra space
         colNo = 0;
         parseError (EXTRA_SPACE);
         return false;
      }
      else if (fstLine.length() != 1) { // need newline
         colNo = 1;
         parseError (MISSING_NEWLINE);
         return false;
      }
   }
   ConnectGate();

   return true;
}

void CirMgr::ConnectGate ()
{  
   int faninID = 0;
   assert (headerInfo[0] + headerInfo[3] >= 0);
   //cout << "Start connecting...\n";
   for (size_t i = 0; i < (size_t) (headerInfo[0] + headerInfo[3] + 1); ++i) {
      if (_vgate[i] == 0) continue;
      //else cout << "Connecting gate: (" << _vgate[i]->getId() << ") type: " << _vgate[i]->getTypeStr() << endl;
      
      faninID = _vgate[i]->getFanin1() / 2;
      if (faninID >= 0 && _vgate[faninID] != 0) {
         _vgate[i]->setFanins ( _vgate[faninID]);
         _vgate[faninID]->setFanouts (_vgate[i]);
         //cout << "  to gate " << faninID << endl;
      }

      faninID = _vgate[i]->getFanin2() / 2;
      if (faninID >= 0 && _vgate[faninID] != 0) {
         _vgate[i]->setFanins ( _vgate[faninID]);
         _vgate[faninID]->setFanouts (_vgate[i]);
         //cout << "  to gate " << faninID << endl;
      }
   }

   //cout << "Creating DFS list...\n";
   CirGate* tmp = 0;
   for (int i = 1; i <= headerInfo [3]; ++i) {
      tmp = _vgate [headerInfo [0] + i];
      //cout << "DFS PO " << tmp->getId() << endl;
      DFSTrace (tmp, true);
   }

   //cout << "Not used gates...\n";
   vector<CirGate*> _fanos;
   for (int i = 1; i <= headerInfo[0]; ++i) {
      if (_vgate[i] == 0) continue;
      else if (_vgate[i]->getFanoSize (_fanos) == 0) {
         _notusedGate.push_back(_vgate[i]->getId());
         DFSTrace (_vgate[i], false);
      }
   }
   _fanos.clear();

   #ifdef DEBUG_H
   cout << "============= Not Used Gates ==============\n";
   for (unsigned i = 0; i < _notusedGate.size(); ++i)
      cout << "ID: " << _notusedGate[i]->getId() << endl;
   cout << "============= Nil Fanin Gates =============\n";
   for (unsigned i = 0; i < _nilfaninGate.size(); ++i)
      cout << "ID: " << _nilfaninGate[i]->getId() << endl;
   #endif // DEBUG_H
}

void 
CirMgr::DFSTrace (CirGate* gate, bool isFromPO)
{
   int faninID1 = 0;
   int faninID2 = 0; 
   gate->setColor (GRAY);
   if (gate->getTypeStr() == "CONST" || gate->getTypeStr() == "PI");
   else if (gate->getTypeStr() == "PO") {
      faninID1 = gate->getFanin1() / 2;
      assert (faninID1 >= 0);
      if (_vgate[faninID1] == 0) _nilfaninGate.push_back(gate->getId());
      else if (_vgate[faninID1]->getColor() == WHITE) DFSTrace (_vgate[faninID1], isFromPO);
   }
   else {
      faninID1 = gate->getFanin1() / 2;
      faninID2 = gate->getFanin2() / 2;
      assert (faninID1 >= 0 && faninID2 >= 0);
      if (_vgate[faninID1] == 0 || _vgate[faninID2] == 0) _nilfaninGate.push_back(gate->getId());
      if (_vgate[faninID1] != 0 && _vgate[faninID1]->getColor() == WHITE) DFSTrace (_vgate[faninID1], isFromPO);
      if (_vgate[faninID2] != 0 && _vgate[faninID2]->getColor() == WHITE) DFSTrace (_vgate[faninID2], isFromPO);
   }

   if (isFromPO) { _DFSTrace.push_back(gate); gate->setColor (BLACK);}
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{  
   int nPI = 0, nPO = 0, nAIG = 0;
   for (size_t i = 0; i < _vgate.size(); ++i) {
      if (_vgate[i] == 0) continue;
      else if (_vgate[i]->getTypeStr() == "PI") ++nPI;
      else if (_vgate[i]->getTypeStr() == "PO") ++nPO;
      else if (_vgate[i]->getTypeStr() == "AIG") ++nAIG;
   }
   cout << std::right;
   cout << setw (0) << "\nCircuit Statistics\n"
        << setw (0) << "==================\n"
        << setw (0) << "  PI"  << setw (12) << nPI << endl
        << setw (0) << "  PO"  << setw (12) << nPO << endl
        << setw (0) << "  AIG" << setw (11) << nAIG << endl
        << setw (0) << "------------------\n  Total" << setw(9)
        << nPI + nPO + nAIG << endl;
}

void
CirMgr::printNetlist() const
{
   bool isNil = false;
   cout << '\n';
   for (size_t i = 0; i < _DFSTrace.size(); ++i) {
      cout << "[" << i << "] ";
      cout << std::left << setw(4) << _DFSTrace[i]->getTypeStr() << setw(0) << _DFSTrace[i]->getId();
      cout << std::right;
      if (_DFSTrace[i]->getTypeStr() == "PO") {
         cout << " ";
         int fanin = _DFSTrace[i]->getFanin1() / 2;
         isNil = ((_vgate[fanin] == 0) ? true : false);
         if (isNil) cout << "*";
         if (_DFSTrace[i]->getFanin1() % 2 == 0) cout << fanin;
         else cout << "!" << fanin;
      }
      else if (_DFSTrace[i]->getTypeStr() == "AIG") {
         cout << " ";
         int fanin1 = _DFSTrace[i]->getFanin1()/2;
         int fanin2 = _DFSTrace[i]->getFanin2()/2;
         
         isNil = ((_vgate[fanin1] == 0) ? true : false);
         if (isNil) cout << "*";
         if (_DFSTrace[i]->getFanin1() % 2 == 0) cout << fanin1 << " ";
         else cout << "!" << fanin1 << " ";

         isNil = ((_vgate[fanin2] == 0) ? true : false);
         if (isNil) cout << "*";
         if (_DFSTrace[i]->getFanin2() % 2 == 0) cout << fanin2;
         else cout << "!" << fanin2;
      }

      if (_DFSTrace[i]->getGateName() != "")
         cout << " (" << _DFSTrace[i]->getGateName() << ")\n";
      else cout << '\n';
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   int* line2ID = new int [headerInfo[1]+2]; // PI starts from line 2
   line2ID[0] = line2ID[1] = 0;
   for (int i = 1; i <= headerInfo[1]; ++i) {
      if (_vgate[i] == 0) continue;
      line2ID[_vgate[i]->getLineNo()] = _vgate[i]->getId();
   }
   
   for (int i = 2; i <= headerInfo[1]+1; ++i) cout << " " << line2ID[i];
   cout << endl;
   delete [] line2ID;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   int gateID = headerInfo[0];
   while (gateID < headerInfo[0] + headerInfo[3]) {
      ++gateID;
      cout << " " << gateID;
      assert (_vgate[gateID] != 0);
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if (_nilfaninGate.size() != 0) cout << "Gates with floating fanin(s):";
   for (unsigned i = 0; i < _nilfaninGate.size(); ++i)
      cout << " " << _nilfaninGate[i];
   if (_nilfaninGate.size() != 0) cout << endl;
   if (_notusedGate.size() != 0)  cout << "Gates defined but not used  :";
   for (unsigned i = 0; i < _notusedGate.size(); ++i)
      cout << " " << _notusedGate[i];
   
   if (_notusedGate.size() != 0) cout << endl;
}

void
CirMgr::printFECPairs() const
{
   #if (!PRINTALLFEC)
   int pairCount = 0;
   for (size_t i = 0; i < _FECgrpList.size(); ++i) 
   {
      if (_FECgrpList[i]->size() <= 1) continue;
      cout << "[" << pairCount << "]";
      size_t fstPattern = _FECgrpList[i]->at(0)->getSimDigit();
      for (size_t j = 0; j < _FECgrpList[i]->size(); ++j) {
         cout << " ";
         assert (_FECgrpList[i]->at(j)->getSimDigit() == fstPattern ||
               _FECgrpList[i]->at(j)->getSimDigit() == (fstPattern ^ ULLONG_MAX));
         if (_FECgrpList[i]->at(j)->getSimDigit() != fstPattern) cout << "!";
         cout << _FECgrpList[i]->at(j)->getId();
      }
      cout << '\n';
      ++pairCount;
   }
   #else
   for (size_t i = 0; i < _FECgrpList.size(); ++i) 
   {
      cout << "[" << i << "]";
      for (size_t j = 0; j < _FECgrpList[i]->size(); ++j) {
         size_t fstPattern = _FECgrpList[i]->at(0)->getSimDigit();
         cout << " ";
         assert (_FECgrpList[i]->at(j)->getSimDigit() == fstPattern ||
               _FECgrpList[i]->at(j)->getSimDigit() == (fstPattern ^ ULLONG_MAX));
         if (_FECgrpList[i]->at(j)->getSimDigit() != fstPattern) cout << "!";
         cout << _FECgrpList[i]->at(j)->getId();
      }
      cout << '\n';
   }
   #endif // PRINTALLFEC
}

bool 
myVecomp (vector<CirGate*>* v1, vector<CirGate*>* v2) {
   assert (v1 != 0 && v2 != 0);
   assert (v1->at(0) != 0 && v2->at(0) != 0);
   return v1->at(0)->getId() < v2->at(0)->getId();
}

void
CirMgr::writeAag(ostream& outfile) const
{
   // count AIG # in DFS List
   int nAIGCount = 0;
   for (unsigned i = 0; i < _DFSTrace.size(); ++i)
      if (_DFSTrace[i]->getTypeStr() == "AIG") ++nAIGCount;
   
   // print header
   outfile << "aag " << headerInfo[0] << " "
           << headerInfo [1] << " "
           << headerInfo [2] << " "
           << headerInfo [3] << " "
           << nAIGCount << endl;

   // PIs
   for (int i = 0; i < headerInfo[1]; ++i) {
      assert (_vgate[PILine2ID[i]]->getTypeStr() == "PI");
      outfile << PILine2ID[i] * 2 << endl;
   }

   // POs
   for (int i = headerInfo[0] + 1; i <= headerInfo[0] + headerInfo[3]; ++i)
      outfile << _vgate[i]->getFanin1() << endl;

   int id = 0;
   for (unsigned i = 0; i < _DFSTrace.size(); ++i) {
      id = _DFSTrace[i]->getId();
      assert (_vgate[id] != 0);
      if (_vgate[id]->getTypeStr() != "AIG") continue;
      else {
         outfile << 2 * id << " " << _vgate[id]->getFanin1 ()
                 << " " << _vgate[id]->getFanin2() << endl;
      }
   }

   // print symbols
   int count = 0;
   for (int i = 1; i <= headerInfo[0]; ++i) {
      if (_vgate[i] == 0) continue;
      else if (_vgate[i]->getTypeStr() == "AIG") continue;
      else if (_vgate[i]->getGateName() == "") ++count;
      else {
         assert (_vgate[i]->getTypeStr() != "PO");
         outfile << "i" << count << " " << _vgate[i]->getGateName() << endl;
         ++count;
      }
   }
   assert (count == headerInfo[1]);

   count = 0;
   for (int i = headerInfo[0] + 1; i <= headerInfo[0] + headerInfo[3]; ++i) {
      assert (_vgate[i] != 0);
      assert (_vgate[i]->getTypeStr() == "PO");
      if (_vgate[i]->getGateName() == "") continue;
      else {
         outfile << "o" << count << " " << _vgate[i]->getGateName() << endl;
         ++count;
      }
   }
   
   outfile << "c\n" << "AAG Output by Jian Han Wu\n";
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

CirMgr::~CirMgr()
{
   while (_DFSTrace.size() > 0) _DFSTrace.pop_back();
   while (_notusedGate.size() > 0) _notusedGate.pop_back();
   while (_nilfaninGate.size() > 0) _nilfaninGate.pop_back();
   delete [] PILine2ID;

   while (_vgate.size() > 0) {
      delete _vgate[(int) _vgate.size()-1];
      _vgate[(int) _vgate.size()-1] = 0;
      _vgate.pop_back();
   }
}