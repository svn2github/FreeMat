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

#include "AppNoGUI.hpp"
#include "CLIThread.hpp"
#include <iostream>
#include <stdio.h>
#include "Command.hpp"
#include "Plot2D.hpp"
#include "ScalarImage.hpp"
#include "Exception.hpp"
#include <signal.h>
#include "TeclaInterface.hpp"

#include <wx/image.h>
#include <wx/splash.h>


#include "Array.hpp"
#include "VolView.hpp"
#include "Malloc.hpp"
#include "mpi.h"

using namespace FreeMat;


#ifdef F77_DUMMY_MAIN
#  ifdef __cplusplus
     extern "C"
#  endif
   int F77_DUMMY_MAIN() { return 1; }
#endif

IMPLEMENT_APP(AppNoGUI);

BEGIN_EVENT_TABLE(AppNoGUI, wxApp)
  EVT_GUI_COMMAND(-1, AppNoGUI::OnProcessCustom)
END_EVENT_TABLE()

wxString wxFindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
{
    wxString str;

    // Try appVariableName
    if (!appVariableName.IsEmpty())
    {
        str = wxGetenv(appVariableName);
        if (!str.IsEmpty())
            return str;
    }

#if defined(__WXMAC__) && !defined(__DARWIN__)
    // On Mac, the current directory is the relevant one when
    // the application starts.
    return cwd;
#endif

    if (wxIsAbsolutePath(argv0))
        return wxPathOnly(argv0);
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0;
        if (wxFileExists(str))
            return wxPathOnly(str);
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0);
    if (!str.IsEmpty())
        return wxPathOnly(str);

    // Failed
    return wxEmptyString;
}


bool AppNoGUI::OnInit()
{
  delete wxLog::SetActiveTarget(new wxLogStderr); // So dialog boxes aren't used
  MPI_Init(&argc, &argv);
  SetExitOnFrameDelete(false);
  // Initialize the plots array...
  for (int i=0;i<MAX_PLOTS;i++) {
    plots[i] = NULL;
    images[i] = NULL;
    volumes[i] = NULL;
  }
  currentPlot = -1;
  currentImage = -1;
  currentVolume = -1;
  ::wxInitAllImageHandlers();
  SetVendorName(wxT("FreeMat.org"));
  SetAppName(wxT("FreeMat")); 
  wxString path(wxFindAppPath(argv[0],wxGetCwd(),wxString("FREEMAT_HOME")));
  path += "/manual.zip";
  SetupHelp(path);
  wxBitmap bitmap;

  // Hack - on some windowing systems, not having a top window breaks
  // things....

  wxFrame *wf;
  wf = new wxFrame(NULL,-1,"FreeMat");
  SetTopWindow(wf);

  //Launch CLI thread...
  //Get an instance of the TeclaInterface
  t = new CLIThread(this,new TeclaInterface);
  t->Create();
  t->Run();
  // Need to disable signals...
  sigset_t st;
  sigfillset(&st);
  sigdelset(&st,SIGINT);
  pthread_sigmask(SIG_SETMASK,&st,NULL);
  return TRUE;
}

void AppNoGUI::Shutdown() {
  ExitMainLoop();  
}

int AppNoGUI::OnExit() {
  t->Wait();
  delete t;
  return 0;
}

void AppNoGUI::OnProcessCustom(wxCommandEvent& event) {
  ProcessGraphicsServerCommands(event);
  Command *cp = (Command *) event.GetClientData();
  switch(cp->cmdNum) {
  case CMD_SystemCapture: {
    wxString command(cp->data.getContentsAsCString());
    wxArrayString output;
    wxExecute(command,output);
    Array *dp = new Array[output.GetCount()];
    for (int k=0;k<output.GetCount();k++) {
      const char *wp = output[k].c_str();
      dp[k] = Array::stringConstructor(std::string(wp));
    }
    Dimensions dim(2);
    dim[0] = output.GetCount();
    dim[1] = 1;
    Array res(Array::Array(FM_CELL_ARRAY,dim,dp));
    Command *rp;
    rp = new Command(CMD_SystemCaptureAcq,res);
    PostGUIReply(rp);
  }
  }
  delete cp;
}

