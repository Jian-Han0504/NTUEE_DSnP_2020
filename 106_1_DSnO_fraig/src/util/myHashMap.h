/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>
#include <cassert>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
class strashKey
{
public:
   strashKey() {}
   strashKey(size_t i1, size_t i2) {
      if (i1 >= i2) { _in1 = i1; _in2 = i2;}
      else { _in1 = i2; _in2 = i1;}
   }

   size_t operator() () const { return _in1 + _in2;}

   bool operator == (const strashKey& k) const { return ((k._in1 == _in1 && k._in2 == _in2) ? true : false);}

private:
   size_t _in1; // in1 >= in2
   size_t _in2;
};

class simValue
{
   public:
      simValue (size_t gateVal, size_t ID) : _64bitVal (gateVal), _gateID (ID) {}

      size_t operator() () const { return _64bitVal;}
      bool operator== (const simValue& s) const {
         if (s._64bitVal == _64bitVal && s._gateID == _gateID) return true;
         else return false;
      }

   private:
      size_t _64bitVal;
      size_t _gateID;
};

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator (vector<HashNode>* d=0, const size_t bucknum=0, const size_t buck=0, const size_t idx=0) { 
         setVector (d, bucknum);
         setIndex (buck, idx); 
      }
      const HashData& operator* () const {
         assert (_data != 0);
         return _data [_buckIdx][_vecIdx].second;
      }
      iterator& operator++ () {
         if (_vecIdx < _data[_buckIdx].size()-1) ++_vecIdx;
         else {
            ++_buckIdx;
            while (_data[_buckIdx].size() == 0 && _buckIdx < _bucketnums) 
               ++_buckIdx;
            
            _vecIdx = 0;
            if (_buckIdx == _bucketnums) _data = 0; // if meets end(), set data = 04
         }
         return (*this);
      }
      iterator& nextBucket () {
         _vecIdx = 0;
         ++_buckIdx;
         while (_data[_buckIdx].size() == 0 && _buckIdx < _bucketnums) 
            ++_buckIdx;
         
         if (_buckIdx == _bucketnums) _data = 0;
         return (*this);
      }
      vector<HashData> getBucketData () const {
         assert (_data != 0);
         vector<HashData> tmp;
         for (size_t i = 0; i < _data[_buckIdx].size(); ++i)
            tmp.push_back (_data[_buckIdx][i].second);
         
         return tmp;
      }
      bool operator!= (const iterator& i) const {
         if (_buckIdx == i._buckIdx && _vecIdx == i._vecIdx) return false;
         else return true;
      }
      bool operator == (const iterator& i) const { return !(*this != i);}

   private:
      void setIndex (const size_t buck=0, const size_t idx=0) { _buckIdx=buck; _vecIdx=idx; }
      void setVector (vector<HashNode>* d=0, const size_t bucknum=0) { _data = d; _bucketnums = bucknum; }
      vector<HashNode>* _data;
      size_t _bucketnums;
      size_t _buckIdx;
      size_t _vecIdx;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

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
      for (size_t i = 0;i < _numBuckets; ++i) {
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

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const { 
      if (_buckets == 0) return false;
      size_t bucket = bucketNum (k);
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (k == _buckets[bucket][i].first) return true;
      }
      return false; 
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query (const HashKey& k, HashData& d) const { 
      size_t bucket = bucketNum (k);
      if (_buckets == 0) return false;
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (k == _buckets[bucket][i].first) {
            d = _buckets[bucket][i].second;
            return true;
         }
      }
      return false; 
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) { 
      assert (_buckets != 0);
      size_t bucket = bucketNum (k);
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (k == _buckets[bucket][i].first) {
            _buckets[bucket][i].second = d;
            return true;
         }
      }
      insert (k, d);
      return false; 
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false if k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) { 
      if (check(k)) return false;
      size_t bucket = bucketNum (k);
      HashNode newNode (k, d);
      _buckets[bucket].push_back (newNode);
      return true; 
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) { 
      bool isFind = false;
      int index = 0;
      size_t bucket = bucketNum (k);
      if (_buckets == 0) return false;
      for (size_t i = 0; i < _buckets[bucket].size(); ++i) {
         if (k == _buckets[bucket][i].first) {
            isFind = true;
            index = i;
         }
      }

      if (!isFind) return false;
      else if (index < (int) _buckets[bucket].size()-1) {
         HashNode tmp = _buckets[bucket][index];
         _buckets[bucket][index] = _buckets[bucket][_buckets[bucket].size()-1];
         _buckets[bucket][_buckets[bucket].size()-1] = tmp;
      }
      _buckets[bucket].pop_back();
      return true;
      
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
