/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.h
// Purpose:     wxCheckBox class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKBOX_H_
#define _WX_CHECKBOX_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "checkbox.h"
#endif

// Checkbox item (single checkbox)
class WXDLLEXPORT wxCheckBox : public wxCheckBoxBase
{
public:
    wxCheckBox() { }
    wxCheckBox(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr);
    virtual void SetValue(bool);
    virtual bool GetValue() const;
    virtual void MacHandleControlClick( WXWidget control , wxInt16 controlpart );
    virtual void Command(wxCommandEvent& event);

    DECLARE_DYNAMIC_CLASS(wxCheckBox)
};

class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxBitmapCheckBox: public wxCheckBox
{
public:
    int checkWidth;
    int checkHeight;

    wxBitmapCheckBox()
        : checkWidth(-1), checkHeight(-1)
        { }

    wxBitmapCheckBox(wxWindow *parent, wxWindowID id, const wxBitmap *label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id, const wxBitmap *bitmap,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr);
    virtual void SetValue(bool);
    virtual bool GetValue() const;
    virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
    virtual void SetLabel(const wxBitmap *bitmap);
    virtual void SetLabel( const wxString & WXUNUSED(name) ) {}

    DECLARE_DYNAMIC_CLASS(wxBitmapCheckBox)
};
#endif
    // _WX_CHECKBOX_H_
