#include "SocketCB.hpp"

SocketCB::SocketCB(cb acb) {
  m_cb = acb;
}

SocketCB::~SocketCB() {
}

void SocketCB::activated(int) {
  m_cb();
}
