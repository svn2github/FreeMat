#ifdef USE_MPI

#include <mpi.h>
#include "HandleList.hpp"
#include "MPIWrap.hpp"
#include "Malloc.hpp"
#include "FunctionDef.hpp"
#include "Context.hpp"

namespace FreeMat {

  HandleList<MPI_Comm> comms;
  MPI_Errhandler errhdl;

  void MPIErrHandler(MPI_Comm *comm, int *errorcode, ...) {
    char buffer[4096];
    int resultlen;
    MPI_Error_string(*errorcode,buffer,&resultlen);
    buffer[resultlen] = 0;
    throw Exception(buffer);
  }

  void InitializeMPIWrap() {
    comms.assignHandle(MPI_COMM_WORLD);
    MPI_Comm_create_errhandler(MPIErrHandler,&errhdl);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, errhdl);
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
v   *     Part 2 - dimension data
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

  //!
  //@Module MPISEND MPI Send Array
  //@@Usage
  //This function sends an array to a destination node on a 
  //given communicator with a specific message tag.  Note that
  //there has to be a matching receive issued by the destination
  //node.  The general syntax for its use is
  //@[
  //  mpisend(x,rank,tag,comm)
  //@]
  //where @|x| is the array to send, @|rank| is the rank of the
  //node to receive the message, @|tag| is the message tag, and
  //@|comm| is the handle of the communicator to use.  If no
  //communicator is specified, then @|MPI_COMM_WORLD| is used.
  //@@Function Internals
  //The @|mpisend| command works by packing the array into a 
  //linear buffer and then sending two messages.  The first
  //message captures the size of the buffer, and the second
  //contains the actual data.  The matching @|mpirecv| command
  //reads the two messages, decodes the buffer, and returns
  //the resulting array.
  //@@Example
  //The @|mpisend| command is fairly straightforward to use.
  //Its power is in the ability to send arrays of arbitrary
  //complexity, including cell arrays, structures, strings, etc.
  //Here is an example of an @|mpisend| and @|mpirecv| being used
  //on the same node to pass a structure through MPI.
  //@<
  //mpiinit
  //x.color = 'blue';
  //x.pi = 3;
  //x.cells = {'2',2};
  //mpisend(x,0,32);
  //y = mpirecv(0,32)
  //@>
  //!
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

  //!
  //@Module MPIBARRIER MPI Barrier
  //@@Usage
  //This function is used as a synchronization point for all
  //processes in a group.  All processes are blocked until
  //every process calls @|mpibarrier|.  The general syntax 
  //for its use is
  //@[
  //  mpibarrier(comm)
  //@]
  //where @|comm| is the communicator.  If no communicator is
  //provided, it defaults to @|MPI_COMM_WORLD|.
  //!
  ArrayVector MPIBarrier(int nargout, const ArrayVector& args) {
    int comhandle;
    if (args.size() == 1) {
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
  //!
  //@Module MPIBCAST MPI Broadcast
  //@@Usage
  //This function is used to broadcast an array to all group
  //members.  The syntax for its use 
  //@[
  //  B = mpibcast(A,root,comm)
  //@]
  //where @|A| is the array to broadcast, @|root| is the rank
  //of the root of the broadcast, and @|comm| is the communicator
  //to do the broadcast on.  If no communicator is provided, it
  //defaults to @|MPI_COMM_WORLD|.  Note that in practice, the process
  //running at the root will use the syntax:
  //@[
  //  mpibcast(A,root,comm),
  //@]
  //while the remaining processes will use the syntax
  //@[
  //  B = mpibcast([],root,comm).
  //@]
  //!
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
  //!
  //@Module MPIRECV MPI Receive Array
  //@@Usage
  //This function receives an array from a source node on
  //a given communicator with the specified tag.  The 
  //general syntax for its use is
  //@[
  //  y = mpirecv(rank,tag,comm)
  //@]
  //where @|rank| is the rank of the node sending the message,
  //@|tag| is the message tag and @|comm| is the communicator
  //to use.  If no communicator is provided, then @|MPI_COMM_WORLD|
  //is used.
  //@@Example
  //The @|mpirecv| command is fairly straightforward to use.
  //Its power is in the ability to receive arrays of arbitrary
  //complexity, including cell arrays, structures, strings, etc.
  //Here is an example of an @|mpisend| and @|mpirecv| being used
  //on the same node to pass a structure through MPI.
  //@<
  //mpiinit
  //x.color = 'blue';
  //x.pi = 3;
  //x.cells = {'2',2};
  //mpisend(x,0,32);
  //y = mpirecv(0,32)
  //@>
  //!
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

  //!
  //@Module MPICOMMRANK MPI Communicator Rank
  //@@Usage
  //This function returns the rank of a process within
  //the specified communicator.  The general syntax for 
  //its use is
  //@[
  //   y = mpicommrank(comm)
  //@]
  //where @|comm| is the communicator to use.  If no communicator
  //is provided, then @|MPI_COMM_WORLD| is used.  The returned value
  //@|y| is the rank of the current process in the communicator.
  //@@Example
  //Here is a simple example of using @|mpicommrank| to obtain the
  //process rank.  It defaults to 0, because the process is the root
  //of the group (which contains only itself).
  //@<
  //mpiinit
  //mpicommrank
  //@>
  //!
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

  //!
  //@Module MPICOMMSIZE MPI Communicator Size
  //@@Usage
  //This function returns the size of the group using the
  //given communicator.  The general syntax for its use is
  //@[
  //    y = mpicommsize(comm)
  //@]
  //where @|comm| is the communicator to use.  If no communicator 
  //is provided, then @|MPI_COMM_WORLD| is assumed.  
  //@@Example
  //Here is a simple example of using @|mpicommsize|:
  //@<
  //mpiinit
  //mpicommrank
  //@>
  //!
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
  //!
  //@Module MPIREDUCE MPI Reduce Operation
  //@@Usage
  //This function implements the reduction operation using MPI.
  //The general syntax for its use is
  //@[
  //   x = mpireduce(y,operation,root,comm)
  //@]
  //where @|y| is the current processes contribution to the
  //reduction operation, @|operation| is either @|'+','*','>','<'| for
  //an additive, multiplicative, max or min type reduction operations
  //respectively,
  //@|root| is the rank of the process that will retrieve the
  //result of the reduction operation, and @|comm| is the MPI
  //communicator handle.  If no communicator is provided,
  //then @|MPI_COMM_WORLD| is used by default.  Note that FreeMat does not
  //check to ensure that the reduction operation @|y| arguments
  //are all the same size across the various processes in the group.
  //Instead, you must make sure that each process passes the same
  //sized array to the @|mpireduce| operation.
  //!
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


  //!
  //@Module MPIINITIALIZED MPI Initialized Test
  //@@Usage
  //This function tests to see if MPI is already initialized.
  //The general syntax for its use is
  //@[
  //   x = mpiinitialized
  //@]
  //It returns a logical 1 if @|mpiinit| has already been called,
  //and a logical 0 otherwise.
  //@@Example
  //Here we call @|mpiinitialized| before and after a call to 
  //@|mpiinit|.
  //@<
  //mpiinitialized
  //mpiinit
  //mpiinitialized
  //@>
  //!
  ArrayVector MPIInitialized(int nargout, const ArrayVector& args) {
    int flag;
    MPI_Initialized(&flag);
    ArrayVector retval;
    retval.push_back(Array::logicalConstructor(flag));
    return retval;
  }

  //!
  //@Module MPIINIT MPI Initialize
  //@@Usage
  //This function initializes the MPI subsystem and joins
  //the current FreeMat process to the MPI environment.
  //The general syntax for its use is
  //@[
  //   mpiinit
  //@]
  //Note that @|mpiinit| must be called before any other
  //MPI routines (with the exception of @|mpiinitialized|), or
  //an MPI error will occur.
  //!
  ArrayVector MPIInit(int nargout, const ArrayVector& args) {
    int flag;
    MPI_Initialized(&flag);
    if (flag) {
      return ArrayVector();
    }
    MPI_Init(NULL,NULL);
    InitializeMPIWrap();
    return ArrayVector();
  }

  //!
  //@Module MPICOMMSPAWN MPI Communicator Spawn
  //@@Usage
  //This function uses MPI to spawn a process on a members of a group.
  //The full power of the underlying routine, @|MPI_Comm_spawn| is not
  //yet available via the @|mpicommspawn| routine in FreeMat.  The
  //general syntax for its use is
  //@[
  //  errcodes = mpicommspawn(command,argv,maxprocs,root,comm)
  //@]
  //where @|command| is the command to execute, @|argv| is a cell-array
  //of strings to pass as arguments to @|command|, @|maxprocs| is the
  //number of processes to spawn, @|root| is the node that will actually
  //do the process spawn, and @|comm| is the communicator to use.
  //If no communicator is specified, @|comm| defaults to @|MPI_COMM_SELF|.
  //If @|root| is not specified, it defaults to 0.  If @|maxprocs| is
  //not specified, it defaults to 1.  If @|argv| is not specified no
  //arguments are passed to the spawned processes.
  //!
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

  //!
  //@Module MPIINTERCOMMMERGE MPI Intercommunicator Merge
  //@@Usage
  //This routine merges the current process with an existing group.
  //The general syntax for its use is 
  //@[
  //  newcomm = mpiintercommmerge(comm,highflag)
  //@]
  //where @|comm| is the communicator we want to merge onto,
  //@|highflag| determines if our rank is at the high end of
  //ranks in the new communicator or at the bottom end, and
  //@|newcomm| is the handle to the new communicator.
  //!
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
  
  //!
  //@Module MPICOMMGETPARENT MPI Get Parent Communicator
  //@@Usage
  //This routine returns the communicator for the group that
  //spawned the current process.  Calling this routine for a 
  //process that was not spawned using @|mpicommspawn| will 
  //cause an error.  The general syntax for its use is
  //@[
  //  comm = mpicommgetparent
  //@]
  //!
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

  //!
  //@Module MPIFINALIZE MPI Finalize
  //This routine will shut down the MPI interface.  Once called,
  //no more MPI calls can be made (except for @|mpiinitialized|).
  //The syntax for its use is
  //@[
  //  mpifinalize
  //@]
  //!
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
  }
}

#endif
