/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#define XtParent XTPARENT
#endif

#include "wx/defs.h"

#include "wx/choice.h"
#include "wx/utils.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)

void wxChoiceCallback (Widget w, XtPointer clientData,
                       XtPointer ptr);

wxChoice::wxChoice()
{
    m_noStrings = 0;
    m_buttonWidget = (WXWidget) 0;
    m_menuWidget = (WXWidget) 0;
    m_widgetList = (WXWidget*) 0;
    m_formWidget = (WXWidget) 0;
}

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n, const wxString choices[],
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_noStrings = 0; // Starts off with none, incremented in Append
    m_windowStyle = style;
    m_buttonWidget = (WXWidget) 0;
    m_menuWidget = (WXWidget) 0;
    m_widgetList = (WXWidget*) 0;
    m_formWidget = (WXWidget) 0;

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    m_font = parent->GetFont();

    Widget parentWidget = (Widget) parent->GetClientWidget();

    m_formWidget = (WXWidget) XtVaCreateManagedWidget(name.c_str(),
        xmRowColumnWidgetClass, parentWidget,
        XmNmarginHeight, 0,
        XmNmarginWidth, 0,
        XmNpacking, XmPACK_TIGHT,
        XmNorientation, XmHORIZONTAL,
        NULL);

    XtVaSetValues ((Widget) m_formWidget, XmNspacing, 0, NULL);

    /*
    * Create the popup menu
    */
    m_menuWidget = (WXWidget) XmCreatePulldownMenu ((Widget) m_formWidget, "choiceMenu", NULL, 0);

    //    int i;
    if (n > 0)
    {
        int i;
        for (i = 0; i < n; i++)
            Append (choices[i]);
    }

    /*
    * Create button
    */
    Arg args[10];
    Cardinal argcnt = 0;

    XtSetArg (args[argcnt], XmNsubMenuId, (Widget) m_menuWidget);
    argcnt++;
    XtSetArg (args[argcnt], XmNmarginWidth, 0);
    argcnt++;
    XtSetArg (args[argcnt], XmNmarginHeight, 0);
    argcnt++;
    XtSetArg (args[argcnt], XmNpacking, XmPACK_TIGHT);
    argcnt++;
    m_buttonWidget = (WXWidget) XmCreateOptionMenu ((Widget) m_formWidget, "choiceButton", args, argcnt);

    m_mainWidget = m_buttonWidget;

    XtManageChild ((Widget) m_buttonWidget);

    // New code from Roland Haenel (roland_haenel@ac.cybercity.de)
    // Some time ago, I reported a problem with wxChoice-items under
    // Linux and Motif 2.0 (they caused sporadic GPFs). Now it seems
    // that I have found the code responsible for this behaviour.
#if XmVersion >= 1002
#if XmVersion <  2000
    // JACS, 24/1/99: this seems to cause a malloc crash later on, e.g.
    // in controls sample.
    //
    // Widget optionLabel = XmOptionLabelGadget ((Widget) m_buttonWidget);
    // XtUnmanageChild (optionLabel);
#endif
#endif

    XtVaSetValues((Widget) m_formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);

    ChangeFont(FALSE);

    AttachWidget (parent, m_buttonWidget, m_formWidget, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour();

    return TRUE;
}

wxChoice::~wxChoice()
{
    // For some reason destroying the menuWidget
    // can cause crashes on some machines. It will
    // be deleted implicitly by deleting the parent form
    // anyway.
    //  XtDestroyWidget (menuWidget);
    if (m_widgetList)
        delete[] m_widgetList;

    if (GetMainWidget())
    {
        DetachWidget(GetMainWidget()); // Removes event handlers
        DetachWidget(m_formWidget);

        XtDestroyWidget((Widget) m_formWidget);
        m_formWidget = (WXWidget) 0;

        // Presumably the other widgets have been deleted now, via the form
        m_mainWidget = (WXWidget) 0;
        m_buttonWidget = (WXWidget) 0;
    }
}

int wxChoice::DoAppend(const wxString& item)
{
    Widget w = XtVaCreateManagedWidget (wxStripMenuCodes(item),
#if USE_GADGETS
        xmPushButtonGadgetClass, (Widget) m_menuWidget,
#else
        xmPushButtonWidgetClass, (Widget) m_menuWidget,
#endif
        NULL);

    DoChangeBackgroundColour((WXWidget) w, m_backgroundColour);

    if (m_font.Ok())
        XtVaSetValues (w,
        XmNfontList, (XmFontList) m_font.GetFontList(1.0, XtDisplay((Widget) m_formWidget)),
        NULL);

    WXWidget *new_widgetList = new WXWidget[m_noStrings + 1];
    int i;
    if (m_widgetList)
        for (i = 0; i < m_noStrings; i++)
            new_widgetList[i] = m_widgetList[i];

    new_widgetList[m_noStrings] = (WXWidget) w;

    if (m_widgetList)
        delete[] m_widgetList;
    m_widgetList = new_widgetList;

    char mnem = wxFindMnemonic ((char*) (const char*) item);
    if (mnem != 0)
        XtVaSetValues (w, XmNmnemonic, mnem, NULL);

    XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) wxChoiceCallback, (XtPointer) this);

    if (m_noStrings == 0 && m_buttonWidget)
    {
        XtVaSetValues ((Widget) m_buttonWidget, XmNmenuHistory, w, NULL);
        Widget label = XmOptionButtonGadget ((Widget) m_buttonWidget);
        XmString text = XmStringCreateSimple ((char*) (const char*) item);
        XtVaSetValues (label,
            XmNlabelString, text,
            NULL);
        XmStringFree (text);
    }
    wxNode *node = m_stringList.Add (item);
    XtVaSetValues (w, XmNuserData, node->Data (), NULL);

    if (m_noStrings == 0)
        m_clientList.Append((wxObject*) NULL);
    else
        m_clientList.Insert( m_clientList.Item(m_noStrings-1),
                                 (wxObject*) NULL );
    m_noStrings ++;

    return Number() - 1;
}

void wxChoice::Delete(int WXUNUSED(n))
{
    wxFAIL_MSG( "Sorry, wxChoice::Delete isn't implemented yet. Maybe you'd like to volunteer? :-)" );

    // What should we do -- remove the callback for this button widget,
    // delete the m_stringList entry, delete the button widget, construct a new widget list
    // (see Append)

    // TODO
    m_noStrings --;
}

void wxChoice::Clear()
{
    m_stringList.Clear ();
    int i;
    for (i = 0; i < m_noStrings; i++)
    {
        XtUnmanageChild ((Widget) m_widgetList[i]);
        XtDestroyWidget ((Widget) m_widgetList[i]);
    }
    if (m_noStrings)
        delete[] m_widgetList;
    m_widgetList = (WXWidget*) NULL;
    if (m_buttonWidget)
        XtVaSetValues ((Widget) m_buttonWidget, XmNmenuHistory, (Widget) NULL, NULL);

    if ( HasClientObjectData() )
    {
        // destroy the data (due to Robert's idea of using wxList<wxObject>
        // and not wxList<wxClientData> we can't just say
        // m_clientList.DeleteContents(TRUE) - this would crash!
        wxNode *node = m_clientList.First();
        while ( node )
        {
            delete (wxClientData *)node->Data();
            node = node->Next();
        }
    }
    m_clientList.Clear();

    m_noStrings = 0;
}

int wxChoice::GetSelection() const
{
    XmString text;
    char *s;
    Widget label = XmOptionButtonGadget ((Widget) m_buttonWidget);
    XtVaGetValues (label,
        XmNlabelString, &text,
        NULL);

    if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
    {
        int i = 0;
        for (wxNode * node = m_stringList.First (); node; node = node->Next ())
        {
            char *s1 = (char *) node->Data ();
            if (s1 == s || strcmp (s1, s) == 0)
            {
                XmStringFree(text) ;
                XtFree (s);
                return i;
            }
            else
                i++;
        }            // for()

        XmStringFree(text) ;
        XtFree (s);
        return -1;
    }
    XmStringFree(text) ;
    return -1;
}

void wxChoice::SetSelection(int n)
{
    m_inSetValue = TRUE;

    wxNode *node = m_stringList.Nth (n);
    if (node)
    {
        Dimension selectionWidth, selectionHeight;

        char *s = (char *) node->Data ();
        XmString text = XmStringCreateSimple (s);
        XtVaGetValues ((Widget) m_widgetList[n], XmNwidth, &selectionWidth, XmNheight, &selectionHeight, NULL);
        Widget label = XmOptionButtonGadget ((Widget) m_buttonWidget);
        XtVaSetValues (label,
            XmNlabelString, text,
            NULL);
        XmStringFree (text);
        XtVaSetValues ((Widget) m_buttonWidget,
            XmNwidth, selectionWidth, XmNheight, selectionHeight,
            XmNmenuHistory, (Widget) m_widgetList[n], NULL);
    }
    m_inSetValue = FALSE;
}

int wxChoice::FindString(const wxString& s) const
{
    int i = 0;
    for (wxNode * node = m_stringList.First (); node; node = node->Next ())
    {
        char *s1 = (char *) node->Data ();
        if (s == s1)
        {
            return i;
        }
        else
            i++;
    }
    return -1;
}

wxString wxChoice::GetString(int n) const
{
    wxNode *node = m_stringList.Nth (n);
    if (node)
        return wxString((char *) node->Data ());
    else
        return wxEmptyString;
}

void wxChoice::SetColumns(int n)
{
    if (n<1) n = 1 ;

    short numColumns = n ;
    Arg args[3];

    XtSetArg(args[0], XmNnumColumns, numColumns);
    XtSetArg(args[1], XmNpacking, XmPACK_COLUMN);
    XtSetValues((Widget) m_menuWidget,args,2) ;
}

int wxChoice::GetColumns(void) const
{
    short numColumns ;

    XtVaGetValues((Widget) m_menuWidget,XmNnumColumns,&numColumns,NULL) ;
    return numColumns ;
}

void wxChoice::SetFocus()
{
    XmProcessTraversal(XtParent((Widget)m_mainWidget), XmTRAVERSE_CURRENT);
}

void wxChoice::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    XtVaSetValues((Widget) m_formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
    bool managed = XtIsManaged((Widget) m_formWidget);

    if (managed)
        XtUnmanageChild ((Widget) m_formWidget);

    int actualWidth = width, actualHeight = height;

    if (width > -1)
    {
        int i;
        for (i = 0; i < m_noStrings; i++)
            XtVaSetValues ((Widget) m_widgetList[i], XmNwidth, actualWidth, NULL);
        XtVaSetValues ((Widget) m_buttonWidget, XmNwidth, actualWidth,
            NULL);
    }
    if (height > -1)
    {
        int i;
        for (i = 0; i < m_noStrings; i++)
            XtVaSetValues ((Widget) m_widgetList[i], XmNheight, actualHeight, NULL);
        XtVaSetValues ((Widget) m_buttonWidget, XmNheight, actualHeight,
            NULL);
    }

    if (managed)
        XtManageChild ((Widget) m_formWidget);
    XtVaSetValues((Widget) m_formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);

    wxControl::DoSetSize (x, y, width, height, sizeFlags);
}

wxString wxChoice::GetStringSelection () const
{
    int sel = GetSelection ();
    if (sel > -1)
        return wxString(this->GetString (sel));
    else
        return wxEmptyString;
}

bool wxChoice::SetStringSelection (const wxString& s)
{
    int sel = FindString (s);
    if (sel > -1)
    {
        SetSelection (sel);
        return TRUE;
    }
    else
        return FALSE;
}

void wxChoice::Command(wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    ProcessCommand (event);
}

void wxChoiceCallback (Widget w, XtPointer clientData, XtPointer WXUNUSED(ptr))
{
    wxChoice *item = (wxChoice *) clientData;
    if (item)
    {
        if (item->InSetValue())
            return;

        char *s = NULL;
        XtVaGetValues (w, XmNuserData, &s, NULL);
        if (s)
        {
            wxCommandEvent event (wxEVT_COMMAND_CHOICE_SELECTED, item->GetId());
            event.SetEventObject(item);
            event.m_commandInt = item->FindString (s);
            //            event.m_commandString = s;
            item->ProcessCommand (event);
        }
    }
}

void wxChoice::ChangeFont(bool keepOriginalSize)
{
    // Note that this causes the widget to be resized back
    // to its original size! We therefore have to set the size
    // back again. TODO: a better way in Motif?
    if (m_font.Ok())
    {
        int width, height, width1, height1;
        GetSize(& width, & height);

        XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay((Widget) m_mainWidget));
        XtVaSetValues ((Widget) m_mainWidget, XmNfontList, fontList, NULL);
        XtVaSetValues ((Widget) m_buttonWidget, XmNfontList, fontList, NULL);

        /* TODO: why does this cause a crash in XtWidgetToApplicationContext?
        int i;
        for (i = 0; i < m_noStrings; i++)
        XtVaSetValues ((Widget) m_widgetList[i], XmNfontList, fontList, NULL);
        */
        GetSize(& width1, & height1);
        if (keepOriginalSize && (width != width1 || height != height1))
        {
            SetSize(-1, -1, width, height);
        }
    }
}

void wxChoice::ChangeBackgroundColour()
{
    DoChangeBackgroundColour(m_formWidget, m_backgroundColour);
    DoChangeBackgroundColour(m_buttonWidget, m_backgroundColour);
    DoChangeBackgroundColour(m_menuWidget, m_backgroundColour);
    int i;
    for (i = 0; i < m_noStrings; i++)
        DoChangeBackgroundColour(m_widgetList[i], m_backgroundColour);
}

void wxChoice::ChangeForegroundColour()
{
    DoChangeForegroundColour(m_formWidget, m_foregroundColour);
    DoChangeForegroundColour(m_buttonWidget, m_foregroundColour);
    DoChangeForegroundColour(m_menuWidget, m_foregroundColour);
    int i;
    for (i = 0; i < m_noStrings; i++)
        DoChangeForegroundColour(m_widgetList[i], m_foregroundColour);
}


// These implement functions needed by wxControlWithItems.
// Unfortunately, they're not all implemented yet.

int wxChoice::GetCount() const
{
    return Number();
}

/*
int wxChoice::DoAppend(const wxString& item)
{
    Append(item);
    return GetCount() - 1;
}
*/

// Just appends, doesn't yet insert
void wxChoice::DoInsertItems(const wxArrayString& items, int WXUNUSED(pos))
{
    size_t nItems = items.GetCount();

    for ( size_t n = 0; n < nItems; n++ )
    {
        Append( items[n]);
    }
}

void wxChoice::DoSetItems(const wxArrayString& items, void **WXUNUSED(clientData))
{
    Clear();
    size_t nItems = items.GetCount();

    for ( size_t n = 0; n < nItems; n++ )
    {
        Append(items[n]);
    }
}

void wxChoice::DoSetFirstItem(int WXUNUSED(n))
{
    wxFAIL_MSG( wxT("wxChoice::DoSetFirstItem not implemented") );
}

void wxChoice::DoSetItemClientData(int n, void* clientData)
{
    wxNode *node = m_clientList.Nth( n );
    wxCHECK_RET( node, wxT("invalid index in wxChoice::DoSetItemClientData") );

    node->SetData( (wxObject*) clientData );
}

void* wxChoice::DoGetItemClientData(int n) const
{
    wxNode *node = m_clientList.Nth( n );
    wxCHECK_MSG( node, NULL, wxT("invalid index in wxChoice::DoGetItemClientData") );

    return node->Data();
}

void wxChoice::DoSetItemClientObject(int n, wxClientData* clientData)
{
    wxNode *node = m_clientList.Nth( n );
    wxCHECK_RET( node, wxT("invalid index in wxChoice::DoSetItemClientObject") );

    wxClientData *cd = (wxClientData*) node->Data();
    delete cd;

    node->SetData( (wxObject*) clientData );
}

wxClientData* wxChoice::DoGetItemClientObject(int n) const
{
    wxNode *node = m_clientList.Nth( n );
    wxCHECK_MSG( node, (wxClientData *)NULL,
                 wxT("invalid index in wxChoice::DoGetItemClientObject") );

    return (wxClientData*) node->Data();
}

void wxChoice::Select(int n)
{
    SetSelection(n);
}

void wxChoice::SetString(int WXUNUSED(n), const wxString& WXUNUSED(s))
{
    wxFAIL_MSG( wxT("wxChoice::SetString not implemented") );
}
