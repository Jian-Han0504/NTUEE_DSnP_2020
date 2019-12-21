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
#include <sstream>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

void CirGate::reportFanin(int level) const
{
  	assert (level >= 0);

  	set<int> idList;
  	reportFanin(level , idList , 0 , false);
}

void CirGate::reportFanin(int level , set<int> &idList , int space , bool invert) const
{
	if(level < 0) return;

	stringstream ss;
	for(int i = 0; i < space; i++)
		ss << " ";

	cout << ss.str() << (invert ? "!" : "") << getTypeStr() << " " << _id << endl;
	idList.insert(_id);
	return;
}

void Output::reportFanin(int level , set<int> &idList , int space , bool invert) const
{
	cout << getTypeStr() << " " << _id << endl;

	if(_fanin == 0) cout << "  " << (_faninId % 2 == 0 ? "" : "!") << "UNDEF " << (_faninId / 2) << endl;
	else _fanin->reportFanin(level-1 , idList , space + 2 , _faninId % 2);
}

void And::reportFanin(int level , set<int> &idList , int space , bool invert) const
{
	if(level < 0) return;

	stringstream ss;
	for(int i = 0; i < space; i++)
		ss << " ";

	bool star = (idList.find(_id) != idList.end());
	cout << ss.str() << (invert ? "!" : "") << getTypeStr() << " " << _id << (star ? " (*)" : "") << endl;

	idList.insert(_id);

	if(star) return;

	for(int i = 0; i < 2; i++)
		if(_fanin[i] != 0)
			_fanin[i]->reportFanin(level-1 , idList , space + 2 , _faninId[i] % 2);
		else
			cout << ss.str() << "  " << (_faninId[i] % 2 == 0 ? "" : "!") << "UNDEF " << (_faninId[i] / 2) << endl;
}

void CirGate::reportFanout(int level) const
{
	assert (level >= 0);

  	set<int> idList;
  	reportFanout(level , idList , 0 , false);
}

void CirGate::reportFanout(int level , set<int> &idList , int space , bool invert) const
{
	if(level < 0) return;

	stringstream ss;
	for(int i = 0; i < space; i++)
		ss << " ";

	cout << ss.str() << (invert ? "!" : "") << getTypeStr() << " " << _id << endl;
	idList.insert(_id);
	return;
}

void Input::reportFanout(int level , set<int> &idList , int space , bool invert) const
{
	cout << getTypeStr() << " " << _id << endl;

	for(unsigned int i = 0; i < _fanout.size(); i++)
		_fanout[i]->reportFanout(level-1 , idList , space + 2 , (_fanoutId[i] % 2));
}

void And::reportFanout(int level , set<int> &idList , int space , bool invert) const
{
	if(level < 0) return;

	stringstream ss;
	for(int i = 0; i < space; i++)
		ss << " ";

	bool star = (idList.find(_id) != idList.end()) && (_fanout.size() > 0);
	cout << ss.str() << (invert ? "!" : "") << getTypeStr() << " " << _id << (star ? " (*)" : "") << endl;
	idList.insert(_id);

	if(star) return;

	for(unsigned int i = 0; i < _fanout.size(); i++)
		_fanout[i]->reportFanout(level-1 , idList , space + 2 , (_fanoutId[i] % 2));
}

void Const::reportFanout(int level , set<int> &idList , int space , bool invert) const
{
	cout << getTypeStr() << " " << _id << endl;

	for(unsigned int i = 0; i < _fanout.size(); i++)
		_fanout[i]->reportFanout(level-1 , idList , space + 2 , (_fanoutId[i] % 2));
}

void CirGate::reportGate() const
{
	stringstream ss;
	cout << "==================================================\n";
	ss << "= " << getTypeStr() << "(" << getId() << "), line " << getLineNo();
	cout << std::left << setw(49) << ss.str() << '=' << endl;
	cout << "==================================================\n";
}

void Input::reportGate() const
{
	stringstream ss;
	cout << "==================================================\n";
	ss << "= " << getTypeStr() << "(" << getId() << ")\"" << _name << "\", line " << getLineNo();
	cout << std::left << setw(49) << ss.str() << '=' << endl;
	cout << "==================================================\n";
}

void Output::reportGate() const
{
	stringstream ss;
	cout << "==================================================\n";
	ss << "= " << getTypeStr() << "(" << getId() << ")\"" << _name << "\", line " << getLineNo();
	cout << std::left << setw(49) << ss.str() << '=' << endl;
	cout << "==================================================\n";
}

void Input::printGate(bool* &flag , int &count) const
{
	if(flag[_id] == true) return;
	cout << "[" << (count++) << "] PI  " << _id;
	if(_name != "") cout << " (" << _name << ")";
	cout << endl;
	flag[_id] = true;
}

void Output::printGate(bool* &flag , int &count) const
{
	if(_fanin != 0) _fanin->printGate(flag , count);

	cout << "[" << (count++) << "] PO  " << _id << " ";

	if(_fanin == 0) cout << '*';
	if(_faninId % 2 == 1) cout << '!';
	cout << (_faninId / 2);

	if(_name != "") cout << " (" << _name << ")";
	cout << endl;
}

void Const::printGate(bool* &flag , int &count) const
{
	if(flag[_id] == true) return;
	cout << "[" << (count++) << "] CONST0" << endl;
	flag[_id] = true;
}

void And::printGate(bool* &flag , int &count) const
{
	if(flag[_id] == true) return;

	if(_fanin[0] != 0) _fanin[0]->printGate(flag , count);
	if(_fanin[1] != 0) _fanin[1]->printGate(flag , count);

	cout << "[" << (count++) << "] AIG " << _id << " ";

	if(_fanin[0] == 0) cout << '*';
	if(_faninId[0] % 2 == 1) cout << '!';
	cout << (_faninId[0] / 2) << " ";

	if(_fanin[1] == 0) cout << '*';
	if(_faninId[1] % 2 == 1) cout << '!';
	cout << (_faninId[1] / 2) << endl;

	flag[_id] = true;
}

void Input::reportFloat(set<int> &outf) const
{
	if(_fanout.size() == 0) outf.insert(_id);
}

void Output::reportFloat(set<int> &inf) const
{
	if(_fanin == 0) inf.insert(_id);
}

void And::reportFloat(set<int> &inf , set<int> &outf) const
{
	if(_fanin[0] == 0 || _fanin[1] == 0) inf.insert(_id);
	if(_fanout.size() == 0) outf.insert(_id);
}

string Input::write() const
{
	stringstream ss;
	ss << (_id * 2);
	return ss.str();
}

string Output::write() const
{
	stringstream ss;
	ss << _faninId;
	return ss.str();
}

string And::write() const
{
	stringstream ss;
	ss << (_id * 2) << " " << _faninId[0] << " " << _faninId[1];
	return ss.str();
}

void And::writeAag(bool* &flag , ostream& outfile , int &count) const
{
	if(flag[_id]) return;
	if(_fanin[0] != 0) _fanin[0]->writeAag(flag , outfile , count);
	if(_fanin[1] != 0) _fanin[1]->writeAag(flag , outfile , count);

	flag[_id] = true;

	outfile << (_id*2) << " " << _faninId[0] << " " << _faninId[1] << endl;
	count++;
}

void Output::writeAag(bool* &flag , ostream& outfile , int &count) const
{
	if(_fanin != 0) _fanin->writeAag(flag , outfile , count);
}
