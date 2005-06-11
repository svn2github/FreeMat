#ifndef __BaseTerminal_hpp__
#define __BaseTerminal_hpp__

#include <list>
#include <string>
#include "KeyManager.hpp"
#include "Interface.hpp"

using namespace FreeMat;
//! 
//BaseTerminal is really the link between the Interface and
//the KeyManager.  It provides common functionality to all
//Terminal types.  An actual (concrete) terminal must implement
//the OutputRawString function and the Move/Clear functions.
class BaseTerminal : public KeyManager, public Interface {
    std::list<std::string> enteredLines;  
public:
  BaseTerminal() : KeyManager(), Interface() {}
  virtual ~BaseTerminal() {}
  virtual char* getLine(const char* prompt);
  virtual void outputMessage(const char* msg);
  virtual void errorMessage(const char* msg);
  virtual void warningMessage(const char* msg);
  virtual void ExecuteLine(const char * line);
  virtual void RestoreOriginalMode() {};
  virtual void SetRawMode() {};
  virtual void Initialize() {};
  virtual void ResizeEvent() {};
  std::vector<std::string> GetCompletionList(const char *line, int word_end, 
					     std::string &matchString);
};

#endif
