/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:     wxControl class
// Author:      David Webster
// Modified by:
// Created:     09/17/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/dcclient.h"
#include "wx/scrolwin.h"
#include "wx/log.h"
#endif
#include "wx/os2/private.h"
#include "wx/control.h"

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
    EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()

// Item members
wxControl::wxControl()
{

#if WXWIN_COMPATIBILITY
  m_callback = 0;
#endif // WXWIN_COMPATIBILITY
} // end of wxControl::wxControl

bool wxControl::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxValidator&                rValidator
, const wxString&                   rsName
)
{
    bool                            bRval = wxWindow::Create( pParent
                                                             ,vId
                                                             ,rPos
                                                             ,rSize
                                                             ,lStyle
                                                             ,rsName
                                                            );
    if (bRval)
    {
#if wxUSE_VALIDATORS
        SetValidator(rValidator);
#endif
    }
    return bRval;
} // end of wxControl::Create

wxControl::~wxControl()
{
    m_isBeingDeleted = TRUE;
}

bool wxControl::OS2CreateControl(
  const wxChar*                     zClassname
, const wxString&                   rsLabel
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
)
{
    WXDWORD                         dwExstyle;
    WXDWORD                         dwStyle = OS2GetStyle( lStyle
                                                          ,&dwExstyle
                                                         );

    return OS2CreateControl( zClassname
                            ,dwStyle
                            ,rPos
                            ,rSize
                            ,rsLabel
                            ,dwExstyle
                           );
} // end of wxControl::OS2CreateControl

bool wxControl::OS2CreateControl(
  const wxChar*                     zClassname
, WXDWORD                           dwStyle
, const wxPoint&                    rPos
, const wxSize&                     rSize
, const wxString&                   rsLabel
, WXDWORD                           dwExstyle
)
{
    bool                            bWant3D = FALSE;

    //
    // Doesn't do anything at all under OS/2
    //
    if (dwExstyle == (WXDWORD)-1)
    {
        dwExstyle = Determine3DEffects(WS_EX_CLIENTEDGE, &bWant3D);
    }
    //
    // All controls should have these styles (wxWindows creates all controls
    // visible by default)
    //
    if (m_isShown )
        dwStyle |= WS_VISIBLE;

    wxWindow*                       pParent = GetParent();
    PSZ                             zClass;

    if (!pParent)
        return FALSE;

    if ((strcmp(zClassname, "COMBOBOX")) == 0)
        zClass = WC_COMBOBOX;
    else if ((strcmp(zClassname, "STATIC")) == 0)
        zClass = WC_STATIC;
    else if ((strcmp(zClassname, "BUTTON")) == 0)
        zClass = WC_BUTTON;
    else if ((strcmp(zClassname, "NOTEBOOK")) == 0)
        zClass = WC_NOTEBOOK;
    dwStyle |= WS_VISIBLE;

    m_hWnd = (WXHWND)::WinCreateWindow( (HWND)GetHwndOf(pParent) // Parent window handle
                                       ,(PSZ)zClass              // Window class
                                       ,(PSZ)rsLabel.c_str()     // Initial Text
                                       ,(ULONG)dwStyle           // Style flags
                                       ,(LONG)0                  // X pos of origin
                                       ,(LONG)0                  // Y pos of origin
                                       ,(LONG)0                  // control width
                                       ,(LONG)0                  // control height
                                       ,(HWND)GetHwndOf(pParent) // owner window handle (same as parent
                                       ,HWND_TOP                 // initial z position
                                       ,(ULONG)GetId()           // Window identifier
                                       ,NULL                     // no control data
                                       ,NULL                     // no Presentation parameters
                                      );

    if ( !m_hWnd )
    {
#ifdef __WXDEBUG__
        wxLogError(wxT("Failed to create a control of class '%s'"), zClassname);
#endif // DEBUG

        return FALSE;
    }
    //
    // Subclass again for purposes of dialog editing mode
    //
    SubclassWin(m_hWnd);

    //
    // Controls use the same font and colours as their parent dialog by default
    //
    InheritAttributes();
    SetXComp(0);
    SetYComp(0);
    SetSize( rPos.x
            ,rPos.y
            ,rSize.x
            ,rSize.y
           );
    return TRUE;
} // end of wxControl::OS2CreateControl

wxSize wxControl::DoGetBestSize() const
{
    return wxSize(DEFAULT_ITEM_WIDTH, DEFAULT_ITEM_HEIGHT);
} // end of wxControl::DoGetBestSize

bool wxControl::ProcessCommand(wxCommandEvent& event)
{
#if WXWIN_COMPATIBILITY
    if ( m_callback )
    {
        (void)(*m_callback)(this, event);

        return TRUE;
    }
    else
#endif // WXWIN_COMPATIBILITY

    return GetEventHandler()->ProcessEvent(event);
}

WXHBRUSH wxControl::OnCtlColor(
  WXHDC                             hWxDC
, WXHWND                            hWnd
, WXUINT                            uCtlColor
, WXUINT                            uMessage
, WXWPARAM                          wParam
, WXLPARAM                          lParam
)
{
    HPS                             hPS = (HPS)hWxDC; // pass in a PS handle in OS/2
    wxColour                        vColFore = GetForegroundColour();
    wxColour                        vColBack = GetBackgroundColour();

    if (GetParent()->GetTransparentBackground())
        ::GpiSetBackMix(hPS, BM_LEAVEALONE);
    else
        ::GpiSetBackMix(hPS, BM_OVERPAINT);

    ::GpiSetBackColor(hPS, vColBack.GetPixel());
    ::GpiSetColor(hPS, vColFore.GetPixel());

    wxBrush*                        pBrush = wxTheBrushList->FindOrCreateBrush( vColBack
                                                                               ,wxSOLID
                                                                              );
    return (WXHBRUSH)pBrush->GetResourceHandle();
} // end of wxControl::OnCtlColor

void wxControl::OnEraseBackground(
  wxEraseEvent&                     rEvent
)
{
    RECTL                           vRect;
    HPS                             hPS = rEvent.GetDC()->GetHPS();
    SIZEL                           vSize = {0,0};

    ::GpiSetPS(hPS, &vSize, PU_PELS | GPIF_DEFAULT);
    ::WinQueryWindowRect((HWND)GetHwnd(), &vRect);
    ::WinFillRect(hPS, &vRect, GetBackgroundColour().GetPixel());
} // end of wxControl::OnEraseBackground

WXDWORD wxControl::OS2GetStyle(
  long                              lStyle
, WXDWORD*                          pdwExstyle
) const
{
    long                            dwStyle = wxWindow::OS2GetStyle( lStyle
                                                                    ,pdwExstyle
                                                                   );

    if (AcceptsFocus())
    {
        dwStyle |= WS_TABSTOP;
    }
    return dwStyle;
} // end of wxControl::OS2GetStyle

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// Call this repeatedly for several wnds to find the overall size
// of the widget.
// Call it initially with -1 for all values in rect.
// Keep calling for other widgets, and rect will be modified
// to calculate largest bounding rectangle.
void wxFindMaxSize(
  WXHWND                            hWnd
, RECT*                             pRect
)
{
    int                             nLeft = pRect->xLeft;
    int                             nRight = pRect->xRight;
    int                             nTop = pRect->yTop;
    int                             nBottom = pRect->yBottom;

    ::WinQueryWindowRect((HWND)hWnd, pRect);

    if (nLeft < 0)
        return;

    if (nLeft < pRect->xLeft)
        pRect->xLeft = nLeft;

    if (nRight > pRect->xRight)
        pRect->xRight = nRight;

    if (nTop < pRect->yTop)
        pRect->yTop = nTop;

    if (nBottom > pRect->yBottom)
        pRect->yBottom = nBottom;
} // end of wxFindMaxSize


