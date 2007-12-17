#ifndef __List_hpp__
#define __List_hpp__

#include <vector>
#include <QList>
#include "Exception.hpp"

template <class T>
class FMList {
  QList<T> *d;
public:
  FMList() : d(NULL) {
  }
  ~FMList() {
    if (d) delete d;
  }
  FMList(const FMList<T>& other) {
    if (!other.d) {
      d = NULL;
      return;
    } else {
      d = new QList<T>(*(other.d));
    }
  }
  FMList<T>& operator=(const FMList<T>& other) {
    if (d) delete d;
    if (other.d) {
      d = new QList<T>(*(other.d));
    } else {
      d = NULL;
    }
    return *this;
  }
  inline int size() const {
    if (d) 
      return d->size(); 
    else 
      return 0;
  }
  inline T& operator[](int i) {
    if (d) 
      return (*d)[i]; 
    else 
      throw Exception("Illegal list access");
  }
  inline const T& operator[](int i) const {
    if (d) 
      return (*d)[i]; 
    else
      throw Exception("Illegal list access (const)");
  }
  inline bool empty() const {
    if (d)
      return d->empty();
    else
      return true;
  }
  inline void push_back(const T & value) {
    if (d)
      d->push_back(value);
    else {
      d = new QList<T>;
      d->push_back(value);
    }
  }
  inline void push_front(const T & value) {
    if (d)
      d->push_front(value);
    else {
      d = new QList<T>;
      d->push_front(value);
    }
  }
  inline const T& at(int i) const {
    if (d)
      return d->at(i);
    else 
      throw Exception("Illegal list access (at)");
  }
  inline void pop_front() {
    if (d)
      d->pop_front();
    else
      throw Exception("Illegal list access (pop_front)");
  }
  inline void pop_back() {
    if (d)
      d->pop_back();
    else
      throw Exception("Illegal list access (pop_back)");
  }
  inline T& front() {
    if (d)
      return d->front();
    else
      throw Exception("Illegal list access (front)");
  }
  inline const T& front() const {
    if (d)
      return d->front();
    else
      throw Exception("Illegal list access (const front)");
  }
  inline T& back() {
    if (d)
      return d->back();
    else
      throw Exception("Illegal list access (back)");
  }
  inline const T& back() const {
    if (d)
      return d->back();
    else
      throw Exception("Illegal list access (const back)");
  }
  inline FMList<T>& operator<<(const T & value) {
    if (d) {
      d->push_back(value);
    } else {
      d = new QList<T>;
      d->push_back(value);
    }
    return *this;      
  }
  inline FMList<T>& operator+=(const FMList<T>& other) {
    if (other.d) {
      if (d) {
	(*d) += *(other.d);
      } else {
	d = new QList<T>;
	(*d) += *(other.d);
      }
    }
    return *this;
  }
  inline bool operator==(const FMList<T>& other) {
    if (d && other.d) {
      return ((*d) == (*other.d));
    } else if (!d && !other.d)
      return true;
    else
      return false;
  }
};

template <class T>
class PList : public std::vector<T> {
public:
  inline void pop_front() {
    erase(this->begin());
  }
  inline PList<T>& operator<< (const T& value) {
    push_back(value);
    return *this;
  }
  inline void push_front(const T& value) {
    insert(this->begin(),value);
  }
  inline PList<T>& operator+= (const PList<T>& other) {
    for (int i=0;i<other.size();i++)
      push_back(other[i]);
    return *this;
  }
};

#endif
