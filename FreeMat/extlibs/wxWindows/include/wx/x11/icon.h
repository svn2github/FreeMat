/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

#ifdef __GNUG__
#pragma interface "icon.h"
#endif

#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// wxIcon
//-----------------------------------------------------------------------------

class wxIcon: public wxBitmap
{
public:
    wxIcon();
    wxIcon( const wxIcon& icon);
    wxIcon( const char **bits, int width=-1, int height=-1 );

    // For compatibility with wxMSW where desired size is sometimes required to
    // distinguish between multiple icons in a resource.
    wxIcon( const wxString& filename, int type = wxBITMAP_TYPE_XPM, 
            int WXUNUSED(desiredWidth)=-1, int WXUNUSED(desiredHeight)=-1 ) :
        wxBitmap(filename, type)
    {
    }
    wxIcon( char **bits, int width=-1, int height=-1 );

    wxIcon& operator = (const wxIcon& icon);
    inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
    inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }

    // create from bitmap (which should have a mask unless it's monochrome):
    // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
    // ctors, assignment operators...), but it's ok to have such function
    void CopyFromBitmap(const wxBitmap& bmp);
  
private:
    DECLARE_DYNAMIC_CLASS(wxIcon)
};


#endif
// _WX_ICON_H_
