/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:     wxDialog class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#define XtWindow XTWINDOW
#define XtParent XTPARENT
#define XtScreen XTSCREEN
#endif

#include "wx/dialog.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/settings.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>

#include <X11/Shell.h>
#if XmVersion >= 1002
#include <Xm/XmAll.h>
#endif
#include <Xm/MwmUtil.h>
#include <Xm/Label.h>
#include <Xm/BulletinB.h>
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelG.h>
#include <Xm/AtomMgr.h>
#if   XmVersion > 1000
#include <Xm/Protocols.h>
#endif
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

static void wxCloseDialogCallback(Widget widget, XtPointer client_data, XmAnyCallbackStruct *cbs);
static void wxDialogBoxEventHandler (Widget    wid,
                                     XtPointer client_data,
                                     XEvent*   event,
                                     Boolean *continueToDispatch);

static void wxUnmapBulletinBoard(Widget dialog, wxDialog *client,XtPointer call);

// A stack of modal_showing flags, since we can't rely
// on accessing wxDialog::m_modalShowing within
// wxDialog::Show in case a callback has deleted the wxDialog.
static wxList wxModalShowingStack;

// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs
wxList wxModalDialogs;
wxList wxModelessWindows;  // Frames and modeless dialogs
extern wxList wxPendingDelete;

#define wxUSE_INVISIBLE_RESIZE 1

IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxPanel)

BEGIN_EVENT_TABLE(wxDialog, wxPanel)
  EVT_SIZE(wxDialog::OnSize)
  EVT_BUTTON(wxID_OK, wxDialog::OnOK)
  EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
  EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)
  EVT_CHAR_HOOK(wxDialog::OnCharHook)
  EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)
  EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()


wxDialog::wxDialog()
{
    m_modalShowing = FALSE;
    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
}

bool wxDialog::Create(wxWindow *parent, wxWindowID id,
                      const wxString& title,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    m_windowStyle = style;
    m_modalShowing = FALSE;
    m_dialogTitle = title;

    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_foregroundColour = *wxBLACK;

    SetName(name);

    if (!parent)
        wxTopLevelWindows.Append(this);

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    Widget parentWidget = (Widget) 0;
    if (parent)
        parentWidget = (Widget) parent->GetTopWidget();
    if (!parent)
        parentWidget = (Widget) wxTheApp->GetTopLevelWidget();

    wxASSERT_MSG( (parentWidget != (Widget) 0), "Could not find a suitable parent shell for dialog." );

    Arg args[2];
    XtSetArg (args[0], XmNdefaultPosition, False);
    XtSetArg (args[1], XmNautoUnmanage, False);
    Widget dialogShell = XmCreateBulletinBoardDialog(parentWidget, (char*) (const char*) name, args, 2);
    m_mainWidget = (WXWidget) dialogShell;

    // We don't want margins, since there is enough elsewhere.
    XtVaSetValues(dialogShell,
        XmNmarginHeight,   0,
        XmNmarginWidth,    0,
        XmNresizePolicy, XmRESIZE_NONE,
        NULL) ;

    Widget shell = XtParent(dialogShell) ;
    if (!title.IsNull())
    {
        XmString str = XmStringCreateSimple((char*) (const char*)title);
        XtVaSetValues(dialogShell,
            XmNdialogTitle, str,
            NULL);
        XmStringFree(str);
    }

    m_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    ChangeFont(FALSE);

    wxAddWindowToTable(dialogShell, this);

    // Intercept CLOSE messages from the window manager
    Atom WM_DELETE_WINDOW = XmInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", False);

    /* Remove and add WM_DELETE_WINDOW so ours is only handler */
    /* Why do we have to do this for wxDialog, but not wxFrame? */
    XmRemoveWMProtocols(shell, &WM_DELETE_WINDOW, 1);
    XmAddWMProtocols(shell, &WM_DELETE_WINDOW, 1);
    XmActivateWMProtocol(shell, WM_DELETE_WINDOW);

    // Modified Steve Hammes for Motif 2.0
#if (XmREVISION > 1 || XmVERSION > 1)
    XmAddWMProtocolCallback(shell, WM_DELETE_WINDOW, (XtCallbackProc) wxCloseDialogCallback, (XtPointer)this);
#elif XmREVISION == 1
    XmAddWMProtocolCallback(shell, WM_DELETE_WINDOW, (XtCallbackProc) wxCloseDialogCallback, (caddr_t)this);
#else
    XmAddWMProtocolCallback(shell, WM_DELETE_WINDOW, (void (*)())wxCloseDialogCallback, (caddr_t)this);
#endif

    XtTranslations ptr ;
    XtOverrideTranslations(dialogShell,
        ptr = XtParseTranslationTable("<Configure>: resize()"));
    XtFree((char *)ptr);

    // Can't remember what this was about... but I think it's necessary.

    if (wxUSE_INVISIBLE_RESIZE)
    {
        if (pos.x > -1)
            XtVaSetValues(dialogShell, XmNx, pos.x,
            NULL);
        if (pos.y > -1)
            XtVaSetValues(dialogShell, XmNy, pos.y,
            NULL);

        if (size.x > -1)
            XtVaSetValues(dialogShell, XmNwidth, size.x, NULL);
        if (size.y > -1)
            XtVaSetValues(dialogShell, XmNheight, size.y, NULL);
    }

    // This patch come from Torsten Liermann lier@lier1.muc.de
    if (XmIsMotifWMRunning(shell))
    {
        int decor = 0 ;
        if (m_windowStyle & wxRESIZE_BORDER)
            decor |= MWM_DECOR_RESIZEH ;
        if (m_windowStyle & wxSYSTEM_MENU)
            decor |= MWM_DECOR_MENU;
        if ((m_windowStyle & wxCAPTION) ||
            (m_windowStyle & wxTINY_CAPTION_HORIZ) ||
            (m_windowStyle & wxTINY_CAPTION_VERT))
            decor |= MWM_DECOR_TITLE;
        if (m_windowStyle & wxTHICK_FRAME)
            decor |= MWM_DECOR_BORDER;
        if (m_windowStyle & wxMINIMIZE_BOX)
            decor |= MWM_DECOR_MINIMIZE;
        if (m_windowStyle & wxMAXIMIZE_BOX)
            decor |= MWM_DECOR_MAXIMIZE;

        XtVaSetValues(shell,XmNmwmDecorations,decor,NULL) ;
    }
    // This allows non-Motif window managers to support at least the
    // no-decorations case.
    else
    {
        if ((m_windowStyle & wxCAPTION) != wxCAPTION)
            XtVaSetValues((Widget) shell,XmNoverrideRedirect,TRUE,NULL);
    }

    XtRealizeWidget(dialogShell);

    XtAddCallback(dialogShell,XmNunmapCallback,
        (XtCallbackProc)wxUnmapBulletinBoard,this) ;

    // Positioning of the dialog doesn't work properly unless the dialog
    // is managed, so we manage without mapping to the screen.
    // To show, we map the shell (actually it's parent).
    if (!wxUSE_INVISIBLE_RESIZE)
        XtVaSetValues(shell, XmNmappedWhenManaged, FALSE, NULL);

    if (!wxUSE_INVISIBLE_RESIZE)
    {
        XtManageChild(dialogShell);
        SetSize(pos.x, pos.y, size.x, size.y);
    }
    XtAddEventHandler(dialogShell,ExposureMask,FALSE,
        wxUniversalRepaintProc, (XtPointer) this);

    XtAddEventHandler(dialogShell,
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask,
        FALSE,
        wxDialogBoxEventHandler,
        (XtPointer)this);

    ChangeBackgroundColour();

    return TRUE;
}

void wxDialog::SetModal(bool flag)
{
    if ( flag )
        m_windowStyle |= wxDIALOG_MODAL ;
    else
        if ( m_windowStyle & wxDIALOG_MODAL )
            m_windowStyle -= wxDIALOG_MODAL ;

        wxModelessWindows.DeleteObject(this);
        if (!flag)
            wxModelessWindows.Append(this);
}

wxDialog::~wxDialog()
{
    m_isBeingDeleted = TRUE;
    
    if (m_mainWidget)
      XtRemoveEventHandler((Widget) m_mainWidget, ExposureMask, FALSE,
          wxUniversalRepaintProc, (XtPointer) this);

    m_modalShowing = FALSE;
    if (!wxUSE_INVISIBLE_RESIZE && m_mainWidget)
    {
        XtUnmapWidget((Widget) m_mainWidget);
    }

    wxTopLevelWindows.DeleteObject(this);

    if ( (GetWindowStyleFlag() & wxDIALOG_MODAL) != wxDIALOG_MODAL )
        wxModelessWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if (wxTheApp && (wxTopLevelWindows.Number() == 0))
    {
        wxTheApp->SetTopWindow(NULL);

        if (wxTheApp->GetExitOnFrameDelete())
        {
            wxTheApp->ExitMainLoop();
        }
    }

    // This event-flushing code used to be in wxWindow::PostDestroyChildren (wx_dialog.cpp)
    // but I think this should work, if we destroy the children first.
    // Note that this might need to be done for wxFrame also.
    DestroyChildren();

    // The idea about doing it here is that if you have to remove the
    // XtDestroyWidget from ~wxWindow, at least top-level windows
    // will still be deleted (and destroy children implicitly).
    if (GetMainWidget())
    {
      DetachWidget(GetMainWidget()); // Removes event handlers
      XtDestroyWidget((Widget) GetMainWidget());
      SetMainWidget((WXWidget) NULL);
    }
}

// By default, pressing escape cancels the dialog
void wxDialog::OnCharHook(wxKeyEvent& event)
{
    if (event.m_keyCode == WXK_ESCAPE)
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

void wxDialog::Iconize(bool WXUNUSED(iconize))
{
    // Can't iconize a dialog in Motif, apparently
    // TODO: try using the parent of m_mainShell.
    //  XtVaSetValues((Widget) m_mainWidget, XmNiconic, iconize, NULL);
}

// Default resizing behaviour - if only ONE subwindow,
// resize to client rectangle size
void wxDialog::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // if we're using constraints - do use them
#if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() ) {
        Layout();
        return;
    }
#endif

    // do we have _exactly_ one child?
    wxWindow *child = NULL;
    for ( wxNode *node = GetChildren().First(); node; node = node->Next() )
    {
        wxWindow *win = (wxWindow *)node->Data();
        if ( !win->IsKindOf(CLASSINFO(wxFrame))  &&
            !win->IsKindOf(CLASSINFO(wxDialog))  )
        {
            if ( child )
                return;     // it's our second subwindow - nothing to do
            child = win;
        }
    }

    if ( child ) {
        // we have exactly one child - set it's size to fill the whole frame
        int clientW, clientH;
        GetClientSize(&clientW, &clientH);

        int x = 0;
        int y = 0;

        child->SetSize(x, y, clientW, clientH);
    }
}


bool wxDialog::IsIconized() const
{
/*
Boolean iconic;
XtVaGetValues((Widget) m_mainWidget, XmNiconic, &iconic, NULL);

  return iconic;
    */
    return FALSE;
}

void wxDialog::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    XtVaSetValues((Widget) m_mainWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
    wxWindow::DoSetSize(x, y, width, height, sizeFlags);
    XtVaSetValues((Widget) m_mainWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
}

void wxDialog::DoSetClientSize(int width, int height)
{
    wxWindow::SetSize(-1, -1, width, height);
}

void wxDialog::SetTitle(const wxString& title)
{
    m_dialogTitle = title;
    if (!title.IsNull())
    {
        XmString str = XmStringCreateSimple((char*) (const char*) title);
        XtVaSetValues((Widget) m_mainWidget,
            XmNtitle, (char*) (const char*) title,
            XmNdialogTitle, str, // Roberto Cocchi
            XmNiconName, (char*) (const char*) title,
            NULL);
        XmStringFree(str);
    }
}

wxString wxDialog::GetTitle() const
{
    return m_dialogTitle;
}

void wxDialog::Raise()
{
    Window parent_window = XtWindow((Widget) m_mainWidget),
        next_parent   = XtWindow((Widget) m_mainWidget),
        root          = RootWindowOfScreen(XtScreen((Widget) m_mainWidget));
    // search for the parent that is child of ROOT, because the WM may
    // reparent twice and notify only the next parent (like FVWM)
    while (next_parent != root) {
        Window *theChildren; unsigned int n;
        parent_window = next_parent;
        XQueryTree(XtDisplay((Widget) m_mainWidget), parent_window, &root,
            &next_parent, &theChildren, &n);
        XFree(theChildren); // not needed
    }
    XRaiseWindow(XtDisplay((Widget) m_mainWidget), parent_window);
}

void wxDialog::Lower()
{
    Window parent_window = XtWindow((Widget) m_mainWidget),
        next_parent   = XtWindow((Widget) m_mainWidget),
        root          = RootWindowOfScreen(XtScreen((Widget) m_mainWidget));
    // search for the parent that is child of ROOT, because the WM may
    // reparent twice and notify only the next parent (like FVWM)
    while (next_parent != root) {
        Window *theChildren; unsigned int n;
        parent_window = next_parent;
        XQueryTree(XtDisplay((Widget) m_mainWidget), parent_window, &root,
            &next_parent, &theChildren, &n);
        XFree(theChildren); // not needed
    }
    XLowerWindow(XtDisplay((Widget) m_mainWidget), parent_window);
}

bool wxDialog::Show(bool show)
{
    m_isShown = show;

    if (show)
    {
        if (!wxUSE_INVISIBLE_RESIZE)
            XtMapWidget(XtParent((Widget) m_mainWidget));
        else
            XtManageChild((Widget) m_mainWidget) ;

        XRaiseWindow(XtDisplay((Widget) m_mainWidget), XtWindow((Widget) m_mainWidget));

    }
    else
    {
        if (!wxUSE_INVISIBLE_RESIZE)
            XtUnmapWidget(XtParent((Widget) m_mainWidget));
        else
            XtUnmanageChild((Widget) m_mainWidget) ;

        XFlush(XtDisplay((Widget) wxTheApp->GetTopLevelWidget()));
        XSync(XtDisplay((Widget) wxTheApp->GetTopLevelWidget()), FALSE);
    }

    return TRUE;
}

// Shows a dialog modally, returning a return code
int wxDialog::ShowModal()
{
    m_windowStyle |= wxDIALOG_MODAL;

    Show(TRUE);

    if (m_modalShowing)
        return 0;

    wxModalShowingStack.Insert((wxObject *)TRUE);

    m_modalShowing = TRUE;
    XtAddGrab((Widget) m_mainWidget, TRUE, FALSE);

    XEvent event;

    // Loop until we signal that the dialog should be closed
    while ((wxModalShowingStack.Number() > 0) && ((int)(wxModalShowingStack.First()->Data()) != 0))
    {
        //        XtAppProcessEvent((XtAppContext) wxTheApp->GetAppContext(), XtIMAll);

        XtAppNextEvent((XtAppContext) wxTheApp->GetAppContext(), &event);
        wxTheApp->ProcessXEvent((WXEvent*) &event);

        if (XtAppPending( (XtAppContext) wxTheApp->GetAppContext() ) == 0)
        {
            if (!wxTheApp->ProcessIdle())
            {
#if wxUSE_THREADS
                // leave the main loop to give other threads a chance to
                // perform their GUI work
                wxMutexGuiLeave();
                wxUsleep(20);
                wxMutexGuiEnter();
#endif
            }
        }
    }

    // Remove modal dialog flag from stack
    wxNode *node = wxModalShowingStack.First();
    if (node)
        delete node;

    // Now process all events in case they get sent to a destroyed dialog
    XSync(XtDisplay((Widget) wxTheApp->GetTopLevelWidget()), FALSE);
    while (XtAppPending((XtAppContext) wxTheApp->GetAppContext()))
    {
        XFlush(XtDisplay((Widget) wxTheApp->GetTopLevelWidget()));
        XtAppNextEvent((XtAppContext) wxTheApp->GetAppContext(), &event);

        wxTheApp->ProcessXEvent((WXEvent*) &event);
    }

    // TODO: is it safe to call this, if the dialog may have been deleted
    // by now? Probably only if we're using delayed deletion of dialogs.
    return GetReturnCode();
}

void wxDialog::EndModal(int retCode)
{
    if (!m_modalShowing)
        return;

    SetReturnCode(retCode);

    // Strangely, we don't seem to need this now.
    //    XtRemoveGrab((Widget) m_mainWidget);

    Show(FALSE);

    m_modalShowing = FALSE;

    wxNode *node = wxModalShowingStack.First();
    if (node)
        node->SetData((wxObject *)FALSE);
}

// Standard buttons
void wxDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    if ( Validate() && TransferDataFromWindow() )
    {
        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
            SetReturnCode(wxID_OK);
            this->Show(FALSE);
        }
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
    if ( IsModal() )
        EndModal(wxID_CANCEL);
    else
    {
        SetReturnCode(wxID_CANCEL);
        this->Show(FALSE);
    }
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

void wxDialog::OnPaint(wxPaintEvent &WXUNUSED(event))
{
  // added for compatiblity only
}

// Destroy the window (delayed, if a managed window)
bool wxDialog::Destroy()
{
    if (!wxPendingDelete.Member(this))
        wxPendingDelete.Append(this);
    return TRUE;
}

void wxDialog::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    Refresh();
}

// Handle a close event from the window manager
static void wxCloseDialogCallback( Widget WXUNUSED(widget), XtPointer client_data,
                                  XmAnyCallbackStruct *WXUNUSED(cbs))
{
    wxDialog *dialog = (wxDialog *)client_data;
    wxCloseEvent closeEvent(wxEVT_CLOSE_WINDOW, dialog->GetId());
    closeEvent.SetEventObject(dialog);

    // May delete the dialog (with delayed deletion)
    dialog->GetEventHandler()->ProcessEvent(closeEvent);
}

void wxDialogBoxEventHandler(Widget    wid,
                             XtPointer WXUNUSED(client_data),
                             XEvent*   event,
                             Boolean*  continueToDispatch)
{
    wxDialog *dialog = (wxDialog *)wxGetWindowFromTable(wid);
    if (dialog)
    {
        wxMouseEvent wxevent(wxEVT_NULL);
        if (wxTranslateMouseEvent(wxevent, dialog, wid, event))
        {
            wxevent.SetEventObject(dialog);
            wxevent.SetId(dialog->GetId());
            dialog->GetEventHandler()->ProcessEvent(wxevent);
        }
        else
        {
            // An attempt to implement OnCharHook by calling OnCharHook first;
            // if this returns TRUE, set continueToDispatch to False
            // (don't continue processing).
            // Otherwise set it to True and call OnChar.
            wxKeyEvent keyEvent(wxEVT_CHAR);
            if (wxTranslateKeyEvent(keyEvent, dialog, wid, event))
            {
                keyEvent.SetEventObject(dialog);
                keyEvent.SetId(dialog->GetId());
                keyEvent.SetEventType(wxEVT_CHAR_HOOK);
                if (dialog->GetEventHandler()->ProcessEvent(keyEvent))
                {
                    *continueToDispatch = False;
                    return;
                }
                else
                {
                    // For simplicity, OnKeyDown is the same as OnChar
                    // TODO: filter modifier key presses from OnChar
                    keyEvent.SetEventType(wxEVT_KEY_DOWN);

                    // Only process OnChar if OnKeyDown didn't swallow it
                    if (!dialog->GetEventHandler()->ProcessEvent (keyEvent))
                    {
                        keyEvent.SetEventType(wxEVT_CHAR);
                        dialog->GetEventHandler()->ProcessEvent(keyEvent);
                    }
                }
            }
        }
    }
    *continueToDispatch = True;
}

static void wxUnmapBulletinBoard(Widget WXUNUSED(dialog), wxDialog *WXUNUSED(client), XtPointer WXUNUSED(call) )
{
/* This gets called when the dialog is being shown, which
* defeats modal showing.
client->m_modalShowing = FALSE ;
client->m_isShown = FALSE;
    */
}

void wxDialog::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxDialog::ChangeBackgroundColour()
{
    if (GetMainWidget())
        DoChangeBackgroundColour(GetMainWidget(), m_backgroundColour);
}

void wxDialog::ChangeForegroundColour()
{
    if (GetMainWidget())
        DoChangeForegroundColour(GetMainWidget(), m_foregroundColour);
}

