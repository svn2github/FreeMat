#include "Interface.hpp"

namespace FreeMat {
  Interface::Interface() {
  }

  Interface::~Interface() {
  }

  void Interface::setMessageContext(const char* msg) {
    if (msg != NULL)
      messageContext = std::string(msg);
    else
      messageContext = std::string("<Interactive>");;
  }

  void Interface::pushMessageContext() {
    messageStack.push_back(messageContext);
  }

  void Interface::popMessageContext() {
    messageContext = messageStack.back();
    messageStack.pop_back();
  }

  void Interface::clearMessageContextStackToDepth(int depth) {
    while (messageStack.size() > depth)
      messageStack.pop_back();      
  }

  int Interface::getMessageContextStackDepth() {
    return(messageStack.size());
  }

  void Interface::clearMessageContextStack() {
    messageStack.clear();
  }

  std::vector<std::string> Interface::getMessageContextStack() {
    // Add the current context to the stack
    return messageStack;
  }
}
