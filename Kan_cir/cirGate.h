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
#include <set>
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
    virtual ~CirGate() {}

    // Basic access methods
    virtual string getTypeStr() const { return "UNDEF"; }
    int getId() const { return _id; }
    unsigned getLineNo() const { return _line; }

    // Printing functions
    virtual void printGate(bool* &flag , int &count) const = 0;
    virtual void reportGate() const;
    virtual void reportFanin(int level) const;
    virtual void reportFanin(int level , set<int> &idList , int space , bool invert) const;
    virtual void reportFanout(int level) const;
    virtual void reportFanout(int level , set<int> &idList , int space , bool invert) const;
    virtual void writeAag(bool* &flag , ostream& outfile , int &count) const { return; }

protected:
    CirGate(const int &id , const int &line) : _id(id) , _line(line) {}
    
    const int        _id;
    const int        _line;
};

class And : public CirGate
{
public:
    And(const int id , const int line) : CirGate(id , line) {}

    void addFanin(CirGate* in , int id)
    {
        _fanin.push_back(in);
        _faninId.push_back(id);
    }

    void addFanout(CirGate* out , int id)
    {
        _fanout.push_back(out);
        _fanoutId.push_back(id);
    }

    string write() const;
    void reportFloat(set<int> &inf , set<int> &outf) const;

    virtual void reportFanin(int level , set<int> &idList , int space , bool invert) const;
    virtual void reportFanout(int level , set<int> &idList , int space , bool invert) const;
    virtual void printGate(bool* &flag , int &count) const;
    virtual void writeAag(bool* &flag , ostream& outfile , int &count) const;
    virtual string getTypeStr() const { return "AIG"; }

private:
    vector<CirGate*>    _fanin;
    vector<int>         _faninId;
    vector<CirGate*>    _fanout;
    vector<int>         _fanoutId;
};

class Const : public CirGate
{
public:
    Const(const int id , const int line) : CirGate(id , line) {}

    void addFanout(CirGate* out , int id)
    {
        _fanout.push_back(out);
        _fanoutId.push_back(id);
    }

    virtual void reportFanout(int level , set<int> &idList , int space , bool invert) const;
    virtual void printGate(bool* &flag , int &count) const;
    virtual string getTypeStr() const { return "CONST"; }

private:
    vector<CirGate*>    _fanout;
    vector<int>         _fanoutId;
};

class Input : public CirGate
{
public:
    Input(const int id , const int line) : CirGate(id , line) , _name("") {}

    void addFanout(CirGate* out , int id)
    {
        _fanout.push_back(out);
        _fanoutId.push_back(id);
    }

    void setName(string name) { _name = name; }
    void reportFloat(set<int> &outf) const;
    string getName() { return _name; }
    string write() const;

    virtual string getTypeStr() const { return "PI"; }
    virtual void printGate(bool* &flag , int &count) const;
    virtual void reportFanout(int level , set<int> &idList , int space , bool invert) const;
    virtual void reportGate() const;

private:
    string              _name;
    vector<CirGate*>    _fanout;
    vector<int>         _fanoutId;
};

class Output : public CirGate
{
public:
    Output(const int id , const int line) : CirGate(id , line) , _fanin(0) , _faninId(0) , _name("") {}

    void setFanin(CirGate* in , int id)
    {
        _fanin = in;
        _faninId = id;
    }

    void setName(string name) { _name = name; }
    void reportFloat(set<int> &inf) const;
    string getName() { return _name; }
    string write() const;

    virtual string getTypeStr() const { return "PO"; }
    virtual void printGate(bool* &flag , int &count) const;
    virtual void reportGate() const;
    virtual void reportFanin(int level , set<int> &idList , int space , bool invert) const;
    virtual void writeAag(bool* &flag , ostream& outfile , int &count) const;

private:
    CirGate*        _fanin;
    int             _faninId;
    string          _name;
};

#endif // CIR_GATE_H
