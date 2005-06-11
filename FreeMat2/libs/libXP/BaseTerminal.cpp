#include "BaseTerminal.hpp"
#include <qapplication.h>
#include <iostream>
#include <qeventloop.h>

std::string TranslateString(std::string x) {
  std::string y(x);
  int n;
  n = 0;
  while (n<y.size()) {
    if (y[n] == '\n') 
      y.insert(n++,"\r");
    n++;
  }
  return y;
}

void BaseTerminal::outputMessage(const char* msg) {
  std::string msg2(TranslateString(msg));
  OutputRawString(msg2);
}

void BaseTerminal::errorMessage(const char* msg) {
  std::string msg2(TranslateString(msg));
  OutputRawString("Error: " + msg2 + "\r\n");
}

void BaseTerminal::warningMessage(const char* msg) {
  std::string msg2(TranslateString(msg));
  OutputRawString("Warning: " + msg2 + "\r\n");
}

void BaseTerminal::ExecuteLine(const char * line) {
  enteredLines.push_back(line);
  ReplacePrompt("");
}

std::vector<std::string> BaseTerminal::GetCompletionList(const char *line, int word_end, 
					   std::string &matchString) {
  return GetCompletions(line,word_end,matchString);
}


char* BaseTerminal::getLine(const char* prompt) {
  fflush(stdout);
  ReplacePrompt(prompt);
  DisplayPrompt();
  while(enteredLines.empty())
    qApp->eventLoop()->processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMore);
    //    qApp->processOneEvent();
  //    qApp->processEvents();
  std::string theline(enteredLines.front());
  enteredLines.pop_front();
  char *cp;
  cp = strdup(theline.c_str());
  return cp;
}
