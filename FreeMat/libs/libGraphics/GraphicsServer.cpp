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

#include "GraphicsServer.hpp"
#include "Command.hpp"
#include "Plot2D.hpp"
#include "ScalarImage.hpp"
#include "VolView.hpp"
#include "Exception.hpp"
#include "SessionFrame.hpp"
#include "CLIThread.hpp"

#include <wx/image.h>
#include <wx/splash.h>
#include <wx/wxhtml.h>
#include <wx/fs_zip.h>

using namespace FreeMat;

DEFINE_EVENT_TYPE(wxEVT_GUI_COMMAND)

void GraphicsServer::SetupHelp(wxString path) {
  wxFileSystem::AddHandler(new wxZipFSHandler);
  wxConfig::Get();
  help = new wxHtmlHelpController;
  help->AddBook(wxFileName(path));
}

void GraphicsServer::NewPlot() {
  // First search for an unused plot number
  int plotNum = 0;
  bool plotFree = false;
  while ((plotNum < MAX_PLOTS) && !plotFree) {
    plotFree = (plots[plotNum] == NULL);
    if (!plotFree) plotNum++;
  }
  if (!plotFree) {
    wxMessageBox("No more plot handles available!  Close some plots...");
  }
  plots[plotNum] = new Plot2D(this,plotNum);
  plots[plotNum]->Show();
  plots[plotNum]->Raise();
  plots[plotNum]->SetHoldFlag(false);
  currentPlot = plotNum;
}

void GraphicsServer::NewImage() {
  // First search for an unused image number
  int imageNum = 0;
  bool imageFree = false;
  while ((imageNum < MAX_PLOTS) && !imageFree) {
    imageFree = (images[imageNum] == NULL);
    if (!imageFree) imageNum++;
  }
  if (!imageFree) {
    wxMessageBox("No more image handles available!  Close some images...");
  }
  images[imageNum] = new ScalarImage(this,imageNum);
  images[imageNum]->Show();
  images[imageNum]->Raise();
  currentImage = imageNum;
}

void GraphicsServer::NewVolume() {
  // First search for an unused volume number
  int volumeNum = 0;
  bool volumeFree = false;
  while ((volumeNum < MAX_PLOTS) && !volumeFree) {
    volumeFree = (volumes[volumeNum] == NULL);
    if (!volumeFree) volumeNum++;
  }
  if (!volumeFree) {
    wxMessageBox("No more volume handles available!  Close some volumes...");
  }
  volumes[volumeNum] = new VolView(this,volumeNum);
  volumes[volumeNum]->Show();
  volumes[volumeNum]->Raise();
  currentVolume = volumeNum;
}

void GraphicsServer::SelectPlot(int plotnum) {
  if (plots[plotnum] == NULL) {
    plots[plotnum] = new Plot2D(this,plotnum);
  }
  plots[plotnum]->Show();
  plots[plotnum]->Raise();
  currentPlot = plotnum;
} 

void GraphicsServer::SelectImage(int imagenum) {
  if (images[imagenum] == NULL) {
    images[imagenum] = new ScalarImage(this,imagenum);
  }
  images[imagenum]->Show();
  images[imagenum]->Raise();
  currentImage = imagenum;
} 

void GraphicsServer::SelectVolume(int volumenum) {
  if (volumes[volumenum] == NULL) {
    volumes[volumenum] = new VolView(this,volumenum);
  }
  volumes[volumenum]->Show();
  volumes[volumenum]->Raise();
  currentVolume = volumenum;
} 

Plot2D* GraphicsServer::GetCurrentPlot() {
  if (currentPlot == -1)
    NewPlot();
  plots[currentPlot]->Raise();
  return plots[currentPlot];
}

ScalarImage* GraphicsServer::GetCurrentImage() {
  if (currentImage == -1)
    NewImage();
  images[currentImage]->Raise();
  return images[currentImage];
}

VolView* GraphicsServer::GetCurrentVolume() {
  if (currentVolume == -1)
    NewVolume();
  volumes[currentVolume]->Raise();
  return volumes[currentVolume];
}

void GraphicsServer::NotifyPlotClose(int figNum) {
  plots[figNum] = NULL;
  if (currentPlot == figNum)
    currentPlot = -1;
}

void GraphicsServer::NotifyImageClose(int figNum) {
  images[figNum] = NULL;
  if (currentImage == figNum)
    currentImage = -1;
}

void GraphicsServer::NotifyVolumeClose(int figNum) {
  volumes[figNum] = NULL;
  if (currentVolume == figNum)
    currentVolume = -1;
}

void GraphicsServer::ProcessGraphicsServerCommands(wxCommandEvent& event) {
  Command *cp = (Command *) event.GetClientData();
  Plot2D* f;
  ScalarImage* g;
  VolView* h;
  try {  
    switch(cp->cmdNum) {
    case CMD_Quit: 
      for (int i=0;i<MAX_PLOTS;i++) {
	if (plots[i] != NULL)
	  plots[i]->Destroy();
	if (images[i] != NULL)
	  images[i]->Destroy();
	if (volumes[i] != NULL)
	  volumes[i]->Destroy();
      }
      Shutdown();
      break;
    case CMD_PlotSetXAxisLabel:
      f = GetCurrentPlot();
      f->SetXLabel(cp->data.getContentsAsCString());
      break;
    case CMD_PlotSetYAxisLabel:
      f = GetCurrentPlot();
      f->SetYLabel(cp->data.getContentsAsCString());
      break;
    case CMD_PlotSetTitle:
      f = GetCurrentPlot();
      f->SetTitleText(cp->data.getContentsAsCString());
      break;
    case CMD_PlotSetGrid:
      f = GetCurrentPlot();
      if (cp->data.getContentsAsIntegerScalar())
	f->SetGrid(true);
      else
	f->SetGrid(false);
      break;
    case CMD_PlotSetHold:
      f = GetCurrentPlot();
      if (cp->data.getContentsAsIntegerScalar())
	f->SetHoldFlag(true);
      else
	f->SetHoldFlag(false);
      break;
    case CMD_PlotNew:
      NewPlot();
      Command *cmd;
      cmd = new Command(CMD_PlotNewAcq, Array::int32Constructor(currentPlot+1));
      PostGUIReply(cmd);
      break;
    case CMD_PlotUse:
      SelectPlot(cp->data.getContentsAsIntegerScalar());
      break;
    case CMD_PlotClose: {
      int action;
      action = cp->data.getContentsAsIntegerScalar();
      if (action == 0) {
	if (currentPlot != -1) {
	  f = GetCurrentPlot();
	  f->Close();
	  currentPlot = -1;
	}
      } else if (action == -1) {
	for (int i=0;i<MAX_PLOTS;i++)
	  if (plots[i] != NULL)
	    plots[i]->Close();
	currentPlot = -1;
      } else {
	if ((action < MAX_PLOTS) && (plots[action-1] != NULL))
	  plots[action-1]->Close();
	if (currentPlot == action-1)
	  currentPlot = -1;
      }
      break;
    }
    case CMD_PlotPrint:
      f = GetCurrentPlot();
      f->printFrame(cp->data.getContentsAsCString());
      break;
    case CMD_PlotStartSequence:
      f = GetCurrentPlot();
      f->StartSequence();
      break;
    case CMD_PlotStopSequence:
      f = GetCurrentPlot();
      f->StopSequence();
      break;
    case CMD_PlotAxisSet: 
      {
	double *dp;
	dp = (double*) cp->data.getDataPointer();
	f = GetCurrentPlot();
	f->SetAxes(dp[0],dp[1],dp[2],dp[3]);
	break;
      }
    case CMD_PlotAxisTight:
      f = GetCurrentPlot();
      f->SetAxesTight();
      break;
    case CMD_PlotAxisAuto:
      f = GetCurrentPlot();
      f->SetAxesAuto();
      break;
    case CMD_PlotAxisGetRequest:
      {
	Array retArr;
	retArr = Array::doubleVectorConstructor(4);
	double *dp;
	dp = (double*) retArr.getReadWriteDataPointer();
	f = GetCurrentPlot();
	f->GetAxes(dp[0],dp[1],dp[2],dp[3]);
	Command *cmd;
	cmd = new Command(CMD_PlotAxisGetAcq, retArr);
	PostGUIReply(cmd);
      }
      break;
    case CMD_ImageNew: {
      NewImage();
      Command *cmd;
      cmd = new Command(CMD_ImageNewAcq, Array::int32Constructor(currentImage+1));
      PostGUIReply(cmd);
      break;
    }
    case CMD_ImageUse:
      SelectImage(cp->data.getContentsAsIntegerScalar());
      break;
    case CMD_ImageClose: {
      int action;
      action = cp->data.getContentsAsIntegerScalar();
      if (action == 0) {
	if (currentImage != -1) {
	  g = GetCurrentImage();
	  g->Close();
	  currentImage = -1;
	}
      } else if (action == -1) {
	for (int i=0;i<MAX_PLOTS;i++)
	  if (images[i] != NULL)
	    images[i]->Close();
	currentImage = -1;
      } else {
	if ((action < MAX_PLOTS) && (images[action-1] != NULL))
	  images[action-1]->Close();
	if (currentImage == action-1)
	  currentImage = -1;
      }
      break;
    }
    case CMD_ImageSet:
      g = GetCurrentImage();
      g->SetImage(cp->data);
      break;
    case CMD_VolumeNew: {
      NewVolume();
      Command *cmd;
      cmd = new Command(CMD_VolumeNewAcq, Array::int32Constructor(currentVolume+1));
      PostGUIReply(cmd);
      break;
    }
    case CMD_VolumeUse:
      SelectVolume(cp->data.getContentsAsIntegerScalar());
      break;
    case CMD_VolumeClose: {
      int action;
      action = cp->data.getContentsAsIntegerScalar();
      if (action == 0) {
	if (currentVolume != -1) {
	  h = GetCurrentVolume();
	  h->Close();
	  currentVolume = -1;
	}
      } else if (action == -1) {
	for (int i=0;i<MAX_PLOTS;i++)
	  if (volumes[i] != NULL)
	    volumes[i]->Close();
	currentVolume = -1;
      } else {
	if ((action < MAX_PLOTS) && (volumes[action-1] != NULL))
	  volumes[action-1]->Close();
	if (currentVolume == action-1)
	  currentVolume = -1;
      }
      break;
    }
    case CMD_VolumeSet:
      h = GetCurrentVolume();
      h->SetVolume(cp->data);
      break;
    case CMD_ImageColormap:
      g = GetCurrentImage();
      g->SetColormap(cp->data);
      break;
    case CMD_ImagePrint:
      g = GetCurrentImage();
      g->printFrame(cp->data.getContentsAsCString());
      break;
    case CMD_ImageZoom:
      g = GetCurrentImage();
      g->Zoom(cp->data.getContentsAsDoubleScalar());
      break;
    case CMD_PlotAdd: {
      const Array* tm;
      f = GetCurrentPlot();
      tm = (const Array*) cp->data.getDataPointer();
      Array x, y, linestyle;
      const char *cline;
      x = tm[0];
      y = tm[1];
      linestyle = tm[2];
      cline = (const char *) linestyle.getDataPointer();
      f->AddPlot(DataSet2D(x,y,cline[0],cline[1],cline[2]));
      break;
    }
    case CMD_FilePick: {
      wxFileDialog fpick(NULL);
      fpick.ShowModal();
      Command *cmd;
      cmd = new Command(CMD_FilePickAcq, 
			Array::stringConstructor(fpick.GetPath().c_str()));
      PostGUIReply(cmd);
      break;
    }
    case CMD_Point: {
      g = GetCurrentImage();
      g->GetClick();
//       Array clickVal(g->GetClick());
//       Command *cmd;
//       cmd = new Command(CMD_PointAcq, clickVal);
//       PostGUIReply(cmd);
      break;
    }
    case CMD_HelpShow: {
      help->DisplayContents();
      break;
    }
    }
  } catch (Exception &e) {
  }
}
