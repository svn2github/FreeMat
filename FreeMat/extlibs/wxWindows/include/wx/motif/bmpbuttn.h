/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTN_H_
#define _WX_BMPBUTTN_H_

#ifdef __GNUG__
#pragma interface "bmpbuttn.h"
#endif

#include "wx/button.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonNameStr;

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLEXPORT wxBitmapButton: public wxButton
{
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
public:
    wxBitmapButton();
    ~wxBitmapButton();
    inline wxBitmapButton(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr);
    
    virtual void SetLabel(const wxBitmap& bitmap)
    {
        SetBitmapLabel(bitmap);
    }
    virtual void SetLabel(const wxString& label)
    {
        wxControl::SetLabel(label);
    }
    
    virtual void SetBitmapLabel(const wxBitmap& bitmap);
    
    inline wxBitmap& GetBitmapLabel() const { return (wxBitmap&) m_buttonBitmap; }
    inline wxBitmap& GetBitmapSelected() const { return (wxBitmap&) m_buttonBitmapSelected; }
    inline wxBitmap& GetBitmapFocus() const { return (wxBitmap&) m_buttonBitmapFocus; }
    inline wxBitmap& GetBitmapDisabled() const { return (wxBitmap&) m_buttonBitmapDisabled; }
    
    void SetBitmapSelected(const wxBitmap& sel);
    void SetBitmapFocus(const wxBitmap& focus);
    void SetBitmapDisabled(const wxBitmap& disabled);
    
    inline void SetMargins(int x, int y) { m_marginX = x; m_marginY = y; }
    inline int GetMarginX() { return m_marginX; }
    inline int GetMarginY() { return m_marginY; }
    
    // Implementation
    void DoSetBitmap();
    virtual void ChangeBackgroundColour();
    virtual wxSize DoGetBestSize() const;
    
protected:
    wxBitmap m_buttonBitmapFocus;
    wxBitmap m_buttonBitmap;
    wxBitmap m_buttonBitmapOriginal; // May be different from m_buttonBitmap
    // if m_buttonBitmap has been changed
    // to reflect button background colour
    wxBitmap m_buttonBitmapSelected;
    wxBitmap m_buttonBitmapSelectedOriginal;
    
    wxBitmap m_buttonBitmapDisabled;
    wxBitmap m_buttonBitmapDisabledOriginal;
    
    int      m_marginX;
    int      m_marginY;
    
    WXPixmap m_insensPixmap;
};

#endif
// _WX_BMPBUTTN_H_
