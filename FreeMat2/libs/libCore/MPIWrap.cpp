#ifdef USE_MPI

#include <mpi.h>
#include "HandleList.hpp"
#include "MPIWrap.hpp"
#include "Malloc.hpp"
#include "FunctionDef.hpp"
#include "Context.hpp"

namespace FreeMat {

  HandleList<MPI_Comm> comms;

  void InitializeMPIWrap() {
    comms.assignHandle(MPI_COMM_WORLD);
  }

  // Helper function
  int ArrayToInt(const Array &A) {
    Array tmp(A);
    return tmp.getContentsAsIntegerScalar();
  }
  
  /* 
   * Send an array via MPI:
   *  Arguments: MPISend(A, dest, tag, communicator)
   *  Throws an exception if there is an error.
   *
   *  How does this work?  Well, tag is a positive integer - we
   *  use it as upper bits in the message id, reserving lower
   *  bits for the submessage.
   *
   *  Suppose A is an array (homogenous) - 
   *     Part 1 - type
   *     Part 2 - dimension data
   *     Part 3 - array contents
   *
   *  Suppose A is an array (structure)
   *     Part 1 - type
   *     Part 2 - dimension data
   *     Part 3 - fields (if its a structure) 
   *     Part 4 - the component arrays
   */


  /*
   * A simpler solution is to pack and unpack the data into a buffer and then
   * send the buffer using the raw protocol.  The problem with this is that 
   * buffers get duplicated...  But that's ok -  The problem is to determine
   * how much buffer space is needed - that too is non-trivial - consider...
   */

  /*
   * We could do something like this:
   *  Is the array homogenous?
   *
   *
   */

  // An array transmission is composed of how many array transmissions:
  //   1.  How many arrays are contained in A? --> ArrayCount
  //   2.  Send msgID + ArrayCount
  //   3.  For each Array

  // Simplest method:
  //   1.  Pack the array into a buffer (and determine the resulting length)
  //   2.  Send msgID*2 as the length of the buffer
  //   3.  Send msgID*2+1 as the contents of the buffer

  int getCanonicalSize(int count, MPI_Datatype atype, MPI_Comm comm) {
    int size;
    MPI_Pack_size(count,atype,comm,&size);
    return size;
  }

  // OK, for now, I'm going to use this method.
  int getArrayByteFootPrint(Array &a, MPI_Comm comm) {
    unsigned int overhead;
    // How many bytes in the overhead
    overhead = getCanonicalSize(maxDims+1,MPI_INT, comm);
    Class dataClass(a.getDataClass());
    // Is input array a reference type?
    if (a.isReferenceType()) {
      if (dataClass == FM_CELL_ARRAY) {
	int total = 0;
	Array *dp;
	dp = (Array *)a.getDataPointer();
	for (int i=0;i<a.getLength();i++)
	  total += getArrayByteFootPrint(dp[i],comm);
	return (total+overhead);
      } else {
	// Array is a structure array
	stringVector fieldnames(a.getFieldNames());
	int fieldcount = 0;
	fieldcount = fieldnames.size();
	// Start out with the number of fields
	int fieldsize = getCanonicalSize(1,MPI_INT,comm);
	// Each field is encoded as a length + the number of characters in the name
	for (int j=0;j<fieldcount;j++)
	  fieldsize += getCanonicalSize(1,MPI_INT,comm) + 
	    getCanonicalSize(fieldnames[j].size(),MPI_CHAR,comm);
	int total = 0;
	Array *dp;
	dp = (Array *) a.getDataPointer();
	for (int i=0;i<a.getLength()*fieldcount;i++)
	  total += getArrayByteFootPrint(dp[i],comm);
	return (total+overhead+fieldsize+1);
      }
    }
    switch(dataClass) {
    case FM_LOGICAL:
      return(overhead+getCanonicalSize(a.getLength(),MPI_CHAR,comm));
    case FM_UINT8:
      return(overhead+getCanonicalSize(a.getLength(),MPI_UNSIGNED_CHAR,comm));
    case FM_INT8:
      return(overhead+getCanonicalSize(a.getLength(),MPI_CHAR,comm));
    case FM_UINT16:
      return(overhead+getCanonicalSize(a.getLength(),MPI_UNSIGNED_SHORT,comm));
    case FM_INT16:
      return(overhead+getCanonicalSize(a.getLength(),MPI_SHORT,comm));
    case FM_UINT32:
      return(overhead+getCanonicalSize(a.getLength(),MPI_UNSIGNED,comm));
    case FM_INT32:
      return(overhead+getCanonicalSize(a.getLength(),MPI_INT,comm));
    case FM_FLOAT:
      return(overhead+getCanonicalSize(a.getLength(),MPI_FLOAT,comm));
    case FM_DOUBLE:
      return(overhead+getCanonicalSize(a.getLength(),MPI_DOUBLE,comm));
    case FM_COMPLEX:
      return(overhead+getCanonicalSize(a.getLength()*2,MPI_FLOAT,comm));
    case FM_DCOMPLEX:
      return(overhead+getCanonicalSize(a.getLength()*2,MPI_DOUBLE,comm));
    case FM_STRING:
      return(overhead+getCanonicalSize(a.getLength(),MPI_CHAR,comm));
    }
  }

  Class decodeDataClassFromInteger(int code) {
    switch(code) {
    case 1024:
      return FM_CELL_ARRAY;
    case 1025:
      return FM_STRUCT_ARRAY;
    case 1026:
      return FM_LOGICAL;
    case 1027:
      return FM_UINT8;
    case 1028:
      return FM_INT8;
    case 1029:
      return FM_UINT16;
    case 1030:
      return FM_INT16;
    case 1031:
      return FM_UINT32;
    case 1032:
      return FM_INT32;
    case 1033:
      return FM_FLOAT;
    case 1034:
      return FM_DOUBLE;
    case 1035:
      return FM_COMPLEX;
    case 1036:
      return FM_DCOMPLEX;
    case 1037:
      return FM_STRING;
    }
  }

  int encodeDataClassAsInteger(Class dataClass) {
    switch (dataClass) {
    case FM_CELL_ARRAY:
      return 1024;
    case FM_STRUCT_ARRAY:
      return 1025;
    case FM_LOGICAL:
      return 1026;
    case FM_UINT8:
      return 1027;
    case FM_INT8:
      return 1028;
    case FM_UINT16:
      return 1029;
    case FM_INT16:
      return 1030;
    case FM_UINT32:
      return 1031;
    case FM_INT32:
      return 1032;
    case FM_FLOAT:
      return 1033;
    case FM_DOUBLE:
      return 1034;
    case FM_COMPLEX:
      return 1035;
    case FM_DCOMPLEX:
      return 1036;
    case FM_STRING:
      return 1037;
    }
  }
  
  // Pack an array into an MPI buffer using the MPI Pack functions
  // We assume that the buffer is large enough, i.e. that it is 
  // at least of size getArrayByteFootPrint(a) in size.
  void packArrayMPI(Array &a, void *buffer, int bufsize, int *packpos, MPI_Comm comm) {
    Class dataClass(a.getDataClass());
    int idclass;
    int dimlength;
    idclass = encodeDataClassAsInteger(dataClass);
    MPI_Pack(&idclass,1,MPI_INT,buffer,bufsize,packpos,comm);
    dimlength = a.getDimensions().getLength();
    MPI_Pack(&dimlength,1,MPI_INT,buffer,bufsize,packpos,comm);
    for (int j=0;j<dimlength;j++) {
      int tmp;
      tmp = a.getDimensionLength(j);
      MPI_Pack(&tmp,1,MPI_INT,buffer,bufsize,packpos,comm);
    }
    if (a.isReferenceType()) {
      if (dataClass == FM_CELL_ARRAY) {
	Array *dp;
	dp = (Array *) a.getDataPointer();
	for (int i=0;i<a.getLength();i++)
	  packArrayMPI(dp[i],buffer,bufsize,packpos,comm);
      } else {
	stringVector fieldnames(a.getFieldNames());
	int fieldcnt(fieldnames.size());
	MPI_Pack(&fieldcnt,1,MPI_INT,buffer,bufsize,packpos,comm);
	for (int i=0;i<fieldcnt;i++) {
	  int flen;
	  flen = fieldnames[i].size();
	  MPI_Pack(&flen,1,MPI_INT,buffer,bufsize,packpos,comm);
	  MPI_Pack((void*) fieldnames[i].c_str(),flen,MPI_CHAR,buffer,bufsize,packpos,comm);
	}
	Array *dp;
	dp = (Array *) a.getDataPointer();
	for (int i=0;i<a.getLength()*fieldcnt;i++)
	  packArrayMPI(dp[i],buffer,bufsize,packpos,comm);
      }
    } else {
      switch(dataClass) {
      case FM_LOGICAL:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_CHAR,buffer,bufsize,packpos,comm);
	break;
      case FM_UINT8:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_UNSIGNED_CHAR,buffer,bufsize,packpos,comm);
	break;
      case FM_INT8:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_CHAR,buffer,bufsize,packpos,comm);
	break;
      case FM_UINT16:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_UNSIGNED_SHORT,buffer,bufsize,packpos,comm);
	break;
      case FM_INT16:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_SHORT,buffer,bufsize,packpos,comm);
	break;
      case FM_UINT32:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_UNSIGNED,buffer,bufsize,packpos,comm);
	break;
      case FM_INT32:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_INT,buffer,bufsize,packpos,comm);
	break;
      case FM_FLOAT:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_FLOAT,buffer,bufsize,packpos,comm);
	break;
      case FM_DOUBLE:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_DOUBLE,buffer,bufsize,packpos,comm);
	break;
      case FM_COMPLEX:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_FLOAT,buffer,bufsize,packpos,comm);
	break;
      case FM_DCOMPLEX:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_DOUBLE,buffer,bufsize,packpos,comm);
	break;
      case FM_STRING:
	MPI_Pack((void *) a.getDataPointer(),a.getLength(),MPI_CHAR,buffer,bufsize,packpos,comm);
	break;
      }
    }
  }

  Array unpackArrayMPI(void *buffer, int bufsize, int *packpos, MPI_Comm comm) {
    Class dataClass;
    int idclass;
    int dimlength;
    MPI_Unpack(buffer,bufsize,packpos,&idclass,1,MPI_INT,comm);
    dataClass = decodeDataClassFromInteger(idclass);
    MPI_Unpack(buffer,bufsize,packpos,&dimlength,1,MPI_INT,comm);
    Dimensions outDim;
    for (int j=0;j<dimlength;j++) {
      int tmp;
      MPI_Unpack(buffer,bufsize,packpos,&tmp,1,MPI_INT,comm);
      outDim[j] = tmp;
    }
    if (dataClass == FM_CELL_ARRAY) {
      Array *dp;
      dp = new Array[outDim.getElementCount()];
      for (int i=0;i<outDim.getElementCount();i++)
	dp[i] = unpackArrayMPI(buffer,bufsize,packpos,comm);
      return Array(FM_CELL_ARRAY,outDim,dp);
    } else if (dataClass == FM_STRUCT_ARRAY) {
      int fieldcnt;
      MPI_Unpack(buffer,bufsize,packpos,&fieldcnt,1,MPI_INT,comm);
      stringVector fieldnames;
      for (int j=0;j<fieldcnt;j++) {
	int fieldnamelength;
	MPI_Unpack(buffer,bufsize,packpos,&fieldnamelength,1,MPI_INT,comm);
	char *dbuff;
	dbuff = (char*) malloc(fieldnamelength+1);
	MPI_Unpack(buffer,bufsize,packpos,dbuff,fieldnamelength,MPI_CHAR,comm);
	dbuff[fieldnamelength] = 0;
	fieldnames.push_back(std::string(dbuff));
	free(dbuff);
      }
      Array *dp;
      dp = new Array[fieldcnt*outDim.getElementCount()];
      for (int i=0;i<fieldcnt*outDim.getElementCount();i++)
	dp[i] = unpackArrayMPI(buffer,bufsize,packpos,comm);
      return Array(FM_STRUCT_ARRAY,outDim,dp,fieldnames);
    }
    void *cp;
    switch(dataClass) {
    case FM_LOGICAL:
      cp = Malloc(sizeof(logical)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_CHAR,comm);
      break;
    case FM_UINT8:
      cp = Malloc(sizeof(uint8)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_UNSIGNED_CHAR,comm);
      break;
    case FM_INT8:
      cp = Malloc(sizeof(int8)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_CHAR,comm);
      break;
    case FM_UINT16:
      cp = Malloc(sizeof(uint16)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_UNSIGNED_SHORT,comm);
      break;
    case FM_INT16:
      cp = Malloc(sizeof(int16)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_SHORT,comm);
      break;
    case FM_UINT32:
      cp = Malloc(sizeof(uint32)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_UNSIGNED,comm);
      break;
    case FM_INT32:
      cp = Malloc(sizeof(int32)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_INT,comm);
      break;
    case FM_FLOAT:
      cp = Malloc(sizeof(float)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_FLOAT,comm);
      break;
    case FM_DOUBLE:
      cp = Malloc(sizeof(double)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_DOUBLE,comm);
      break;
    case FM_COMPLEX:
      cp = Malloc(sizeof(float)*2*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount()*2,MPI_FLOAT,comm);
      break;
    case FM_DCOMPLEX:
      cp = Malloc(sizeof(double)*2*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount()*2,MPI_DOUBLE,comm);
      break;
    case FM_STRING:
      cp = Malloc(sizeof(char)*outDim.getElementCount());
      MPI_Unpack(buffer,bufsize,packpos,cp,outDim.getElementCount(),MPI_CHAR,comm);
      break;
    }
    return Array(dataClass,outDim,cp);
  }

  ArrayVector MPISend(int nargout, const ArrayVector& args) {
    if ((args.size() < 3) || (args.size() > 4))
      throw Exception("Expect 4 arguments to MPISend: array to send, destination rank, message tag and (optionally communicator handle -- defaults to MPI_COMM_WORLD)");
    Array A(args[0]);
    Array tmp(args[1]);
    int dest(tmp.getContentsAsIntegerScalar());
    tmp = args[2];
    int tag(tmp.getContentsAsIntegerScalar());
    int comhandle;
    if (args.size() > 3) {
      tmp = args[3];
      comhandle = tmp.getContentsAsIntegerScalar();
    } else {
      comhandle = 1;
    }
    // Calculate how much space we need to pack A
    MPI_Comm comm(comms.lookupHandle(comhandle));
    int Asize = getArrayByteFootPrint(A,comm);
    int bufsize = Asize;
    // Allocate it...
    void *cp = malloc(Asize);
    int packpos = 0;
    packArrayMPI(A,cp,bufsize,&packpos,comm);
    MPI_Send(&packpos,1,MPI_INT,dest,tag,comm);
    MPI_Send(cp,packpos,MPI_PACKED,dest,tag,comm);
    free(cp);
    return ArrayVector();
  }

  ArrayVector MPIBarrier(int nargout, const ArrayVector& args) {
    int comhandle;
    if (args.size() == 0) {
      Array tmp(args[0]);
      comhandle = tmp.getContentsAsIntegerScalar();
    } else {
      comhandle = 1;
    }
    MPI_Comm comm(comms.lookupHandle(comhandle));
    MPI_Barrier(comm);
    return ArrayVector();
  }

  /*
   * Broadcast an array via MPI:
   *  Arguments: A = MPIBcast(A,root,communicator)
   */
  ArrayVector MPIBcast(int nargout, const ArrayVector& args) {
    if ((args.size() < 2) || (args.size() > 3))
      throw Exception("expect 3 arguments to MPIBcast: array, root rank and (optionally) communicator handle");
    Array A(args[0]);
    Array tmp(args[1]);
    int root(tmp.getContentsAsIntegerScalar());
    int comhandle;
    if (args.size() > 2) {
      tmp = args[2];
      comhandle = tmp.getContentsAsIntegerScalar();
    } else {
      comhandle = 1;
    }
    MPI_Comm comm(comms.lookupHandle(comhandle));
    // Get our rank
    int ourrank;
    MPI_Comm_rank(comm,&ourrank);
    ArrayVector retval;
    // Are we the originator of this broadcast?
    if (ourrank == root) {
      // Marshall the array into a message
      int Asize = getArrayByteFootPrint(A,comm);
      int bufsize = Asize;
      void *cp = malloc(Asize);
      int packpos = 0;
      packArrayMPI(A,cp,bufsize,&packpos,comm);
      // First broadcast the size
      MPI_Bcast(&packpos,1,MPI_INT,root,comm);
      // Then broadcast the data
      MPI_Bcast(cp,packpos,MPI_PACKED,root,comm);
      // Clean up
      free(cp);
      // Return it to the sender...
      retval.push_back(A);
    } else {
      // We are note the originator - wait for the size to 
      // appear
      int msgsize;
      MPI_Bcast(&msgsize,1,MPI_INT,root,comm);
      void *cp = malloc(msgsize);
      MPI_Bcast(cp,msgsize,MPI_PACKED,root,comm);
      int packpos = 0;
      Array A2(unpackArrayMPI(cp,msgsize,&packpos,comm));
      free(cp);
      retval.push_back(A2);
    }
    return retval;
  }

  /*
   * Recv an array via MPI:
   *  Arguments: A = MPIRecv(source, tag, communicator)
   *  Throws an exception if there is an error.
   */
  ArrayVector MPIRecv(int nargout, const ArrayVector& args) {
    if ((args.size() < 2) || (args.size() > 3))
      throw Exception("Expect 3 arguments to MPIRecv: source rank, message tag and (optionally communicator handle -- defaults to MPI_COMM_WORLD)");
    Array tmp(args[0]);
    int source(tmp.getContentsAsIntegerScalar());
    tmp = args[1];
    int tag(tmp.getContentsAsIntegerScalar());
    int comhandle;
    if (args.size() > 2) {
      tmp = args[2];
      comhandle = tmp.getContentsAsIntegerScalar();
    } else {
      comhandle = 1;
    }
    MPI_Comm comm(comms.lookupHandle(comhandle));
    int msgsize;
    MPI_Status status;
    MPI_Recv(&msgsize,1,MPI_INT,source,tag,comm,&status);
    void *cp = malloc(msgsize);
    MPI_Recv(cp,msgsize,MPI_PACKED,status.MPI_SOURCE,status.MPI_TAG,comm,MPI_STATUS_IGNORE);
    int packpos = 0;
    Array A(unpackArrayMPI(cp,msgsize,&packpos,comm));
    free(cp);
    ArrayVector retval;
    retval.push_back(A);
    retval.push_back(Array::int32Constructor(status.MPI_SOURCE));
    retval.push_back(Array::int32Constructor(status.MPI_TAG));
    return retval;
  }

  /*
   * Arguments MPIEval(commandline, ranks, communicator)
   */
  ArrayVector MPIEval(int nargout, const ArrayVector& args) {
    if (args.size()  < 2)
      throw Exception("mpieval requires at least a command line to execute, and a set of ranks to eval at");
    Array tmp(args[0]);
    char *cmd_line;
    cmd_line = tmp.getContentsAsCString();
    tmp = args[1];
    int rankcount;
    rankcount = tmp.getLength();
    tmp.promoteType(FM_INT32);
    int32 *ranks;
    ranks = (int32*) tmp.getDataPointer();
    int comhandle;
    if (args.size() < 3) {
      comhandle = 1;
    } else {
      tmp = args[2];
      comhandle = tmp.getContentsAsIntegerScalar();
    }
    MPI_Comm comm(comms.lookupHandle(comhandle));
    for (int i=0;i<rankcount;i++)
      MPI_Send(cmd_line,strlen(cmd_line)+1,MPI_CHAR,ranks[i],0,comm);
    return ArrayVector();
  }  

  ArrayVector MPICommRank(int nargout, const ArrayVector& args) {
    int comhandle;
    if (args.size() == 0) {
      comhandle = 1;
    } else {
      Array tmp(args[0]);
      comhandle = tmp.getContentsAsIntegerScalar();
    }
    MPI_Comm comm(comms.lookupHandle(comhandle));
    int rank;
    MPI_Comm_rank(comm,&rank);
    ArrayVector retval;
    retval.push_back(Array::int32Constructor(rank));
    return retval;
  }

  ArrayVector MPICommSize(int nargout, const ArrayVector& args) {
    int comhandle;
    if (args.size() == 0) {
      comhandle = 1;
    } else {
      Array tmp(args[0]);
      comhandle = tmp.getContentsAsIntegerScalar();
    }
    MPI_Comm comm(comms.lookupHandle(comhandle));
    int size;
    MPI_Comm_size(comm,&size);
    ArrayVector retval;
    retval.push_back(Array::int32Constructor(size));
    return retval;
  }

  /*
   * syntax: x = mpireduce(y,operation,root,comm)
   */
  ArrayVector MPIReduce(int nargout, const ArrayVector& args) {
    int comhandle;
    if (args.size() < 4)
      comhandle = 1;
    else 
      comhandle = ArrayToInt(args[3]);
    MPI_Comm comm(comms.lookupHandle(comhandle));
    if (args.size() < 3)
      throw Exception("mpireduce requires an array, an operation, and a root rank");
    int root = ArrayToInt(args[2]);
    char *op;
    Array oper(args[1]);
    op = oper.getContentsAsCString();
    MPI_Op mpiop;
    switch (*op) {
    case '+':
      mpiop = MPI_SUM;
      break;
    case '*':
      mpiop = MPI_PROD;
      break;
    case '<':
      mpiop = MPI_MIN;
      break;
    case '>':
      mpiop = MPI_MAX;
      break;
    default:
      throw Exception(std::string("unrecognized mpiop type:") + op + ": supported types are '+','*','>' and '<'");
    }
    Array source(args[0]);
    Array dest(source);
    Class dataClass(source.getDataClass());
    switch (dataClass) {
    case FM_LOGICAL:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_UNSIGNED_CHAR,mpiop,root,comm);
      break;
    case FM_UINT8:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_UNSIGNED_CHAR,mpiop,root,comm);
      break;
    case FM_INT8:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_CHAR,mpiop,root,comm);
      break;
    case FM_UINT16:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_UNSIGNED_SHORT,mpiop,root,comm);
      break;
    case FM_INT16:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_SHORT,mpiop,root,comm);
      break;
    case FM_UINT32:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_UNSIGNED,mpiop,root,comm);
      break;
    case FM_INT32:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_INT,mpiop,root,comm);
      break;
    case FM_FLOAT:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_FLOAT,mpiop,root,comm);
      break;
    case FM_DOUBLE:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),source.getLength(),MPI_DOUBLE,mpiop,root,comm);
      break;
    case FM_COMPLEX:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),2*source.getLength(),MPI_FLOAT,mpiop,root,comm);
      break;
    case FM_DCOMPLEX:
      MPI_Reduce((void*)source.getDataPointer(),dest.getReadWriteDataPointer(),2*source.getLength(),MPI_DOUBLE,mpiop,root,comm);
      break;
    default:
      throw Exception("unsupported array type in argument to reduce - must be a numerical type");
    }
    ArrayVector retval;
    retval.push_back(dest);
    return retval;
  }

  ArrayVector MPIInitialized(int nargout, const ArrayVector& args) {
    int flag;
    MPI_Initialized(&flag);
    ArrayVector retval;
    retval.push_back(Array::logicalConstructor(flag));
    return retval;
  }

  ArrayVector MPIInit(int nargout, const ArrayVector& args) {
    MPI_Init(NULL,NULL);
    InitializeMPIWrap();
    return ArrayVector();
  }

  //       MPI_Comm_spawn(char* command, char** argv, int maxprocs, MPI_Info info,
  //                      int root, MPI_Comm comm, MPI_Comm *intercomm,
  //                      int *errcodes)

  ArrayVector MPICommSpawn(int nargout, const ArrayVector& args) {
    char *command;
    char **argv;
    int maxprocs;
    int root;
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Comm intercomm;
    MPI_Comm comm;
    if (args.size() == 0)
      throw Exception("mpicommspawn requires at least one argument (name of the command to spawn");
    if (args.size() > 5)
      throw Exception("too many arguments to mpicommspawn.");
    Array t1(args[0]);
    command = t1.getContentsAsCString();
    if (args.size() < 2)
      argv = NULL;
    else {
      Array t2(args[1]);
      if (t2.isEmpty())
	argv = NULL;
      else {
	if (t2.isString()) {
	  argv = (char**) malloc(sizeof(char*)*2);
	  argv[1] = NULL;
	  argv[0] = t2.getContentsAsCString();
	} else if (t2.getDataClass() == FM_CELL_ARRAY) {
	  Array *dp;
	  dp = (Array*) t2.getDataPointer();
	  int len;
	  len = t2.getLength();
	  argv = (char**) malloc(sizeof(char*)*(len+1));
	  argv[len] = 0;
	  for (int m=0;m<len;m++) {
	    Array q(dp[m]);
	    argv[m] = q.getContentsAsCString();
	  }
	} else
	  throw Exception("mpicommspawn requires the argument array to either be a string or a cell array of strings (i.e., {'arg','arg',...'arg'}).");
      }
    }
    if (args.size() < 3)
      maxprocs = 1;
    else
      maxprocs = ArrayToInt(args[2]);
    if (args.size() < 4)
      root = 0;
    else
      root = ArrayToInt(args[3]);
    if (args.size() < 5)
      comm = MPI_COMM_SELF;
    else
      comm = comms.lookupHandle(ArrayToInt(args[4]));
    int *errcodes;
    errcodes = (int*) Malloc(sizeof(int)*maxprocs);
    int res;
    res = MPI_Comm_spawn(command,argv,maxprocs,info,root,
			 comm,&intercomm,errcodes);
    ArrayVector retarr;
    retarr.push_back(Array::int32Constructor(comms.assignHandle(intercomm)));
    Dimensions dim;
    dim[0] = maxprocs;
    dim[1] = 1;
    retarr.push_back(Array::Array(FM_INT32,dim,errcodes));
    return retarr;
  }

  ArrayVector MPIIntercommMerge(int nargout, const ArrayVector& args) {
    ArrayVector retval;
    MPI_Comm newcomm;
    int highflag;
    MPI_Comm comm;
    if (args.size() < 1)
      throw Exception("must supply a handle for the intercommunicator");
    comm = comms.lookupHandle(ArrayToInt(args[0]));
    if (args.size() < 2) {
      highflag = 0;
    } else {
      highflag = ArrayToInt(args[1]);
    }
    MPI_Intercomm_merge(comm,highflag,&newcomm);
    retval.push_back(Array::int32Constructor(comms.assignHandle(newcomm)));
    return retval;
  }

  ArrayVector MPICommGetParent(int nargout, const ArrayVector& args) {
    ArrayVector retval;
    MPI_Comm parent;
    int res;

    res = MPI_Comm_get_parent(&parent);
    // Map this back to a handle
    int maxsize(comms.maxHandle());
    bool matchFound = false;
    int i;
    for (i=1;i<=maxsize;i++) {
      int result;
      MPI_Comm_compare(parent,comms.lookupHandle(i),&result);
      matchFound = (result == MPI_IDENT);
      if (matchFound) break;
    }
    // Was the comm found?/
    if (matchFound)
      retval.push_back(Array::int32Constructor(i));
    else
      retval.push_back(Array::int32Constructor(comms.assignHandle(parent)));
    return retval;
  }

  ArrayVector MPIFinalize(int nargout, const ArrayVector& args) {
    MPI_Finalize();
    return ArrayVector();
  }

  void LoadMPIFunctions(Context*context) {
    stringVector args;

    args.push_back("array");
    args.push_back("dest");
    args.push_back("tag");
    args.push_back("communicator");
    context->addFunction("mpisend",MPISend,4,0,args);
    args.clear();
    args.push_back("source");
    args.push_back("tag");
    args.push_back("communicator");
    context->addFunction("mpirecv",MPIRecv,3,3,args);
    args.clear();
    args.push_back("array");
    args.push_back("root");
    args.push_back("communicator"); 
    context->addFunction("mpibcast",MPIBcast,3,1,args);
    args.clear();
    args.push_back("communicator");
    context->addFunction("mpibarrier",MPIBarrier,1,0,args);
    args.clear();
    args.push_back("communicator");
    context->addFunction("mpicommrank",MPICommRank,1,1,args);
    context->addFunction("mpicommsize",MPICommSize,1,1,args);
    args.clear();
    args.push_back("y");
    args.push_back("operation");
    args.push_back("root");
    args.push_back("comm");
    context->addFunction("mpireduce",MPIReduce,4,1,args);
    args.clear();
    context->addFunction("mpiinitialized",MPIInitialized,0,1,args);
    context->addFunction("mpiinit",MPIInit,0,0,args);
    context->addFunction("mpifinalize",MPIFinalize,0,0,args);
    context->addFunction("mpicommgetparent",MPICommGetParent,0,1,args);
    args.clear();
    args.push_back("command");
    args.push_back("args");
    args.push_back("maxprocs");
    args.push_back("root");
    args.push_back("comm");
    context->addFunction("mpicommspawn",MPICommSpawn,5,2,args);
    args.clear();
    args.push_back("intercomm");
    args.push_back("highflag");
    context->addFunction("mpiintercommmerge",MPIIntercommMerge,2,1,args);
//     f2def = new BuiltInFunctionDef;
//     f2def->retCount = 0;
//     f2def->argCount = 3;
//     f2def->name = "mpieval";
//     f2def->fptr = MPIEval;
//     context->insertFunctionGlobally(f2def);
//    InitializeMPIWrap();
  }
}

#endif
