/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:     wxListBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOX_H_
#define _WX_LISTBOX_H_

#ifdef __GNUG__
#pragma interface "listbox.h"
#endif

#include "wx/ctrlsub.h"

WXDLLEXPORT_DATA(extern const char*) wxListBoxNameStr;

// forward decl for GetSelections()
class WXDLLEXPORT wxArrayInt;

WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// List box item
class WXDLLEXPORT wxListBox: public wxListBoxBase
{
    DECLARE_DYNAMIC_CLASS(wxListBox)
        
public:
    wxListBox();
    wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr):
        m_clientDataList(wxKEY_INTEGER)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr);
    
    ~wxListBox();
    
    // implementation of wxControlWithItems
    virtual int GetCount() const;
    virtual int DoAppend(const wxString& item);
    virtual void DoSetItemClientData(int n, void* clientData); //
    virtual void* DoGetItemClientData(int n) const; //
    virtual void DoSetItemClientObject(int n, wxClientData* clientData); //
    virtual wxClientData* DoGetItemClientObject(int n) const; //
    virtual int GetSelection() const;
    virtual void Delete(int n);
    virtual int FindString(const wxString& s) const;
    virtual void Clear();
    virtual void SetString(int n, const wxString& s);
    virtual wxString GetString(int n) const;

    // implementation of wxListBoxbase
    virtual void SetSelection(int n, bool select = TRUE);
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoSetFirstItem(int n);
    virtual int GetSelections(wxArrayInt& aSelections) const;
    virtual bool IsSelected(int n) const;

    // For single or multiple choice list item
    void Command(wxCommandEvent& event);
    
    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    WXWidget GetTopWidget() const;

#if wxUSE_CHECKLISTBOX
    virtual void DoToggleItem(int item, int x) {};
#endif
protected:
    int       m_noItems;
    
    // List mapping positions->client data
    wxList    m_clientDataList;
    
    virtual void DoSetSize(int x, int y,
        int width, int height,
        int sizeFlags = wxSIZE_AUTO);
};

#endif
// _WX_LISTBOX_H_
