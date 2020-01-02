/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "../util/util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
    // TODO: to read in data from Json file and store them in a DB 
    // - You can assume the input file is with correct JSON file format
    // - NO NEED to handle error file format
    // Note that "is" already an non-empty file
    assert (j._obj.empty());

    j._isReadIn = false;
  
    while (1)
    {
        string sName = "";
        int nValue = 0;
        string sLineContext;

        getline (is, sLineContext);
        
        if (is.eof())
        {
            is.unget();
            is.clear();
            break;
        }

        // check if need push back <- if a line contains any "
        bool bIsPush = false;
        for (int i = 0; i < (int) sLineContext.length(); ++i)
        {
            if (sLineContext[i] == 34)
                bIsPush = true;
        }

        if (bIsPush)
        {
            int nBraceCount = 0;
            bool bCalcNum = false;
            bool isNegative = false; //whether the number is negative
            for (int i = 0; i < (int) sLineContext.length(); ++i)
            {
                // get element key
                if (sLineContext[i] == 34)
                    nBraceCount++; // note that str_name[i] is a "const char"
                if (nBraceCount % 2 == 1 && sLineContext[i] != 34)
                    sName += sLineContext[i];

                //get element value
                if (sLineContext[i] == 58) bCalcNum = true;
                else if (sLineContext[i] == 44) bCalcNum = false;
        
                if (bCalcNum == true && sLineContext[i] >= 48 && sLineContext[i] <= 57)
                    nValue = nValue*10 + (int) sLineContext[i] - 48; // num = ascii - 48
                else if (bCalcNum == true && sLineContext[i] == 45) isNegative = true;
            }
      
            // some have to change to negative
            if (isNegative) nValue = -nValue;

            // assign to JsonElem
            DBJsonElem Jelem (sName, nValue);
            j._obj.push_back (Jelem);
        }
    }
    j._isReadIn = true;

    return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
    // TODO
    // namely print in hw1
    cout << "{\n";
    for (int i = 0; i < (int)j._obj.size(); ++i)
    {
        if (i == (int)j._obj.size() - 1) cout << '\t' << j._obj[i] << "\n}\n";
        else cout << '\t' << j._obj[i] << ",\n";
    }
    if (j._obj.size() == 0) cout << "}\n";

    cout << "Total JSON elements: " << j._obj.size() << endl;
    
    return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
    // TODO
    while ((int)_obj.size() > 0)
        _obj.pop_back();

    _isReadIn = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{ 
    if (!*this) return true;
    for (int i = 0; i < (int) _obj.size(); ++i)
    {
        if (_obj[i].key() == elm.key())
            return false;
    }

    _obj.push_back(elm);

    return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
    // TODO
    int nSum = 0;

    if ((int) _obj.size() == 0) return NAN;
    for (int i = 0; i < (int) _obj.size(); ++i)
        nSum += _obj[i].value();

    if (nSum != 0.0) return (nSum * 1.0 / (int) _obj.size());

    return 0.0;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if (_obj.size() == 0)
      idx = _obj.size();
   else
   {
      for (int i = 0; i < (int) _obj.size(); ++i)
      {
          if (!(_obj[i].value() > maxN)) continue;
          maxN = _obj[i].value();
          idx = i;
      }
   }

   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if (_obj.size() == 0)
      idx = _obj.size();
   else
   {
      for (int i = 0; i < (int) _obj.size(); ++i)
      {
          if (!(_obj[i].value() < minN)) continue;
          minN = _obj[i].value();
          idx = i;
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   if (_obj.size() == 0) return 0;
   for (int i = 0; i < (int) _obj.size(); ++i)
      s += _obj[i].value();
   return s;
}
