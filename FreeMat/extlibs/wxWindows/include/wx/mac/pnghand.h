/////////////////////////////////////////////////////////////////////////////
// Name:        pnghand.h
// Purpose:     PNG bitmap handler
// Author:      Julian Smart
// Modified by: 
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Microsoft, Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "pnghand.h"
#endif

#ifndef _WX_PNGHAND_H_
#define _WX_PNGHAND_H_

class WXDLLEXPORT wxPNGFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxPNGFileHandler)
public:
  inline wxPNGFileHandler(void)
  {
	m_name = "PNG bitmap file";
	m_extension = "bmp";
	m_type = wxBITMAP_TYPE_PNG;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth, int desiredHeight);
  virtual bool SaveFile(const wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);
};

#endif
  // _WX_PNGHAND_H_

