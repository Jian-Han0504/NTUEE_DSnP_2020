/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "../sat/sat.h"
#include "../util/myHashMap.h"
#include "../util/util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    HashMap<strashKey, CirGate*> _hash (headerInfo[4]);
    strashKey* _keyPair = 0;
    
    vector<CirGate*> fanouts;
    size_t nFanouts;
    int fanin1;
    int fanin2;
    for (size_t dfs = 0; dfs < _DFSTrace.size(); ++dfs) {
        if (_DFSTrace[dfs]->getTypeStr() != "AIG") continue;
        assert (_DFSTrace[dfs]->getFanin1() >= 0 && _DFSTrace[dfs]->getFanin2() >= 0);
        fanin1 = (size_t) _DFSTrace[dfs]->getFanin1();
        fanin2 = (size_t) _DFSTrace[dfs]->getFanin2();
        (_keyPair = 0) = new strashKey ((size_t) _DFSTrace[dfs]->getFanin1(), 
                                (size_t) _DFSTrace[dfs]->getFanin2());
        if (_hash.insert(*_keyPair, _DFSTrace[dfs])) continue;
        
        // merging...
        CirGate* mergeGate = _DFSTrace[dfs];
        _hash.query(*_keyPair, mergeGate);
        cout << "Strashing: " << mergeGate->getId() << " merging " 
             << _DFSTrace[dfs]->getId() << "...\n";
        
        // updates fanins' fanouts
        fanin1 = _DFSTrace[dfs]->getFanin1();
        fanin2 = _DFSTrace[dfs]->getFanin2();
        assert (fanin1 >= 0 && fanin2 >= 0);
        bool isNilIn = false;
        if (_vgate[fanin1/2] == 0) isNilIn = true;
        else {
            _vgate[fanin1/2]->delFanouts (_DFSTrace[dfs]);
            //_vgate[fanin1/2]->setFanouts (mergeGate);
        }
        
        if (_vgate[fanin2/2] == 0) isNilIn = true;
        else {
            _vgate[fanin2/2]->delFanouts (_DFSTrace[dfs]);
            //_vgate[fanin2/2]->setFanouts (mergeGate);
        }

        // update nilFaninGate
        if (isNilIn) { 
            for (size_t i = 0; i < _nilfaninGate.size(); ++i) {
                if ((size_t) _nilfaninGate[i] != _DFSTrace[dfs]->getId()) continue;
                _nilfaninGate[i] = _nilfaninGate[_nilfaninGate.size()-1];
                _nilfaninGate.pop_back();
            }
        }

        // update fanouts: replace by the mergeGate
        nFanouts = _DFSTrace[dfs]->getFanoSize (fanouts);
        for (size_t i = 0; i < nFanouts; ++i) {
            if ((size_t) fanouts[i]->getFanin1()/2 == _DFSTrace[dfs]->getId()) {
                if (fanouts[i]->getFanin1() % 2 == 0) 
                    fanouts[i]->setFanin1 (2*mergeGate->getId());
                else fanouts[i]->setFanin1 (2*mergeGate->getId() + 1);
            }

            if ((size_t) fanouts[i]->getFanin2()/2 == _DFSTrace[dfs]->getId()) {
                if (fanouts[i]->getFanin2() % 2 == 0) 
                    fanouts[i]->setFanin2 (2*mergeGate->getId());
                else fanouts[i]->setFanin2 (2*mergeGate->getId() + 1);
            }

            fanouts[i]->delFanins(_DFSTrace[dfs]);
            fanouts[i]->setFanins(mergeGate);
            mergeGate->setFanouts (fanouts[i]);
        }
        fanouts.clear();

        // delete gate
        delete _vgate [_DFSTrace[dfs]->getId()];
        _vgate [_DFSTrace[dfs]->getId()] = 0;
        _DFSTrace [dfs] = 0;
    }

    // redo DFSTrace
    for (size_t i = 0; i < _nilfaninGate.size(); ++i) {
        if (_vgate[_nilfaninGate[i]]->getColor() != BLACK) continue;
        _nilfaninGate[i] = _nilfaninGate[_nilfaninGate.size()-1];
        _nilfaninGate.pop_back();
        --i;
    }

    for (size_t i = 0; i < _DFSTrace.size(); ++i) {
        if (_DFSTrace[i] == 0) continue;
        _DFSTrace[i]->setColor (WHITE);
    }
    _DFSTrace.clear();

    CirGate* tmp = 0;
    for (int i = 1; i <= headerInfo [3]; ++i) {
        tmp = _vgate [headerInfo [0] + i];
        DFSTrace (tmp, true);
    }
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
