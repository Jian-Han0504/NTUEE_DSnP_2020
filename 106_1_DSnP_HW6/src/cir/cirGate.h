/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate() {
     _gateName = "";
   }
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return _gateType;}
   string getTypeName() const { return _gateName;}
   unsigned getLineNo() const { return _gateLine;}
   unsigned getID () const {return _gateID;}
   virtual int getFanin1 () const { return -1;}
   virtual int getFanin2 () const { return -1;}

   // Basic write function
   void writeTypeName (const string& name) { _gateName = name;}

   // DFS trace
   void writeColor () { _isColor = true;}
   void writeHasFltIn () {_hasFltFanin = true;}
   bool getColor () const { return _isColor;}
   bool getHasFltIn () const { return _hasFltFanin;}

   // DFS_take2
   bool isGlobalRef () const { return _ref == _globalref;}
   void set2Global () const { _ref = _globalref;}
   // cout << "SET TO GLOBAL... " << _ref << endl;
   static void addGlobalRef () { ++_globalref;}
   //cout << "GLOBALREF: " << _globalref << endl;

   // linking lists
   void addSuccessor (const CirGate* sucGate) { successor.push_back (sucGate);} // toward PO 
   void addPredecess (const CirGate* preGate) { predecess.push_back (preGate);} // toward PI
   void printSuccess () const {
        for (unsigned i = 0; i < successor.size(); ++i) 
            cout << successor[i]->_gateID << " ";
    }
   void printPredece () const {
        for (unsigned i = 0; i < predecess.size(); ++i) 
            cout << predecess[i]->_gateID << " ";
    }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

    // DEBUG usgae
    virtual void BUG_printGate () const = 0;

private:

protected:
  unsigned _gateLine;
  unsigned _gateID;
  string _gateType;
  string _gateName; // for symbolic
  bool _isColor;
  bool _hasFltFanin;

  // for gate report...
  vector<const CirGate*> successor;
  vector<const CirGate*> predecess;
  static unsigned _globalref;
  mutable unsigned _ref;

  void reportGateCone (const string&, const int&, const int&, bool invert=false) const;
};

class CirCONSTGate : public CirGate
{
  public:
    CirCONSTGate () {
      _gateLine = 0;
      _gateID = 0;
      _gateType = "CONST";
      _isColor = false;
      _hasFltFanin = false;
      _ref = 0;
    }

    void printGate () const { cout << "CONST " << _gateID;}
    void BUG_printGate () const { cout << endl;}

};

class CirPIGate : public CirGate
{
    public:
      CirPIGate (const unsigned& line, const unsigned& id) { 
        _gateLine = line;
        _gateID = id;
        _gateType = "PI";
        _isColor = false;
        _hasFltFanin = false;
        _ref = 0;
      }
      void printGate() const { cout << "PI " << _gateID;}
      void BUG_printGate () const { cout << endl;}
};

class CirPOGate : public CirGate
{
    public:
      CirPOGate (const unsigned& line, const unsigned& id, const int& in) : _fanin(in) {
        _gateLine = line; 
        _gateID = id;
        _gateType = "PO";
        _isColor = false;
        _hasFltFanin = false;
        _ref = 0;
      }
      
      int getFanin1 () const { return _fanin;}

      void printGate() const { cout << "PO " << _gateID;}
      void BUG_printGate () const {
        cout << "PO input gate ID: ";
        if (_fanin == 0) cout << "CONST0\n";
        else if (_fanin == 1) cout << "CONST1\n";
        else {
          if (_fanin % 2 == 0) cout << _fanin / 2 << endl;
          else cout << -_fanin / 2 << endl; 
        }
      }

    private:
      int _fanin; // not ID
};

class CirANDGate : public CirGate
{   
    public:
      CirANDGate (const unsigned& line, const unsigned& id, const int& in1, const int& in2) : _fanin1 (in1), _fanin2 (in2) {
          _gateLine = line;
          _gateID = id;
          _gateType = "AIG";
          _isColor = false;
          _hasFltFanin = false;
          _ref = 0;
      }

      int getFanin1 () const { return _fanin1;}
      int getFanin2 () const { return _fanin2;}
      void printGate() const { cout << "AIG " << _gateID;}
      void BUG_printGate () const {
        cout << "AIG gate input gate ID: ";
        if (_fanin1 == 1) cout << "CONST1 / ";
        else if (_fanin1 == 0) cout << "CONST0 / ";
        else { 
            if (_fanin1 % 2 == 0) cout << _fanin1 / 2 << " / ";
            else cout << -_fanin1 / 2 << " / ";
        }

        if (_fanin2 == 1) cout << "CONST1\n";
        else if (_fanin2 == 0) cout << "CONST0\n";
        else {
            if (_fanin2 % 2 == 0) cout << _fanin2 / 2 << endl;
            else cout << -_fanin2 / 2 << endl;
        }
      }

    private:
      int _fanin1; // not ID
      int _fanin2; // not ID
};

#endif // CIR_GATE_H
