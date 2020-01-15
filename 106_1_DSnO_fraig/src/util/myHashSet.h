/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator (vector<Data>* d=0, const size_t bucknum=0, const size_t buck=0, const size_t idx=0) { 
         setVector (d, bucknum);
         setIndex (buck, idx); 
      }
      const Data& operator * () const { 
         assert (_data != 0);
         return _data [_buckIdx][_vecIdx]; 
      }
      iterator& operator ++ () {
         if (_vecIdx < _data[_buckIdx].size()-1) ++_vecIdx;
         else {
            ++_buckIdx;
            while (_data[_buckIdx].size() == 0 && _buckIdx < _bucketnums) 
               ++_buckIdx;
            
            _vecIdx = 0;
            if (_buckIdx == _bucketnums) _data = 0; // if meets end(), set data = 04
         }
         #ifdef DEBUG_H
         cout << "call \"++\" operator... ";
         if (_data == 0) cout << "Note!! Data set to 0 since reach the end()... ";
         cout << "now at bucket: " << _buckIdx << ", vector: " << _vecIdx << endl;
         #endif // DEBUG_H
         return (*this); 
      }
      bool operator != (const iterator& i) const {
         #ifdef DEBUG_H
         cout << "\ncall \"!=\" operator...\n";
         cout << "Comparing with bucket " << i._buckIdx << ", index " << i._vecIdx << endl << endl; 
         #endif // DEBUG_H
         if (_buckIdx == i._buckIdx && _vecIdx == i._vecIdx) return false;
         else return true; 
      }
      bool operator == (const iterator& i) const { return !(*this != i); }

      void printIndex () const { 
         if (_data == 0) {
            cout << "NULL vector....\n";
            cout << "But with bucketnumbers: " << _bucketnums << " and at bucket: "
                 << _buckIdx << ", at vector: " << _vecIdx << endl;
            return;
         }

         for (size_t i = 0; i < _bucketnums; ++i) {
            if (_data[i].size() == 0) cout << "bucket: " << i << ", size: 0\n";
            else cout << "bucket: " << i << ", size: " << _data[i].size() << endl;
         }
         cout << "Now at bucket: " << _buckIdx << ", at vector: " << _vecIdx << endl;
         cout << "==================== DATA =======================\n";
         for (size_t i = 0; i < _bucketnums; ++i) {
            for (size_t j = 0; j < _data[i].size(); ++j) {
               cout << "bucket: " << i << ", index: " << j << ", data is " << _data[i][j] << endl;
            }
         }
         cout << "================================================\n\n";
      }
      
   private:
      void setIndex (const size_t buck=0, const size_t idx=0) { _buckIdx=buck; _vecIdx=idx; }
      void setVector (vector<Data>* d=0, const size_t bucknum=0) { _data = d; _bucketnums = bucknum; }

      vector<Data>* _data;
      size_t _bucketnums;
      size_t _buckIdx;
      size_t _vecIdx;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      if (_buckets == 0 || empty()) return end(); 
      size_t bucket = 0;
      for (size_t i = 0; i < _numBuckets; ++i) {
         if (_buckets[i].size() != 0) { bucket = i; break; }
      }
      iterator it (_buckets, _numBuckets, bucket, 0);
      return it;
   }
   // Pass the end
   iterator end() const { 
      iterator it (0, _numBuckets, _numBuckets, 0);
      return it; 
   }
   // return true if no valid data
   bool empty() const { 
      if (_buckets == 0) return true;
      bool isEmpty = true;
      for (size_t i = 0; i < _numBuckets; ++i) {
         if (_buckets[i].size() != 0) { isEmpty = false; break;}
      }
      return isEmpty;
   }
   // number of valid data
   size_t size() const {
      if (_buckets == 0) return 0;
      size_t s = 0;
      for (size_t i = 0; i < _numBuckets; ++i) s += _buckets[i].size();
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      if (_buckets == 0) return false;
      size_t bucket = bucketNum (d);
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (d == _buckets[bucket][i]) return true;
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      size_t bucket = bucketNum (d);
      if (_buckets == 0) return false;
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (d == _buckets[bucket][i]) {
            d = _buckets[bucket][i];
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      assert (!empty());
      size_t bucket = bucketNum (d);
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (d == _buckets[bucket][i]) {
            _buckets[bucket][i] = d;
            return true;
         }
      }
      insert (d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      if (check (d)) return false;
      size_t bucket = bucketNum (d);
      _buckets[bucket].push_back (d);
      #ifdef DEBUG_H
      cout << "Inserted at bucket: " << bucket << ", index: " 
           << _buckets[bucket].size() - 1 << endl;
      cout << "Value: " << _buckets[bucket][_buckets[bucket].size()-1] << endl;
      #endif // DEBUG_H
      return true; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      bool isFind = false;
      int index = 0;
      size_t bucket = bucketNum (d);
      if (_buckets == 0) return false;
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (d == _buckets[bucket][i]) {
            isFind = true;
            index = i;
         }
      }

      if (!isFind) return false;
      else if (index < (int) _buckets[bucket].size()-1) {
         Data tmp = _buckets[bucket][index];
         _buckets[bucket][index] = _buckets[bucket][_buckets[bucket].size()-1];
         _buckets[bucket][_buckets[bucket].size()-1] = tmp;
      }
      _buckets[bucket].pop_back();
      return true;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
