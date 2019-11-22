/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H
//#define DEBUG_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode (const T &d, BSTreeNode<T>* l=0, BSTreeNode<T>* r=0, BSTreeNode<T>* p=0) 
      : _data(d), _left(l), _right(r), _parent(p) {};
   
   // data members
   T _data;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
   BSTreeNode<T>* _parent;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree () : _size(0) {
      // initialize dummy node
      _root = new BSTreeNode<T> (T());
      _root->_parent = 0;
      _root->_left = _root->_right = _root;
   }
   ~BSTree () {
      clear();
      delete _root;
   }

   class iterator 
   { 
      friend BSTree;
      
      public:
         iterator (BSTreeNode<T>* n=0) : _node(n) {}
         iterator (const iterator &i) : _node(i._node) {}
         ~iterator() {}

         // TODO:
         const T& operator*() const {return _node->_data;}
         T& operator*() {return _node->_data;}
         iterator& operator++ () {*this = successor (*this); return *this;}
         iterator operator++ (int) {
            iterator it = *this;
            ++(*this);
            return it;
         }
         iterator& operator-- () {*this = predecessor (*this); return *this;}
         iterator operator-- (int) {
            iterator it = *this;
            --(*this);
            return it;
         }
         iterator& operator= (const iterator &i) {
            this->_node = i._node;
            return *this;
         }
         bool operator== (const iterator &i) {
            if (this->_node == i._node) return true;
            else return false;
         }
         bool operator!= (const iterator &i) {
            if (this->_node != i._node) return true;
            else return false;
         }

      private:
         BSTreeNode<T>* _node;

         bool isDummy() const {
            if (this->_node->_left == this->_node &&
                this->_node->_right == this->_node)
               return true;
            else return false;
         }

         void findMin () {
            while (this->_node->_left != 0 && !isDummy())
               this->_node = this->_node->_left;
         }
         
         void findMax () {
            while (this->_node->_right != 0 && !isDummy())
               this->_node = this->_node->_right;
         }

         iterator successor (const iterator &i) 
         {
            assert (!isDummy());
            if (i._node->_right != 0)
            {
               iterator tmp (i._node->_right);
               #ifdef DEBUG_H
               if (tmp.isDummy()) cout << "\nget dummy node!\n";
               #endif
               tmp.findMin();
               return tmp;
            }
            else
            {
               iterator tmp1 (i._node);
               iterator tmp2 (i._node->_parent);
               while (tmp2._node != 0 && tmp1._node == tmp2._node->_right)
               {
                  tmp1._node = tmp2._node;
                  tmp2._node = tmp2._node->_parent;
               }
               assert (tmp2._node != 0);
               return tmp2;
            } 
         }

         iterator predecessor (const iterator &i) 
         {
            if (isDummy()) return i._node->_parent;
            else if (i._node->_left != 0)
            {
               iterator tmp (i._node->_left);
               tmp.findMax ();
               return tmp;
            }
            else
            {
               iterator tmp1 (i._node);
               iterator tmp2 (i._node->_parent);

               while (tmp2._node != 0 && tmp1._node == tmp2._node->_left)
               {
                  tmp1._node = tmp2._node;
                  tmp2._node = tmp2._node->_parent;
               }

               return tmp2;
            } 
         }
   };

   // TODO: member functions
   iterator begin() const {
      iterator it (_root);
      if (empty()) return it; // _root is dummy if empty
      
      it.findMin();
      assert (!it.isDummy());
      return it;
   }
   iterator end() const {
      iterator it (_root);
      if (empty()) {
         assert (it == begin());
         return it;
      }

      assert (!it.isDummy());
      it.findMax ();

      // make sure is dummy node
      assert (it.isDummy());
      return it;
   }
   bool empty() const {
      if (_size == 0) return true;
      else return false;
   }
   size_t size() const {return _size;}
   void insert (const T& x) {
      BSTreeNode<T>* newNode = new BSTreeNode<T> (x);
      BSTreeNode<T>* tmp1 = _root;
      BSTreeNode<T>* tmp2 = 0;

      while (tmp1 != 0 && !isDummy(tmp1))
      {
         tmp2 = tmp1;
         if (x < tmp1->_data) tmp1 = tmp1->_left;
         else tmp1 = tmp1->_right;
      }

      newNode->_parent = tmp2;
      if (tmp2 == 0) // Tree is empty
      {
         assert (isDummy (tmp1));
         tmp1->_parent = newNode;
         newNode->_right = tmp1;
         _root = newNode;
         assert (isDummy (newNode->_right));
      }
      else if (x < tmp2->_data) tmp2->_left = newNode;
      else
      {
         if (isDummy (tmp1))
         {
            tmp1->_parent = newNode;
            newNode->_right = tmp1;
         }
         tmp2->_right = newNode;
      }
      
      ++_size;
      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H
   }
   void pop_front () {
      if (empty()) return;
      iterator it = begin();

      #ifdef DEBUG_H
      cout << "delete: " << it._node->_data << endl;
      assert (it._node->_left == 0);
      assert (it != end());
      #endif // DEBUG_H

      deleteNode (it._node);

      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      it = begin();
      if (empty()) cout << "empty\n"; 
      else cout << "begin is: " << it._node->_data << endl;
      #endif // DEBUG_H
   }
   void pop_back () {
      if (empty()) return;
      iterator it = end();
      --it;

      #ifdef DEBUG_H
      cout << "end node to delete: " << it._node->_data << endl;
      assert (isDummy (it._node->_right));
      assert (it != end());
      #endif // DEBUG_H

      deleteNode (it._node);

      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      it = end();
      if (empty()) cout << "empty\n";
      else cout << "end node becomes: " << it._node->_parent->_data << endl;
      #endif // DEBUG_H
   }
   bool erase (iterator pos) {
      if (empty()) return false;
      assert (pos != end());
      
      deleteNode (pos._node);

      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H

      return true;
   }
   bool erase (const T& x) {
      if (empty()) return false;
      iterator it = find (x);
      if (it == end()) return false;
      else deleteNode (it._node);
      
      #ifdef DEBUG_H
      cout << "size: " << size() << endl;
      #endif // DEBUG_H

      return true;
   }
   iterator find (const T& x) {
      iterator it (_root);
      if (empty()) return it;

      while (it._node != 0 && !isDummy (it._node))
      {
         if (it._node->_data == x) return it;
         else if (it._node->_data < x) it._node = it._node->_right;
         else it._node = it._node->_left;
      }

      return end();
   }
   void clear () { while (_size > 0) deleteNode (_root); }
   void sort () {} // no action!
   void print () const { printChild (_root); }  

private:
   BSTreeNode<T>* _root;
   size_t _size;

   bool isDummy (const BSTreeNode<T>* n) const {
      if (n == 0) return false;
      else if (n->_left == n && n->_right == n) return true;
      else return false;
   }
   
   void transplant (BSTreeNode<T>* node1, BSTreeNode<T>* node2 = 0)
   {
      assert (!isDummy (node1));
      if (node1->_parent == 0)
         _root = node2;
      else if (node1 == node1->_parent->_left)
         node1->_parent->_left = node2;
      else node1->_parent->_right = node2;

      if (node2 != 0) node2->_parent = node1->_parent;
   } 

   void deleteNode (BSTreeNode<T>* node) 
   {
      assert (!isDummy(node));
      if (node->_left == 0)
         transplant (node, node->_right);
      else if (node->_right == 0)
         transplant (node, node->_left);
      else if (isDummy (node->_right))
      {
         // find the predecessor of node, assign it->right = dummy
         BSTreeNode<T>* pds = node->_left;
         assert (pds != 0);
         while (pds->_right != 0) pds = pds->_right;

         pds->_right = node->_right;
         node->_right->_parent = pds;
         node->_right = 0; 
         assert (isDummy (pds->_right));

         // back to case 2
         transplant (node, node->_left);
      }
      else
      {
         BSTreeNode<T>* tmp = node->_right;
         assert (!isDummy (tmp));
         while (tmp->_left != 0)
            tmp = tmp->_left;
         
         if (tmp->_parent != node)
         {
            transplant (tmp, tmp->_right);
            tmp->_right = node->_right;
            tmp->_right->_parent = tmp;
         }

         transplant (node, tmp);
         tmp->_left = node->_left;
         tmp->_left->_parent = tmp;
      }
      delete node;
      --_size;
   }

   void printChild (BSTreeNode<T>* const node, const int& height = 0) const
   {
      if (node == 0) { cout << "[0]\n"; return; }
      else if (isDummy (node)) { cout << "[1]\n"; return; }
      
      cout << node->_data << endl;
      
      for (int i = 0; i < height+1; ++i) cout << "  ";
      printChild (node->_left, height+1);
      for (int i = 0; i < height+1; ++i) cout << "  ";
      printChild (node->_right, height+1);
   }
};

#endif // BST_H
