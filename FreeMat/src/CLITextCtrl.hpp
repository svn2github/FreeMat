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

#ifndef __CLITextCtrl_h__
#define __CLITextCtrl_h__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "CLIBuffer.hpp"

#define MAX_HISTORY 100

WX_DECLARE_OBJARRAY(wxKeyEvent, wxKeyArray);

class CLITextCtrl : public wxTextCtrl 
{
 public:
  /**
   * Constructor
   */
  CLITextCtrl(wxWindow* parent, wxWindowID id);

  /**
   * Destructor
   */
  ~CLITextCtrl();

  /**
   * Process key codes.
   */
  void OnChar(wxKeyEvent& event);

  /**
   * Update our tally of the number
   * of lines (only if our current count is
   * not valid).
   */
  void RefreshLineCount();

  /**
   * Process a command
   */
  void DoCommand(wxString cmd);

  void PutPrompt();

  /**
   * Keystroke commands
   */
  void InsertKey(wxChar key);
  void NewLine();
  void HomeCommand();
  void EndCommand();
  void Backspace();
  void Delete();
  void DeleteLine();
  void SetCommand(wxString cmd);
  void RecallPreviousCommand();
  void RecallNextCommand();
  void outputMessage(const char *);
  void warningMessage(const char *);
  void errorMessage(const char *);
  bool testGetLineAvailable();
  void IssueGetLineRequest(const char *);
  const char* RetrieveGetLineResponse();

  protected:
  DECLARE_EVENT_TABLE();

 private:
  int LastNewLine();

  CLIBuffer *buffer;
  wxString CommandHistory[MAX_HISTORY];
  wxArrayString GetLineResponses;
  wxKeyArray keys;
  wxFont font;
  int CommandPointer;
  int CommandChooser;
  int CommandCount;

  bool RecalledCommand;
  int lineCount;
  bool lineCountValid;
  int promptLength;
  bool getLinePending;
};

#endif
