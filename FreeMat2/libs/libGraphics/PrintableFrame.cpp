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

#include <wx/image.h>
#include <wx/dcps.h>
#include <wx/textfile.h>
#include <wx/filename.h>

#include "PrintableFrame.hpp"
#include "Exception.hpp"

namespace FreeMat {

  PrintableFrame::PrintableFrame(const wxString& title, const wxSize& size) 
    : wxFrame((wxFrame*) NULL, -1, title, wxDefaultPosition,
	      size)
  {
  }

  PrintableFrame::PrintableFrame() 
    : wxFrame()
  {
  }

  void PrintableFrame::printFrame(const char* filename) {
    wxFileName fName(filename);
    wxString extensionCode(fName.GetExt());
    
    if ((extensionCode.CmpNoCase("eps")==0) || 
	(extensionCode.CmpNoCase("ps")==0))
      printFrameToPostScript(filename);
    else
      printFrameToBitmap(filename);
  }

  void PrintableFrame::printFrameToBitmap(const char* filename) {
    int client_width;
    int client_height;
    GetClientSize(&client_width, &client_height);
    wxBitmap bmp(client_width,client_height);
    wxMemoryDC memDC;
    memDC.SelectObject(bmp);
    memDC.SetBrush( *wxLIGHT_GREY_BRUSH);
    memDC.DrawRectangle(0,0,client_width,client_height);
    DrawMe(memDC);
    wxImage img(bmp.ConvertToImage());
    if (!img.SaveFile(filename))
      throw Exception("Unrecognized or unsupported filename");
  }

  void PrintableFrame::printFrameToPostScript(const char* filename) {
    wxPrintData pData;
    pData.SetFilename(filename);
    pData.SetPrintMode(wxPRINT_MODE_FILE);
    pData.SetPaperId(wxPAPER_NONE);
    wxPostScriptDC pDC(pData);
    pDC.SetResolution(144);
    pDC.StartDoc(filename);
    pDC.StartPage();
    DrawMe(pDC);
    pDC.EndPage();
    pDC.EndDoc();
    int client_width;
    int client_height;
    GetClientSize(&client_width, &client_height);
    int minY = pDC.LogicalToDeviceY(0);
    int maxY = pDC.LogicalToDeviceY(client_height);
    int minX = pDC.LogicalToDeviceX(0);
    int maxX = pDC.LogicalToDeviceX(client_width);
    if (minY > maxY) {
      int tmp;
      tmp = minY;
      minY = maxY;
      maxY = tmp;
    }
    if (minX > maxX) {
      int tmp;
      tmp = minX;
      minX = maxX;
      maxX = tmp;
    }
    wxTextFile fp;
    if (!fp.Open(filename)) return;
    wxString bbox;
    fp.RemoveLine(5);
    bbox.Printf("%%%%BoundingBox: %d %d %d %d",
		minX/2, minY/2, maxX/2, maxY/2);
    fp.InsertLine(bbox,5);
    fp.Write();
  }
}
