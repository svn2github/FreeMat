/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCOMBOBOXH__
#define __GTKCOMBOBOXH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "combobox.h"
#endif

#include "wx/defs.h"

#if wxUSE_COMBOBOX

#include "wx/object.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxComboBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar* wxComboBoxNameStr;
extern const wxChar* wxEmptyString;

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

class wxComboBox : public wxControl
{
public:
    inline wxComboBox() {}
    inline wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = (const wxString *) NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr)
    {
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }
    ~wxComboBox();
    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = (const wxString *) NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr);

    void Append( const wxString &item );
    void Append( const wxString &item, void* clientData );
    void Append( const wxString &item, wxClientData* clientData );
  
    void SetClientData( int n, void* clientData );
    void* GetClientData( int n );
    void SetClientObject( int n, wxClientData* clientData );
    wxClientData* GetClientObject( int n );
  
    void SetClientObject( wxClientData *data )  { wxControl::SetClientObject( data ); }
    wxClientData *GetClientObject() const       { return wxControl::GetClientObject(); }
    void SetClientData( void *data )            { wxControl::SetClientData( data ); }
    void *GetClientData() const                 { return wxControl::GetClientData(); }
    
    void Clear();
    void Delete( int n );
  
    int FindString( const wxString &item );
    int GetSelection() const;
    wxString GetString( int n ) const;
    wxString GetStringSelection() const;
    int GetCount() const { return Number(); }
    int Number() const;
    void SetSelection( int n );
    void SetStringSelection( const wxString &string );

    wxString GetValue() const;
    void SetValue(const wxString& value);

    void Copy();
    void Cut();
    void Paste();
    void SetInsertionPoint( long pos );
    void SetInsertionPointEnd();
    long GetInsertionPoint() const;
    long GetLastPosition() const;
    void Replace( long from, long to, const wxString& value );
    void Remove( long from, long to );
    void SetSelection( long from, long to );
    void SetEditable( bool editable );
  
    // implementation
    
    virtual void SetFocus();
    
    void OnSize( wxSizeEvent &event );
    void OnChar( wxKeyEvent &event );
  
    bool     m_alreadySent;
    wxList   m_clientDataList;
    wxList   m_clientObjectList;
    int      m_prevSelection;

    void DisableEvents();
    void EnableEvents();
    void AppendCommon( const wxString &item );
    GtkWidget* GetConnectWidget();
    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();
 
protected:
    virtual wxSize DoGetBestSize() const;

private: 
    DECLARE_DYNAMIC_CLASS(wxComboBox)
    DECLARE_EVENT_TABLE()
};

#endif

#endif

  // __GTKCOMBOBOXH__
