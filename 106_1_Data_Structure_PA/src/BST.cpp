#include <iostream>
#include "BST.h"
using namespace std;

bool myStr2Int (const string& str, int& num) 
{
    num = 0;
    size_t i = 0;
    int sign = 1;
    if (str[0] == '-') { sign = -1; i = 1; }
    bool valid = false;
    for (; i < str.size(); ++i) {
        if (isdigit(str[i])) {
            num *= 10;
            num += int (str[i] - '0'); // ASCII
            valid = true;
        }
        else return false;
    }
    num *= sign;
    return valid;
}

size_t myStrGetTok (const string& str, string& tok, size_t pos = 0, const char del = ' ')
{
   size_t begin = str.find_first_not_of(del, pos);
   if (begin == string::npos) { tok = ""; return begin; }
   size_t end = str.find_first_of(del, begin);
   tok = str.substr(begin, end - begin);
   return end;
}

bool BSTree::readTree (const string& tree, const size_t& lineNum)
{
    assert (tree.length() > 0); 
    string newVal("");
    int nodeVal;
    size_t pos = 0;
    size_t newpos = pos;

    while (newpos < tree.length()) {
        newpos = myStrGetTok (tree, newVal, pos);
        pos = newpos;
        if (newpos+1 == tree.length()) { // extra space at the end
            cerr << "Error: Extra space at the end of the line!!\n";
            return false;
        }
        else if (tree [newpos+1] == ' ') { // extra space between two nodes
            cerr << "Error: Extra space in line " << lineNum << ", col " << newpos+2 << "!!\n";
            return false;
        }
        else if (!myStr2Int (newVal, nodeVal)) {
            cerr << "Error: Illegal node value (" << newVal << ")!!\n";
            return false;
        }
        insert (nodeVal);
    }
    trace ();

    #ifdef DEBUG_H
    cout << _root->_value << endl;
    assert (!_root->_isDummy());
    for (iterator it = begin(); it != end(); ++it) {
        assert (!(*it)._isDummy());
        cout << (*it)._value << ", (" << (*it)._x << ", " << (*it)._y << ")\n";
    }
    #endif // DEBUG_H

    return true;
}

void BSTree::trace()
{
    int trace = 0;
    maxDepth = 0;
    setY (_root);
    for (iterator it = begin(); it != end(); ++it, ++trace) {
        (*it).setX (trace);
        if (-(*it)._y > maxDepth) maxDepth = -(*it)._y; // negative
    }
    
    //for (iterator it = begin(); it != end(); ++it)
        //cout << "Node (" << (*it)._value << ") at (" << (*it)._x << ", " << (*it)._y << ")\n";

    nodes = new vector<BSTreeNode*> [++maxDepth];
    for (iterator it = begin(); it != end(); ++it)
        nodes[-(*it)._y].push_back (&(*it));    
}

bool::BSTree::writeTree (const string& out) const
{
    string fileName = out + ".txt"; 
    ofstream oTree (fileName, std::fstream::app);
    if (!oTree) {
        cerr << "Cannot open file " << fileName << "!!\n";
        return false; 
    }

    fileName = out + "_PRep.txt";
    ofstream oPRep (fileName, std::fstream::app);
    if (!oPRep) {
        cerr << "Cannot open file " << fileName << "!!\n";
        return false; 
    }

    fileName = out + "_boundary.txt";
    ofstream oBound (fileName, std::fstream::app);
    if (!oBound) {
        cerr << "Cannot open file " << fileName << "!!\n";
        return false; 
    }

    print (oTree, oPRep, oBound);
    return true;
}

void BSTree::print (ostream& tree, ostream& prep, ostream& bound) const
{
    printPar (_root, prep);
    prep << '\n';

    printTree (_root, tree);

    printBound (_root, bound);
}

void BSTree::printPar (BSTreeNode* node, ostream& os) const 
{
    assert (node != 0 && !node->_isDummy());
    os << node->_value;
    if (node->_left == 0 && 
        (node->_right == 0 || node->_right->_isDummy())) return; 
    else {
        os << "(";
        if (node->_left == 0) os << "-";
        else printPar (node->_left, os);
        os << "\u00A0";
        if (node->_right == 0 || node->_right->_isDummy()) os << "-";
        else printPar (node->_right, os);
        os << ")";
    }
}
void BSTree::printTree (BSTreeNode* node, ostream& os) const
{
    int width = 0;
    for (int i = 0; i < maxDepth; ++i) {
        for (size_t j = 0; j < nodes[i].size(); ++j) {
            if (j > 0) width = nodes[i][j]->_x - nodes[i][j-1]->_x;
            else width = nodes[i][j]->_x + 1;
            for (int i = 0; i < 3*(width-1); ++i) os << "\u00A0";
            
            os << nodes[i][j]->_value;
            if (nodes[i][j]->_value < 10) os << "\u00A0" << "\u00A0";
            else if (nodes[i][j]->_value < 100) os << "\u00A0";
        }
        os << '\n';
    }
    
    #ifdef DEBUG_H
    for (int i = 0; i < maxDepth; ++i) {
        for (size_t j = 0; j < nodes[i].size(); ++j) {
            cout << nodes[i][j]->_x << ", " << nodes[i][j]->_y << endl;
        }
    }
    #endif // DEBUG_H
}
void BSTree::printBound (BSTreeNode* node, ostream& os) const // left boundary
{
    for (int i = 0; i < maxDepth; ++i) { 
        os << nodes[i][0]->_value;
        if (i < maxDepth-1) os << ' ';
    }
    os << '\n';
}

void BSTstd::insert (const int& val)
{   
    //cout << "Inserting node (" << val << ")....\n";
    BSTreeNode* newNode = new BSTreeNode(val);
    BSTreeNode* tmp1 = _root;
    BSTreeNode* tmp2 = 0;

    while (tmp1 != 0 && !tmp1->_isDummy()) {
        tmp2 = tmp1;
        if (val <= tmp1->_value) tmp1 = tmp1->_left;
        else tmp1 = tmp1->_right;
    }
    
    newNode->_parent = tmp2;
    if (tmp2 == 0) { // Tree is empty
        assert (tmp1->_isDummy());
        tmp1->_parent = newNode;
        newNode->_right = tmp1;
        _root = newNode;
        newNode->setY(0);
        assert (newNode->_right->_isDummy());
    }
    else if (val <= tmp2->_value) {
        tmp2->_left = newNode;
        if (newNode->_parent != _root) assert (newNode->_parent->_y < 0);
        newNode->setY(newNode->_parent->_y-1);
    }
    else {
        if (tmp1 != 0 && tmp1->_isDummy ()) {
            tmp1->_parent = newNode;
            newNode->_right = tmp1;
        }
        tmp2->_right = newNode;
        newNode->setY(newNode->_parent->_y-1);
    }
      
    ++_size;
}

void BSTree::deleteNode (BSTreeNode* node) 
{
    assert (!node->_isDummy());
    if (node->_left == 0)
        transplant (node, node->_right);
    else if (node->_right == 0)
        transplant (node, node->_left);
    else if (node->_right->_isDummy()) {
        // find the predecessor of node, assign it->right = dummy
        BSTreeNode* pds = node->_left;
        assert (pds != 0);
        while (pds->_right != 0) pds = pds->_right;

        pds->_right = node->_right;
        node->_right->_parent = pds;
        node->_right = 0; 
        assert (pds->_right->_isDummy());
        transplant (node, node->_left);
    }
    else {
        BSTreeNode* tmp = node->_right;
        assert (!tmp->_isDummy());
        while (tmp->_left != 0) tmp = tmp->_left;

        if (tmp->_parent != node) {
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
void BSTree::transplant (BSTreeNode* node1, BSTreeNode* node2=0)
{
    assert (!node1->_isDummy());
    if (node1->_parent == 0) _root = node2;
    else if (node1 == node1->_parent->_left)
        node1->_parent->_left = node2;
    else node1->_parent->_right = node2;

    if (node2 != 0) node2->_parent = node1->_parent;
}

void BSTsplay::rightRotate (BSTreeNode* n)
{
    assert (n->_left != 0);
    BSTreeNode* tmp = n->_left;
    n->_left = tmp->_right;
    if (tmp->_right != 0) tmp->_right->_parent = n;
    tmp->_parent = n->_parent;
    if (n->_parent == 0) _root = tmp;
    else if (n == n->_parent->_left) n->_parent->_left = tmp;
    else n->_parent->_right = tmp;
    tmp->_right = n;
    n->_parent = tmp;
}

void BSTsplay::leftRotate (BSTreeNode* n)
{
    assert (n->_right != 0);
    BSTreeNode* tmp = n->_right;
    n->_right = tmp->_left;
    if (tmp->_left != 0) tmp->_left->_parent = n;
    tmp->_parent = n->_parent;
    if (n->_parent == 0) _root = tmp;
    else if (n->_parent->_left == n) n->_parent->_left = tmp;
    else n->_parent->_right = tmp;
    tmp->_left = n;
    n->_parent = tmp;
}

void BSTsplay::splay (BSTreeNode* n)
{
    assert (n != 0);
    if (n == _root) return;
    else if (n->_parent == _root && n->_parent->_left == n) rightRotate(n->_parent);
    else if (n->_parent == _root && n->_parent->_right == n) leftRotate(n->_parent);
    else {
        if (n == n->_parent->_left && n->_parent == n->_parent->_parent->_left) { // zig-zig
            rightRotate (n->_parent->_parent);
            rightRotate (n->_parent);
            splay (n);
        }
        else if (n == n->_parent->_right && n->_parent == n->_parent->_parent->_left) { // zig-zag
            leftRotate (n->_parent);
            rightRotate (n->_parent);
            splay (n);
        }
        else if (n == n->_parent->_right && n->_parent == n->_parent->_parent->_right) { // zag-zag
            leftRotate (n->_parent->_parent);
            leftRotate (n->_parent);
            splay (n);
        }
        else { // zag-zig
            rightRotate (n->_parent);
            leftRotate (n->_parent);
            splay (n);
        }
    }
}

void BSTsplay::insert (const int& val)
{
    //cout << "Inserting node (" << val << ")....\n";
    BSTreeNode* newNode = new BSTreeNode(val);
    BSTreeNode* tmp1 = _root;
    BSTreeNode* tmp2 = 0;

    while (tmp1 != 0 && !tmp1->_isDummy()) {
        tmp2 = tmp1;
        if (val <= tmp1->_value) tmp1 = tmp1->_left;
        else tmp1 = tmp1->_right;
    }
    
    newNode->_parent = tmp2;
    if (tmp2 == 0) { // Tree is empty
        assert (tmp1->_isDummy());
        tmp1->_parent = newNode;
        newNode->_right = tmp1;
        _root = newNode;
        assert (newNode->_right->_isDummy());
    }
    else if (val <= tmp2->_value) tmp2->_left = newNode;
    else {
        if (tmp1 != 0 && tmp1->_isDummy ()) {
            tmp1->_parent = newNode;
            newNode->_right = tmp1;
        }
        tmp2->_right = newNode;
    }
    ++_size;
    splay (newNode);

    
    /*
    if (empty()) {
        newNode->_right = _root;
        _root->_parent = newNode;
        _root = newNode;
    }
    else split (newNode);
    */

    #ifdef DEBUG_H
    for (iterator it = begin(); it != end(); ++it) {
        cout << "Trace: " << (*it)._value << endl;
    }
    #endif // DEBUG_H
}

void BSTsplay::split (BSTreeNode* n)
{
    assert (n != 0);
    iterator it = begin();
    for (; it != end(); ++it)
        if ((*(it++))._value >= n->_value) break;

    splay (&(*it));
    if (it == begin() && n->_value < (*it)._value) { // less than begin()
        n->_right = &(*it);
        (*it)._parent = n;
    }
    else {
        n->_right = (*it)._right;
        if ((*it)._right != 0) (*it)._right->_parent = n;
        (*it)._right = 0;
        (*it)._parent = n;
        n->_left = &(*it);
    }
    _root = n;
}

void BSTsplay::setY (BSTreeNode* n)
{
    if (n->_isDummy()) return;
    else if (n == _root) n->setY(0);
    else n->setY (n->_parent->_y - 1);
    if (n->_left != 0) setY (n->_left);
    if (n->_right != 0) setY (n->_right);
}