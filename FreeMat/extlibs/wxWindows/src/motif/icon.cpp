/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "icon.h"
#endif

#include "wx/icon.h"
#include "wx/window.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#include <X11/cursorfont.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)

/*
* Icons
*/

wxIcon::wxIcon()
{
}

// Create from XBM data
wxIcon::wxIcon(const char bits[], int width, int height)
{
    (void) Create((void*) bits, wxBITMAP_TYPE_XBM_DATA, width, height, 1);
}

// Create from XPM data
wxIcon::wxIcon(char **data)
{
    (void) Create((void*) data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);
}

wxIcon::wxIcon(const char **data)
{
    (void) Create((void*) data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);
}

wxIcon::wxIcon(const wxString& icon_file, long flags,
               int desiredWidth, int desiredHeight)
               
{
    LoadFile(icon_file, flags, desiredWidth, desiredHeight);
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
    wxIcon *icon = (wxIcon*)(&bmp);
    *this = *icon;
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(const wxString& filename, long type,
                      int desiredWidth, int desiredHeight)
{
    UnRef();
    
    m_refData = new wxBitmapRefData;
    
    wxBitmapHandler *handler = FindHandler(type);
    
    if ( handler )
        return handler->LoadFile(this, filename, type, desiredWidth, desiredHeight);
    else
        return FALSE;
}

