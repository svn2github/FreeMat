/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:     wxStaticText class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_
#define _WX_STATTEXT_H_

#ifdef __GNUG__
#pragma interface "stattext.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticTextNameStr;

class WXDLLEXPORT wxStaticText: public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxStaticText)
        
public:
    wxStaticText() { }
    
    wxStaticText(wxWindow *parent, wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxStaticTextNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxStaticTextNameStr);
    
    // implementation
    // --------------
    
    // operations
    virtual bool ProcessCommand(wxCommandEvent& WXUNUSED(event))
    {
        return FALSE;
    }
    
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    virtual void SetLabel(const wxString& label);
    
    // Get the widget that corresponds to the label (for font setting, label setting etc.)
    virtual WXWidget GetLabelWidget() const
        { return m_labelWidget; }
    
protected:
    WXWidget              m_labelWidget;
};

#endif
// _WX_STATTEXT_H_
