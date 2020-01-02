/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>
//#define DEBUG_H

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {_isSorted = true;}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*this); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { this->_node = this->_node + 1; return (*this); }
      iterator operator ++ (int) { iterator tmp = *this; ++(*this); return tmp; }
      iterator& operator -- () { this->_node = this->_node - 1; return (*this); }
      iterator operator -- (int) { iterator tmp = *this; --(*this); return tmp; }

      iterator operator + (int i) const { return this->_node + i; }
      iterator& operator += (int i) { this->_node = this->_node + i; return (*this); }

      iterator& operator = (const iterator& i) { return (*this); }

      bool operator != (const iterator& i) const { 
         if (this->_node != i._node) return true;
         return false; 
      }
      bool operator == (const iterator& i) const { 
         if (this->_node == i._node) return true;
         return false; 
      }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
      if (_capacity == 0) return 0;
      else return _data; 
   }
   iterator end() const { 
      if (_capacity == 0) return 0;
      else if (empty()) return begin();
      else 
      {
         iterator it = begin();
         it += size();
         return it;
      }
   }
   bool empty() const { 
      if (_size == 0) return true;
      return false; 
   }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) { 
      
      if (_size == _capacity)
      {
         if (_capacity >= 8) _capacity *= 2;
         else ++_capacity;

         resizeData (_capacity);
      }

      _data [_size++] = x;

      _isSorted = false;
   }
   void pop_front() {
      if (empty()) return;
      else if (_size > 1) _data [0] = _data [_size-1];
      _isSorted = false;
      pop_back();
   }
   void pop_back() {
      if (empty()) return;
      _size--;
      
      #ifdef DEBUG_H
      cout << "size: " << _size << endl;
      #endif //DEBUG_H
   }

   bool erase (iterator pos) { 
      if (empty()) return false;
      *pos = _data [_size - 1];
      pop_back ();

      _isSorted = false;
      
      return true; 
   }
   bool erase (const T& x) { 
      iterator it = find (x);
      if (it == 0 || it == end()) return false;
      else erase (it);
      return true; 
   }

   iterator find (const T& x) {
      if (_capacity == 0) return 0;
      
      for (iterator it = begin(); it != end(); ++it)
         if (*(it._node) == x) return it;
      
      return end(); 
   }

   void clear() { while (_size > 0) pop_back(); _isSorted = true;}

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { 
      if (!empty() && !_isSorted) ::sort(_data, _data+_size);
   }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
   void resizeData (size_t n)
   {
      // _size is to be altered only after this function!
      T* tmp = _data;
      _data = new T [n];
      for (size_t i = 0; i < _size; ++i) _data[i] = tmp[i];
      
      delete [] tmp;
   }
};

#endif // ARRAY_H
