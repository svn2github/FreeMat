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

#include "CLITextCtrl.hpp"
#include <iostream>
#include "Command.hpp"
#include "CLIThread.hpp"
#include "Array.hpp"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxKeyArray);

bool dataReady;

CLITextCtrl::CLITextCtrl(wxWindow* parent, wxWindowID id)
  : wxTextCtrl(parent, id, wxString(""),
				wxDefaultPosition,wxDefaultSize,
				wxTE_MULTILINE)
{
  buffer = new CLIBuffer(1000);
  buffer->Reset();
  lineCountValid = false;
  SetInsertionPointEnd();
  CommandPointer = 0;
  CommandCount = 0;
  RecalledCommand = false;
  font = wxFont(10, wxMODERN,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  SetFont(font);
  getLinePending = false;
  promptLength = 0;
}

CLITextCtrl::~CLITextCtrl()
{
}

void CLITextCtrl::warningMessage(const char *msg) {
  AppendText(msg);
}

void CLITextCtrl::errorMessage(const char *msg) {
  AppendText(msg);
}

void CLITextCtrl::outputMessage(const char *msg) {
  AppendText(msg);
}

void CLITextCtrl::PutPrompt() {
  //  AppendText("-> ");
}

bool CLITextCtrl::testGetLineAvailable() {
  return (GetLineResponses.GetCount() > 0);
}

void CLITextCtrl::IssueGetLineRequest(const char * msg) {
  AppendText(msg);
  promptLength = strlen(msg);
  getLinePending = true;
  // Clear out the key queue...
  bool keepRunning;
  keepRunning = true;
  while (!keys.IsEmpty() && keepRunning) {
    OnChar(keys.Item(0));
    keepRunning = (keys.Item(0).GetKeyCode() != WXK_RETURN);
    keys.RemoveAt(0,1);
  }
}

const char * CLITextCtrl::RetrieveGetLineResponse() {
  const char * res;

  res = strdup(GetLineResponses[0].c_str());
  GetLineResponses.Remove(0,1);
  return res;
}


void CLITextCtrl::DoCommand(wxString command) {
  int i;

  // Command pointer points to the next available slot
  // Stick the command into the slot (provided this is not a 
  // recalled command)
  if (!RecalledCommand && command.Length()) {
    CommandHistory[CommandPointer] = command;
    CommandChooser = CommandPointer;
    // Update the count, but don't allow the count to exceed
    // MAX_HISTORY
    CommandCount++;
    if (CommandCount > MAX_HISTORY) CommandCount = MAX_HISTORY;
    // Move the Command pointer
    CommandPointer++;
    CommandPointer %= MAX_HISTORY;
//      cout << "Command count = " << CommandCount << "\n";
//      for (i=0;i<CommandCount;i++)
    //        cout << "History[" << i << "] = " << CommandHistory[i] << "\n";
  } else 
    CommandChooser = CommandPointer;
  command.Append('\n');

  FreeMat::Command *cmd;
  cmd = new FreeMat::Command(FreeMat::CMD_GUIGetLineAcq,
			     FreeMat::Array::stringConstructor(command.c_str()));
  FreeMat::PostGUIReply(cmd);

  //  GetLineResponses.Add(command);
  dataReady = true;
  getLinePending = false;
}

void CLITextCtrl::RefreshLineCount() {
  if (!lineCountValid) {
    lineCount = GetNumberOfLines();
    lineCountValid = true;
  }
}
  
void CLITextCtrl::OnChar(wxKeyEvent& event)
{
  int key;
  bool altDown;
  bool shiftDown;
  bool ctrlDown;
  bool metaDown;

  key = (int) event.GetKeyCode();
  if (!getLinePending) {
    if ((key != WXK_UP) && (key != WXK_DOWN) && (key != WXK_BACK) &&
	(key != WXK_DELETE) && (key != WXK_LEFT) && (key != WXK_RIGHT)
	&& (key != WXK_END) && (key != WXK_HOME)) {
      keys.Add(event);
      event.Skip(); 
    }
    return;
  }
  //  std::cout << "Key = " << key << "\n";
  altDown = event.AltDown();
  shiftDown = event.ShiftDown();
  ctrlDown = event.ControlDown();
  metaDown = event.ControlDown();

  // Check for normal characters
  if (key == WXK_RETURN)
    NewLine();
  else if (key == WXK_END || (ctrlDown && key == 5))
    EndCommand();
  else if (key == WXK_HOME || (ctrlDown && key == 1))
    HomeCommand();
  else if (key == WXK_BACK)
    Backspace();
  else if (key == WXK_DELETE || (ctrlDown && key == 4))
    Delete();
  else if (ctrlDown && key == 11)
    DeleteLine();
  else if (key == WXK_LEFT || key == WXK_RIGHT)
    event.Skip();
  else if (key == WXK_UP)
    RecallPreviousCommand();
  else if (key == WXK_DOWN)
    RecallNextCommand();
  else if (!metaDown && !ctrlDown && !altDown) 
    InsertKey(key);
  else 
    event.Skip();
}

void CLITextCtrl::InsertKey(wxChar key) {
  int pos;
  long row, col;
  int lastnewline;
  
  if (key >= WXK_SPACE && key <WXK_DELETE) {   
    pos = GetInsertionPoint();
    lastnewline = LastNewLine();
    if ((pos < lastnewline) ||
	((pos - lastnewline) < (promptLength+1))) {
      SetInsertionPointEnd();
      AppendText((wxChar) key);
      buffer->AppendChar(key);
      RecalledCommand = false;
    } else {
      WriteText((wxChar) key);
      buffer->InsertChar(pos-lastnewline-promptLength-1, key);
      RecalledCommand = false;
      SetInsertionPoint(pos+1);
    }
  }
}

void CLITextCtrl::NewLine() {
  // Move us to the end
  SetInsertionPointEnd();
  WriteText((wxChar) '\n');
  //  AppendText((wxChar)WXK_RETURN);
  //  AppendText(DoCommand(buffer->GetContents()));
  DoCommand(buffer->GetContents());
  //  PutPrompt();
  buffer->Reset();
  lineCountValid = false;
}

void CLITextCtrl::HomeCommand() {
  SetInsertionPoint(LastNewLine()+promptLength+1);
}

void CLITextCtrl::EndCommand() {
  SetInsertionPointEnd();
}

int CLITextCtrl::LastNewLine() {
  // Get the contents of the control
  wxString text = GetValue();
  // Find the last newline
  return (text.Find('\n',true));
}

void CLITextCtrl::Backspace() {
  int pos, lastnewline;

  pos = GetInsertionPoint();
  lastnewline = LastNewLine();
  if (pos < lastnewline) return;
  if ((pos - lastnewline) < (promptLength+2)) return;
  Remove(pos-1,pos);
  buffer->DeleteChar(pos-lastnewline-promptLength-2);
  RecalledCommand = false;
  SetInsertionPoint(pos-1);
}

void CLITextCtrl::Delete() {
  int pos, lastnewline;

  pos = GetInsertionPoint();
  lastnewline = LastNewLine();
  if (pos < lastnewline) return;
  if ((pos - lastnewline) < (promptLength+1)) return;
  if (pos == GetLastPosition()) return;
  Remove(pos,pos+1);
  buffer->DeleteChar(pos-lastnewline-promptLength-1);
  RecalledCommand = false;
  SetInsertionPoint(pos);
}

void CLITextCtrl::DeleteLine() {
  SetCommand(wxString(""));
  RecalledCommand = false;
}

void CLITextCtrl::SetCommand(wxString cmd) {
  int startpos;

  startpos = LastNewLine();
  Remove(startpos+promptLength+1,GetLastPosition());
  AppendText(cmd);
  buffer->SetContents(cmd);
}

void CLITextCtrl::RecallPreviousCommand() {
  if (CommandCount > 0) {
    SetCommand(CommandHistory[CommandChooser]);
    CommandChooser += (CommandCount-1);
    CommandChooser %= CommandCount;
    RecalledCommand = true;
  }
}

void CLITextCtrl::RecallNextCommand() {
  if (CommandCount > 0) {
    CommandChooser++;
    CommandChooser %= CommandCount;
    SetCommand(CommandHistory[CommandChooser]);
    RecalledCommand = true;
  }
}


BEGIN_EVENT_TABLE(CLITextCtrl, wxTextCtrl)
  EVT_CHAR(CLITextCtrl::OnChar)
END_EVENT_TABLE()
