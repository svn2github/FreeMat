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

#ifndef __GraphicsServer_hpp__
#define __GraphicsServer_hpp__

#include "Plot2D.hpp"
#include "ScalarImage.hpp"
#include "VolView.hpp"
#include "Command.hpp"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#define MAX_PLOTS 50

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_GUI_COMMAND, 7530)
END_DECLARE_EVENT_TYPES()

// it may also be convenient to define an event table macro for this event type
#define EVT_GUI_COMMAND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_GUI_COMMAND, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
    ),

class wxHtmlHelpController;

class GraphicsServer
{
public:
  virtual void SetupHelp(wxString path);
  virtual void ProcessGraphicsServerCommands(wxCommandEvent& event);
  virtual void NotifyPlotClose(int figNum);
  virtual void NotifyImageClose(int figNum);
  virtual void NotifyVolumeClose(int figNum);
  virtual void SelectPlot(int plotnum);
  virtual void SelectImage(int plotnum);
  virtual void SelectVolume(int plotnum);
  virtual void NewPlot();
  virtual void NewImage();
  virtual void NewVolume();
  virtual void Shutdown() = 0;
  virtual FreeMat::Plot2D* GetCurrentPlot();
  virtual FreeMat::ScalarImage* GetCurrentImage();  
  virtual FreeMat::VolView* GetCurrentVolume();
protected:
  FreeMat::Plot2D* plots[MAX_PLOTS];
  FreeMat::ScalarImage* images[MAX_PLOTS];
  FreeMat::VolView* volumes[MAX_PLOTS];
  wxHtmlHelpController *help;
  int currentPlot;
  int currentImage;
  int currentVolume;
};

#endif
