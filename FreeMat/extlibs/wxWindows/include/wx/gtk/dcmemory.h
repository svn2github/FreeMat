/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDCMEMORYH__
#define __GTKDCMEMORYH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMemoryDC;

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

class wxMemoryDC : public wxWindowDC
{
public:
    wxMemoryDC();
    wxMemoryDC( wxDC *dc ); // Create compatible DC
    ~wxMemoryDC();
    virtual void SelectObject( const wxBitmap& bitmap );
    void DoGetSize( int *width, int *height ) const;

    // these get reimplemented for mono-bitmaps to behave
    // more like their Win32 couterparts. They now interpret
    // wxWHITE, wxWHITE_BRUSH and wxWHITE_PEN as drawing 0
    // and everything else as drawing 1.
    virtual void SetPen( const wxPen &pen );
    virtual void SetBrush( const wxBrush &brush );
    virtual void SetTextForeground( const wxColour &col );
    virtual void SetTextBackground( const wxColour &col );

    // implementation
    wxBitmap  m_selected;

private:
    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif
    // __GTKDCMEMORYH__

