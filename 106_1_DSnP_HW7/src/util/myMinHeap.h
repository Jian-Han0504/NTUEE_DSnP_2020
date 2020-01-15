/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO:
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      _data.push_back (d);
      size_t pos = _data.size();
      while (pos > 1) {
         if (!(d < _data [pos/2 - 1])) break; // if d >= parent node
         swap (pos, pos / 2);
         pos /= 2;
      }
   }
   void delMin() { delData (0); }
   void delData (size_t i) { // delete data in _data [i]
      size_t pos = i + 1;
      _data [pos-1] = _data [_data.size()-1];
      _data.pop_back();
      size_t minpos = pos;
      while (pos <= _data.size()) {
         if (pos * 2 > _data.size()) break;
         else {
            if (_data [pos*2 - 1] < _data [pos-1]) minpos = pos*2;
            if (pos*2 + 1 <= _data.size() && _data [pos*2] < _data [minpos-1]) 
               minpos = pos*2 + 1;
            
            if (minpos == pos) break;
            else {
               swap (pos, minpos);
               pos = minpos;
            }
         }
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;

   void swap (const size_t pos1, const size_t pos2) {
      Data tmp = _data [pos1 - 1];
      _data [pos1 - 1] = _data [pos2 - 1];
      _data [pos2 - 1] = tmp;
   }
};

#endif // MY_MIN_HEAP_H
