/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:     wxButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "button.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

#include "wx/defs.h"

#include "wx/button.h"
#include "wx/utils.h"
#include "wx/panel.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

void wxButtonCallback (Widget w, XtPointer clientData, XtPointer ptr);

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)

// Button

bool wxButton::Create(wxWindow *parent, wxWindowID id, const wxString& label,
                      const wxPoint& pos,
                      const wxSize& size, long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_windowStyle = style;
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    m_font = parent->GetFont();

    parent->AddChild((wxButton *)this);

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    wxString label1(wxStripMenuCodes(label));

    XmString text = XmStringCreateSimple ((char*) (const char*) label1);
    Widget parentWidget = (Widget) parent->GetClientWidget();

    XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay(parentWidget));

    /*
    * Patch Note (important)
    * There is no major reason to put a defaultButtonThickness here.
    * Not requesting it give the ability to put wxButton with a spacing
    * as small as requested. However, if some button become a DefaultButton,
    * other buttons are no more aligned -- This is why we set
    * defaultButtonThickness of ALL buttons belonging to the same wxPanel,
    * in the ::SetDefaultButton method.
    */
    m_mainWidget = (WXWidget) XtVaCreateManagedWidget ("button",
        xmPushButtonWidgetClass,
        parentWidget,
        XmNfontList, fontList,
        XmNlabelString, text,
        //                  XmNdefaultButtonShadowThickness, 1, // See comment for wxButton::SetDefault
        NULL);

    XmStringFree (text);

    XtAddCallback ((Widget) m_mainWidget, XmNactivateCallback, (XtCallbackProc) wxButtonCallback,
        (XtPointer) this);

    SetCanAddEventHandler(TRUE);
    
    int x = 0;  int y = 0;
    wxFont new_font( parent->GetFont() );
    GetTextExtent( label1, &x, &y, (int*)NULL, (int*)NULL, &new_font );

    wxSize newSize = size;
    if (newSize.x == -1) newSize.x = 30+x;
    if (newSize.y == -1) newSize.y = 27+y;
    SetSize( newSize.x, newSize.y );
    
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, newSize.x, newSize.y);

    ChangeBackgroundColour();

    return TRUE;
}

void wxButton::SetDefault()
{
    wxWindow *parent = GetParent();
    if ( parent )
        parent->SetDefaultItem(this);

    // We initially do not set XmNdefaultShadowThickness, to have small buttons.
    // Unfortunately, buttons are now mis-aligned. We try to correct this
    // now -- setting this ressource to 1 for each button in the same row.
    // Because it's very hard to find wxButton in the same row,
    // correction is straighforward: we set resource for all wxButton
    // in this parent (but not sub panels)
    for (wxNode * node = parent->GetChildren().First (); node; node = node->Next ())
    {
        wxButton *item = (wxButton *) node->Data ();
        if (item->IsKindOf(CLASSINFO(wxButton)))
        {
            bool managed = XtIsManaged((Widget) item->GetMainWidget());
            if (managed)
                XtUnmanageChild ((Widget) item->GetMainWidget());

            XtVaSetValues ((Widget) item->GetMainWidget(),
                XmNdefaultButtonShadowThickness, 1,
                NULL);

            if (managed)
                XtManageChild ((Widget) item->GetMainWidget());
        }
    } // while

    //  XtVaSetValues((Widget)handle, XmNshowAsDefault, 1, NULL);
    XtVaSetValues ((Widget) parent->GetMainWidget(), XmNdefaultButton, (Widget) GetMainWidget(), NULL);
}

/* static */
wxSize wxButton::GetDefaultSize()
{
    // TODO: check font size as in wxMSW ?  MB
    //
    return wxSize(80,26);
}

void wxButton::Command (wxCommandEvent & event)
{
    ProcessCommand (event);
}

void wxButtonCallback (Widget w, XtPointer clientData, XtPointer WXUNUSED(ptr))
{
    if (!wxGetWindowFromTable(w))
        // Widget has been deleted!
        return;

    wxButton *item = (wxButton *) clientData;
    wxCommandEvent event (wxEVT_COMMAND_BUTTON_CLICKED, item->GetId());
    event.SetEventObject(item);
    item->ProcessCommand (event);
}

void wxButton::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxButton::ChangeBackgroundColour()
{
    DoChangeBackgroundColour(m_mainWidget, m_backgroundColour, TRUE);
}

void wxButton::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

