#ifndef __VTKWrap_hpp__
#define __VTKWrap_hpp__

#include "Array.hpp"
#include "Struct.hpp"
#include "Interpreter.hpp"
#include "Algorithms.hpp"
#include "Print.hpp"
#include "vtkObjectBase.h"
#include <sstream>

template <class T>
inline T* GetVTKPointer(const Array &arg) {
  if (arg.dataClass() != Struct)
    throw Exception("Cannot convert non vtk-objects to vtk-objects");
  if (!arg.className().startsWith("vtk"))
    throw Exception("Cannot convert non vtk-objects to vtk-objects");
  if (!arg.isScalar())
    throw Exception("only scalar instances are supported currently");
  Array acopy(arg);
  StructArray sa = acopy.structPtr();
  int pindex = sa.fieldIndex("pointer");
  BasicArray<Array>& val = sa[pindex];
  return reinterpret_cast<T*>(val[1].constRealScalar<uint64>());
}

inline Array MakeVTKPointer(vtkObjectBase *p) {
  StringVector fields;
  fields.push_back("pointer");
  ArrayVector values;
  values.push_back(Array(reinterpret_cast<uint64>(p)));
  return StructConstructor(fields,values);
}

#endif

