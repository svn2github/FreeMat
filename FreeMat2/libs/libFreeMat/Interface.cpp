#include "Interface.hpp"

namespace FreeMat {
  Interface::Interface() {
  }

  Interface::~Interface() {
  }

  void Interface::setMessageContext(const char* msg) {
    if (messageContext != NULL)
      free(messageContext);
    if (msg != NULL) 
      messageContext = strdup(msg);
    else
      messageContext = NULL;
  }

  void Interface::pushMessageContext() {
    if (messageContext != NULL)
      messageStack.push_back(messageContext);
    else
      messageStack.push_back("<Interactive>");
  }

  void Interface::popMessageContext() {
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
    std::vector<std::string> ret;
    if (messageContext != NULL)
      ret.push_back(messageContext);
    else
      ret.push_back("<Interactive>");
    return messageStack;
  }
}
