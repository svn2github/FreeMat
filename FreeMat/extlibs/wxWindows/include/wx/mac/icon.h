/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:     wxIcon class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "icon.h"
#endif

#include "wx/bitmap.h"

// Icon
class WXDLLEXPORT wxIcon: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxIcon)

public:
  wxIcon();

  // Copy constructors
  wxIcon(const wxIcon& icon)
      : wxBitmap()
      { Ref(icon); }

  wxIcon(const char **data);
  wxIcon(char **data);
  wxIcon(const char bits[], int width , int height );
  wxIcon(const wxString& name, int flags = wxBITMAP_TYPE_ICON_RESOURCE,
    int desiredWidth = -1, int desiredHeight = -1);
  ~wxIcon();

  bool LoadFile(const wxString& name, wxBitmapType flags /* = wxBITMAP_TYPE_ICON_RESOURCE */ ,
      int desiredWidth /* = -1 */ , int desiredHeight = -1);
  bool LoadFile(const wxString& name ,wxBitmapType flags = wxBITMAP_TYPE_ICON_RESOURCE )
  	{ return LoadFile( name , flags , -1 , -1 ) ; } 

  inline wxIcon& operator = (const wxIcon& icon) { if (*this == icon) return (*this); Ref(icon); return *this; }
  inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
  inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }
  
  // create from bitmap (which should have a mask unless it's monochrome):
  // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
  // ctors, assignment operators...), but it's ok to have such function
  void CopyFromBitmap(const wxBitmap& bmp);
};

/*
class WXDLLEXPORT wxICONFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxICONFileHandler)
public:
  inline wxICONFileHandler()
  {
	m_name = "ICO icon file";
	m_extension = "ico";
	m_type = wxBITMAP_TYPE_ICO;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);
};
*/

class WXDLLEXPORT wxICONResourceHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxICONResourceHandler)
public:
  inline wxICONResourceHandler()
  {
	m_name = "ICON resource";
	m_extension = "";
	m_type = wxBITMAP_TYPE_ICON_RESOURCE;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);

};

#endif
    // _WX_ICON_H_
