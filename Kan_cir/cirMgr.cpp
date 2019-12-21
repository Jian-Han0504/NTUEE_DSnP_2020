/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/

#define spacetok " \v\t\f\r"

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
static unsigned lineNo = 0;  // in printing, lineNo needs to ++
static size_t colNo  = 0;  // in printing, colNo needs to ++
// static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

size_t yourStrGetTok(const string& str, string& tok, size_t pos = 0, const string& del = spacetok);
bool parseParameter(const string &in , int &a , const string &name);
bool parseI(const string &in , int &a);
bool parseO(const string &in , int &a);
bool parseAnd(const string &in , int &id , int &a , int &b , const int &M , CirGate** mapping);
bool parseName(const string &in , int &id , string &name , bool &isIn);
string operator + (const string &s1 , const string s2);

static bool parseError(CirParseError err)
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
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
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
/*bool CirMgr::readCircuit(const string& fileName) // no error version
{
   fstream file(fileName , ios::in);

   if(!file)
   {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

   int M , I , L , O , A;
   string s;
   file >> s >> M >> I >> L >> O >> A;

   CirGate* *mapping = new CirGate*[M+1];
   memset(mapping , 0 , sizeof(CirGate*) * M);
   mapping[0] = _const;

   for(int i = 0 , id; i < I; i++)
   {
      file >> id;
      id /= 2;
      mapping[id] = new Input(id , i + 2);
      _in.push_back( (Input*)mapping[id] );
   }

   int *out = new int[O];
   for(int i = 0; i < O; i++)
   {
      file >> out[i];
      _out.push_back( new Output(M + i + 1 , I + i + 2) );
   }

   pair<int , int> *ag = new pair<int , int>[A];
   for(int i = 0 , id; i < A; i++)
   {
      file >> id >> ag[i].first >> ag[i].second;
      id /= 2;
      mapping[id] = new And(id , I + O + i + 2);
      _and.push_back( (And*)mapping[id] );
   }

   for(int i = 0; i < O; i++)
   {
      _out[i]->setFanin(mapping[out[i] / 2] , out[i]);

      if(mapping[out[i] / 2])
      {
         if( dynamic_cast<And*>(mapping[out[i] / 2]) )
            dynamic_cast<And*>(mapping[out[i] / 2])->addFanout(_out[i] , _out[i]->getId() * 2 + (out[i] % 2));
         else if( dynamic_cast<Const*>(mapping[out[i] / 2]) )
            dynamic_cast<Const*>(mapping[out[i] / 2])->addFanout(_out[i] , _out[i]->getId() * 2 + (out[i] % 2));
         else
            dynamic_cast<Input*>(mapping[out[i] / 2])->addFanout(_out[i] , _out[i]->getId() * 2 + (out[i] % 2));
      }
   }

   for(int i = 0; i < A; i++)
   {
      _and[i]->addFanin(mapping[ag[i].first / 2] , ag[i].first);

      if(mapping[ag[i].first / 2])
      {
         if( dynamic_cast<And*>(mapping[ag[i].first / 2]) )
            dynamic_cast<And*>(mapping[ag[i].first / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].first % 2));
         else if( dynamic_cast<Const*>(mapping[ag[i].first / 2]) )
            dynamic_cast<Const*>(mapping[ag[i].first / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].first % 2));
         else
            dynamic_cast<Input*>(mapping[ag[i].first / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].first % 2));
      }

      _and[i]->addFanin(mapping[ag[i].second / 2] , ag[i].second);

      if(mapping[ag[i].second / 2])
      {
         if( dynamic_cast<And*>(mapping[ag[i].second / 2]) )
            dynamic_cast<And*>(mapping[ag[i].second / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].second % 2));
         else if( dynamic_cast<Const*>(mapping[ag[i].second / 2]) )
            dynamic_cast<Const*>(mapping[ag[i].second / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].second % 2));
         else
            dynamic_cast<Input*>(mapping[ag[i].second / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].second % 2));
      }
   }

   string tok , name;
   file >> tok;
   while(tok != "" && tok != "c" && !file.eof())
   {
      file >> name;

      if(tok[0] == 'i')
      {
         int ith;
         myStr2Int(tok.substr(1 , tok.length() - 1) , ith);
         _in[ith]->setName(name);
      }
      else
      {
         int ith;
         myStr2Int(tok.substr(1 , tok.length() - 1) , ith);
         _out[ith]->setName(name);
      }
      file >> tok;
   }

   delete [] mapping;
   delete [] out;
   delete [] ag;

   _M = M;

   return true;
}*/

bool CirMgr::readCircuit(const string& fileName)
{
   fstream file(fileName , ios::in);

   if(!file)
   {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

   int M , I , L , O , A;
   string in;

   lineNo = 0;
   colNo = 0;
   errInt = 0;
   errMsg = "";

//Header--------------------------------------------------------------
   getline(file , in);

   if(in.length() == 0)
   {
      errMsg = "aag";
      parseError(MISSING_IDENTIFIER);
      return false;
   }

   if(in[0] == ' ')
   {
      parseError(EXTRA_SPACE);
      return false;
   }
   else if(isspace(in[0]))
   {
      errInt = (int)in[0];
      parseError(ILLEGAL_WSPACE);
      return false;
   }
   
   string tok;
   colNo = yourStrGetTok(in , tok , 0 , spacetok);
   if( (myStrNCmp("aag" , tok , 3) != 0) || (tok.length() != 3) )
   {
      errMsg = tok;
      parseError(ILLEGAL_IDENTIFIER);
      return false;
   }

   if(!parseParameter(in , M , "variables")) return false;
   if(!parseParameter(in , I , "PIs")) return false;
   if(!parseParameter(in , L , "latches")) return false;
   if(!parseParameter(in , O , "POs")) return false;
   if(!parseParameter(in , A , "AIGs")) return false;

   if(colNo != string::npos)
   {
      parseError(MISSING_NEWLINE);
      return false;
   }
   else if(M < I + L + A)
   {
      errMsg = "Number of variables";
      errInt = M;
      parseError(NUM_TOO_SMALL);
      return false;
   }
   else if(L != 0)
   {
      errMsg = "latches";
      parseError(ILLEGAL_NUM);
      return false;
   }
//Input---------------------------------------------------------------
   CirGate* *mapping = new CirGate*[M+1];
   memset(mapping , 0 , sizeof(CirGate*) * (M+1));
   mapping[0] = _const;

   for(int i = 0 , id; i < I; i++)
   {
      lineNo++;
      getline(file , in); colNo = 0;
      if(file.eof()){ errMsg = "PI"; parseError(MISSING_DEF); return false; }
      if(!parseI(in , id)) return false;

      if(id/2 == 0)
      {
         errInt = id;
         parseError(REDEF_CONST);
         return false;
      }
      else if(id/2 > M)
      {
         errInt = id;
         parseError(MAX_LIT_ID);
         return false;
      }
      else if(id%2 != 0)
      {
         errMsg = "PI";
         errInt = id;
         parseError(CANNOT_INVERTED);
         return false;
      }
      else if(mapping[id/2])
      {
         errInt = id;
         errGate = mapping[id/2];
         parseError(REDEF_GATE);
         return false;
      }

      id /= 2;
      mapping[id] = new Input(id , lineNo+1);
      _in.push_back( (Input*)mapping[id] );
   }
//Output--------------------------------------------------------------
   int *out = new int[O];
   for(int i = 0; i < O; i++)
   {
      lineNo++;
      getline(file , in); colNo = 0;
      if(file.eof()){ errMsg = "PO"; parseError(MISSING_DEF); return false; }
      if(!parseO(in , out[i])) return false;

      if(out[i]/2 > M)
      {
         errInt = out[i];
         parseError(MAX_LIT_ID);
         return false;
      }

      _out.push_back( new Output(M + i + 1 , lineNo+1) );
   }
//And-----------------------------------------------------------------
   pair<int , int> *ag = new pair<int , int>[A];
   for(int i = 0 , id; i < A; i++)
   {
      lineNo++;
      getline(file , in); colNo = 0;
      if(file.eof()){ errMsg = "AIG"; parseError(MISSING_DEF); return false; }
      if(!parseAnd(in , id , ag[i].first , ag[i].second , M , mapping)) return false;
      id /= 2;
      mapping[id] = new And(id , lineNo+1);
      _and.push_back( (And*)mapping[id] );
   }
//Connection----------------------------------------------------------
   for(int i = 0; i < O; i++)
   {
      _out[i]->setFanin(mapping[out[i] / 2] , out[i]);

      if(mapping[out[i] / 2])
      {
         if( dynamic_cast<And*>(mapping[out[i] / 2]) )
            dynamic_cast<And*>(mapping[out[i] / 2])->addFanout(_out[i] , _out[i]->getId() * 2 + (out[i] % 2));
         else if( dynamic_cast<Const*>(mapping[out[i] / 2]) )
            dynamic_cast<Const*>(mapping[out[i] / 2])->addFanout(_out[i] , _out[i]->getId() * 2 + (out[i] % 2));
         else
            dynamic_cast<Input*>(mapping[out[i] / 2])->addFanout(_out[i] , _out[i]->getId() * 2 + (out[i] % 2));
      }
   }

   for(int i = 0; i < A; i++)
   {
      _and[i]->addFanin(mapping[ag[i].first / 2] , ag[i].first);

      if(mapping[ag[i].first / 2])
      {
         if( dynamic_cast<And*>(mapping[ag[i].first / 2]) )
            dynamic_cast<And*>(mapping[ag[i].first / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].first % 2));
         else if( dynamic_cast<Const*>(mapping[ag[i].first / 2]) )
            dynamic_cast<Const*>(mapping[ag[i].first / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].first % 2));
         else
            dynamic_cast<Input*>(mapping[ag[i].first / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].first % 2));
      }

      _and[i]->addFanin(mapping[ag[i].second / 2] , ag[i].second);

      if(mapping[ag[i].second / 2])
      {
         if( dynamic_cast<And*>(mapping[ag[i].second / 2]) )
            dynamic_cast<And*>(mapping[ag[i].second / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].second % 2));
         else if( dynamic_cast<Const*>(mapping[ag[i].second / 2]) )
            dynamic_cast<Const*>(mapping[ag[i].second / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].second % 2));
         else
            dynamic_cast<Input*>(mapping[ag[i].second / 2])->addFanout(_and[i] , _and[i]->getId() * 2 + (ag[i].second % 2));
      }
   }
//name----------------------------------------------------------------
   string name;
   int id;
   bool isIn;
   lineNo++;
   getline(file , in); colNo = 0;

   while(in != "c" && !file.eof())
   {
      if(!parseName(in , id , name , isIn)) return false;

      if(isIn)
      {
         if((unsigned int)id >= _in.size())
         {
            errInt = id;
            errMsg = "PI index";
            parseError(NUM_TOO_BIG);
            return false;
         }
         else if(_in[id]->getName() != "")
         {
            errMsg = "i";
            errInt = id;
            parseError(REDEF_SYMBOLIC_NAME);
            return false;
         }
         _in[id]->setName(name);
      }
      else
      {
         if((unsigned int)id >= _out.size())
         {
            errInt = id;
            errMsg = "PO index";
            parseError(NUM_TOO_BIG);
            return false;
         }
         else if(_out[id]->getName() != "")
         {
            errMsg = "o";
            errInt = id;
            parseError(REDEF_SYMBOLIC_NAME);
            return false;
         }

         _out[id]->setName(name);
      }

      lineNo++;
      getline(file , in); colNo = 0;
   }

   delete [] mapping;
   delete [] out;
   delete [] ag;

   _M = M;

   return true;
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
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << setw(12) << _in.size() << endl;
   cout << "  PO" << setw(12) << _out.size() << endl;
   cout << "  AIG" << setw(11) << _and.size() << endl;
   cout << "------------------" << endl;
   cout << "  Total" << setw(9) << (_in.size() + _out.size() + _and.size()) << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;

   bool* flag = new bool[_M+1];
   memset( flag , 0 , sizeof(bool)*(_M+1) );
   int count = 0;
   for(auto i = _out.begin(); i != _out.end(); i++) (*i)->printGate(flag , count);
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(auto i = _in.begin(); i != _in.end(); i++)
   {
      cout << " " << (*i)->getId();
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(auto i = _out.begin(); i != _out.end(); i++)
   {
      cout << " " << (*i)->getId();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   set<int> inf , outf;

   for(auto i = _in.begin(); i != _in.end(); i++) (*i)->reportFloat(outf);
   for(auto i = _out.begin(); i != _out.end(); i++) (*i)->reportFloat(inf);
   for(auto i = _and.begin(); i != _and.end(); i++) (*i)->reportFloat(inf , outf);

   bool first = false;
   for(auto i = inf.begin(); i != inf.end(); i++)
   {
      if(!first)
      {
         cout << "Gates with floating fanin(s):";
         first = true;
      }
      cout << " " << (*i);
   }
   if(first) cout << endl;

   first = false;
   for(auto i = outf.begin(); i != outf.end(); i++)
   {
      if(!first)
      {
         cout << "Gates defined but not used  :";
         first = true;
      }
      cout << " " << (*i);
   }
   if(first) cout << endl;
}

void CirMgr::writeAag(ostream& outfile) const
{
   bool* flag = new bool[_M+1];
   memset( flag , 0 , sizeof(bool)*(_M+1) );

   int count = 0;
   stringstream ss;

   for(auto i = _out.begin(); i != _out.end(); i++)
      (*i)->writeAag(flag , ss , count);

   outfile << "aag " << _M << " " << _in.size() << " 0 " << _out.size() << " " << count << endl;

   for(auto i = _in.begin(); i != _in.end(); i++)
      outfile << (*i)->write() << endl;

   for(auto i = _out.begin(); i != _out.end(); i++)
      outfile << (*i)->write() << endl;

   outfile << ss.str();

   for(unsigned int i = 0; i < _in.size(); i++)
      if(_in[i]->getName() != "")
         outfile << "i" << i << " " << _in[i]->getName() << endl;

   for(unsigned int i = 0; i < _out.size(); i++)
      if(_out[i]->getName() != "")
         outfile << "o" << i << " " << _out[i]->getName() << endl;

   outfile << "c\nI want A+." << endl;
}

CirGate* CirMgr::getGate(int gid) const
{
   if(gid == 0) return (CirGate*)_const;

   for(auto i = _in.begin(); i != _in.end(); i++)
      if((*i)->getId() == gid)
         return (CirGate*)(*i);

   for(auto i = _and.begin(); i != _and.end(); i++)
      if((*i)->getId() == gid)
         return (CirGate*)(*i);

   for(auto i = _out.begin(); i != _out.end(); i++)
      if((*i)->getId() == gid)
         return (CirGate*)(*i);

   return 0;
}

size_t yourStrGetTok(const string& str, string& tok, size_t pos, const string& del)
{
   size_t begin = str.find_first_not_of(del, pos);
   if (begin == string::npos) { tok = ""; return begin; }
   size_t end = str.find_first_of(del, begin);
   tok = str.substr(begin, end - begin);
   return end;
}

bool parseParameter(const string &in , int &a , const string &name)
{
   size_t tail = in.length();
   string tok;

   if(colNo == string::npos)
   {
      colNo = in.length();
      errMsg = "number of " + name;
      parseError(MISSING_NUM);//variables
      return false;
   }

   if(isspace(in[colNo]) && in[colNo] != ' ')
   {
      errInt = (int)in[colNo];
      parseError(ILLEGAL_WSPACE);
      return false;
   }

   if((++colNo) >= tail)
   {
      errMsg = "number of " + name;
      parseError(MISSING_NUM);//variables
      return false;
   }

   if(in[colNo] == ' ')
   {
      parseError(EXTRA_SPACE);
      return false;
   }
   else if(isspace(in[colNo]))
   {
      errInt = (int)in[colNo];
      parseError(ILLEGAL_WSPACE);
      return false;
   }

   colNo = yourStrGetTok(in , tok , colNo , spacetok);
   //cout << "parsing " << name << " , end = " << colNo << " , npos = " << string::npos << endl;
   if(!myStr2Int(tok , a) || a < 0)
   {
      errMsg = "number of " + name + "(" + tok + ")";
      parseError(ILLEGAL_NUM);
      return false;
   }

   return true;
}

bool parseI(const string &in , int &a)
{
   if(in.length() == 0)
   {
      errMsg = "PI literal ID";
      parseError(MISSING_NUM);
      return false;
   }

   if(in[0] == ' ')
   {
      parseError(EXTRA_SPACE);
      return false;
   }
   else if(isspace(in[0]))
   {
      errInt = (int)in[0];
      parseError(ILLEGAL_WSPACE);
      return false;
   }

   string tok;
   size_t end = yourStrGetTok(in , tok , 0 , spacetok);
   if(!myStr2Int(tok , a) || a < 0)
   {
      errMsg = "PI literal ID(" + tok + ")";
      parseError(ILLEGAL_NUM);
      return false;
   }
   else if(a == 0)
   {
      errInt = 0;
      parseError(REDEF_CONST);
      return false;
   }

   if(end != string::npos)
   {
      colNo = end;
      parseError(MISSING_NEWLINE);
      return false;
   }

   return true;
}

bool parseO(const string &in , int &a)
{
   if(in.length() == 0)
   {
      errMsg = "PO literal ID";
      parseError(MISSING_NUM);
      return false;
   }

   if(in[0] == ' ')
   {
      parseError(EXTRA_SPACE);
      return false;
   }
   else if(isspace(in[0]))
   {
      errInt = (int)in[0];
      parseError(ILLEGAL_WSPACE);
      return false;
   }

   string tok;
   size_t end = yourStrGetTok(in , tok , 0 , spacetok);
   if(!myStr2Int(tok , a) || a < 0)
   {
      errMsg = "PO literal ID(" + tok + ")";
      parseError(ILLEGAL_NUM);
      return false;
   }

   if(end != string::npos)
   {
      colNo = end;
      parseError(MISSING_NEWLINE);
      return false;
   }

   return true;
}

bool parseAnd(const string &in , int &id , int &a , int &b , const int &M , CirGate** mapping)
{
   if(in.length() == 0)
   {
      errMsg = "AIG gate literal ID";
      parseError(MISSING_NUM);
      return false;
   }
//id----------------------------------------------------------------
   if(in[0] == ' ')
   {
      parseError(EXTRA_SPACE);
      return false;
   }
   else if(isspace(in[0]))
   {
      errInt = (int)in[0];
      parseError(ILLEGAL_WSPACE);
      return false;
   }

   string tok;
   size_t end = yourStrGetTok(in , tok , 0 , spacetok);
   if(!myStr2Int(tok , id) || id < 0)
   {
      errMsg = "AIG gate literal ID(" + tok + ")";
      parseError(ILLEGAL_NUM);
      return false;
   }
   else if(id/2 == 0)
   {
      errInt = id;
      parseError(REDEF_CONST);
      return false;
   }
   else if(id/2 > M)
   {
      errInt = id;
      parseError(MAX_LIT_ID);
      return false;
   }
   else if(id%2 != 0)
   {
      errMsg = "AIG gate";
      errInt = id;
      parseError(CANNOT_INVERTED);
      return false;
   }
   else if(mapping[id/2])
   {
      errInt = id;
      errGate = mapping[id/2];
      parseError(REDEF_GATE);
      return false;
   }

   if(end == string::npos || in[end] != ' ')
   {
      colNo = (end == string::npos? in.length() : end);
      parseError(MISSING_SPACE);
      return false;
   }

   colNo = end+1;
//left--------------------------------------------------------------
   if(in.find_first_not_of(spacetok , colNo) != colNo)
   {
      if(colNo >= in.length())
      {
         errMsg = "AIG input literal ID";
         parseError(MISSING_NUM);
      }
      else if(in[colNo] == ' ')
      {
         parseError(EXTRA_SPACE);
      }
      else
      {
         errInt = (int)in[colNo];
         parseError(ILLEGAL_WSPACE);
      }
      return false;
   }

   end = yourStrGetTok(in , tok , colNo , spacetok);
   if(!myStr2Int(tok , a) || a < 0)
   {
      errMsg = "AIG input literal ID(" + tok + ")";
      parseError(ILLEGAL_NUM);
      return false;
   }
   else if(a/2 > M)
   {
      errInt = a;
      parseError(MAX_LIT_ID);
      return false;
   }

   if(end == string::npos || in[end] != ' ')
   {
      colNo = (end == string::npos? in.length() : end);
      parseError(MISSING_SPACE);
      return false;
   }

   colNo = end+1;
//right-------------------------------------------------------------
   if(in.find_first_not_of(spacetok , colNo) != colNo)
   {
      if(colNo >= in.length())
      {
         errMsg = "AIG input literal ID";
         parseError(MISSING_NUM);
      }
      else if(in[colNo] == ' ')
      {
         parseError(EXTRA_SPACE);
      }
      else
      {
         errInt = (int)in[colNo];
         parseError(ILLEGAL_WSPACE);
      }
      return false;
   }

   end = yourStrGetTok(in , tok , colNo , spacetok);
   if(!myStr2Int(tok , b) || b < 0)
   {
      errMsg = "AIG input literal ID(" + tok + ")";
      parseError(ILLEGAL_NUM);
      return false;
   }
   else if(b/2 > M)
   {
      errInt = b;
      parseError(MAX_LIT_ID);
      return false;
   }

   if(end != string::npos)
   {
      colNo = end;
      parseError(MISSING_NEWLINE);
      return false;
   }

   return true;
}

bool parseName(const string &in , int &id , string &name , bool &isIn)
{
   if(in.length() == 0)
   {
      errMsg = "";
      parseError(ILLEGAL_SYMBOL_TYPE);
      return false;
   }
   else if(in[0] == 'c')
   {
      colNo = 1;
      parseError(MISSING_NEWLINE);
      return false;
   }
   else if(in[0] == ' ')
   {
      parseError(EXTRA_SPACE);
      return false;
   }
   else if(isspace(in[0]))
   {
      errInt = (int)in[0];
      parseError(ILLEGAL_WSPACE);
      return false;
   }
   else if(in[0] == 'i')
   {
      isIn = true;
   }
   else if(in[0] == 'o')
   {
      isIn = false;
   }
   else
   {
      errMsg = in.substr(0 , 1);
      parseError(ILLEGAL_SYMBOL_TYPE);
      return false;
   }

   colNo = 1;

   if(in.find_first_of(spacetok , 1) == 1)
   {
      if(in[1] == ' ')
      {
         parseError(EXTRA_SPACE);
         return false;
      }
      else
      {
         errInt = (int)in[1];
         parseError(ILLEGAL_WSPACE);
         return false;
      }

   }

   string tok;
   size_t end = yourStrGetTok(in , tok , 1 , spacetok);
   if(tok.length() == 0)
   {
      errMsg = "symbol index";
      parseError(MISSING_NUM);
      return false;
   }
   else if(!myStr2Int(tok , id) || id < 0)
   {
      errMsg = "symbol index(" + tok + ")";
      parseError(ILLEGAL_NUM);
      return false;
   }

   if(end == string::npos)
   {
      errMsg = "symbolic name";
      parseError(MISSING_IDENTIFIER);
      return false;
   }
   else if(in[end] != ' ')
   {
      colNo = end;
      parseError(MISSING_SPACE);
      return false;
   }
   else if(end + 1 >= in.length())
   {
      errMsg = "symbolic name";
      parseError(MISSING_IDENTIFIER);
      return false;
   }

   name = "";
   for(colNo = end + 1; colNo < in.length(); colNo++)
   {
      if(isprint(in[colNo]))
         name = name + in[colNo];
      else
      {
         errInt = in[colNo];
         parseError(ILLEGAL_SYMBOL_NAME);
         return false;
      }
   }

   return true;
}

string operator + (const string &s1 , const string s2)
{
   stringstream ss;
   ss << s1 << s2;
   return ss.str();
}

string operator + (const string &s , const char &c)
{
   stringstream ss;
   ss << s << c;
   return ss.str();;
}