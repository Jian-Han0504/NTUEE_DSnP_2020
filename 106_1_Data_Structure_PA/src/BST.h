#ifndef BST_H
#define BST_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>
#include <climits>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

bool myStr2Int (const string&, int&);
size_t myStrGetTok (const string&, string&, size_t, const char);

class BSTreeNode
{
    friend class BSTree;
    friend class BSTstd;
    friend class BSTsplay;

    BSTreeNode (const int& val=INT_MAX, BSTreeNode* l=0, BSTreeNode* r=0, BSTreeNode* p=0) :
        _value(val), _left(l), _right(r), _parent(p) { _x = 0; _y = 0;}
    ~BSTreeNode () { _left = _right = _parent = 0;}

    int _value;

    // coordinates
    int _x; // in-order traversal number
    int _y; // depth

    BSTreeNode* _left;
    BSTreeNode* _right;
    BSTreeNode* _parent;

    bool _isDummy () const {
        if (_left == this && _right == this) return true;
        else return false;
    }

    void setX (const int& x) { _x = x;}
    void setY (const int& y) { _y = y;}
};

class BSTree
{
    public:
        BSTree () : _size(0) {
            // initialize dummy node
            _root = new BSTreeNode (INT_MAX);
            _root->_parent = 0;
            _root->_left = _root->_right = _root;
        }
        virtual ~BSTree () {
            for (int i = 0; i < maxDepth; ++i) nodes[i].clear();
            delete [] nodes;
        }

        bool readTree (const string&, const size_t&);
        bool writeTree (const string&) const;

        class iterator
        {
            friend BSTree;
        public:
            iterator(BSTreeNode* nd=0) : _node(nd) {}
            ~iterator() { _node = 0;}

            const BSTreeNode& operator*() const { return *_node; }
            BSTreeNode& operator*() { return *_node; }
            iterator& operator++ () {*this = successor (*this); return *this;}
            iterator operator++ (int) { 
                iterator tmp = *this;
                ++tmp;
                return tmp; // *this does not plus 1
            }
            bool operator== (const iterator &i) {
                if (_node == i._node) return true;
                else return false;
            }
            bool operator!= (const iterator &i) {
                if (_node != i._node) return true;
                else return false;
            } 

        private:
            BSTreeNode* _node;

            bool isDummy () const { return _node->_isDummy();}

            void findMin () {
                while (this->_node->_left != 0 && !isDummy())
                    this->_node = this->_node->_left;
            }
            void findMax () {
                while (_node->_right != 0 && !isDummy())
                    _node = _node->_right;
            }
            iterator successor (const iterator &i) 
            {
                assert (!isDummy());
                if (i._node->_right != 0)
                {
                    iterator tmp (i._node->_right);
                    tmp.findMin();
                    return tmp;
                }
                else {
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
        }; 

        iterator begin() const {
            iterator it (_root);
            if (empty()) return it;
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
            assert (it.isDummy()); // make sure dummy node
            return it;
        }

        bool empty() const {
            if (_size == 0) return true;
            else return false;
        }

        size_t size() const { return _size;}
        void trace();
        void print (ostream&, ostream&, ostream&) const;
        virtual void insert (const int& val) = 0;
        virtual void setY (BSTreeNode*) = 0;
        virtual void clear () = 0;
         
    protected:
        BSTreeNode* _root;
        vector<BSTreeNode*>* nodes; // stores nodes in diffenet level (in-order)
        int maxDepth;
        size_t _size;

        void deleteNode (BSTreeNode*);
        void transplant (BSTreeNode*, BSTreeNode*);

    private:
        int getDepth ();

        // print
        void printPar (BSTreeNode*, ostream&) const;
        void printTree (BSTreeNode*, ostream&) const;
        void printBound (BSTreeNode*, ostream&) const;
        
};

class BSTstd : public BSTree
{
    ~BSTstd() { clear();}
    void insert (const int&);
    void clear () { while (_size > 0) deleteNode (_root);}
    void setY (BSTreeNode*) {}
};

class BSTsplay : public BSTree
{
    ~BSTsplay() { clear();}
    void insert (const int&);
    void clear () { while (_size > 0) deleteNode (_root);}

    void rightRotate (BSTreeNode*);
    void leftRotate (BSTreeNode*);
    void splay (BSTreeNode*);
    void split (BSTreeNode*);

    void setY (BSTreeNode*);
};
#endif // BST_H