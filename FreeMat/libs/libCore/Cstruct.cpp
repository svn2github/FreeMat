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
#include "Algorithms.hpp"
#include <map>

// Base of the type system
class Ctype {
  // Nothing interesting here...
public:
  virtual ~Ctype() {}
  virtual void print(Interpreter *m_eval) = 0;
  virtual void freeze(QByteArray& out, Array s, int length, Interpreter* m_eval) = 0;
  virtual Array thaw(QByteArray& input, int& pos, int length, Interpreter* m_eval) = 0;
  virtual int size(int length) = 0;
};

// Type table
class CTable : public QMap<QString, Ctype*> {
public:
  Ctype* lookup(QString name) {
    if (!contains(name)) 
      throw Exception("Request for lookup of unknown type " + name);
    return value(name);
  }
  void add(QString name, Ctype* val) {
    (*this)[name] = val;
  }
  CTable();
};

static CTable CtypeTable;

// A builtin type
class Cbuiltin : public Ctype {
  DataClass dataClass;
  int t_size;
public:
  Cbuiltin(DataClass i_Class, int i_size) : dataClass(i_Class), t_size(i_size) {}
  DataClass getDataClass() { return dataClass; }
  int getSize() { return t_size;}
  void print(Interpreter *m_eval) {
    m_eval->outputMessage("built in\n");
  }
  void freeze(QByteArray& out, Array s, int length, Interpreter* m_eval) {
    s = s.toClass(dataClass).asDenseArray();
    if ((int) s.length() > length)
      throw Exception("field length overflow");
    const char *cp = (const char*) s.getConstVoidPointer();
    index_t s_len = s.length();
    for (index_t i=0;i<s_len*t_size;i++)
      out.push_back(cp[int(i)]);
    for (index_t i=s_len*t_size;i<length*t_size;i++)
      out.push_back((char) 0);
  }
  Array thaw(QByteArray& input, int& pos, int length, Interpreter* m_eval) {
    int bytecount = length*t_size;
    Array retvec(dataClass,NTuple(1,length));
    char* dp = (char*) retvec.getVoidPointer();
    if (input.size() < (pos+bytecount))
      throw Exception("source buffer is too short");
    for (int i=0;i<bytecount;i++) 
      dp[i] = input.at(pos+i);
    pos += bytecount;
    return retvec;
  }
  int size(int length) {
    return (length*t_size);
  }
};

// An enumerated type
class Cenum : public Ctype {
  map<int,QString> elementsByInt;
  map<QString,int> elementsByName;
public:
  QString lookupByNumber(int n) {
    if (elementsByInt.count(n) == 0) return "unknown";
    return elementsByInt.find(n)->second;
  }
  int lookupByName(QString name) {
    if (elementsByName.count(name) == 0) return 0;
    return elementsByName.find(name)->second;
  }
  void addPair(QString name, int value) {
    elementsByInt[value] = name;
    elementsByName[name] = value;
  }
  void print(Interpreter *m_eval) {
    m_eval->outputMessage("enumeration\n");
    for (map<int,QString>::const_iterator i=elementsByInt.begin();
	 i != elementsByInt.end(); i++) 
      m_eval->outputMessage(QString("  ") + 
			    i->second.leftJustified(30) + 
			    QString("    : ") + 
			    QString("%1\n").arg(i->first));
  }
  void freeze(QByteArray& out, Array s, int length, Interpreter* m_eval) {
    if (IsInteger(s) && !s.isString() && 
	(s.length() == (int)length)) {
      CtypeTable.lookup("int32")->freeze(out,s,length,m_eval);
      return;
    }
    if (length == 1) {
      if (!s.isString()) throw Exception("Expected string for enumerated type");
      CtypeTable.lookup("int32")->freeze(out,Array(int(lookupByName(s.asString()))),1,m_eval);
    } else {
      if (s.dataClass() != CellArray) 
	throw Exception("Expected a cell array of strings for enumerated type");
      if (s.length() != (int)length)
	throw Exception("Length mismatch between cell array of strings and requested enumerated type");
      const BasicArray<Array> &dp(s.constReal<Array>());
      for (index_t i=1;i<=length;i++) {
	if (IsInteger(dp[i]) && !dp[i].isString()) 
	  CtypeTable.lookup("int32")->freeze(out,s,length,m_eval);
	else if (dp[i].isString()) 
	  CtypeTable.lookup("int32")->freeze(out,
					     Array(int(lookupByName(dp[i].asString()))),
					     1,m_eval);
	else 
	  throw Exception("Expected strings as elements of cell array for enumerated type");
      }
    }
  }
  Array thaw(QByteArray& input, int& pos, int length, Interpreter* m_eval) {
    Array values(CtypeTable.lookup("int32")->thaw(input,pos,length,m_eval));
    if (length == 1)
      return Array(lookupByNumber(values.asInteger()));
    const Array &values_int(values.toClass(Int32).asDenseArray());
    const BasicArray<int32> &dp(values_int.constReal<int32>());
    ArrayVector vals;
    for (int i=0;i<length;i++) 
      vals << Array(lookupByNumber(dp[i]));
    return CellConstructor(vals);
  }
  int size(int length) {
    return CtypeTable.lookup("int32")->size(length);
  }
};

// Field in a structure
class CstructField {
  QString name;
  QString type;
  int length;
public:
  CstructField(QString i_name, QString i_type, int i_length) : name(i_name), 
							     type(i_type), 
							     length(i_length) {}
  CstructField() {}
  QString getName() {return name;}
  QString getType() {return type;}
  int getLength() {return length;}
};

// Structure type
class Cstruct : public Ctype {
  QVector<CstructField*> fields;
public:
  ~Cstruct() {
    for (int i=0;i<fields.size();i++) delete fields[i];
  }
  int getFieldCount() {return fields.size();}
  CstructField* getField(int num) {return fields[num];}
  void addField(CstructField* fld) {fields.push_back(fld);}
  void print(Interpreter *m_eval) {
    m_eval->outputMessage("struct\n");
    for (int i=0;i<fields.size();i++) {
      m_eval->outputMessage(QString("  ") + 
			    fields[i]->getName().leftJustified(30) + 
			    QString("    ") + 
			    fields[i]->getType());
      if (fields[i]->getLength() > 1) 
	m_eval->outputMessage(QString("[%1]").arg(fields[i]->getLength()));
      m_eval->outputMessage("\n");
    }
  }
  void freeze(QByteArray& out, Array s, int length, Interpreter* m_eval) {
    int s_count = int(s.length());
    if (s_count != length)
      throw Exception("Length mismatch between expected and actual array count");
    for (int m=0;m<s_count;m++) {
      Array s_el(s.get(Array(index_t(m+1))));
      for (int i=0;i<fields.size();i++) 
	try {
	  CtypeTable.lookup(fields[i]->getType())->freeze(out,s_el.get(fields[i]->getName()).front(),
							  fields[i]->getLength(),m_eval);
	} catch (Exception& e) {
	  throw Exception("Failed during freeze of struct field " + fields[i]->getName() + " reason: " + e.msg());
	}
    }
  }
  Array thaw(QByteArray& input, int& pos, int length, Interpreter* m_eval) {
    // Set up a set of vectors
    ArrayMatrix mat;
    for (int i=0;i<fields.size();i++) 
      mat.push_back(ArrayVector());
    // collect the field names
    StringVector names;
    for (int i=0;i<fields.size();i++) 
      names << fields[i]->getName();
    // Populate the matrix
    for (int m=0;m<length;m++) 
      for (int i=0;i<fields.size();i++) 
	mat[i] << CtypeTable.lookup(fields[i]->getType())->thaw(input,pos,fields[i]->getLength(),m_eval);
    ArrayVector t;
    for (int i=0;i<fields.size();i++)
      t << CellConstructor(mat[i]);
    return StructConstructor(names,t);
  }
  int size(int length) {
    int accum = 0;
    for (int i=0;i<fields.size();i++)
      accum += CtypeTable.lookup(fields[i]->getType())->size(fields[i]->getLength());
    return accum*length;
  }
};

// Alias type
class Calias : public Ctype {
  QString alias;
public:
  Calias(QString i_alias) : alias(i_alias) {}
  QString getAlias() {return alias;}
  void freeze(QByteArray& out, Array s, int length, Interpreter* m_eval) {
    CtypeTable.lookup(alias)->freeze(out,s,length,m_eval);
  }
  Array thaw(QByteArray& input, int& pos, int length, Interpreter* m_eval) {
    return CtypeTable.lookup(alias)->thaw(input,pos,length,m_eval);
  }
  int size(int length) {
    return CtypeTable.lookup(alias)->size(length);
  }
  void print(Interpreter *m_eval) {
    m_eval->outputMessage("alias to " + alias + "\n");
  }
};

CTable::CTable() {
  add("uint8",new Cbuiltin(UInt8,sizeof(uint8)));
  add("uint16",new Cbuiltin(UInt16,sizeof(uint16)));
  add("uint32",new Cbuiltin(UInt32,sizeof(uint32)));
  add("uint64",new Cbuiltin(UInt64,sizeof(uint64)));
  add("int8",new Cbuiltin(Int8,sizeof(int8)));
  add("int16",new Cbuiltin(Int16,sizeof(int16)));
  add("int32",new Cbuiltin(Int32,sizeof(int32)));
  add("int64",new Cbuiltin(Int64,sizeof(int64)));
  add("float",new Cbuiltin(Float,sizeof(float)));
  add("double",new Cbuiltin(Double,sizeof(double)));
}


//!
//@Module CENUM Lookup Enumerated C Type
//@@Section EXTERNAL
//@@Usage
//The @|cenum| function allows you to use the textual strings of C 
//enumerated types (that have been defined using @|ctypedefine|) in
//your FreeMat scripts isntead of the hardcoded numerical values.  The
//general syntax for its use is
//@[
//  enum_int = cenum(enum_type,enum_string)
//@]
//which looks up the integer value of the enumerated type based on the
//string.  You can also supply an integer argument, in which case
//@|cenum| will find the matching string
//@[
//  enum_string = cenum(enum_type,enum_int)
//@]
//@@Signature
//function cenum CenumFunction
//inputs enumtype enumint
//outputs enumstring
//!
ArrayVector CenumFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("cenum requires at least two arguments");
  QString enumname = arg[0].asString();
  if (!CtypeTable.contains(enumname))
    throw Exception("type " + enumname + " is not defined");
  Ctype* p = CtypeTable.lookup(enumname);
  Cenum* q = dynamic_cast<Cenum*>(p);
  if (!q)
    throw Exception("type " + enumname + " is not an enumerated type");
  if (arg[1].isString()) {
    return ArrayVector() << Array(double(q->lookupByName(arg[1].asString())));
  } else {
    return ArrayVector() << Array(q->lookupByNumber(arg[1].asInteger()));
  }
  return ArrayVector();
}

//!
//@Module CTYPEDEFINE Define C Type
//@@Section EXTERNAL
//@@Usage
//The @|ctypedefine| function allows you to define C types for use
//with FreeMat.  Three variants of C types can be used.  You can
//use structures, enumerations, and aliases (typedefs).  All three are defined
//through a single function @|ctypedefine|.  The general syntax for
//its use is
//@[
//  ctypedefine(typeclass,typename,...)
//@]
//where @|typeclass| is the variant of the type (legal values are
//@|'struct'|, @|'alias'|, @|'enum'|).  The second argument is the
//name of the C type.  The remaining arguments depend on what the
//class of the typedef is.  
//
//To define a C structure, use the @|'struct'| type class.  The usage
//in this case is:
//@[
//  ctypedefine('struct',typename,field1,type1,field2,type2,...)
//@]
//The argument @|typename| must be a valid identifier string.  Each of
//of the @|field| arguments is also a valid identifier string that 
//describe in order, the elements of the C structure.  The @|type| arguments
//are @|typespecs|.  They can be of three types:
//\begin{itemize}
//\item Built in types, e.g. @|'uint8'| or @|'double'| to name a couple of
//      examples.
//\item C types that have previously been defined with a call to 
//      @|ctypedefine|, e.g. @|'mytype'| where @|'mytype'| has already been
//      defined through a call to @|ctypedefine|.
//\item Arrays of either built in types or previously defined C types
//      with the length of the array coded as an integer in square brackets, 
//      for example: @|'uint8[10]'| or @|'double[1000]'|.
//\end{itemize}
//
//To define a C enumeration, use the @|'enum'| type class.  The usage
//in this case is:
//  ctypedefine('enum',typename,name1,value1,name2,value2,...)
//@]
//The argument @|typename| must be a valid identifier string.  Each of the
//@|name| arguments must also be valid identifier strings that describe
//the possible values that the enumeration can take an, and their corresponding
//integer values.  Note that the names should be unique.  The behavior of
//the various @|cenum| functions is undefined if the names are not unique.
//
//To define a C alias (or typedef), use the following form of @|ctypedefine|:
//@[
//  ctypedefine('alias',typename,aliased_typename)
//@]
//where @|aliased_typename| is the type that is being aliased to.
//@@Signature
//function ctypedefine CtypeDefineFunction
//inputs varargin
//outputs none
//!
ArrayVector CtypeDefineFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) return ArrayVector();
  if (arg.size() < 3) throw Exception("ctypedefine requires at least three arguments - the typeclass ('struct','alias','enum'), the typename, and some type definition information");
  QString tclass = arg[0].asString();
  if (tclass == "enum") {
    QString tname = arg[1].asString();
    int cnt = 2;
    Cenum *enumDef = new Cenum;
    while (cnt < arg.size()) {
      QString ftype = arg[cnt].asString();
      if (arg.size() < (cnt+1))
	throw Exception("Expecting value for enum name " + ftype);
      int fvalue = arg[cnt+1].asInteger();
      enumDef->addPair(ftype,fvalue);
      cnt += 2;
    }
    CtypeTable.add(tname,enumDef);
  } else if (tclass == "struct") {
    QString tname = arg[1].asString();
    int cnt = 2;
    Cstruct *new_struct = new Cstruct;
    while (cnt < arg.size()) {
      QString ftype = arg[cnt].asString();
      if (arg.size() < (cnt+1))
	throw Exception("Expecting typespec for fieldname " + ftype);
      QString ttypespec = arg[cnt+1].asString();
      QString ttype;
      int tlength;
      if (ttypespec.indexOf('[') != -1) {
	int left_brace = ttypespec.indexOf('[');
	int right_brace = ttypespec.indexOf(']');
	ttype = ttypespec.left(left_brace);
	tlength = ttypespec.mid(left_brace+1,right_brace-left_brace-1).toInt();
	if (tlength <= 0)
	  throw Exception(QString("illegal length of %d in defining type ").arg(tlength) + tname);
      } else {
	ttype = ttypespec;
	tlength = 1;
      }
      if (!CtypeTable.contains(ttype)) 
	throw Exception("type " + ttype + " is not defined");
      new_struct->addField(new CstructField(ftype,ttype,tlength));
      cnt += 2;
    }
    CtypeTable.add(tname,new_struct);
  } else if (tclass == "alias") {
    QString tname = arg[1].asString();
    QString aname = arg[2].asString();
    if (tname == aname)
      throw Exception("self-referencing aliases not allowed: " + tname);
    if (!CtypeTable.contains(aname))
      throw Exception("type " + aname + " is not defined");
    CtypeTable.add(tname,new Calias(aname));
  }
  return ArrayVector();
}

//!
//@Module CTYPEPRINT Print C Type
//@@Section EXTERNAL
//@@Usage
//The @|ctypeprint| function prints a C type on the console.  The 
//usage is
//@[
//  ctypeprint(typename)
//@]
//where @|typename| is a string containing the name of the C type to print.
//Depending on the class of the C type (e.g., structure, alias or enumeration)
//the @|ctypeprint| function will dump information about the type definition.
//@@Signature
//sfunction ctypeprint CtypePrintFunction
//inputs typename
//outputs none
//!
ArrayVector CtypePrintFunction(int nargout, const ArrayVector& arg, Interpreter *m_eval) {
  if (arg.size() < 1) return ArrayVector();
  QString tname = arg[0].asString();
  if (!CtypeTable.contains(tname)) {
    m_eval->outputMessage("ctype " + tname + " not in table");
    return ArrayVector();
  }
  m_eval->outputMessage("ctype: " + tname + " ");
  CtypeTable.lookup(tname)->print(m_eval);
  return ArrayVector();  
}

//!
//@Module CTYPEFREEZE Convert FreeMat Structure to C Type
//@@Section EXTERNAL
//@@Usage
//The @|ctypefreeze| function is used to convert a FreeMat structure into
//a C struct as defined by a C structure typedef.  To use the @|cstructfreeze|
//function, you must first define the type of the C structure using the 
//@|ctypedefine| function.  The @|ctypefreeze| function then serializes
//a FreeMat structure to a set of bytes, and returns it as an array.  The
//usage for @|ctypefreeze| is
//@[
//  byte_array = ctypefreeze(mystruct, 'typename')
//@]
//where @|mystruct| is the array we want to 'freeze' to a memory array,
//and @|typename| is the name of the C type that we want the resulting
//byte array to conform to.
//@@Signature
//sfunction ctypefreeze CtypeFreezeFunction
//inputs mystruct typename
//outputs bytearray
//!
ArrayVector CtypeFreezeFunction(int nargout, const ArrayVector& arg, Interpreter* m_eval) {
  if (arg.size() != 2) 
    throw Exception("ctypefreeze requires two arguments - the structure to freeze and the typename to use");
  Array s(arg[0]);
  QString ttype(arg[1].asString());
  if (!CtypeTable.contains(ttype))
    throw Exception("unable to find a C type definition for " + ttype);
  QByteArray output;
  CtypeTable.lookup(ttype)->freeze(output,s,int(s.length()),m_eval);
  Array retvec(UInt8,NTuple(1,output.length()));
  memcpy(retvec.real<uint8>().data(),output.constData(),output.length());
  return ArrayVector() << retvec;
}

//!
//@Module CTYPESIZE Compute Size of C Struct
//@@Section EXTERNAL
//@@Usage
//The @|ctypesize| function is used to compute the size of a C structure
//that is defined using the @|ctypedefine| function.  The usage of 
//@|ctypesize| is 
//@[
//   size = ctypesize('typename')
//@]
//where @|typename| is the name of the C structure you want to compute
//the size of.  The returned count is measured in bytes.  Note that as
//indicated in the help for @|ctypedefine| that FreeMat does not 
//automatically pad the entries of the structure to match the particulars
//of your C compiler.  Instead, the assumption is that you have adequate
//padding entries in your structure to align the FreeMat members with the
//C ones.  See @|ctypedefine| for more details.  You can also specify
//an optional count parameter if you want to determine how large multiple
//structures are
//@[
//   size = ctypesize('typename',count)
//@]
//@@Signature
//function ctypesize CtypeSizeFunction
//inputs type count
//outputs size
//!
ArrayVector CtypeSizeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("ctypesize requires an argument - the structure name");
  int count = 1;
  if (arg.size() > 1)
    count = arg[1].asInteger();
  QString ttype(arg[0].asString());
  if (!CtypeTable.contains(ttype))
    throw Exception("unable to find a C struct definition for type " + ttype);
  return ArrayVector() << Array(double(CtypeTable.lookup(ttype)->size(count)));
}

//!
//@Module CTYPETHAW Convert C Struct to FreeMat Structure
//@@Section EXTERNAL
//@@Usage
//The @|ctypethaw| function is used to convert a C structure that is
//encoded in a byte array into a FreeMat structure using a C structure
//typedef.  To use the @|ctypethaw| function, you must first define
//the type of the C structure using the @|ctypedefine| function.  The
//usage of @|ctypethaw| is
//@[
//  mystruct = ctypethaw(byte_array, 'typename')
//@]
//where @|byte_array| is a @|uint8| array containing the bytes that encode
//the C structure, and @|typename| is a string that contains the type
//description as registered with @|ctypedefine|.  If you want to
//retrieve multiple structures from a single byte array, you can specify
//a count as
//@[
//  mystruct = ctypethaw(byte_array, 'typename', count)
//@]
//where @|count| is an integer containing the number of structures to 
//retrieve.  Sometimes it is also useful to retrieve only part of the
//structure from a byte array, and then (based on the contents of the
//structure) retrieve more data.  In this case, you can retrieve the
//residual byte array using the optional second output argument of
//@|ctypethaw|:
//@[
//  [mystruct,byte_array_remaining] = ctypethaw(byte_array, 'typename',...)
//@]
//@@Tests
//@{ test_ctype1.m
//function test_val = test_ctype1
//  ctypedefine('struct','tstruct','v1','int32','v2','double','v3','float','v4','uint32[5]');
//  g.v1 = int32(54); g.v2 = pi; g.v3 = float(exp(1)); g.v4 = uint32(1:5);
//  h = ctypefreeze(g,'tstruct');
//  k = ctypethaw(h,'tstruct');
//  test_val = issame(k,g);
//@}
//@@Signature
//sfunction ctypethaw CtypeThawFunction
//inputs x type count
//outputs mystruct byarray
//!
ArrayVector CtypeThawFunction(int nargout, const ArrayVector& arg, Interpreter* m_eval) {
  if (arg.size() < 2)
    throw Exception("ctypethaw requires two arguments - the uint8 array to thaw the structure from, and the typename to use");
  Array s(arg[0].asDenseArray());
  QString ttype(arg[1].asString());
  int count = 1;
  if (arg.size() > 2) count = arg[2].asInteger();
  if (!CtypeTable.contains(ttype))
    throw Exception("unable to find a C struct definition for type " + ttype);
  if (s.dataClass() != UInt8)
    throw Exception("first argument to ctypethaw must be a uint8 array");
  QByteArray input((const char*)(s.constReal<uint8>().constData()),int(s.length()));
  int pos = 0;
  ArrayVector outputs;
  outputs << CtypeTable.lookup(ttype)->thaw(input,pos,count,m_eval);
  if (nargout > 1) {
    input.remove(0,pos);
    Array retvec(UInt8,NTuple(1,int(input.length())));
    memcpy(retvec.real<uint8>().data(),input.constData(),input.length());
    outputs << retvec;
  }
  return outputs;
}

