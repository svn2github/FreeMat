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

#ifndef __SessionFrame_h__
#define __SessionFrame_h__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxCLI2.hpp"
//#include "CLITextCtrl.hpp"

class SessionFrame : public wxFrame
{
 public:
  /**
   * Constructor.  Creates a new SessionFrame.
   */
  SessionFrame() {};
  SessionFrame(App *tMain, const wxChar* title, 
	       int xpos, int ypos, int width, int height);

  /**
   * Destructor
   */
  ~SessionFrame();

  /**
   * Processes menu events.
   */
  void OnOpen(wxCommandEvent &event);
  void OnSave(wxCommandEvent &event);
  void OnQuit(wxCommandEvent &event);
  void OnFont(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
  
  wxCLI *getTextControl();

 protected:
  DECLARE_CLASS(SessionFrame)
  DECLARE_EVENT_TABLE()
 private:
  App *mainApp;
  /**
   * Member controls.
   */
  wxCLI *m_pTextCtrl;

  /**
   * Command types.
   */
  enum
    {
      MENU_FILE_OPEN = 1000,
      MENU_FILE_SAVE,
      MENU_FILE_QUIT,
      MENU_EDIT_FONT,
      MENU_INFO_ABOUT
    };
};

#endif
