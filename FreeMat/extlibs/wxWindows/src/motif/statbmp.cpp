/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbmp.h"
#endif

#include "wx/defs.h"

#include "wx/statbmp.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)

/*
 * wxStaticBitmap
 */

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_messageBitmap = bitmap;
    m_messageBitmapOriginal = bitmap;
    SetName(name);
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    m_mainWidget = (WXWidget) XtVaCreateManagedWidget ("staticBitmap",
#if USE_GADGETS
                    xmLabelGadgetClass, parentWidget,
#else
                    xmLabelWidgetClass, parentWidget,
#endif
                    XmNalignment, XmALIGNMENT_BEGINNING,
                    NULL);

    ChangeBackgroundColour ();

    DoSetBitmap();

    m_font = parent->GetFont();
    ChangeFont(FALSE);

    SetCanAddEventHandler(TRUE);

    wxSize actualSize(size);
    // work around the cases where the bitmap is a wxNull(Icon/Bitmap)
    if (actualSize.x == -1)
        actualSize.x = bitmap.GetWidth() ? bitmap.GetWidth() : 1;
    if (actualSize.y == -1)
        actualSize.y = bitmap.GetHeight() ? bitmap.GetHeight() : 1;
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, actualSize.x, actualSize.y);

    return TRUE;
}

wxStaticBitmap::~wxStaticBitmap()
{
    SetBitmap(wxNullBitmap);
}

void wxStaticBitmap::DoSetBitmap()
{
    Widget widget = (Widget) m_mainWidget;
    int x, y, w1, h1, w2, h2;

    GetPosition(&x, &y);

    if (m_messageBitmapOriginal.Ok())
    {
        w2 = m_messageBitmapOriginal.GetWidth();
        h2 = m_messageBitmapOriginal.GetHeight();

        Pixmap pixmap;

        // Must re-make the bitmap to have its transparent areas drawn
        // in the current widget background colour.
        if (m_messageBitmapOriginal.GetMask())
        {
            int backgroundPixel;
            XtVaGetValues( widget, XmNbackground, &backgroundPixel,
                NULL);

            wxColour col;
            col.SetPixel(backgroundPixel);

            wxBitmap newBitmap = wxCreateMaskedBitmap(m_messageBitmapOriginal, col);
            m_messageBitmap = newBitmap;

            pixmap = (Pixmap) m_messageBitmap.GetPixmap();
        }
        else
            pixmap = (Pixmap) m_messageBitmap.GetLabelPixmap(widget);

        XtVaSetValues (widget,
            XmNlabelPixmap, pixmap,
            XmNlabelType, XmPIXMAP,
            NULL);
        GetSize(&w1, &h1);

        if (! (w1 == w2) && (h1 == h2))
            SetSize(x, y, w2, h2);
    }
    else
    {
        // Null bitmap: must not use current pixmap
        // since it is no longer valid.
        XtVaSetValues (widget,
            XmNlabelType, XmSTRING,
            XmNlabelPixmap, XmUNSPECIFIED_PIXMAP,
            NULL);
    }    
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_messageBitmap = bitmap;
    m_messageBitmapOriginal = bitmap;

    DoSetBitmap();
}

void wxStaticBitmap::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxStaticBitmap::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();

    // must recalculate the background colour
    DoSetBitmap();
}

void wxStaticBitmap::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

