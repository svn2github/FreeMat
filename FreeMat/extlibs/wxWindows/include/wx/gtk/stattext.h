/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICTEXTH__
#define __GTKSTATICTEXTH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxStaticText;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

class wxStaticText : public wxControl
{
public:
    wxStaticText();
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize, 
                 long style = 0,
                 const wxString &name = wxStaticTextNameStr );

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize, 
                long style = 0,
                const wxString &name = wxStaticTextNameStr );

    wxString GetLabel() const;
    void SetLabel( const wxString &label );

    bool SetFont( const wxFont &font );
    
    // implementation
    // --------------
    
    void ApplyWidgetStyle();

protected:
    virtual wxSize DoGetBestSize() const;

    DECLARE_DYNAMIC_CLASS(wxStaticText)
};

#endif // __GTKSTATICTEXTH__
