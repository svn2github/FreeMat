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

#include "SessionFrame.hpp"
#include "Command.hpp"
#include <wx/fontdlg.h>
#include <iostream>
#include "App.hpp"

SessionFrame::SessionFrame(App* tMain, const wxChar *title, 
			   int xpos, int ypos, int width, int height)
  : wxFrame((wxFrame *)NULL,-1,title,wxPoint(xpos,ypos),wxSize(width,height))
{
  mainApp = tMain;
  m_pTextCtrl = new wxCLI(tMain, this);
  m_pTextCtrl->SetFocus();
  //  m_pTextCtrl = new CLITextCtrl(this,-1);
  //  m_pTextCtrl = new wxTextCtrl(this,-1,"",wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  //  m_pTextCtrl = new CLITrick(this);
  wxMenuBar *m_pMenuBar;
  wxMenu *m_pMenu;

  m_pMenuBar = new wxMenuBar();
  // File Menu
  m_pMenu = new wxMenu();
  m_pMenu->Append(MENU_FILE_OPEN, "&Open", "Opens a connection to a remote kernel");
  m_pMenu->Append(MENU_FILE_SAVE, "&Save", "Save the contents of the current session");
  m_pMenu->AppendSeparator();
  m_pMenu->Append(MENU_FILE_QUIT, "&Quit", "Quit the application (GUI only)");
  m_pMenuBar->Append(m_pMenu,"&File");
  // Edit Menu
  m_pMenu = new wxMenu();
  m_pMenu->Append(MENU_EDIT_FONT, "&Font", "Change the font used for display");
  m_pMenuBar->Append(m_pMenu,"&Edit");
  // About Menu
  m_pMenu = new wxMenu();
  m_pMenu->Append(MENU_INFO_ABOUT, "&About", "Shows information about the application");
  m_pMenuBar->Append(m_pMenu, "&Info");

  SetMenuBar(m_pMenuBar);
}

SessionFrame::~SessionFrame()
{
}

IMPLEMENT_DYNAMIC_CLASS(SessionFrame, wxFrame)

BEGIN_EVENT_TABLE(SessionFrame, wxFrame)
  EVT_MENU(MENU_FILE_OPEN, SessionFrame::OnOpen)
  EVT_MENU(MENU_FILE_SAVE, SessionFrame::OnSave)
  EVT_MENU(MENU_FILE_QUIT, SessionFrame::OnQuit)
  EVT_MENU(MENU_EDIT_FONT, SessionFrame::OnFont)
  EVT_MENU(MENU_INFO_ABOUT, SessionFrame::OnAbout)
END_EVENT_TABLE()

void SessionFrame::OnOpen(wxCommandEvent &event)
{
}

void SessionFrame::OnSave(wxCommandEvent &event)
{
  wxFileDialog *dlg;

  dlg = new wxFileDialog(this, "Save a text file",
			 "", "", "All files(*.*)|*.*|Text Files(*.txt)|*.txt",
			 wxSAVE, wxDefaultPosition);
  if (dlg->ShowModal() == wxID_OK)
    {
      //      m_pTextCtrl->SaveFile(dlg->GetPath());
      SetStatusText(dlg->GetFilename());
    }
  dlg->Destroy();
}

void SessionFrame::OnFont(wxCommandEvent &event)
{
  wxFontData fontData;
  wxFont font;
  wxColour colour;
  //  font = m_pTextCtrl->GetFont();
  fontData.SetInitialFont(font);
  //  colour = m_pTextCtrl->GetForegroundColour();
  fontData.SetColour(colour);
  fontData.SetShowHelp(true);
  wxFontDialog *dlg = new wxFontDialog(this, &fontData);
  if ( dlg->ShowModal() == wxID_OK )
    {
      fontData = dlg->GetFontData();
      font = fontData.GetChosenFont();
      m_pTextCtrl->SetFont(font);
      m_pTextCtrl->Refresh();
      //      m_pTextCtrl->SetFont(font);
      //      m_pTextCtrl->SetForegroundColour(fontData.GetColour());
    }
  dlg->Destroy();
}

void SessionFrame::OnQuit(wxCommandEvent &event)
{
  Close();
  mainApp->Shutdown();
}

void SessionFrame::OnAbout(wxCommandEvent &event) {
}

wxCLI* SessionFrame::getTextControl() {
  return m_pTextCtrl;
}
