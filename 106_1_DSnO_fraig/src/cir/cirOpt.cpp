/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "../util/util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
#define DEBUG_H 0
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    #ifdef DEBUG_H
    for (size_t j = 0; j < _DFSTrace.size(); ++j)
        assert (_DFSTrace[j]->getColor() == BLACK);
    #endif // DEBUG_H

    bool isSweep = false;

    vector<CirGate*> _v;
    int faninID1 = 0;
    int faninID2 = 0;
    for (int i = 1; i <= headerInfo[0]; ++i) {
        if (_vgate[i] == 0) continue;
        else if (_vgate[i]->getTypeStr() == "PI") continue;
        else if (_vgate[i]->getColor() == BLACK) continue;
        else isSweep = true;
        assert (_vgate[i]->getColor() == GRAY);
        assert (_vgate[i]->getTypeStr() == "AIG");
        cout << "Sweeping: AIG(" << i << ") removed...\n";

        // updating fanout list of connected gates in DFS list
        faninID1 = _vgate[i]->getFanin1 () / 2;
        faninID2 = _vgate[i]->getFanin2 () / 2;
        if (faninID1 >= 0 && _vgate[faninID1] != 0) {
            if (_vgate[faninID1]->getColor() == BLACK || 
                _vgate[faninID1]->getTypeStr() == "CONST" ||
                _vgate[faninID1]->getTypeStr() == "PI")
                _vgate[faninID1]->delFanouts (_vgate[i]);
        }

        if (faninID2 >= 0 && _vgate[faninID2] != 0) {
            if (_vgate[faninID2]->getColor() == BLACK || 
                _vgate[faninID2]->getTypeStr() == "CONST" ||
                _vgate[faninID2]->getTypeStr() == "PI")
                _vgate[faninID2]->delFanouts (_vgate[i]);
        }

        delete _vgate[i];
        _vgate[i] = 0;
    }

    if (!isSweep) return;

    // updating floating gate lists
    for (int i = 1; i <= headerInfo[1]; ++i) {
        if (_vgate[i] == 0) continue;
        else if (_vgate[i]->getColor() == BLACK) continue;
        assert (_vgate[i]->getColor() == GRAY);
        // if PI is GRAY, it must be a not used gate!
        _notusedGate.push_back (_vgate[i]->getId());
    }

    size_t countValid = 0;
    for (size_t i = 0; i < _nilfaninGate.size(); ++i) {
        if (_vgate[_nilfaninGate[i]] != 0) { 
            _nilfaninGate[countValid] = _nilfaninGate[i]; 
            ++countValid;
        }
    }
    while (_nilfaninGate.size() > countValid) _nilfaninGate.pop_back();

    countValid = 0;
    for (size_t i = 0; i < _notusedGate.size(); ++i) {
        if (_vgate[_notusedGate[i]] != 0) { 
            _notusedGate[countValid] = _notusedGate[i]; 
            ++countValid;
        }
    }
    while (_notusedGate.size() > countValid) _notusedGate.pop_back();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    // TODO: There is a case that fanin1 and fanin2 are reversed!!!
    // If the circuit has been simulated, shall not perform opt
    bool isOpt = false;

    // check
    #if (DEBUG_H)
    for (int i = 1; i <= headerInfo[0] + headerInfo[3]; ++i) {
        if (_vgate[i] == 0) continue;
        bool isInDFS = false;
        for (size_t j = 0; j < _DFSTrace.size(); ++j)
            if ((int)_DFSTrace[j]->getId() == i) isInDFS = true;
        if (isInDFS) assert (_vgate[i]->getColor() == BLACK);
        else assert (_vgate[i]->getColor() == GRAY);
    }
    for (size_t i = 0; i < _DFSTrace.size(); ++i)
        assert (_DFSTrace[i]->getColor() == BLACK);
    for (size_t i = 0; i < _notusedGate.size(); ++i)
        assert (_vgate[_notusedGate[i]]->getColor() == GRAY);
    #endif // DEBUG_H
    
    // go through every tree in DFS list
    int faninNum1; // the gate's first fanin num
    int faninNum2; // the gate's second fanin num
    vector<CirGate*> fanouts; // the gate's fanouts
    size_t nFanouts; // # of gate's fanouts

    // For the gate's fanouts, they need to know the new, only one fanin #
    // For the gate's fanins, they need to know whether their fanouts become NULL
    // or fanouts of the deleted gate
    int newFaninNum;
    bool isFanin1FanoutNIL;
    bool isFanin2FanoutNIL;

    CirGate* it = 0;
    for (size_t i = 0; i < _DFSTrace.size(); ++i) {
        it = _DFSTrace[i];
        if (it->getTypeStr() != "AIG") continue;
        faninNum1 = it->getFanin1();
        faninNum2 = it->getFanin2();
        vector<CirGate*> fanouts;
        nFanouts = it->getFanoSize (fanouts);

        assert (faninNum1 >= 0 && faninNum2 >= 0);
        if (_vgate[faninNum1/2] == 0 || _vgate[faninNum2/2] == 0) continue;
        else if (faninNum1 == faninNum2) { // same inputs
            // only one of the fanins can add original fanouts
            // since they are the same gate
            // fanouts' mutual fanin: the original fanin1 num
            isFanin1FanoutNIL = false;
            isFanin2FanoutNIL = true;
            newFaninNum = faninNum1;
            //cout << "Fanins are both " << faninNum1 << endl;
        }
        else if (faninNum1/2 == faninNum2/2) { // complement gates
            // both fanins' fanouts become NIL
            // fanouts' fanins become CONST 0
            // CONST 0 catches original fanouts
            isFanin1FanoutNIL = isFanin2FanoutNIL = true;
            newFaninNum = 0;
            //cout << "Fanins are complemented (" << faninNum1 << ") & (" << faninNum2 << ")\n";
        }
        else if (faninNum1 == 0 || faninNum2 == 1) { // CONST 0 only at fanin 1
            // fanin1 catch original fanouts
            // fanin2's fanout becomes NIL
            // fanouts' fanin becomes CONST 0
            isFanin1FanoutNIL = false;
            isFanin2FanoutNIL = true;
            newFaninNum = faninNum1;
        }
        else if (faninNum2 == 0 || faninNum1 == 1) { // CONST 0 only at fanin 2
            isFanin1FanoutNIL = true;
            isFanin2FanoutNIL = false;
            newFaninNum = faninNum2;
        }
        else continue;
        
        isOpt = true;
        cout << "Simplifying: " << newFaninNum / 2 << " merging "
             << ((newFaninNum % 2 == 0) ? "" : "!") << it->getId() << "...\n";
        
        for (size_t j = 0; j < nFanouts; ++j) {
            // update fanins of the gate's fanouts (including vector and number)
            fanouts[j]->delFanins (it);
            if (fanouts[j]->getFanin1()/2 == (int) it->getId()) {
                if (fanouts[j]->getFanin1()%2 == 0) fanouts[j]->setFanin1 (newFaninNum);
                else if (newFaninNum%2 == 0) fanouts[j]->setFanin1 (newFaninNum+1);
                else { assert(newFaninNum>=1); fanouts[j]->setFanin1 (newFaninNum-1);}
                assert (_vgate[newFaninNum/2] != 0);
                fanouts[j]->setFanins (_vgate[newFaninNum/2]);
            }
            if (fanouts[j]->getFanin2()/2 == (int) it->getId()) {
                if (fanouts[j]->getFanin2()%2 == 0) fanouts[j]->setFanin2 (newFaninNum);
                else if (newFaninNum%2 == 0) fanouts[j]->setFanin2 (newFaninNum+1);
                else { assert(newFaninNum>=1); fanouts[j]->setFanin2 (newFaninNum-1);}
                assert (_vgate[newFaninNum/2] != 0);
                fanouts[j]->setFanins (_vgate[newFaninNum/2]);
            }

            // update fanouts of the gate's fanins (or CONST 0)
            _vgate[faninNum1/2]->delFanouts (it);
            _vgate[faninNum2/2]->delFanouts (it);
            if (isFanin1FanoutNIL && isFanin2FanoutNIL) _vgate[0]->setFanouts (fanouts[j]);
            else if (isFanin1FanoutNIL) _vgate[faninNum2/2]->setFanouts (fanouts[j]);
            else if (isFanin2FanoutNIL) _vgate[faninNum1/2]->setFanouts (fanouts[j]);
        }
        fanouts.clear();

        // deleting gate
        // cout << "Deleting gate " << it->getId() << endl;
        delete _vgate [it->getId()];
        _vgate [it->getId()] = 0;
        _DFSTrace [i] = 0;
    }

    if (!isOpt) return;

    // Update DFS list and not used gates
    vector<CirGate*> _olDFSTrace;
    for (size_t i = 0; i < _DFSTrace.size(); ++i) {
        if (_DFSTrace [i] == 0) continue;
        _olDFSTrace.push_back(_DFSTrace[i]);
        _DFSTrace[i]->setColor(WHITE);
    }
    
    _DFSTrace.clear();
    CirGate* tmp = 0;
    for (int i = 1; i <= headerInfo [3]; ++i) {
        tmp = _vgate [headerInfo [0] + i];
        DFSTrace (tmp, true);
    }

    for (size_t i = 0; i < _olDFSTrace.size(); ++i) {
        vector<CirGate*> fanouts;
        if (_olDFSTrace[i]->getColor() == WHITE) {
            // check used by other gates
            // cout << "White gate: " << _olDFSTrace[i]->getId() << endl;
            if (_olDFSTrace[i]->getTypeStr() != "CONST" && 
                _olDFSTrace[i]->getFanoSize(fanouts) == 0)
                _notusedGate.push_back (_olDFSTrace[i]->getId());
            _olDFSTrace[i]->setColor (GRAY);
        }
    }

    #if (DEBUG_H)
    cout << "DFS list:\n";
    for (size_t i = 0; i < _DFSTrace.size(); ++i) {
        _DFSTrace[i]->printGate ();
        cout << '\n';
    }

    cout << "Not used gates update:\n";
    for (size_t i = 0; i < _notusedGate.size(); ++i) {
        _vgate[_notusedGate[i]]->printGate();
        cout << endl;
    }
    #endif // DEBUF_H

}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
