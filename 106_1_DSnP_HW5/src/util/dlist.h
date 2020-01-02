/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H
//#define DEBUG_H
#include <cassert>

template <class T>
class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T &d, DListNode<T> *p = 0, DListNode<T> *n = 0) : _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T _data;
   DListNode<T> *_prev;
   DListNode<T> *_next;
};

template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList()
   {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList()
   {
      clear();
      delete _head;
   }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T> *n = 0) : _node(n) {}
      iterator(const iterator &i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T &operator*() const { return _node->_data; }
      T &operator*() { return _node->_data; }
      iterator &operator++()  { this->_node = this->_node->_next; return *(this); }
      iterator operator++(int){ auto temp = *(this); ++(*this); return temp; }
      iterator &operator--()  { this->_node = this->_node->_prev; return *(this); }
      iterator operator--(int){ auto temp = *(this); --(*this); return temp; }
      iterator &operator=(const iterator &i) { this->_node = i._node; return *(this); }

      bool operator!=(const iterator &i) const {
         if (this->_node != i._node) return true;
         else return false;
      }
      
      bool operator==(const iterator &i) const {
         if (this->_node == i._node) return true;
         else return false;
      }

   private:
      DListNode<T> *_node;
   };

   // TODO: implement these functions
   iterator begin() const { return _head; }
   iterator end() const { return _head->_prev; }
   bool empty() const
   {
      if (_head->_next == _head && _head->_prev == _head)
         return true;
      else
         return false;
   }
   size_t size() const
   {
      int nCount = 0;
      DListNode<T> *temp = _head;
      while (temp->_next != _head)
      {
         ++nCount;
         temp = temp->_next;
      }
      return nCount;
   }

   void push_back(const T &x)
   {
      if (empty())
      {
         DListNode<T> *newNode = new DListNode<T>(x, _head->_prev, _head->_next);
         _head = newNode;
         _head->_prev->_next = _head;
         _head->_next->_prev = _head;
      }
      else
      {
         DListNode<T> *newNode = new DListNode<T>(x, _head->_prev->_prev, _head->_prev);
         _head->_prev->_prev->_next = newNode;
         _head->_prev->_prev = newNode;
      }
      
      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H

      _isSorted = false;
   }
   void pop_front()
   {
      if (empty())
         return;
      DListNode<T> *temp = _head->_next;
      temp->_prev = _head->_prev;
      _head->_prev->_next = temp;

      delete _head;
      _head = temp;
      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H
   }
   void pop_back()
   {
      if (empty())
         return;
      DListNode<T> *temp = _head->_prev->_prev;
      temp->_prev->_next = temp->_next;
      temp->_next->_prev = temp->_prev;
      if (temp == _head)
         _head = temp->_prev;
      delete temp;
      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H
   }

   // return false if nothing to erase
   bool erase(iterator pos)
   {
      if (empty()) return false;
      else
      {
         pos._node->_next->_prev = pos._node->_prev;
         pos._node->_prev->_next = pos._node->_next;
         if (pos._node == _head) _head = pos._node->_next;
         delete pos._node;
      }
      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H
      _isSorted = false;
      return true;
   }
   bool erase(const T &x)
   {
      iterator it = find(x);
      if (it == end())
         return false;
      else
      {
         it._node->_next->_prev = it._node->_prev;
         it._node->_prev->_next = it._node->_next;
         if (it._node == _head) _head = it._node->_next;
         delete it._node;
      }
      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H

      _isSorted = false;
      return true;
   }

   iterator find(const T &x)
   {
      iterator it = begin();
      while (it != end())
      {
         if (it._node->_data == x) return it;
         else it._node = it._node->_next;
      }

      return end();
   }

   void clear()
   {
      while (_head != _head->_prev)
         pop_front();
   } // delete all nodes except for the dummy node

   void sort() const {
      if (_isSorted) return;
      else if (_head->_prev == _head) return; // no element
      else if (size() == 1) return;
      else insertSort (_head, _head->_prev->_prev);
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T> *_head;    // = dummy node if list is empty
   mutable bool _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void insertSort (DListNode<T>* _beg, DListNode<T>* _end) const
   {
      if (_beg == _end) return;
      DListNode<T>* _stn = _beg;
      while (_stn != _end)
      {
         _stn = _stn->_next;
         T _key = _stn->_data;
         DListNode<T>* _temp = _stn->_prev;
         while (_temp->_next != _beg && _temp->_data > _key)
         {
            _temp->_next->_data = _temp->_data;
            _temp = _temp->_prev;
         }

         _temp->_next->_data = _key;
      }

      _isSorted = true;
   }
};

#endif // DLIST_H
