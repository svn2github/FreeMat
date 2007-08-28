/*
 * Copyright (c) 2007 GE Global Research
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

// The central idea is to add a mechanism for passing complex data structures 
// efficiently between C/C++ and FreeMat .m files.  It will consist of these 
// new functions
//   cstruct_define('typename','field1','type1','field2','type2',...)
//                  where 'fieldn' is the name of the field in the data structure
//                  and   'typen' is a typespec for the field (same as used for the import command)
//   s = cstruct_thaw(membuffer, 'typename')
//       where membuffer is a buffer of raw bytes (uint8) containing the data structure as laid out
//       by the C/C++ compiler (may need some extra arguments to handle element alignment, not sure)
//       The output is a first class struct in FreeMat.  Enumerations are mapped to strings.
//   membuffer = cstruct_freeze(s, 'typename')
//       where s is a struct (that should correspond to the info in 'typename', and membuffer is the
//       array of raw bytes that can then be passed to a C/C++ struct and cast in the C code 
//       to a pointer of the appropriate type. 
//
// The data structure to capture the type maps is fairly simple.  The typenames are 
// legal strings, so we can use a SymbolTable or a STL Hash or a QStringHash.  The field/type 
// info are parsed and stored in a data structure to speed access.  The format for the type 
// info is borrowed from import.  It consists of a spec of:
//      typespec = <typename>[array spec]
// where array spec is either a constant or another field in the structure (or substructure) that 
// contains the size of the array.  For example, the following C structure:
//     typedef struct {
//        int len;
//        char p;
//     } simple_string;
// would be mapped to the following cstruct_define call:
//    cstruct_define('simple_string','len','int32','p','int8[len]')
// which allows us to handle the case of "dynamic" structures - used in some notable cases to 
// store variable-sized data structures in structs in C.  Note that the problems of structure 
// alignment and padding are _not_ solved for you.  If padding is required, you must specify 
// it.  For example, suppose your C code converts the following structure:
//     typedef struct {
//        char achar;
//        double d;
//     } t_struct;
// into
//     typedef struct {
//        char achar;
//        char pad1[7];
//        double d;
//     } t_struct;
// where the double entry has to be 8-byte aligned (for example).  There is no way for FreeMat to 
// determine this at run time (indeed, such factors can sometimes be affected by C compiler options
// as well).  It is your responsibility to make sure the alignment is correct by inserting pads as
// necessary.  Sorry, but there is no real way around this problem.
//      

// Need to add arrays of structures

#include "Array.hpp"
#include "Interpreter.hpp"
#include <string>
#include <vector>
#include <map>

using namespace std;

class CenumPair {
  string name;
  int value;
public:
  CenumPair(string i_name, int i_value) : name(i_name),
						value(i_value) {}
  CenumPair() {}
  string getName() {return name;}
  int getValue() {return value;}
};

class Cenum : public vector<CenumPair> {
  public:
  string lookup(int n) {
    for (int i=0;i<size();i++)
      if (at(i).getValue() == n) 
	return at(i).getName();
    return "unknown";
  }
  
  int rlookup(string v) {
    for (int i=0;i<size();i++)
      if (at(i).getName() == v)
	return at(i).getValue();
    return 0;
  }
};

class CstructField {
  string name;
  string type;
  int length;
public:
  CstructField(string i_name, string i_type, int i_length) : name(i_name), 
							     type(i_type), 
							     length(i_length) {}
  CstructField() {}
  string getName() {return name;}
  string getType() {return type;}
  int getLength() {return length;}
};

typedef vector<CstructField> CstructFields;

map<string, CstructFields> CstructTable;
map<string, Cenum> CenumTable;

//!
//@Module CENUMDEFINE Define C Enumeration
//@@Section EXTERNAL
//@@Usage
//The @|cstructenum| function allows you to work with C structs that
//use enumerations as element members in a much cleaner way than 
//having to hard code the various constants into your FreeMat scripts.
//The usage for this function is 
//@[
//  cenumdefine(typename,name1,value1,name2,value2,...)
//@]
//The argument @|typename| must be a valid identifier string.  Each of the
//@|name| arguments must also be valid identifier strings that describe
//the possible values that the enumeration can take an, and their corresponding
//integer values.  Note that the names should be unique.  The behavior of
//the various @|cenum| functions is undefined if the names are not unique.
//!
ArrayVector CenumDefineFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) return ArrayVector();
  string tname = ArrayToString(arg[0]);
  int cnt = 1;
  Cenum enumDef;
  while (cnt < arg.size()) {
    string ftype = ArrayToString(arg[cnt]);
    if (arg.size() < (cnt+1))
      throw Exception("Expecting value for enum name " + ftype);
    int fvalue = ArrayToInt32(arg[cnt+1]);
    enumDef.push_back(CenumPair(ftype,fvalue));
    cnt += 2;
  }
  CenumTable[tname] = enumDef;
  return ArrayVector();
}

//!
//@Module CENUMDESCRIBE Describe C Enumeration
//@@Section EXTERNAL
//@@Usage
//The @|cenumdescribe| function dumps the current values for an enumeration
//to the current console.  The enumeration must have been defined using the
//@|cenumdefine| function.  The usage is 
//@[
//  cenumdescribe(typename)
//@]
//where @|typename| is a string containing the name of the C enumeration to
//describe.
//!
ArrayVector CenumDescribeFunction(int nargout, const ArrayVector& arg, Interpreter *m_eval) {
  if (arg.size() < 1) return ArrayVector();
  string tname = ArrayToString(arg[0]);
  if (CenumTable.count(tname) == 0) {
    m_eval->outputMessage("cenum " + tname + " not in table");
    return ArrayVector();
  }
  Cenum fields(CenumTable.find(tname)->second);
  m_eval->outputMessage("cenum " + tname + "\n");
  for (int i=0;i<fields.size();i++)
    m_eval->outputMessage("  " + fields[i].getName() + " : " + fields[i].getValue() + "\n");
  return ArrayVector();  
}

//!
//@Module CSTRUCTDEFINE Define a C Structure
//@@Section EXTERNAL
//@@Usage
//The @|cstructdefine| function is paired with two other functions,
//@|cstructthaw| and @|cstructfreeze|.  These three functions together
//provide a simple, but powerful way to pass data between FreeMat and
//C libraries.  The usage is
//@[
//  cstructdefine(typename,field1,type1,field2,type2,...)
//@]
//The argument @|typename| must be a valid identifier string.  Each of
//of the @|field| arguments is also a valid identifier string that 
//describe in order, the elements of the C structure.  The @|type| arguments
//are @|typespecs|.  They can be of four types:
//\begin{itemize}
//\item Built in types, e.g. @|'uint8'| or @|'double'| to name a couple of
//      examples.
//\item Cstruct types that have previously been or will be defined with a call to 
//      @|cstructdefine|, e.g. @|'mytype'| where @|'mytype'| has already been
//      defined through a call to @|cstructdefine|.
//\item Arrays of either built in types or previously defined Cstruct types
//      with the length of the array coded as an integer in square brackets, 
//      for example: @|'uint8[10]'| or @|'double[1000]'|.
//\item Arrays of variable length, with the length of the array coded as an 
//      element of the structure itself, e.g. @|'uint8[length]'|.  Typically
//      these dynamic arrays appear at the end of the C structure.
//\item Enumerated types, as defined by @|cenumdefine|, or arrays of such types.
//\end{itemize}
//Note that if @|'typename'| is defined as both a struct and an enumeration
//(not a good practice) then the exact behavior of the @|cstruct| functions
//is not defined.  Current practice is to look for a structure definition first,
//and then look for an enumeration definition, but this is subject to change.
//!
ArrayVector CstructDefineFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) return ArrayVector();
  string tname = ArrayToString(arg[0]);
  int cnt = 1;
  CstructFields fields;
  while (cnt < arg.size()) {
    string ftype = ArrayToString(arg[cnt]);
    if (arg.size() < (cnt+1))
      throw Exception("Expecting typespec for fieldname " + ftype);
    string ttypespec = ArrayToString(arg[cnt+1]);
    string ttype;
    int tlength;
    if (ttypespec.find('[') != ttypespec.npos) {
      int left_brace = ttypespec.find('[');
      int right_brace = ttypespec.find(']');
      ttype = ttypespec.substr(0,left_brace);
      tlength = atoi(ttypespec.substr(left_brace+1,right_brace-left_brace-1).c_str());
    } else {
      ttype = ttypespec;
      tlength = 1;
    }
    fields.push_back(CstructField(ftype,ttype,tlength));
    cnt += 2;
  }
  CstructTable[tname] = fields;
  return ArrayVector();
}

//!
//@Module CSTRUCTDESCRIBE Describe a C Structure
//@@Section EXTERNAL
//@@Usage
//The @|cstructdescribe| function can describe the internal type information 
//for a C struct as defined using the @|cstructdefine| function.  The usage
//is
//@[
//  cstructdescribe(typename)
//@]
//where @|typename| is a string containing the name of the C struct to describe.
//!
ArrayVector CstructDescribeFunction(int nargout, const ArrayVector& arg, Interpreter *m_eval) {
  if (arg.size() < 1) return ArrayVector();
  string tname = ArrayToString(arg[0]);
  if (CstructTable.count(tname) == 0) {
    m_eval->outputMessage("cstruct " + tname + " not in table");
    return ArrayVector();
  }
  CstructFields fields(CstructTable.find(tname)->second);
  m_eval->outputMessage("cstruct " + tname + "\n");
  for (int i=0;i<fields.size();i++)
    m_eval->outputMessage("  " + fields[i].getName() + "     " + fields[i].getType() + "["
			  + fields[i].getLength() + "]\n");
  return ArrayVector();
}

static bool CstructElementalType(string ttype) {
  return ((ttype == "int8") || (ttype == "uint8") || (ttype == "int16") || (ttype == "uint16") ||
	  (ttype == "int32") || (ttype == "uint32") || (ttype == "int64") || (ttype == "uint64") ||
	  (ttype == "double") || (ttype == "float"));
}

static void WriteBytes(QByteArray& output, const void *dp, int len) {
  const char *cp = (const char*) dp;
  for (int i=0;i<len;i++)
    output.push_back(cp[i]);
}


static void CstructFreezeElementalType(QByteArray& output, Array s, string ttype, int length) {
  if (ttype == "int8") {
    s.promoteType(FM_INT8);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(int8));
  } else if (ttype == "uint8") {
    s.promoteType(FM_UINT8);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(uint8));
  } else if (ttype == "int16") {
    s.promoteType(FM_INT16);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(int16));
  } else if (ttype == "uint16") {
    s.promoteType(FM_UINT16);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(uint16));
  } else if (ttype == "int32") {
    s.promoteType(FM_INT32);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(int32));
  } else if (ttype == "uint32") {
    s.promoteType(FM_UINT32);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(uint32));
  } else if (ttype == "int64") {
    s.promoteType(FM_INT64);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(int64));
  } else if (ttype == "uint64") {
    s.promoteType(FM_UINT64);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(uint64));
  } else if (ttype == "float") {
    s.promoteType(FM_FLOAT);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(float));
  } else if (ttype == "double") {
    s.promoteType(FM_DOUBLE);
    if (s.getLength() != length)
      throw Exception("field length mismatch");
    WriteBytes(output,s.getDataPointer(),length*sizeof(double));
  }
}

static void CstructFreezeStruct(QByteArray& output, Array s, CstructFields& fields);

// If the field is in INT32 form already, just store it
static void CstructFreezeEnum(QByteArray& output, Array s, Cenum& tenum, int length) {
  if (s.isIntegerClass() && !s.isString() && (s.getLength() == length)) {
    CstructFreezeElementalType(output,s,"int32",length);
    return;
  }
  if (length == 1) {
    if (!s.isString()) throw Exception("Expected string for enumerated type");
    int32 val(tenum.rlookup(ArrayToString(s)));
    WriteBytes(output,&val,sizeof(int32));
  } else {
    if (s.dataClass() != FM_CELL_ARRAY) 
      throw Exception("Expected a cell array of strings for enumerated type");
    if (s.getLength() != length)
      throw Exception("Length mismatch between cell array of strings and requested enumerated type");
    const Array *dp = (const Array *) s.getDataPointer();
    for (int i=0;i<length;i++) {
      if (dp[i].isIntegerClass() && !dp[i].isString()) {
	CstructFreezeElementalType(output,dp[i],"int32",1);
      } else if (dp[i].isString()) {
	int32 val(tenum.rlookup(ArrayToString(dp[i])));
	WriteBytes(output,&val,sizeof(int32));
      } else 
	throw Exception("Expected strings as elements of cell array for enumerated type");
    }
  }
}

static void CstructFreezeField(QByteArray& output, Array s, string ttype, int length) {
  if (CstructElementalType(ttype))
    CstructFreezeElementalType(output,s,ttype,length);
  else if (CenumTable.count(ttype) != 0) {
    CstructFreezeEnum(output,s,CenumTable.find(ttype)->second,length);
  } else {
    // Lookup the type
    if (CstructTable.count(ttype) == 0)
      throw Exception("unable to find a C struct definition for type " + ttype);
    CstructFreezeStruct(output,s,CstructTable.find(ttype)->second);
  }
}

static void CstructFreezeStruct(QByteArray& output, Array s, CstructFields& fields) {
  for (int i=0;i<fields.size();i++) 
    CstructFreezeField(output,s.getField(fields[i].getName()),
			  fields[i].getType(),fields[i].getLength());
}

static Array ReadBytes(QByteArray& input, int len, int size, Class dclass) {
  char* dp = (char*) Array::allocateArray(dclass,len);
  int bytecount = len*size;
  if (input.size() < bytecount)
    throw Exception("source buffer is too short");
  for (int i=0;i<bytecount;i++) 
    dp[i] = input.at(i);
  input.remove(0,bytecount);
  return Array::Array(dclass, Dimensions(1,len), dp);
}

static Array CstructThawElementalType(QByteArray& input, string ttype, int length) {
  if (ttype == "int8")
    return ReadBytes(input, length, sizeof(int8), FM_INT8);
  else if (ttype == "uint8")
    return ReadBytes(input, length, sizeof(uint8), FM_UINT8);
  else if (ttype == "int16")
    return ReadBytes(input, length, sizeof(int16), FM_INT16);
  else if (ttype == "uint16")
    return ReadBytes(input, length, sizeof(uint16), FM_UINT16);
  else if (ttype == "int32")
    return ReadBytes(input, length, sizeof(int32), FM_INT32);
  else if (ttype == "uint32")
    return ReadBytes(input, length, sizeof(uint32), FM_UINT32);
  else if (ttype == "int64")
    return ReadBytes(input, length, sizeof(int64), FM_INT64);
  else if (ttype == "uint64")
    return ReadBytes(input, length, sizeof(uint64), FM_UINT64);
  else if (ttype == "float")
    return ReadBytes(input, length, sizeof(float), FM_FLOAT);
  else if (ttype == "double")
    return ReadBytes(input, length, sizeof(double), FM_DOUBLE);
}

static Array CstructThawEnum(QByteArray& input, Cenum& data, int length) {
  Array values(ReadBytes(input,length,sizeof(int32),FM_INT32));
  if (length == 1)
    return Array::stringConstructor(data.lookup(ArrayToInt32(values)));
  const int32* dp = (const int32*) values.getDataPointer();
  ArrayVector vals;
  for (int i=0;i<length;i++) {
    vals << Array::stringConstructor(data.lookup(dp[i]));
  }
  return Array::cellConstructor(vals);
}

static Array CstructThawStruct(QByteArray& input, CstructFields& fields);

static Array CstructThawField(QByteArray& input, string ttype, int length) {
  if (CstructElementalType(ttype))
    return CstructThawElementalType(input,ttype,length);
  else if (CenumTable.count(ttype) != 0) {
    return CstructThawEnum(input,CenumTable.find(ttype)->second,length);
  } else {
    // Lookup the type
    if (CstructTable.count(ttype) == 0)
      throw Exception("unable to find a C struct definition for type " + ttype);
    return CstructThawStruct(input,CstructTable.find(ttype)->second);
  }
}

static Array CstructThawStruct(QByteArray& input, CstructFields& fields) {
  rvstring names;
  ArrayVector values;
  for (int i=0;i<fields.size();i++) {
    names << fields[i].getName();
    values << CstructThawField(input,fields[i].getType(),fields[i].getLength());
  }
  return Array::structConstructor(names,values);
}

//!
//@Module CSTRUCTFREEZE Convert FreeMat Structure to C Struct
//@@Section EXTERNAL
//@@Usage
//The @|cstructfreeze| function is used to convert a FreeMat structure into
//a C struct as defined by a C structure typedef.  To use the @|cstructfreeze|
//function, you must first define the type of the C structure using the 
//@|cstructdefine| function.  The @|cstructfreeze| function then serializes
//a FreeMat structure to a set of bytes, and returns it as an array.  The
//usage for @|cstructfreeze| is
//@[
//  byte_array = cstructfreeze(mystruct, 'typename')
//@]
//where @|mystruct| is the structure we want to 'freeze' to a memory array,
//and @|typename| is the name of the C Struct that we want the resulting
//byte array to conform to.
//!
ArrayVector CstructFreezeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2) 
    throw Exception("cstructfreeze requires two arguments - the structure to freeze and the typename to use");
  Array s(arg[0]);
  string ttype(ArrayToString(arg[1]));
  if (CstructTable.count(ttype) == 0)
    throw Exception("unable to find a C struct definition for type " + ttype);
  CstructFields fields(CstructTable.find(ttype)->second);
  // Use a QByteArray because its convenient
  if (s.dataClass() != FM_STRUCT_ARRAY)
    throw Exception("first argument to cstructfreeze must be a structure or struct array");
  QByteArray output;
  CstructFreezeStruct(output,s,fields);
  uint8* dp = (uint8*) Array::allocateArray(FM_UINT8, output.length());
  memcpy(dp,output.constData(),output.length());
  return ArrayVector() << Array::Array(FM_UINT8,Dimensions(1,output.length()),dp);
}

//!
//@Module CSTRUCTTHAW Convert C Struct to FreeMat Structure
//@@Section EXTERNAL
//@@Usage
//The @|cstructthaw| function is used to convert a C structure that is
//encoded in a byte array into a FreeMat structure using a C structure
//typedef.  To use the @|cstructthaw| function, you must first define
//the type of the C structure using the @|cstructdefine| function.  The
//usage of @|cstructthaw| is
//@[
//  mystruct = cstructthaw(byte_array, 'typename')
//@]
//where @|byte_array| is a @|uint8| array containing the bytes that encode
//the C structure, and @|typename| is a string that contains the type
//description as registered with @|cstructdefine|.
//!
ArrayVector CstructThawFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("cstructthaw requires two arguments - the uint8 array to thaw the structure from, and the typename to use");
  Array s(arg[0]);
  string ttype(ArrayToString(arg[1]));
  if (CstructTable.count(ttype) == 0)
    throw Exception("unable to find a C struct definition for type " + ttype);
  if (s.dataClass() != FM_UINT8)
    throw Exception("first argument to cstructthaw must be a uint8 array");
  CstructFields fields(CstructTable.find(ttype)->second);
  QByteArray input((const char*) s.getDataPointer(),s.getLength());
  return ArrayVector() << CstructThawStruct(input,fields);
}
