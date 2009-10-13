/*
 * Copyright (c) 2009 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __Struct_hpp__
#define __Struct_hpp__

#include "BasicArray.hpp"
#include "Types.hpp"

class Array;

class StructArray {
  StringVector m_fields;
  QVector<BasicArray<Array> > m_data;
  StringVector m_classPath;
  NTuple m_dims;
public:
  bool isUserClass() const {return !m_classPath.empty();}
  QString className() const {return m_classPath.back();}
  void sliceClass(QString name) {m_classPath.push_back(name);}
  void unsliceClass() {m_classPath.pop_back();}
  const StringVector & classPath() const {return m_classPath;}
  void setClassPath(const StringVector & classPath) {m_classPath = classPath;}
  StringVector fieldNames() const {return m_fields;}
  void setFieldNamesAndData(const StringVector& fields,
			    const QVector<BasicArray<Array> > data) {
    m_fields = fields;
    m_data = data;
    updateDims();
  }
  int fieldCount() const {return m_fields.size();}
  QString fieldName(int i) const {return m_fields[i];}
  index_t bytes() const {
    index_t count = 0;
    for (int i=0;i<m_fields.size();i++) {
      count += m_fields[i].size()*sizeof(QChar);
      count += m_data[i].bytes();
    }
    return count;
  }
  int fieldIndex(QString name) const {
    if (m_fields.contains(name)) 
      return m_fields.indexOf(name);
    throw Exception("Fieldname " + name + " not defined");
  }
  bool contains(QString name) const { return m_fields.contains(name); }
  void insert(QString name, const BasicArray<Array> &t) {
    if (!contains(name)) {
      m_fields += name;
      m_data.push_back(t);
    } else {
      m_data[fieldIndex(name)] = t;
    }
    updateDims();
  }
  NTuple dimensions() const {return m_dims;}
  const index_t length() const {return m_dims.count();}
  void setDimensions(const NTuple &x) {m_dims = x;}
  void updateDims() {
    if (m_data.size() == 0)
      m_dims = NTuple(0,0);
    else
      m_dims = m_data[0].dimensions();
  }
  BasicArray<Array>& operator[](int i) {return m_data[i];}
  const BasicArray<Array>& operator[](int i) const {return m_data[i];}
  BasicArray<Array>& operator[](QString name) {
    if (!m_fields.contains(name)) {
      m_fields += name;
      m_data.push_back(BasicArray<Array>());
    }
    return m_data[fieldIndex(name)];
  }
  const BasicArray<Array>& operator[](QString name) const {return m_data[fieldIndex(name)];}
};

#endif
