/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.h
// Purpose:     wxGauge class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GAUGE_H_
#define _WX_GAUGE_H_

#ifdef __GNUG__
#pragma interface "gauge.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxGaugeNameStr;

// Group box
class WXDLLEXPORT wxGauge : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxGauge)
        
public:
    inline wxGauge() { m_rangeMax = 0; m_gaugePos = 0; }
    
    inline wxGauge(wxWindow *parent, wxWindowID id,
        int range,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxGA_HORIZONTAL,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxGaugeNameStr)
    {
        Create(parent, id, range, pos, size, style, validator, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id,
        int range,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxGA_HORIZONTAL,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxGaugeNameStr);
    
    void SetShadowWidth(int w);
    void SetBezelFace(int w);
    void SetRange(int r);
    void SetValue(int pos);
    
    int GetShadowWidth() const ;
    int GetBezelFace() const ;
    int GetRange() const ;
    int GetValue() const ;
    
    virtual void Command(wxCommandEvent& WXUNUSED(event)) {} ;
    
    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    
protected:
    int      m_rangeMax;
    int      m_gaugePos;
};

#endif
// _WX_GAUGE_H_
