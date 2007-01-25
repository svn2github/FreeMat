/*
 * Copyright (c) 2002-2007 Samit Basu
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
#include <QtNetwork>
#include "Array.hpp"


// Here is the idea behind FreeMat's RPC service.
//
//  1. It is enabled and controlled via functions
//  2. It acts as an independant entity - driven by the main event loop
//  3. It supports simple Get/Put operations for arrays.
//  4. It is more of a peer-to-peer service than a centralized one
//
// Here is the mock-up:
//
// rpccontrol enable on queue 10 port 3254 % turn on our RPC service
//                                         % Allow a queue depth of 10
//                                         % set our RPC port to 3254
// recon = rpcreg('192.168.0.100:2950')    % returns an integer ID for the remote process
// rpcping(recon)                          % query the state of the remote RPC service
// rpcput(recon,A)                         % throws an exception if the put fails
// cnt = rpcpeek(recon)                    % how many messages are available from recon?
// A = rpcget(recon)                       % retrieve next message from recon
//

class RemoteAddress {
public:
  QString hostname;
  unsigned short port;
  RemoteAddress(const char *name, unsigned int portnum) : hostname(name), port(portnum) {}
};

// A slot is a place to store a received array, or the fact than an error occured 
// trying to receive/decode it

HandleList<RemoteAddress*> raHandles;
QList<Slot*> slots;

ArrayVector RPCRegFunction(int nargout, const ArrayVector& args) {
  if (arg.size() != 2) 
    throw Exception("rpcreg requires two arguments - the ip address and port to connect to");
  const char *host = ArrayToString(arg[0]);
  unsigned int port = ArrayToInt32(arg[1]);
  // Get a socket to test out 
  return ArrayVector() << 
    Array::uint32Constructor(raHandles.assignHandle(new RemoteAddress(host,port)));
}

ArrayVector RPCPutFunction(int nargout, const ArrayVector& args) {
  
}

// Must be started in the main thread
ArrayVector RPCStartFunction(int nargout, const ArrayVector& args) {
  if (arg.size() != 1)
    throw Exception("rpcstart requires a port number to listen in");
  uint16 portnum = (uint16) ArrayToInt32(arg[0]);
  RPCMgr *mgr = new RPCMgr;
  if (!mgr->open(portnum)) {
    delte mgr;
    throw Exception(string("unable to start rpc service on port ") + 
		    ArrayToInt32(arg[0]));
  }
}

bool RPCMgr::open(uint16 portnum) {
  tcpServer = new QTcpServer(this)
  if (!tcpServer->listen(QHostAddress::Any,portnum)) {
    delete tcpServer;
    return false;
  }
  connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void RPCMgr::newConnection() {
  Slot *dest = new Slot;
  slots.push_back(dest);
  new RPCClient(tcpServer->nextPendingConnection(),dest);
}

RPCClient::RPCClient(QTcpSocket *sock, Slot *dst) {
  m_sock = sock;
  connect(m_sock, SIGNAL(disconnected()), m_sock, SLOT(deleteLater()));
  connect(m_sock, SIGNAL(disconnected()), this, SLOT(deleteLater()));
  connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)),
	  this, SLOT(error(QAbstractSocket::SocketError)));
  connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
  blockSize = 0;
  m_dest = dst;
}

void RPCClient::fail(QString errMsg) {
  m_dest->slotFilled = true;
  m_dest->success = false;
  m_dest->errMsg = errMsg;
}

void RPCClient::error(QAbstractSocket::SocketError err) {
  fail(m_sock->errorString());
}

template <class T>
void getArray(int N, T* c, QDataStream &in) {
  for (int i=0;i<N;i++)
    out >> c[i];
}

template <class T>
void getSparseArray(int cols, T** c, QDataStream &in) {
  for (int i=0;i<cols;i++) {
    T len;
    in >> len;
    c[i] = new T[len];
    getArray(len,c[i],in);
  }
}

template <class T>
void putArray(int N, const T* c, QDataStream &out) {
  for (int i=0;i<N;i++)
    out << c[i];
}

template <class T>
void putSparseArray(int cols, const T** c, QDataStream &out) {
  for (int i=0;i<cols;i++) {
    out << 1+c[i][0];
    for (int j=0;j<1+c[i][0];j++)
      out << c[i][j];
  }
}

void getArrayFromQDS(QDataStream &in, Array& dat) {
  Class dclass;
  bool sparseFlag;
  Dimensions dims;
  uint8 dimCount;
  in >> (uint8) dclass;
  in >> (uint8) sparseFlag;
  in >> dimCount;
  for (int i=0;i<dimCount;i++) {
    uint32 dimVal;
    in >> dimVal;
    dims.setDimensionLength(i,dimVal);
  }
  int elCount(dims.getElementCount());
  if (elCount == 0) {
    dat = Array(dclass,dims,NULL);
    return;
  }
  switch(dclass) {
  case FM_CELL_ARRAY: {
    Array *dp = new Array[elCount];
    for (int i=0;i<elCount;i++)
      getArrayFromQDS(in,dp[i]);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_STRUCT_ARRAY: {
    rvstring fnames;
    int ncount;
    in >> (quint32) ncount;
    int i;
    for (i=0;i<ncount;i++) {
      char *dp;
      in >> dp;
      fnames.push_back(dp);
      delete dp;
    }
    Array *dp = new Array[elCount*ncount];
    for (i=0;i<elCount*ncount;i++)
      getArrayFromQDS(in,dp[i]);
    dat = Array(dclass,dims,dp,false,fnames);
    return;
  }
  case FM_LOGICAL: {
    logical *dp = (logical*) Malloc(sizeof(logical)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_STRING:
  case FM_UINT8: {
    uint8 *dp = (uint8*) Malloc(sizeof(uint8)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT8: {
    int8 *dp =  (int8*) Malloc(sizeof(int8)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_UINT16: {
    uint16 *dp = (uint16*) Malloc(sizeof(uint16)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT16: {
    int16 *dp = (int16*) Malloc(sizeof(int16)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_UINT32: {
    uint32 *dp = (uint32*) Malloc(sizeof(uint32)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_UINT64: {
    uint64 *dp = (uint64*) Malloc(sizeof(uint64)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT64: {
    uint64 *dp = (uint64*) Malloc(sizeof(uint64)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT32: {
    if (!sparseflag) {
      int32 *dp = (int32*) Malloc(sizeof(int32)*elCount);
      getArray(elCount, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      int32 **dp = new int32*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_FLOAT: {
    if (!sparseflag) {
      float *dp =  (float*) Malloc(sizeof(float)*elCount);
      getArray(elCount, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      float **dp = new float*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_DOUBLE: {
    if (!sparseflag) {
      double *dp = (double*) Malloc(sizeof(double)*elCount);
      getArray(elCount, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      double **dp = new double*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_COMPLEX: {
    if (!sparseflag) {
      float *dp = (float*) Malloc(sizeof(float)*elCount*2);
      getArray(elCount*2, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      float **dp = new float*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_DCOMPLEX: {
    if (!sparseflag) {
      double *dp = (double*) Malloc(sizeof(double)*elCount*2);
      getArray(elCount*2, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      double **dp = new double*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  }  
}

void putArrayToQDS(QDataStream &out, const Array& dat) {
  out << (uint8) dat.dataClass();
  out << (uint8) dat.sparse();
  out << (uint8) dat.dimensions().getLength();
  for (int i=0;i<dat.dimensions().getLength();i++)
    out << (uint32) dim.getDimensionLength(i);
  if (dat.isEmpty()) return;
  switch(dclass) {
  case FM_CELL_ARRAY: {
    const Array *dp=((const Array *) dat.getDataPointer());
    for (int i=0;i<elCount;i++)
      putArrayToQDS(out,dp[i]);
    return;
  }
  case FM_STRUCT_ARRAY: {
    rvstring fnames(dat.fieldNames());
    int ncount(fnames.size());
    out << (quint32) ncount;
    int i;
    for (i=0;i<ncount;i++)
      out << fnames.at(i).c_str();
    const Array *dp=((const Array *) dat.getDataPointer());
    for (i=0;i<elCount*ncount;i++)
      putArrayToQDS(out,dp[i]);
    return;
  }
  case FM_LOGICAL:
    putArray(elCount,(const logical *)dat.getDataPointer(),out);
    return;
  case FM_STRING:
  case FM_UINT8: 
    putArray(elCount,(const uint8 *)dat.getDataPointer(),out);
    return;
  case FM_UINT16:
    putArray(elCount,(const uint16 *)dat.getDataPointer(),out);
    return;
  case FM_UINT32: 
    putArray(elCount, (const uint32 *)dat.getDataPointer(),out);
    return;
  case FM_UINT64: 
    putArray(elCount, (const uint64 *)dat.getDataPointer(),out);
    return;    
  case FM_INT8: 
    putArray(elCount, (const int8 *)dat.getDataPointer(),out);
    return;
  case FM_INT16:
    putArray(elCount, (const int16 *)dat.getDataPointer(),out);
    return;
  case FM_INT32: 
    if (!dat.sparse()) 
      putArray(elCount, (const int32 *)dat.getDataPointer(),out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const int32 **) dat.getSparseDataPointer(), out);
    return;
  case FM_INT64: 
    putArray(elCount, (const int64 *)dat.getDataPointer(), out);
    return;
  case FM_FLOAT: 
    if (!dat.sparse()) 
      putArray(elCount, (const float *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const float **) dat.getSparseDataPointer(), out);
    return;
  case FM_DOUBLE: 
    if (!dat.sparse()) 
      putArray(elCount, (const double *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const double **) dat.getSparseDataPointer(), out);
    return;
  case FM_COMPLEX: 
    if (!dat.sparse()) 
      putArray(elCount*2, (const float *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const float **) dat.getSparseDataPointer(), out);
    return;
  case FM_DCOMPLEX: 
    if (!dat.sparse()) 
      putArray(elCount*2, (const double *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const double **) dat.getSparseDataPointer(), out);
    return;
  }
}

void RPCClient::readData() {
  QDataStream in(m_sock);
  in.setVersion(QDataStream::Qt_4_2);
  if (blockSize == 0) {
    if (m_sock->bytesAvailable() < (int) sizeof(quint64))
      return;
    in >> blockSize;
  }
  if (m_sock->bytesAvailable() < blockSize)
    return;
  // Read the magic number
  quint32 magic;
  in >> magic;
  if (magic != 0xFEEDADAD) {
    fail("Invalid data received from sender - magic number mismatch");
    return;
  }
  // Read the UUID of the sender
  in >> m_dest->senderID;
  // Read the packet type
  quint8 packet_type;
  in >> packet_type;
  // The following packet types are recognized.
  //   0 - data packet - payload is an array of data
  //   1 - id packet   - payload is empty.  Please reply with an id packet.
  if (packet_type == RPC_DATA_PACKET) 
    getArrayFromQDS(in,m_dest->value);
  m_dest->slotFilled = true;
  m_dest->success = true;
}

