#ifndef __RefVec_hpp__
#define __RefVec_hpp__

#include <vector>
#include "Exception.hpp"
using namespace std;

template <class T>
class RefVec;

template <class T>
class RefVecNode {
private:
  vector<T> data;
  int count;
  friend class RefVec<T>;
  RefVecNode() : count(0) {}
};

template <class T>
class RefVec {
  RefVecNode<T> *p;
  void Release() {
    if (p) {
      p->count--;
      if (p->count <=0) 
	delete p;
    }
  }
  void Allocate() {
    p = new RefVecNode<T>;
    p->count++;
  }
  void Duplicate() {
    RefVecNode<T> *q = new RefVecNode<T>;
    q->data = p->data;
    q->count = 1;
    Release();
    p = q;
  }
public:
  RefVec() {
    p = NULL;
  }
  RefVec(int cnt) {
    Allocate();
    p->data = std::vector<T>(cnt);
  }
  RefVec(const RefVec& copy) {
    p = copy.p;
    if (p) p->count++;
  }
  ~RefVec() {
    Release();
  }
  void operator=(const RefVec& copy) {
    Release();
    p = copy.p;
    if (p) p->count++;
  }
  bool operator==(const RefVec& copy) {
    if ((!p) && (!copy.p)) return true;
    if ((!p) || (!copy.p)) return false;
    return (p->data == copy.p->data);
  }
  unsigned size() const {
    if (!p) return 0;
    return p->data.size();
  }
  void push_front(const T& x) {
    if (!p) Allocate();
    if (p->count > 1) Duplicate();
    p->data.insert(p->data.begin(),x);
  }
  void append(const RefVec<T>& x) {
    for (unsigned p=0;p<x.size();p++)
      push_back(x[p]);
  }
  void push_back(const T& x) {
    if (!p) Allocate();
    if (p->count > 1) Duplicate();
    p->data.push_back(x);
  }
  void pop_back() {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.pop_back();
    }
  }
  void pop_front() {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.erase(p->data.begin());
    }
  }
  void clear() {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.clear();
    }
  }
  bool empty() const {
    if (!p) return true;
    return p->data.empty();
  }
  T& back() {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data.back();  
  }
  T& front() {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data.front();  
  }
  T& operator[] (unsigned n) {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data[n];
  }
  T& at(unsigned n) {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data.at(n);
  }
  void erase(unsigned n) {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.erase(p->data.begin()+n);
    }
  }
  const T& back() const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data.back();  
  }
  const T& front() const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data.front();  
  }
  const T& operator[] (unsigned n) const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data[n];
  }
  const T& at(unsigned n) const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data.at(n);
  }
};

#endif
