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
#include "../sat/sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.
typedef pair<int, bool> Partner;
enum Color { WHITE, GRAY, BLACK};
class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   CirGate (const unsigned& ID, const unsigned& Line) : _id (ID), _line (Line)
   { 
        _color = WHITE;
        _simDigit = 0;
        _isSimed = false;
        _fanins.clear(); 
        _fanouts.clear();
    }
   virtual ~CirGate() {
      _color = WHITE;
      _simDigit = 0;
      _fanins.clear();
      _fanouts.clear();
      _FECpartners.clear();
   }

   // Basic access methods
   unsigned getLineNo() const { return _line; }
   unsigned getId() const { return _id; }
   Color getColor() const { return _color;}
   const size_t getFanoSize(vector<CirGate*>& _v) const {
      _v.clear();
      _v = _fanouts;
      return _fanouts.size();
   }
   virtual bool isAig() const { return false; }
   virtual string getTypeStr() const { return "UNDEF";}
   virtual string getGateName() const { return "";}
   virtual int getFanin1 () const { return -2;}
   virtual int getFanin2 () const { return -2;}
   bool isBecomeUnused () const {
      if (getTypeStr() == "PO") return false; 
      bool isBlack = false;
      for (size_t i = 0; i < _fanouts.size(); ++i) {
          if (_fanouts[i]->getColor() == BLACK) isBlack = true;
      }
      return !isBlack;
   }

   // setting functions
   virtual void setGateName (const string name) {}
   virtual void setFanin1 (unsigned newFaninNum) {} // for optimization
   virtual void setFanin2 (unsigned newFaninNum) {} // for optimization
   virtual void setFanins  (CirGate* gate) {} 
   virtual void setFanouts (CirGate* gate) {}
   virtual bool delFanins  (CirGate* gate) { return false;} // for optimization
   virtual bool delFanouts (CirGate* gate) { return false;} // for optimization

   bool isGlobalRef () const { return _ref == _globalref;}
   void set2Global () const { _ref = _globalref;}
   static void addGlobalRef () { ++_globalref;}
   void reset () {
      _color = WHITE;
   }
   void setColor (Color c) { _color = c;}

   // Printing functions
   virtual void printGate() const {}
   virtual void reportGate() const;
   virtual void reportFanin(int level) const;
   virtual void reportFanout(int level) const;

   // sim use
   void setSimDigit (size_t d, bool reset) { 
      _simDigit = d; 
      _isSimed = ((reset) ? false : true);
      if (reset) _FECpartners.clear(); 
   }
   void setFECpartner (CirGate* pGate) {
      assert (pGate != 0);
      bool ispInv;
      size_t pDigit = pGate->getSimDigit();
      assert (pDigit == _simDigit || _simDigit == (pDigit ^ ULLONG_MAX));
      ispInv = ((pDigit == _simDigit) ? false : true);
      Partner p (pGate->getId(), ispInv);
      _FECpartners.push_back(p);
   }
   void addSimDigit (bool isPlus1) { _simDigit *= 2; if (isPlus1) ++_simDigit;}
   size_t getSimDigit () const { return _simDigit;}
   bool getisSimed () const { return _isSimed;}

   // DEBUG usgae
   virtual void BUG_printGate () const = 0;
   
private:

protected:
    const unsigned _id;
    const unsigned _line;
    static unsigned _globalref;
    mutable unsigned _ref;
    size_t _simDigit;
    bool _isSimed;
    
    Color _color;

    vector<CirGate*> _fanouts;
    vector<CirGate*> _fanins;
    vector<Partner> _FECpartners;

    void reportFanIO (int level, const int& totLevel, bool isFwd, bool isInv) const;
};


class CirCONSTGate : public CirGate
{
  public:
    CirCONSTGate (const unsigned& ID, const unsigned& Line) :
        CirGate (ID, Line) {}
    
    // basic access functions
    string getTypeStr() const { return "CONST";}

    // setting functions
    void setFanouts (CirGate* gate) { if (gate != 0) { _fanouts.push_back(gate);}}
    bool delFanouts (CirGate* gate) {
        assert (gate != 0);
        bool isFind = 0;
        for (size_t i = 0; i < _fanouts.size(); ++i) {
          if (_fanouts[i]->getId() == gate->getId()) {
              isFind = true;
              _fanouts[i] = _fanouts[_fanouts.size()-1];
              _fanouts.pop_back();
              --i;
          }
        }

        return isFind;
    }


    void BUG_printGate () const { cout << endl;}
    void printGate () const { cout << "CONST " << _id;}

};

class CirPIGate : public CirGate
{
  public:
    CirPIGate (const unsigned& ID, const unsigned& Line) :
        CirGate (ID, Line) {}
    
    // basic access functions
    string getTypeStr() const { return "PI";}
    string getGateName() const { return _name;}

    // setting functions
    void setGateName (const string name) { _name = name;}
    void setFanouts (CirGate* gate) { if (gate != 0) {_fanouts.push_back(gate);}}
    bool delFanouts (CirGate* gate) {
        assert (gate != 0);
        bool isFind = 0;
        for (size_t i = 0; i < _fanouts.size(); ++i) {
          if (_fanouts[i]->getId() == gate->getId()) {
              isFind = true;
              _fanouts[i] = _fanouts[_fanouts.size()-1];
              _fanouts.pop_back();
              --i;
          }
        }

        return isFind;
    }

    // debug
    void printGate() const { cout << "PI " << _id;}
    void BUG_printGate () const { cout << endl;}
  
  private:
    string _name;
};

class CirPOGate : public CirGate
{
  public:
    CirPOGate (const unsigned& ID, const unsigned& Line, const int& in) :
        CirGate (ID, Line), _fanin(in) {}

    // basic access functions
    string getTypeStr() const { return "PO";}
    string getGateName() const { return _name;}
    int getFanin1 () const { return _fanin;}

    // setting functions
    void setGateName (const string name) { _name = name;}
    void setFanins  (CirGate* gate) {
      if (gate!=0) {
        assert (gate->getId() == _fanin/2);
        _fanins.push_back(gate);
      }
      assert (_fanins.size() <= 1);
    }
    void setFanin1 (unsigned newFaninNum) {_fanin = newFaninNum;} // for optimization
    void setFanin2 (unsigned newFaninNum) {} // for optimization
    bool delFanins (CirGate* gate) {
        assert (gate != 0);
        bool isFind = 0;
        for (size_t i = 0; i < _fanins.size(); ++i) {
            if (_fanins[i]->getId() == gate->getId()) { 
                isFind = true;
                _fanins[i] = _fanins[_fanins.size()-1];
                _fanins.pop_back();
                --i;
            }
        }
        return isFind;
    }

    // debug
    void printGate() const { cout << "PO " << _id;}
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
    unsigned _fanin; // not ID
    string _name;
};

class CirANDGate : public CirGate
{
public:
    CirANDGate (const unsigned& ID, const unsigned& Line, const int& in1, const int& in2) :
        CirGate (ID, Line), _fanin1 (in1), _fanin2 (in2) {}
    
    // basic access functions
    string getTypeStr() const { return "AIG";}
    int getFanin1 () const { return _fanin1;}
    int getFanin2 () const { return _fanin2;}

    // setting functions
    void setFanouts (CirGate* gate) { if (gate != 0) _fanouts.push_back(gate);}
    bool delFanouts (CirGate* gate) {
        assert (gate != 0);
        bool isFind = 0;
        for (size_t i = 0; i < _fanouts.size(); ++i) {
          if (_fanouts[i]->getId() == gate->getId()) {
              isFind = true;
              _fanouts[i] = _fanouts[_fanouts.size()-1];
              _fanouts.pop_back();
              --i;
          }
        }

        return isFind;
    }
    void setFanins  (CirGate* gate) {
      if (gate!=0) {
        assert (gate->getId() == _fanin1/2 || gate->getId() == _fanin2/2);
        _fanins.push_back(gate);
      }
      assert (_fanins.size() <= 2);
    }
    void setFanin1 (unsigned newFaninNum) { _fanin1 = newFaninNum;} // for optimization
    void setFanin2 (unsigned newFaninNum) { _fanin2 = newFaninNum;} // for optimization
    bool delFanins (CirGate* gate) {
        assert (gate != 0);
        bool isFind = 0;
        for (size_t i = 0; i < _fanins.size(); ++i) {
            if (_fanins[i]->getId() == gate->getId()) { 
                isFind = true;
                _fanins[i] = _fanins[_fanins.size()-1];
                _fanins.pop_back();
                --i;
            }
        }

        return isFind;
    }

    void printGate() const { cout << "AIG " << _id;}
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
    unsigned _fanin1; // not ID
    unsigned _fanin2; // not ID
};

#endif // CIR_GATE_H
