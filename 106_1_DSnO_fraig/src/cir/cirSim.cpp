/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "../util/util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
#define MAXPATTERN 64
#define DEBUG_H 0
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
    int range;
    if (headerInfo[1] <= ceil (pow(2, 6)) && headerInfo[0] <= ceil (pow(2, 6))) range = 5 + headerInfo[1];
    else if (headerInfo[1] <= ceil (pow(2, 6))) range = headerInfo[1] * 12;
    else { range = headerInfo[1]*3;}
    
    int patterNum = 0;
    int tmpatterNum = patterNum;
    int rndNum = 0;
    for (int i = 0; i < 5; ++i) {
        if ((rndNum = rnGen(range)) == 0) { --i; continue;}
        patterNum += rndNum;
    }
    tmpatterNum = patterNum *= MAXPATTERN;


    typedef vector<CirGate*> FECgrp;

    // initialize...
    FECgrp* fecgrp = new FECgrp;
    fecgrp->push_back (_vgate[0]);
    for (size_t i = 0; i < _vgate.size(); ++i) {
        if (_vgate[i] == 0) continue;
        else if (_vgate[i]->getTypeStr() == "PI" || _vgate[i]->getTypeStr() == "CONST") {
            _vgate[i]->setSimDigit(0, false);
        }
        else { _vgate[i]->setSimDigit(0, true);}
        
        // put AIGs in DFS list into a hashgrp
        if (_vgate[i]->getColor() != BLACK || _vgate[i]->getTypeStr() != "AIG") continue;
        else fecgrp->push_back (_vgate[i]);
    }
    _FECgrpList.push_back (fecgrp);

    // gen PIs signals
    unordered_map<size_t, FECgrp*>* newFECmap;
    unordered_map<size_t, FECgrp*>::iterator it;
     //rndgenPattern(tmpatterNum)
     //int pttcount = 128;
    while (rndgenPattern(tmpatterNum)) {
        // do simulation on POs
        for (int po = headerInfo[0]+1; po <= (headerInfo[0]+headerInfo[3]); ++po) {
            assert (_vgate[po] != 0 && _vgate[po]->getTypeStr() == "PO");
            simDFSList (_vgate[po]);
        }
        /*
        cout << '\n';
        for (int i = 0; i <= headerInfo[0]+headerInfo[3]; ++i) {
            cout << "Gate (" << _vgate[i]->getId() << ") Type: " << _vgate[i]->getTypeStr()
                 << " with simulation signals " << _vgate[i]->getSimDigit() << endl;
        }
        */

        bool needSeperate;
        for (size_t listNum = 0; listNum < _FECgrpList.size(); ++listNum) {
            needSeperate = false;
            for (size_t i = 1; i < _FECgrpList[listNum]->size(); ++i) {
                if (_FECgrpList[listNum]->at(i)->getSimDigit() != _FECgrpList[listNum]->at(0)->getSimDigit() &&
                    (_FECgrpList[listNum]->at(i)->getSimDigit() ^ ULLONG_MAX) != _FECgrpList[listNum]->at(0)->getSimDigit()) {
                    needSeperate = true;
                    break;
                }
            }
            if (!needSeperate) continue;

            // Create an unordered map to implement hashing
            FECgrp* newfecgrp;
            newFECmap = new unordered_map<size_t, FECgrp*>;
            for (size_t i = 0; i < _FECgrpList[listNum]->size(); ++i) {
                if ((it = newFECmap->find(_FECgrpList[listNum]->at(i)->getSimDigit())) != newFECmap->end()) {
                    it->second->push_back(_FECgrpList[listNum]->at(i));
                }
                else if ((it = newFECmap->find(_FECgrpList[listNum]->at(i)->getSimDigit() ^ ULLONG_MAX)) 
                    != newFECmap->end()) {
                    it->second->push_back(_FECgrpList[listNum]->at(i));
                }
                else {
                    newfecgrp = new FECgrp;
                    newfecgrp->push_back (_FECgrpList[listNum]->at(i));
                    newFECmap->insert ({_FECgrpList[listNum]->at(i)->getSimDigit(), newfecgrp});
                    newfecgrp = 0;
                }
            }

            // copy fecgroups to the FEC group list and delete the original
            delete _FECgrpList[listNum];
            _FECgrpList[listNum] = _FECgrpList[_FECgrpList.size()-1];
            _FECgrpList.pop_back();
            --listNum;

            for (unordered_map<size_t, FECgrp*>::iterator it = newFECmap->begin(); it != newFECmap->end(); ++it)
                _FECgrpList.push_back (it->second);

            delete newFECmap;
        }
        /*
        // print pairs
        cout << "\nSimulate " << patterNum/64 << "th time...\n";
        for (size_t i = 0; i < _FECgrpList.size(); ++i) {
            cout << "[" << i << "]";
            for (size_t j = 0; j < _FECgrpList[i]->size(); ++j)
                cout << " " << _FECgrpList[i]->at(j)->getId();
            cout << '\n';
        }
        */
        
        // reseting...
        if (tmpatterNum <= 0) continue;
        for (size_t i = 0; i < _vgate.size(); ++i) {
            if (_vgate[i] == 0) continue;
            else if (_vgate[i]->getTypeStr() == "PI" || _vgate[i]->getTypeStr() == "CONST") 
                _vgate[i]->setSimDigit(0, false);
            else _vgate[i]->setSimDigit(0, true);
        }
    }
    cout << patterNum << " patterns simulated.\n";
    std::sort (_FECgrpList.begin(), _FECgrpList.end(), myVecomp);

    // collect partners
    for (size_t i = 0; i < _FECgrpList.size(); ++i) {
        if (_FECgrpList[i]->size() <= 1) continue;
        for (size_t j = 0; j < _FECgrpList[i]->size(); ++j) {
            for (size_t k = 0; k < _FECgrpList[i]->size(); ++k) {
                if (j == k) continue;
                _FECgrpList[i]->at(j)->setFECpartner(_FECgrpList[i]->at(k));
            }
        }
    }

    if (*_simLog) printSim2File (_simLog);
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    typedef vector<CirGate*> FECgrp;
    size_t simCount = 0;
    
    // initialize...
    assert (patternFile);
    FECgrp* fecgrp = new FECgrp;
    fecgrp->push_back (_vgate[0]);
    for (size_t i = 0; i < _vgate.size(); ++i) {        
        // put AIGs in DFS list into a hashgrp
        if (_vgate[i]->getColor() != BLACK || _vgate[i]->getTypeStr() != "AIG") continue;
        else fecgrp->push_back (_vgate[i]);
    }
    _FECgrpList.push_back (fecgrp);

    unordered_map<size_t, FECgrp*>* newFECmap;
    unordered_map<size_t, FECgrp*>::iterator it;
    while (parsePatternFile (patternFile, simCount)) {
        // reseting...
        for (size_t i = 0; i < _vgate.size(); ++i) {
            if (_vgate[i] == 0) continue;
            else if (_vgate[i]->getTypeStr() != "PI" && _vgate[i]->getTypeStr() != "CONST") 
                _vgate[i]->setSimDigit(0, true);
            else assert (_vgate[i]->getisSimed() == true);
        }
        
        // do simulation on POs
        for (int po = headerInfo[0]+1; po <= (headerInfo[0]+headerInfo[3]); ++po) {
            assert (_vgate[po] != 0 && _vgate[po]->getTypeStr() == "PO");
            simDFSList (_vgate[po]);
        }
        
        // Check if need hashing for every fecgrp containing multiple gates
        bool needSeperate;
        for (size_t listNum = 0; listNum < _FECgrpList.size(); ++listNum) {
            needSeperate = false;
            for (size_t i = 1; i < _FECgrpList[listNum]->size(); ++i) {
                if (_FECgrpList[listNum]->at(i)->getSimDigit() != _FECgrpList[listNum]->at(0)->getSimDigit() &&
                    (_FECgrpList[listNum]->at(i)->getSimDigit() ^ ULLONG_MAX) != _FECgrpList[listNum]->at(0)->getSimDigit()) {
                    needSeperate = true;
                    break;
                }
            }
            if (!needSeperate) continue;
            
            // Create an unordered map to implement hashing
            FECgrp* newfecgrp;
            newFECmap = new unordered_map<size_t, FECgrp*>;
            for (size_t i = 0; i < _FECgrpList[listNum]->size(); ++i) {
                if ((it = newFECmap->find(_FECgrpList[listNum]->at(i)->getSimDigit())) != newFECmap->end()) {
                    it->second->push_back(_FECgrpList[listNum]->at(i));
                }
                else if ((it = newFECmap->find(_FECgrpList[listNum]->at(i)->getSimDigit() ^ ULLONG_MAX)) 
                    != newFECmap->end()) {
                    it->second->push_back(_FECgrpList[listNum]->at(i));
                }
                else {
                    newfecgrp = new FECgrp;
                    newfecgrp->push_back (_FECgrpList[listNum]->at(i));
                    newFECmap->insert ({_FECgrpList[listNum]->at(i)->getSimDigit(), newfecgrp});
                    newfecgrp = 0;
                }
            }

            // copy fecgroups to the FEC group list and delete the original
            delete _FECgrpList[listNum];
            _FECgrpList[listNum] = _FECgrpList[_FECgrpList.size()-1];
            _FECgrpList.pop_back();
            --listNum;

            for (unordered_map<size_t, FECgrp*>::iterator it = newFECmap->begin(); it != newFECmap->end(); ++it)
                _FECgrpList.push_back (it->second);

            delete newFECmap;
        }
    }

    cout << simCount << " patterns simulated.\n";
    std::sort (_FECgrpList.begin(), _FECgrpList.end(), myVecomp);

    // collect partners
    for (size_t i = 0; i < _FECgrpList.size(); ++i) {
        if (_FECgrpList[i]->size() <= 1) continue;
        for (size_t j = 0; j < _FECgrpList[i]->size(); ++j) {
            for (size_t k = 0; k < _FECgrpList[i]->size(); ++k) {
                if (j == k) continue;
                _FECgrpList[i]->at(j)->setFECpartner(_FECgrpList[i]->at(k));
            }
        }
    }

    if (*_simLog) printSim2File (_simLog);
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
bool
CirMgr::parsePatternFile (ifstream& patternFile, size_t& simCount)
{
    string sPattern ("");
    int countLine = 0;

    // reset PIs
    int countPI = 0;
    _vgate[0]->setSimDigit (0, false);
    for (int i = 1; i <= headerInfo[0]; ++i) {
        if (_vgate[i] == 0) continue;
        else if (_vgate[i]->getTypeStr() != "PI") continue;
        else {
            _vgate[i]->setSimDigit (0, false);
            ++countPI;
        }
        if (countPI >= headerInfo[1]) break;
    }

    while (getline(patternFile, sPattern)) {
        size_t pos = sPattern.find_first_not_of(' ');
        size_t end = sPattern.length();
        for (size_t e = sPattern.length()-1; e > 0; --e)
            if (sPattern[e] == ' ') --end;

        if (end <= pos) sPattern = "";
        else sPattern = sPattern.substr (pos, end-pos);

        if (sPattern.length() != (size_t) headerInfo[1]) {
            cerr << "Error: pattern(" << sPattern << ") length(" << sPattern.length()
                 << ") does not match the number of inputs(" << headerInfo[1] 
                 << ") in a circuit!!\n";
            return false;
        }

        // set PIs
        int countPI = 0;
        for (int i = 1; i <= headerInfo[0]; ++i) {
            if (_vgate[i] == 0) continue;
            else if (_vgate[i]->getTypeStr() != "PI") continue;
            else ++countPI;

            if (sPattern[countPI-1] == '0') _vgate[i]->addSimDigit(false);
            else if (sPattern[countPI-1] == '1') _vgate[i]->addSimDigit(true);
            else {
                cerr << "Error: Pattern(" << sPattern << ") contains a non-0/1 "
                     << "character('" << int(sPattern[countPI-1]) << "')\n";
                return false;
            }
            // cout << "DEBUG: digit becomes " << _vgate[i]->getSimDigit() << endl;
        }
        assert (countPI == headerInfo[1]);
        ++countLine;
        if (countLine >= MAXPATTERN) break;
        else sPattern.clear();
    }
    simCount += countLine;
    if (countLine == 0) return false;
    else if (countLine < MAXPATTERN) { // need padding the remaining digits 
        int countPI = 0;
        for (int i = 1; i <= headerInfo[0]; ++i) {
            if (_vgate[i] == 0) continue;
            else if (_vgate[i]->getTypeStr() != "PI") continue;
            else ++countPI;

            for (int p = 0; p < (MAXPATTERN - countLine); ++p)
                _vgate[i]->addSimDigit(false);
        }
        assert (countPI == headerInfo[1]);
    }
    return true;
} 

bool 
CirMgr::rndgenPattern (int& patterNum)
{
    assert (patterNum % 64 == 0);
    if (patterNum <= 0) return false;
    else { // gen 64-bit patterns for PIs
        CirGate* tmp = 0;
        size_t num;
        for (int i = 1; i <= headerInfo[0]; ++i) {
            tmp = _vgate[i];
            if (tmp == 0) continue;
            else if (tmp->getTypeStr() != "PI") continue;
            else {
                assert (tmp->getTypeStr() == "PI");
                while ((num = ((size_t)rnGen(INT_MAX)<<1)+rnGen(2)) == 0);
                num = (num<<32) + (size_t)(rnGen(INT_MAX)<<1) + rnGen(2);
                tmp->setSimDigit (num, false);
            }
        }
    }
    patterNum -= 64;
    return true;
}

size_t
CirMgr::simDFSList (CirGate* gate)
{
    assert (gate != 0);
    //cout << "Call gate " << gate->getId() << endl;
    if (gate->getTypeStr() == "PI" || gate->getTypeStr() == "CONST") 
        return gate->getSimDigit();
    else if (gate->getisSimed()) {
        //cout << "No need recursive call... gate " << gate->getId() << "\n";
        return gate->getSimDigit();
    }
    else if (gate->getTypeStr() == "PO") { 
        int fanin = gate->getFanin1();
        if (_vgate[fanin/2] == 0) return 0;
        size_t pattern = simDFSList (_vgate[fanin/2]);
        if (fanin % 2 == 1) pattern = (pattern ^ ULLONG_MAX);
        gate->setSimDigit (pattern, false);
        return pattern;
    }
    else {
        assert (gate->getTypeStr() == "AIG");
        int fanin1 = gate->getFanin1();
        int fanin2 = gate->getFanin2();
        if (_vgate[fanin1/2] == 0 || _vgate[fanin2/2] == 0) return 0;
        size_t pattern1 = simDFSList (_vgate[fanin1/2]);
        size_t pattern2 = simDFSList (_vgate[fanin2/2]);
        //cout << "Get pattern of gate " << gate->getId() << ": " << pattern1 << ", " << pattern2 << endl;
        if (fanin1 % 2 == 1) pattern1 = (pattern1 ^ ULLONG_MAX);
        if (fanin2 % 2 == 1) pattern2 = (pattern2 ^ ULLONG_MAX);
        gate->setSimDigit ((pattern1 & pattern2), false);
        return (pattern1 & pattern2); // bitwise
    }

    // Note: if a gate contains undef fanin(s), return 0
}
/*
        for (int i = 0; i <= headerInfo[0]+headerInfo[3]; ++i) {
            cout << "Gate (" << _vgate[i]->getId() << ") Type: " << _vgate[i]->getTypeStr()
                 << " with simulation signals " << _vgate[i]->getSimDigit() << endl;
        }
        */

void 
CirMgr::printSim2File (ofstream* os) {
    assert (*os);
    int PIcount = 0;
    size_t PIpattern [headerInfo[1]];
    size_t POpattern [headerInfo[3]];
    for (int i = 1; i <= headerInfo[0]; ++i) {
        if (_vgate[i] == 0) continue;
        else if (_vgate[i]->getTypeStr() != "PI") continue;
        PIpattern [PIcount] = _vgate[i]->getSimDigit();
        if (++PIcount >= headerInfo[1]) break;
    }

    int POcount = 0;
    for (int i = headerInfo[0]+1; i <= headerInfo[0]+headerInfo[3]; ++i) {
        POpattern [POcount] = _vgate[i]->getSimDigit();
        ++POcount;
    }
    assert (POcount == headerInfo[3]);

    string PIpatternStr ("");
    string POpatternStr ("");
    for (int d = 0; d < MAXPATTERN; ++d) {
        for (int i = 0; i < headerInfo[1]; ++i) {
            PIpatternStr += to_string (PIpattern [i] % 2);
            PIpattern [i] /= 2;
        }

        for (int i = 0; i < headerInfo[3]; ++i) {
            POpatternStr += to_string (POpattern [i] % 2);
            POpattern [i] /= 2;
        }

        (*os) << PIpatternStr << " " << POpatternStr << '\n';

        PIpatternStr.clear();
        POpatternStr.clear();
    }
}
    