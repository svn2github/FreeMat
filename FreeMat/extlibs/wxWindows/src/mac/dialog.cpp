/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:     wxDialog class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#include "wx/dialog.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/settings.h"

#include "wx/mac/uma.h"

// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs
wxList wxModalDialogs;
//wxList wxModelessWindows;  // Frames and modeless dialogs
extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxTopLevelWindow)

BEGIN_EVENT_TABLE(wxDialog, wxDialogBase)
  EVT_BUTTON(wxID_OK, wxDialog::OnOK)
  EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
  EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)

  EVT_CHAR_HOOK(wxDialog::OnCharHook)

  EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)

  EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()

#endif

wxDialog::wxDialog()
{
  	m_isShown = FALSE;
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
}

bool wxDialog::Create(wxWindow *parent, wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{

  SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
  

  if ( !wxTopLevelWindow::Create(parent, id, title, pos, size, style, name) )
        return FALSE;

	MacCreateRealWindow( title , pos , size , MacRemoveBordersFromStyle(style)  & ~(wxYES|wxOK|wxNO|wxCANCEL)  , name ) ;

	m_macWindowBackgroundTheme = kThemeBrushDialogBackgroundActive ;
	SetThemeWindowBackground( (WindowRef) m_macWindow , m_macWindowBackgroundTheme , false ) ;
  return TRUE;
}

void wxDialog::SetModal(bool flag)
{
  if ( flag )
    {
        m_windowStyle |= wxDIALOG_MODAL;

        wxModelessWindows.DeleteObject(this);
    }
    else
    {
        m_windowStyle &= ~wxDIALOG_MODAL;

        wxModelessWindows.Append(this);
    }
}

wxDialog::~wxDialog()
{
	m_isBeingDeleted = TRUE ;
  Show(FALSE);
}

// By default, pressing escape cancels the dialog , on mac command-stop does the same thing
void wxDialog::OnCharHook(wxKeyEvent& event)
{
  if (
    ( event.m_keyCode == WXK_ESCAPE || 
      ( event.m_keyCode == '.' && event.MetaDown() ) )
     && FindWindow(wxID_CANCEL) )
  {
		// Behaviour changed in 2.0: we'll send a Cancel message
		// to the dialog instead of Close.
		wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
		cancelEvent.SetEventObject( this );
		GetEventHandler()->ProcessEvent(cancelEvent);

		return;
  }
  // We didn't process this event.
  event.Skip();
}

bool wxDialog::IsModal() const
{
    return (GetWindowStyleFlag() & wxDIALOG_MODAL) != 0;
}


bool wxDialog::IsModalShowing() const
{
    return wxModalDialogs.Find((wxDialog *)this) != NULL; // const_cast
}

extern bool s_macIsInModalLoop ;

bool wxDialog::Show(bool show)
{
    if ( !wxDialogBase::Show(show) )
    {
        // nothing to do
        return FALSE;
    }

    if ( show )
    {
        // usually will result in TransferDataToWindow() being called
        InitDialog();
    }

    if ( IsModal() )
    {
        if ( show )
        {
            DoShowModal();
        }
        else // end of modal dialog
        {
            // this will cause IsModalShowing() return FALSE and our local
            // message loop will terminate
            wxModalDialogs.DeleteObject(this);
        }
    }

    return TRUE;
}

void wxDialog::DoShowModal()
{
    wxCHECK_RET( !IsModalShowing(), _T("DoShowModal() called twice") );

    wxModalDialogs.Append(this);

  	wxWindow *parent = GetParent();

    // remember where the focus was
    wxWindow *winFocus = FindFocus();
    if ( !winFocus )
    {
        winFocus = parent;
    }
    if ( !winFocus )
    {
        winFocus = wxTheApp->GetTopWindow();
    }
	// TODO : test whether parent gets disabled

	bool formerModal = s_macIsInModalLoop ;
	s_macIsInModalLoop = true ;

	while ( IsModalShowing() )
	{
		while ( !wxTheApp->Pending() && wxTheApp->ProcessIdle() )
		{
		}
		wxTheApp->MacDoOneEvent() ;
	}
	
	s_macIsInModalLoop = formerModal ;

    // TODO probably reenable the parent window if any

    // and restore focus
    if ( winFocus )
    {
        winFocus->SetFocus();
    }
}


// Replacement for Show(TRUE) for modal dialogs - returns return code
int wxDialog::ShowModal()
{
  m_windowStyle |= wxDIALOG_MODAL;
  Show(TRUE);
  return GetReturnCode();
}

// NB: this function (surprizingly) may be called for both modal and modeless
//     dialogs and should work for both of them
void wxDialog::EndModal(int retCode)
{
  SetReturnCode(retCode);
  Show(FALSE);
}

// Standard buttons
void wxDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  if ( Validate() && TransferDataFromWindow() )
  {
      EndModal(wxID_OK);
  }
}

void wxDialog::OnApply(wxCommandEvent& WXUNUSED(event))
{
  if (Validate())
    TransferDataFromWindow();
  // TODO probably need to disable the Apply button until things change again
}

void wxDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}

void wxDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    // We'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close().
    // We wouldn't want to destroy the dialog by default, since the dialog may have been
    // created on the stack.
    // However, this does mean that calling dialog->Close() won't delete the dialog
    // unless the handler for wxID_CANCEL does so. So use Destroy() if you want to be
    // sure to destroy the dialog.
    // The default OnCancel (above) simply ends a modal dialog, and hides a modeless dialog.

    static wxList closing;

    if ( closing.Member(this) )
        return;

    closing.Append(this);

    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent); // This may close the dialog

    closing.DeleteObject(this);
}

void wxDialog::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
  SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
  Refresh();
}

