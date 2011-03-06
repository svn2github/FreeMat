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

inline Array MakeVTKPointer(vtkObjectBase *p, QString cname, Interpreter *eval, bool nodelete = false) {
  StructArray *sap = new StructArray;
  sap->insert("pointer",Array(reinterpret_cast<uint64>(p)));
  sap->insert("nodelete",Array(nodelete));
  sap->setClassName(cname);
  // Make it a handle-semantics class
  StructArray hsap(sap,eval);
  return Array(hsap);
}

inline void DeleteVTKObject(const Array &arg) {
  StructArray sa = arg.constStructPtr();
  BasicArray<Array>& val = sa["nodelete"];
  if (val[1].constRealScalar<bool>()) return;
  vtkObjectBase *vtk_pointer = GetVTKPointer<vtkObjectBase>(arg);
  vtk_pointer->Delete();
}

#endif

