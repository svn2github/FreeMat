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
 * Recv an array via MPI:
 *  Arguments: A = MPIRecv(dest, tag, communicator)
 *  Throws an exception if thre is an error.
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
      dp = a.getDataPointer();
      for (int i=0;i<a.getLength();i++)
	total += getArrayByteFootPrint(dp[i]);
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
      dp = a.getDataPointer();
      for (int i=0;i<a.getLength()*fieldcount;i++)
	total += getArrayByteFootPrint(dp[i]);
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

// Pack an array into an MPI buffer using the MPI Pack functions
// We assume that the buffer is large enough, i.e. that it is 
// at least of size getArrayByteFootPrint(a) in size.
void packArrayMPI(Array &a, void *buffer, int bufsize, int *packpos, MPI_Comm comm) {
  Class dataClass(a.getDataClass());
  int idclass;
  int dimlength;
  idclass = encodeDataClassAsInteger(dataClass);
  MPI_Pack(&idclass,1,MPI_INT,buffer,bufsize,packpos,comm);
  dimlength = a.getDimension().getLength();
  MPI_Pack(&dummy,1,MPI_INT,buffer,bufsize,packpos,comm);
  for (int j=0;j<dimlength;j++) {
    int tmp;
    tmp = a.getDimensionLength(j);
    MPI_Pack(&tmp,1,MPI_INT,buffer,bufsize,packpos,comm);
  }
  if (a.isReferenceType()) {
    if (dataClass == FM_CELL_ARRAY) {
      Array *dp;
      dp = a.getDataPointer();
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
	MPI_Pack(fieldnames[i].c_str(),flen,MPI_CHAR,buffer,bufsize,packpos,comm);
      }
      Array *dp;
      dp = a.getDataPointer();
      for (int i=0;i<a.getLength()*fieldcnt;i++)
	packArrayMPI(dp[i],buffer,bufsize,packpos,comm);
    }
  } else {
    switch(dataClass) {
    case FM_LOGICAL:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_CHAR,buffer,bufsize,packpos,comm);
      break;
    case FM_UINT8:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_UNSIGNED_CHAR,buffer,bufsize,packpos,comm);
      break;
    case FM_INT8:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_CHAR,buffer,bufsize,packpos,comm);
      break;
    case FM_UINT16:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_UNSIGNED_SHORT,buffer,bufsize,packpos,comm);
      break;
    case FM_INT16:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_SHORT,buffer,bufsize,packpos,comm);
      break;
    case FM_UINT32:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_UNSIGNED,buffer,bufsize,packpos,comm);
      break;
    case FM_INT32:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_INT,buffer,bufsize,packpos,comm);
      break;
    case FM_FLOAT:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_FLOAT,buffer,bufsize,packpos,comm);
      break;
    case FM_DOUBLE:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_DOUBLE,buffer,bufsize,packpos,comm);
      break;
    case FM_COMPLEX:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_FLOAT,buffer,bufsize,packpos,comm);
      break;
    case FM_DCOMPLEX:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_DOUBLE,buffer,bufsize,packpos,comm);
      break;
    case FM_STRING:
      MPI_Pack(a.getDataPointer(),a.getLength(),MPI_CHAR,buffer,bufsize,packpos,comm);
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
    outDim(j) = tmp;
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
      dbuff = malloc(fieldnamelength+1);
      MPU_Unpack(buffer,bufsize,packpos,dbuff,fieldnamelength,MPI_CHAR,comm);
      dbuff[fieldnamelength] = 0;
      fieldnames.push_back(std::string(dbuff));
      free(dbuff);
    }
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
