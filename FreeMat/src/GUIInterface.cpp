// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "GUIInterface.hpp"
#include "Command.hpp"
#include <string.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

namespace FreeMat {

  static char* messageContext = NULL;
  
  void GUIInterface::outputMessage(const char* msg) {
    SendGUICommand(new Command(CMD_GUIOutputMessage,
			       Array::stringConstructor(msg)));
  }

  void GUIInterface::errorMessage(const char* msg) {
    SendGUICommand(new Command(CMD_GUIErrorMessage,
			       Array::stringConstructor(msg)));
  }

  void GUIInterface::warningMessage(const char* msg) {
    SendGUICommand(new Command(CMD_GUIWarningMessage,
			       Array::stringConstructor(msg)));
  }
  
  void GUIInterface::setMessageContext(const char* context) {
    if (messageContext != NULL)
      free(messageContext);
    if (context != NULL) 
      messageContext = strdup(context);
    else
      messageContext = NULL;
  }
  
  GUIInterface::GUIInterface() {
  }

  void GUIInterface::initialize(std::string path, Context *ctxt) {
    pathList = wxString(path.c_str());
    context = ctxt;
    rescanPath();
  }

  void GUIInterface::processFilename(wxString filename) {
    wxString path, name, ext;
    ::wxFileName::SplitPath(filename,&path,&name,&ext);
    FunctionDef *fdef;
    if (!context->lookupFunctionGlobally(std::string(name.c_str()),fdef)) {
      MFunctionDef *adef;
      adef = new MFunctionDef();
      adef->name = std::string(name.c_str());
      adef->fileName = std::string(filename.c_str());
      context->insertFunctionGlobally(adef);
      if (transientScan)
	transientFuncs.push_back(std::string(name.c_str()));
    }
  }

  void GUIInterface::rescanPath() {
    for (int i=0;i<transientFuncs.size();i++)
      context->deleteFunctionGlobally(transientFuncs[i]);
    // Scan the current directory...
    transientScan = false;
#ifndef WIN32
    wxStringTokenizer tkz(pathList,":");
#else
    wxStringTokenizer tkz(pathList,";");
#endif
    while (tkz.HasMoreTokens()) {
      wxString token = tkz.GetNextToken();
#ifndef WIN32
      wxString fname = wxFindFirstFile(token + "/*.m",wxFILE);
#else
      wxString fname = wxFindFirstFile(token + "\\*.m",wxFILE);
#endif
      while (!fname.IsEmpty()) {
	processFilename(fname);
	fname = wxFindNextFile();
      }
    }
    wxString fname = wxFindFirstFile("*.m",wxFILE);
    transientScan = true;
    while (!fname.IsEmpty()) {
      processFilename(fname);
      fname = wxFindNextFile();
    }
  }

  GUIInterface::~GUIInterface() {
  }

  int GUIInterface::getTerminalWidth() {
    SendGUICommand(new Command(CMD_GUIGetWidth));
    Command *cp;
    cp = GetGUIResponse();
    int ret;
    ret = cp->data.getContentsAsIntegerScalar();
    delete cp;
    return ret;
  }
  
  char* GUIInterface::getLine(const char* prompt) {
    SendGUICommand(new Command(CMD_GUIGetLine,
			       Array::stringConstructor(prompt)));
    Command *cp;
    cp = GetGUIResponse();
    char *ret;
    ret = cp->data.getContentsAsCString();
    delete cp;
    return ret;
  }
}
