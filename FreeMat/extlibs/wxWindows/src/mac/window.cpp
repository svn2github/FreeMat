/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindowMac
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#include "wx/setup.h"
#include "wx/menu.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/panel.h"
#include "wx/layout.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/scrolbar.h"
#include "wx/statbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/tooltip.h"
#include "wx/statusbr.h"
#include "wx/menuitem.h"
#include "wx/spinctrl.h"
#include "wx/log.h"
#include "wx/geometry.h"

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#define wxWINDOW_HSCROLL 5998
#define wxWINDOW_VSCROLL 5997
#define MAC_SCROLLBAR_SIZE 16

#include "wx/mac/uma.h"
#ifndef __DARWIN__
#include <Windows.h>
#include <ToolUtils.h>
#endif

#if  wxUSE_DRAG_AND_DROP
#include "wx/dnd.h"
#endif

#include <string.h>

extern wxList wxPendingDelete;
wxWindowMac* gFocusWindow = NULL ;

#ifdef __WXUNIVERSAL__
    IMPLEMENT_ABSTRACT_CLASS(wxWindowMac, wxWindowBase)
#else // __WXMAC__
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif // __WXUNIVERSAL__/__WXMAC__

#if !USE_SHARED_LIBRARY

BEGIN_EVENT_TABLE(wxWindowMac, wxWindowBase)
  EVT_NC_PAINT(wxWindowMac::OnNcPaint)
  EVT_ERASE_BACKGROUND(wxWindowMac::OnEraseBackground)
  EVT_SYS_COLOUR_CHANGED(wxWindowMac::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindowMac::OnInitDialog)
  EVT_IDLE(wxWindowMac::OnIdle)
  EVT_SET_FOCUS(wxWindowMac::OnSetFocus)
END_EVENT_TABLE()

#endif

#define wxMAC_DEBUG_REDRAW 0
#ifndef wxMAC_DEBUG_REDRAW
#define wxMAC_DEBUG_REDRAW 0
#endif

#define wxMAC_USE_THEME_BORDER 0


// ===========================================================================
// implementation
// ===========================================================================


// ----------------------------------------------------------------------------
// constructors and such
// ----------------------------------------------------------------------------

void wxWindowMac::Init()
{
    // generic
    InitBase();

    // MSW specific
    m_doubleClickAllowed = 0;
    m_winCaptured = FALSE;

    m_isBeingDeleted = FALSE;

    m_useCtl3D = FALSE;
    m_mouseInWindow = FALSE;

    m_xThumbSize = 0;
    m_yThumbSize = 0;
    m_backgroundTransparent = FALSE;

    // as all windows are created with WS_VISIBLE style...
    m_isShown = TRUE;

    m_x = 0;
    m_y = 0 ;
    m_width = 0 ;
    m_height = 0 ;

    m_hScrollBar = NULL ;
    m_vScrollBar = NULL ;

    m_label = wxEmptyString;
}

// Destructor
wxWindowMac::~wxWindowMac()
{
    // Send destroy event
    wxWindowDestroyEvent destroyEvent(this);
    destroyEvent.SetId(GetId());
    GetEventHandler()->ProcessEvent(destroyEvent);

    // deleting a window while it is shown invalidates the region
    if ( IsShown() ) {
        wxWindowMac* iter = this ;
        while( iter ) {
            if ( iter->IsTopLevel() )
            {
                Refresh() ;
                break ;
            }
            iter = iter->GetParent() ;

        }
    }

    m_isBeingDeleted = TRUE;

#ifndef __WXUNIVERSAL__
    // VS: make sure there's no wxFrame with last focus set to us:
    for ( wxWindow *win = GetParent(); win; win = win->GetParent() )
    {
        wxFrame *frame = wxDynamicCast(win, wxFrame);
        if ( frame )
        {
            if ( frame->GetLastFocus() == this )
            {
                frame->SetLastFocus((wxWindow*)NULL);
            }
            break;
        }
    }
#endif // __WXUNIVERSAL__

    if ( s_lastMouseWindow == this )
    {
        s_lastMouseWindow = NULL ;
    }

    wxFrame* frame = wxDynamicCast( wxGetTopLevelParent( this ) , wxFrame ) ;
    if ( frame )
    {
    	if ( frame->GetLastFocus() == this )
    		frame->SetLastFocus( NULL ) ;
    }

    if ( gFocusWindow == this )
    {
        gFocusWindow = NULL ;
    }

	// CS: copied from MSW :
    // VS: destroy children first and _then_ detach *this from its parent.
    //     If we'd do it the other way around, children wouldn't be able
    //     find their parent frame (see above).
    DestroyChildren();

    if ( m_parent )
        m_parent->RemoveChild(this);

    // delete our drop target if we've got one
#if wxUSE_DRAG_AND_DROP
    if ( m_dropTarget != NULL )
    {
        delete m_dropTarget;
        m_dropTarget = NULL;
    }
#endif // wxUSE_DRAG_AND_DROP
}

// Constructor
bool wxWindowMac::Create(wxWindowMac *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindowMac without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    parent->AddChild(this);

    m_x = (int)pos.x;
    m_y = (int)pos.y;
    AdjustForParentClientOrigin(m_x, m_y, wxSIZE_USE_EXISTING);
    m_width = WidthDefault( size.x );
    m_height = HeightDefault( size.y ) ;
#ifndef __WXUNIVERSAL__
    // Don't give scrollbars to wxControls unless they ask for them
    if ( (! IsKindOf(CLASSINFO(wxControl)) && ! IsKindOf(CLASSINFO(wxStatusBar))) ||
         (IsKindOf(CLASSINFO(wxControl)) && ( style & wxHSCROLL || style & wxVSCROLL)))
    {
        MacCreateScrollBars( style ) ;
    }
#endif
    return TRUE;
}

void wxWindowMac::SetFocus()
{
    if ( gFocusWindow == this )
        return ;

    if ( AcceptsFocus() )
    {
        if (gFocusWindow )
        {
#if wxUSE_CARET
                // Deal with caret
                if ( gFocusWindow->m_caret )
                {
                      gFocusWindow->m_caret->OnKillFocus();
                }
#endif // wxUSE_CARET
#ifndef __WXUNIVERSAL__
            wxControl* control = wxDynamicCast( gFocusWindow , wxControl ) ;
            if ( control && control->GetMacControl() )
            {
                UMASetKeyboardFocus( (WindowRef) gFocusWindow->MacGetRootWindow() , (ControlHandle) control->GetMacControl()  , kControlFocusNoPart ) ;
                control->MacRedrawControl() ;
            }
#endif
            // Without testing the window id, for some reason
            // a kill focus event can still be sent to
            // the control just being focussed.
            int thisId = this->m_windowId;
            int gFocusWindowId = gFocusWindow->m_windowId;
            if (gFocusWindowId != thisId)
            {
                wxFocusEvent event(wxEVT_KILL_FOCUS, gFocusWindow->m_windowId);
                event.SetEventObject(gFocusWindow);
                gFocusWindow->GetEventHandler()->ProcessEvent(event) ;
            }
        }
        gFocusWindow = this ;
        {
            #if wxUSE_CARET
            // Deal with caret
            if ( m_caret )
            {
                m_caret->OnSetFocus();
            }
            #endif // wxUSE_CARET
            // panel wants to track the window which was the last to have focus in it
            wxChildFocusEvent eventFocus(this);
            GetEventHandler()->ProcessEvent(eventFocus);

      #ifndef __WXUNIVERSAL__
            wxControl* control = wxDynamicCast( gFocusWindow , wxControl ) ;
            if ( control && control->GetMacControl() )
            {
                UMASetKeyboardFocus( (WindowRef) gFocusWindow->MacGetRootWindow() , (ControlHandle) control->GetMacControl()  , kControlFocusNextPart ) ;
            }
      #endif
            wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
            event.SetEventObject(this);
            GetEventHandler()->ProcessEvent(event) ;
        }
    }
}

bool wxWindowMac::Enable(bool enable)
{
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;

    MacSuperEnabled( enable ) ;

    return TRUE;
}

void wxWindowMac::DoCaptureMouse()
{
    wxTheApp->s_captureWindow = this ;
}

wxWindow* wxWindowBase::GetCapture()
{
    return wxTheApp->s_captureWindow ;
}

void wxWindowMac::DoReleaseMouse()
{
    wxTheApp->s_captureWindow = NULL ;
}

#if    wxUSE_DRAG_AND_DROP

void wxWindowMac::SetDropTarget(wxDropTarget *pDropTarget)
{
  if ( m_dropTarget != 0 ) {
    delete m_dropTarget;
  }

  m_dropTarget = pDropTarget;
  if ( m_dropTarget != 0 )
  {
    // TODO
  }
}

#endif

// Old style file-manager drag&drop
void wxWindowMac::DragAcceptFiles(bool accept)
{
    // TODO
}

// Get total size
void wxWindowMac::DoGetSize(int *x, int *y) const
{
     if(x)   *x = m_width ;
     if(y)   *y = m_height ;
}

void wxWindowMac::DoGetPosition(int *x, int *y) const
{
    int xx,yy;

    xx = m_x ;
    yy = m_y ;
    if ( !IsTopLevel() && GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        xx -= pt.x;
        yy -= pt.y;
    }
    if(x)   *x = xx;
    if(y)   *y = yy;
}

#if wxUSE_MENUS

static void MacMenuAfterDisplay( wxMenu* menu );
static void MacMenuBeforeDisplay( wxMenu* menu );

bool wxWindowMac::DoPopupMenu(wxMenu *menu, int x, int y)
{
    wxCHECK_MSG( menu != NULL, FALSE, wxT("invalid popup-menu") );

    menu->SetInvokingWindow(this);
    menu->UpdateUI();
    ClientToScreen( &x , &y ) ;

    MacMenuBeforeDisplay(menu);
    long menuResult = ::PopUpMenuSelect((MenuHandle) menu->GetHMenu() ,y,x, 0) ;
    if ( HiWord(menuResult) != 0 )
    {
        MenuCommand id ;
        GetMenuItemCommandID( GetMenuHandle(HiWord(menuResult)) , LoWord(menuResult) , &id ) ;
        wxMenuItem* item = NULL ;
        wxMenu* realmenu ;
        item = menu->FindItem(id, &realmenu) ;
        if (item->IsCheckable())
        {
            item->Check( !item->IsChecked() ) ;
        }
        menu->SendEvent( id , item->IsCheckable() ? item->IsChecked() : -1 ) ;
    }
    MacMenuAfterDisplay(menu);
    menu->SetInvokingWindow(NULL);

  return TRUE;
}


// NOTE: In 2.5 functions similar to these are members of wxMenu.  For 2.4 we
// put a slightly modified version of them here as statics just to get the
// popup submenus working, and to preserve binary compatibility.

// MacOS needs to know about submenus somewhere within this menu
// before it can be displayed
static void MacMenuBeforeDisplay( wxMenu* menu )
{
    int pos ;
    wxMenuItemList::Node *node;
    wxMenuItem *item;
    for (pos = 0, node = menu->GetMenuItems().GetFirst(); node; node = node->GetNext(), pos++)
    {
        item = (wxMenuItem *)node->GetData();
        wxMenu* subMenu = item->GetSubMenu() ;
        if (subMenu)
        {
            MacMenuBeforeDisplay( subMenu ) ;
        }
    }

    ::InsertMenu(MAC_WXHMENU( menu->GetHMenu()), -1);

}

// undo all changes from the MacMenuBeforeDisplay call
static void MacMenuAfterDisplay( wxMenu* menu )
{
    ::DeleteMenu(menu->MacGetMenuId());

    int pos ;
    wxMenuItemList::Node *node;
    wxMenuItem *item;
    for (pos = 0, node = menu->GetMenuItems().GetFirst(); node; node = node->GetNext(), pos++)
    {
        item = (wxMenuItem *)node->GetData();
        wxMenu* subMenu = item->GetSubMenu() ;
        if (subMenu)
        {
            MacMenuAfterDisplay( subMenu ) ;
        }
    }
}

#endif


void wxWindowMac::DoScreenToClient(int *x, int *y) const
{
    WindowRef window = (WindowRef) MacGetRootWindow() ;

    Point       localwhere = {0,0} ;

    if(x)   localwhere.h = * x ;
    if(y)   localwhere.v = * y ;

    GrafPtr     port ;
    ::GetPort( &port ) ;
    ::SetPort( UMAGetWindowPort( window ) ) ;
    ::GlobalToLocal( &localwhere ) ;
    ::SetPort( port ) ;

    if(x)   *x = localwhere.h ;
    if(y)   *y = localwhere.v ;

    MacRootWindowToWindow( x , y ) ;
    if ( x )
            *x -= MacGetLeftBorderSize() ;
    if ( y )
            *y -= MacGetTopBorderSize() ;
}

void wxWindowMac::DoClientToScreen(int *x, int *y) const
{
    WindowRef window = (WindowRef) MacGetRootWindow() ;

    if ( x )
            *x += MacGetLeftBorderSize() ;
    if ( y )
            *y += MacGetTopBorderSize() ;

    MacWindowToRootWindow( x , y ) ;

    Point       localwhere = { 0,0 };
    if(x)   localwhere.h = * x ;
    if(y)   localwhere.v = * y ;

    GrafPtr     port ;
    ::GetPort( &port ) ;
    ::SetPort( UMAGetWindowPort( window ) ) ;

    ::LocalToGlobal( &localwhere ) ;
    ::SetPort( port ) ;
    if(x)   *x = localwhere.h ;
    if(y)   *y = localwhere.v ;
}

void wxWindowMac::MacClientToRootWindow( int *x , int *y ) const
{
    wxPoint origin = GetClientAreaOrigin() ;
    if(x)   *x += origin.x ;
    if(y)   *y += origin.y ;

    MacWindowToRootWindow( x , y ) ;
}

void wxWindowMac::MacRootWindowToClient( int *x , int *y ) const
{
    wxPoint origin = GetClientAreaOrigin() ;
    MacRootWindowToWindow( x , y ) ;
    if(x)   *x -= origin.x ;
    if(y)   *y -= origin.y ;
}

void wxWindowMac::MacWindowToRootWindow( int *x , int *y ) const
{
    if ( !IsTopLevel() )
    {
        if(x)   *x += m_x ;
        if(y)   *y += m_y ;
        GetParent()->MacWindowToRootWindow( x , y ) ;
    }
}

void wxWindowMac::MacRootWindowToWindow( int *x , int *y ) const
{
    if ( !IsTopLevel() )
    {
        if(x)   *x -= m_x ;
        if(y)   *y -= m_y ;
        GetParent()->MacRootWindowToWindow( x , y ) ;
    }
}

bool wxWindowMac::SetCursor(const wxCursor& cursor)
{
    if (m_cursor == cursor)
       return FALSE;

    if (wxNullCursor == cursor)
    {
       if ( ! wxWindowBase::SetCursor( *wxSTANDARD_CURSOR ) )
        return FALSE ;
    }
    else
    {
       if ( ! wxWindowBase::SetCursor( cursor ) )
        return FALSE ;
    }

  wxASSERT_MSG( m_cursor.Ok(),
                  wxT("cursor must be valid after call to the base version"));

  Point pt ;
  wxWindowMac *mouseWin ;
  GetMouse( &pt ) ;

  // Change the cursor NOW if we're within the correct window

  if ( MacGetWindowFromPoint( wxPoint( pt.h , pt.v ) , &mouseWin ) )
  {
    if ( mouseWin == this && !wxIsBusy() )
    {
        m_cursor.MacInstall() ;
    }
  }

  return TRUE ;
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowMac::DoGetClientSize(int *x, int *y) const
{
    int ww, hh;
    ww = m_width ;
    hh = m_height ;

    ww -= MacGetLeftBorderSize(  )  + MacGetRightBorderSize(  ) ;
    hh -= MacGetTopBorderSize(  ) + MacGetBottomBorderSize( );

  if ( (m_vScrollBar && m_vScrollBar->IsShown()) || (m_hScrollBar  && m_hScrollBar->IsShown()) )
  {
    int x1 = 0 ;
    int y1 = 0 ;
    int w = m_width ;
    int h = m_height ;

    MacClientToRootWindow( &x1 , &y1 ) ;
    MacClientToRootWindow( &w , &h ) ;

    wxWindowMac *iter = (wxWindowMac*)this ;

    int totW = 10000 , totH = 10000;
    while( iter )
    {
        if ( iter->IsTopLevel() )
        {
            totW = iter->m_width ;
            totH = iter->m_height ;
            break ;
        }

        iter = iter->GetParent() ;
    }

    if (m_hScrollBar  && m_hScrollBar->IsShown() )
    {
        hh -= MAC_SCROLLBAR_SIZE;
        if ( h-y1 >= totH )
        {
            hh += 1 ;
        }
    }
    if (m_vScrollBar  && m_vScrollBar->IsShown() )
    {
        ww -= MAC_SCROLLBAR_SIZE;
        if ( w-x1 >= totW )
        {
            ww += 1 ;
        }
    }
  }
  if(x)   *x = ww;
  if(y)   *y = hh;
}


// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindowMac::DoSetToolTip(wxToolTip *tooltip)
{
    wxWindowBase::DoSetToolTip(tooltip);

    if ( m_tooltip )
        m_tooltip->SetWindow(this);
}

#endif // wxUSE_TOOLTIPS

void wxWindowMac::DoMoveWindow(int x, int y, int width, int height)
{
    int former_x = m_x ;
    int former_y = m_y ;
    int former_w = m_width ;
    int former_h = m_height ;

  int actualWidth = width;
  int actualHeight = height;
  int actualX = x;
  int actualY = y;

    if ((m_minWidth != -1) && (actualWidth < m_minWidth))
        actualWidth = m_minWidth;
    if ((m_minHeight != -1) && (actualHeight < m_minHeight))
        actualHeight = m_minHeight;
    if ((m_maxWidth != -1) && (actualWidth > m_maxWidth))
        actualWidth = m_maxWidth;
    if ((m_maxHeight != -1) && (actualHeight > m_maxHeight))
        actualHeight = m_maxHeight;

    bool doMove = false ;
    bool doResize = false ;

    if ( actualX != former_x || actualY != former_y )
    {
        doMove = true ;
    }
    if ( actualWidth != former_w || actualHeight != former_h )
    {
        doResize = true ;
    }

    if ( doMove || doResize )
    {
        // erase former position

        bool partialRepaint = false ;

        if ( HasFlag(wxNO_FULL_REPAINT_ON_RESIZE) )
        {
            wxPoint oldPos( m_x , m_y ) ;
            wxPoint newPos( actualX , actualY ) ;
            MacWindowToRootWindow( &oldPos.x , &oldPos.y ) ;
            MacWindowToRootWindow( &newPos.x , &newPos.y ) ;
            if ( oldPos == newPos )
            {
                partialRepaint = true ;
                RgnHandle oldRgn,newRgn,diffRgn ;
                oldRgn = NewRgn() ;
                newRgn = NewRgn() ;
                diffRgn = NewRgn() ;
                SetRectRgn(oldRgn , oldPos.x , oldPos.y , oldPos.x + m_width , oldPos.y + m_height ) ;
                SetRectRgn(newRgn , newPos.x , newPos.y , newPos.x + actualWidth , newPos.y + actualHeight ) ;
                DiffRgn( newRgn , oldRgn , diffRgn ) ;
                InvalWindowRgn( (WindowRef) MacGetRootWindow() , diffRgn ) ;
                DiffRgn( oldRgn , newRgn , diffRgn ) ;
                InvalWindowRgn( (WindowRef) MacGetRootWindow() , diffRgn ) ;
                DisposeRgn(oldRgn) ;
                DisposeRgn(newRgn) ;
                DisposeRgn(diffRgn) ;
            }
        }

        if ( !partialRepaint )
            Refresh() ;

        m_x = actualX ;
        m_y = actualY ;
        m_width = actualWidth ;
        m_height = actualHeight ;

		// update any low-level frame-relative positions

		MacUpdateDimensions() ;
        // erase new position

        if ( !partialRepaint )
            Refresh() ;
        if ( doMove )
            wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified

        MacRepositionScrollBars() ;
        if ( doMove )
        {
            wxPoint point(m_x, m_y);
            wxMoveEvent event(point, m_windowId);
            event.SetEventObject(this);
            GetEventHandler()->ProcessEvent(event) ;
        }
        if ( doResize )
        {
             MacRepositionScrollBars() ;
             wxSize size(m_width, m_height);
             wxSizeEvent event(size, m_windowId);
             event.SetEventObject(this);
             GetEventHandler()->ProcessEvent(event);
        }
    }

}

// set the size of the window: if the dimensions are positive, just use them,
// but if any of them is equal to -1, it means that we must find the value for
// it ourselves (unless sizeFlags contains wxSIZE_ALLOW_MINUS_ONE flag, in
// which case -1 is a valid value for x and y)
//
// If sizeFlags contains wxSIZE_AUTO_WIDTH/HEIGHT flags (default), we calculate
// the width/height to best suit our contents, otherwise we reuse the current
// width/height
void wxWindowMac::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // get the current size and position...
    int currentX, currentY;
    GetPosition(&currentX, &currentY);

    int currentW,currentH;
    GetSize(&currentW, &currentH);

    // ... and don't do anything (avoiding flicker) if it's already ok
    if ( x == currentX && y == currentY &&
         width == currentW && height == currentH )
    {
        MacRepositionScrollBars() ; // we might have a real position shift
        return;
    }

    if ( x == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        x = currentX;
    if ( y == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        y = currentY;

    AdjustForParentClientOrigin(x, y, sizeFlags);

    wxSize size(-1, -1);
    if ( width == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
        {
            size = DoGetBestSize();
            width = size.x;
        }
        else
        {
            // just take the current one
            width = currentW;
        }
    }

    if ( height == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
        {
            if ( size.x == -1 )
            {
                size = DoGetBestSize();
            }
            //else: already called DoGetBestSize() above

            height = size.y;
        }
        else
        {
            // just take the current one
            height = currentH;
        }
    }

    DoMoveWindow(x, y, width, height);

}
// For implementation purposes - sometimes decorations make the client area
// smaller

wxPoint wxWindowMac::GetClientAreaOrigin() const
{
    return wxPoint(MacGetLeftBorderSize(  ) , MacGetTopBorderSize(  ) );
}

void wxWindowMac::SetTitle(const wxString& title)
{
    m_label = title ;
}

wxString wxWindowMac::GetTitle() const
{
    return m_label ;
}

bool wxWindowMac::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return FALSE;

/*
    WindowRef window = (WindowRef) MacGetRootWindow() ;
    wxWindowMac* win = wxFindWinFromMacWindow( window ) ;
    if ( win == NULL && win->m_isBeingDeleted )
        return FALSE ;
*/
    MacSuperShown( show ) ;
    Refresh() ;
/*
    if ( !show )
    {
        if ( win && !win->m_isBeingDeleted )
            Refresh() ;
    }
    else
    {
        Refresh() ;
    }
*/
    return TRUE;
}

void wxWindowMac::MacSuperShown( bool show )
{
    wxNode *node = GetChildren().First();
    while ( node )
    {
        wxWindowMac *child = (wxWindowMac *)node->Data();
        if ( child->m_isShown )
            child->MacSuperShown( show ) ;
        node = node->Next();
    }
}

void wxWindowMac::MacSuperEnabled( bool enabled )
{
    if ( !IsTopLevel() )
    {
      // to be absolutely correct we'd have to invalidate (with eraseBkground
      // because unter MacOSX the frames are drawn with an addXXX mode)
      // the borders area
    }
    wxNode *node = GetChildren().First();
    while ( node )
    {
        wxWindowMac *child = (wxWindowMac *)node->Data();
        if ( child->m_isShown )
            child->MacSuperEnabled( enabled ) ;
        node = node->Next();
    }
}

bool wxWindowMac::MacIsReallyShown() const
{
    if ( m_isShown && (m_parent != NULL) ) {
        return m_parent->MacIsReallyShown();
    }
    return m_isShown;
/*
    bool status = m_isShown ;
    wxWindowMac * win = this ;
    while ( status && win->m_parent != NULL )
    {
        win = win->m_parent ;
        status = win->m_isShown ;
    }
    return status ;
*/
}

int wxWindowMac::GetCharHeight() const
{
    wxClientDC dc ( (wxWindowMac*)this ) ;
    return dc.GetCharHeight() ;
}

int wxWindowMac::GetCharWidth() const
{
    wxClientDC dc ( (wxWindowMac*)this ) ;
    return dc.GetCharWidth() ;
}

void wxWindowMac::GetTextExtent(const wxString& string, int *x, int *y,
                           int *descent, int *externalLeading, const wxFont *theFont ) const
{
    const wxFont *fontToUse = theFont;
    if ( !fontToUse )
        fontToUse = &m_font;

    wxClientDC dc( (wxWindowMac*) this ) ;
    long lx,ly,ld,le ;
    dc.GetTextExtent( string , &lx , &ly , &ld, &le, (wxFont *)fontToUse ) ;
    if ( externalLeading )
        *externalLeading = le ;
    if ( descent )
        *descent = ld ;
    if ( x )
        *x = lx ;
    if ( y )
        *y = ly ;
}

/*
 * Rect is given in client coordinates, for further reading, read wxTopLevelWindowMac::InvalidateRect
 * we always intersect with the entire window, not only with the client area
 */

void wxWindowMac::Refresh(bool eraseBack, const wxRect *rect)
{
    if ( MacGetTopLevelWindow() == NULL )
        return ;

    wxPoint client = GetClientAreaOrigin();
    int x1 = -client.x;
    int y1 = -client.y;
    int x2 = m_width - client.x;
    int y2 = m_height - client.y;

    if (IsKindOf( CLASSINFO(wxButton)))
    {
        // buttons have an "aura"
        y1 -= 5;
        x1 -= 5;
        y2 += 5;
        x2 += 5;
    }

    Rect clientrect = { y1, x1, y2, x2 };

    if ( rect )
    {
        Rect r = { rect->y , rect->x , rect->y + rect->height , rect->x + rect->width } ;
        SectRect( &clientrect , &r , &clientrect ) ;
    }

    if ( !EmptyRect( &clientrect ) )
    {
      int top = 0 , left = 0 ;

      MacClientToRootWindow( &left , &top ) ;
      OffsetRect( &clientrect , left , top ) ;

      MacGetTopLevelWindow()->MacInvalidate( &clientrect , eraseBack ) ;
    }
}

#if wxUSE_CARET && WXWIN_COMPATIBILITY
// ---------------------------------------------------------------------------
// Caret manipulation
// ---------------------------------------------------------------------------

void wxWindowMac::CreateCaret(int w, int h)
{
    SetCaret(new wxCaret(this, w, h));
}

void wxWindowMac::CreateCaret(const wxBitmap *WXUNUSED(bitmap))
{
    wxFAIL_MSG("not implemented");
}

void wxWindowMac::ShowCaret(bool show)
{
    wxCHECK_RET( m_caret, "no caret to show" );

    m_caret->Show(show);
}

void wxWindowMac::DestroyCaret()
{
    SetCaret(NULL);
}

void wxWindowMac::SetCaretPos(int x, int y)
{
    wxCHECK_RET( m_caret, "no caret to move" );

    m_caret->Move(x, y);
}

void wxWindowMac::GetCaretPos(int *x, int *y) const
{
    wxCHECK_RET( m_caret, "no caret to get position of" );

    m_caret->GetPosition(x, y);
}
#endif // wxUSE_CARET

wxWindowMac *wxGetActiveWindow()
{
    // actually this is a windows-only concept
    return NULL;
}

// Coordinates relative to the window
void wxWindowMac::WarpPointer (int x_pos, int y_pos)
{
    // We really dont move the mouse programmatically under mac
}

const wxBrush& wxWindowMac::MacGetBackgroundBrush()
{
    if ( m_backgroundColour == wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) )
    {
        m_macBackgroundBrush.SetMacTheme( kThemeBrushDocumentWindowBackground ) ;
    }
    else if (  m_backgroundColour == wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE ) )
    {
        // on mac we have the difficult situation, that 3dface gray can be different colours, depending whether
        // it is on a notebook panel or not, in order to take care of that we walk up the hierarchy until we have
        // either a non gray background color or a non control window

            WindowRef window = (WindowRef) MacGetRootWindow() ;

            wxWindowMac* parent = GetParent() ;
            while( parent )
            {
                if ( parent->MacGetRootWindow() != window )
                {
                    // we are in a different window on the mac system
                    parent = NULL ;
                    break ;
                }

                {
                    if ( parent->m_backgroundColour != wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE )
                      && parent->m_backgroundColour != wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) )
                    {
                        // if we have any other colours in the hierarchy
                        m_macBackgroundBrush.SetColour( parent->m_backgroundColour ) ;
                        break ;
                    }
                    // if we have the normal colours in the hierarchy but another control etc. -> use it's background
                    if ( parent->IsKindOf( CLASSINFO( wxNotebook ) ) || parent->IsKindOf( CLASSINFO( wxTabCtrl ) ))
                    {
                        Rect extent = { 0 , 0 , 0 , 0 } ;
                        int x , y ;
                        x = y = 0 ;
                        wxSize size = parent->GetSize() ;
                        parent->MacClientToRootWindow( &x , &y ) ;
                        extent.left = x ;
                        extent.top = y ;
                        extent.top-- ;
                        extent.right = x + size.x ;
                        extent.bottom = y + size.y ;
                        m_macBackgroundBrush.SetMacThemeBackground( kThemeBackgroundTabPane , (WXRECTPTR) &extent ) ; // todo eventually change for inactive
                        break ;
                    }
                }
                parent = parent->GetParent() ;
            }
            if ( !parent )
            {
              m_macBackgroundBrush.SetMacTheme( kThemeBrushDialogBackgroundActive ) ; // todo eventually change for inactive
            }
    }
    else
    {
        m_macBackgroundBrush.SetColour( m_backgroundColour ) ;
    }

    return m_macBackgroundBrush ;

}

void wxWindowMac::OnEraseBackground(wxEraseEvent& event)
{
    event.GetDC()->Clear() ;
}

void wxWindowMac::OnNcPaint( wxNcPaintEvent& event )
{
    wxWindowDC dc(this) ;
    wxMacPortSetter helper(&dc) ;

    MacPaintBorders( dc.m_macLocalOrigin.x , dc.m_macLocalOrigin.y) ;
}

int wxWindowMac::GetScrollPos(int orient) const
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           return m_hScrollBar->GetThumbPosition() ;
    }
    else
    {
       if ( m_vScrollBar )
           return m_vScrollBar->GetThumbPosition() ;
    }
    return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindowMac::GetScrollRange(int orient) const
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           return m_hScrollBar->GetRange() ;
    }
    else
    {
       if ( m_vScrollBar )
           return m_vScrollBar->GetRange() ;
    }
    return 0;
}

int wxWindowMac::GetScrollThumb(int orient) const
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           return m_hScrollBar->GetThumbSize() ;
    }
    else
    {
       if ( m_vScrollBar )
           return m_vScrollBar->GetThumbSize() ;
    }
    return 0;
}

void wxWindowMac::SetScrollPos(int orient, int pos, bool refresh)
{
    if ( orient == wxHORIZONTAL )
    {
       if ( m_hScrollBar )
           m_hScrollBar->SetThumbPosition( pos ) ;
    }
    else
    {
       if ( m_vScrollBar )
           m_vScrollBar->SetThumbPosition( pos ) ;
    }
}

void wxWindowMac::MacPaintBorders( int left , int top )
{
    if( IsTopLevel() )
        return ;

    RGBColor white = { 0xFFFF, 0xFFFF , 0xFFFF } ;
    RGBColor black = { 0x0000, 0x0000 , 0x0000 } ;
    RGBColor face = { 0xDDDD, 0xDDDD , 0xDDDD } ;
    RGBColor shadow = { 0x4444, 0x4444 , 0x4444 } ;
    PenNormal() ;

    if (HasFlag(wxRAISED_BORDER) || HasFlag( wxSUNKEN_BORDER) || HasFlag(wxDOUBLE_BORDER) )
    {
#if wxMAC_USE_THEME_BORDER
          Rect rect = { top , left , m_height + top , m_width + left } ;
          SInt32 border = 0 ;
          /*
          GetThemeMetric( kThemeMetricListBoxFrameOutset , &border ) ;
          InsetRect( &rect , border , border );
      DrawThemeListBoxFrame(&rect,IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
      */

        DrawThemePrimaryGroup(&rect  ,IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
#else
        bool sunken = HasFlag( wxSUNKEN_BORDER ) ;
        RGBForeColor( &face );
        MoveTo( left + 0 , top + m_height - 2 );
        LineTo( left + 0 , top + 0 );
        LineTo( left + m_width - 2 , top + 0 );

        MoveTo( left + 2 , top + m_height - 3 );
        LineTo( left + m_width - 3 , top + m_height - 3 );
        LineTo( left + m_width - 3 , top + 2 );

        RGBForeColor( sunken ? &face : &black );
        MoveTo( left + 0 , top + m_height - 1 );
        LineTo( left + m_width - 1 , top + m_height - 1 );
        LineTo( left + m_width - 1 , top + 0 );

        RGBForeColor( sunken ? &shadow : &white );
        MoveTo( left + 1 , top + m_height - 3 );
        LineTo( left + 1, top + 1 );
        LineTo( left + m_width - 3 , top + 1 );

        RGBForeColor( sunken ? &white : &shadow );
        MoveTo( left + 1 , top + m_height - 2 );
        LineTo( left + m_width - 2 , top + m_height - 2 );
        LineTo( left + m_width - 2 , top + 1 );

        RGBForeColor( sunken ? &black : &face );
        MoveTo( left + 2 , top + m_height - 4 );
        LineTo( left + 2 , top + 2 );
        LineTo( left + m_width - 4 , top + 2 );
#endif
    }
    else if (HasFlag(wxSIMPLE_BORDER))
    {
            Rect rect = { top , left , m_height + top , m_width + left } ;
        RGBForeColor( &black ) ;
        FrameRect( &rect ) ;
    }
}

void wxWindowMac::RemoveChild( wxWindowBase *child )
{
    if ( child == m_hScrollBar )
        m_hScrollBar = NULL ;
    if ( child == m_vScrollBar )
        m_vScrollBar = NULL ;

    wxWindowBase::RemoveChild( child ) ;
}

// New function that will replace some of the above.
void wxWindowMac::SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh)
{
        if ( orient == wxHORIZONTAL )
        {
            if ( m_hScrollBar )
            {
                if ( range == 0 || thumbVisible >= range )
                {
                    if ( m_hScrollBar->IsShown() )
                        m_hScrollBar->Show(false) ;
                }
                else
                {
                    if ( !m_hScrollBar->IsShown() )
                        m_hScrollBar->Show(true) ;
                    m_hScrollBar->SetScrollbar( pos , thumbVisible , range , thumbVisible , refresh ) ;
                }
            }
        }
        else
        {
            if ( m_vScrollBar )
            {
                if ( range == 0 || thumbVisible >= range )
                {
                    if ( m_vScrollBar->IsShown() )
                        m_vScrollBar->Show(false) ;
                }
                else
                {
                    if ( !m_vScrollBar->IsShown() )
                        m_vScrollBar->Show(true) ;
                    m_vScrollBar->SetScrollbar( pos , thumbVisible , range , thumbVisible , refresh ) ;
                }
            }
        }
        MacRepositionScrollBars() ;
}

// Does a physical scroll
void wxWindowMac::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    wxClientDC dc(this) ;
    wxMacPortSetter helper(&dc) ;

    {
        int width , height ;
        GetClientSize( &width , &height ) ;

        Rect scrollrect = { dc.YLOG2DEVMAC(0) , dc.XLOG2DEVMAC(0) , dc.YLOG2DEVMAC(height) , dc.XLOG2DEVMAC(width) } ;
        RgnHandle updateRgn = NewRgn() ;
        ClipRect( &scrollrect ) ;
        if ( rect )
        {
            Rect r = { dc.YLOG2DEVMAC(rect->y) , dc.XLOG2DEVMAC(rect->x) , dc.YLOG2DEVMAC(rect->y + rect->height) ,
              dc.XLOG2DEVMAC(rect->x + rect->width) } ;
            SectRect( &scrollrect , &r , &scrollrect ) ;
        }
        ScrollRect( &scrollrect , dx , dy , updateRgn ) ;
        InvalWindowRgn( (WindowRef) MacGetRootWindow() ,  updateRgn ) ;
        DisposeRgn( updateRgn ) ;
    }

    for (wxNode *node = GetChildren().First(); node; node = node->Next())
    {
        wxWindowMac *child = (wxWindowMac*)node->Data();
        if (child == m_vScrollBar) continue;
        if (child == m_hScrollBar) continue;
        if (child->IsTopLevel()) continue;

        int x,y;
        child->GetPosition( &x, &y );
        int w,h;
        child->GetSize( &w, &h );
        child->SetSize( x+dx, y+dy, w, h );
    }

}

void wxWindowMac::MacOnScroll(wxScrollEvent &event )
{
    if ( event.m_eventObject == m_vScrollBar || event.m_eventObject == m_hScrollBar )
    {
        wxScrollWinEvent wevent;
        wevent.SetPosition(event.GetPosition());
        wevent.SetOrientation(event.GetOrientation());
        wevent.m_eventObject = this;

        if (event.m_eventType == wxEVT_SCROLL_TOP) {
            wevent.m_eventType = wxEVT_SCROLLWIN_TOP;
        } else
        if (event.m_eventType == wxEVT_SCROLL_BOTTOM) {
            wevent.m_eventType = wxEVT_SCROLLWIN_BOTTOM;
        } else
        if (event.m_eventType == wxEVT_SCROLL_LINEUP) {
            wevent.m_eventType = wxEVT_SCROLLWIN_LINEUP;
        } else
        if (event.m_eventType == wxEVT_SCROLL_LINEDOWN) {
            wevent.m_eventType = wxEVT_SCROLLWIN_LINEDOWN;
        } else
        if (event.m_eventType == wxEVT_SCROLL_PAGEUP) {
            wevent.m_eventType = wxEVT_SCROLLWIN_PAGEUP;
        } else
        if (event.m_eventType == wxEVT_SCROLL_PAGEDOWN) {
            wevent.m_eventType = wxEVT_SCROLLWIN_PAGEDOWN;
        } else
        if (event.m_eventType == wxEVT_SCROLL_THUMBTRACK) {
            wevent.m_eventType = wxEVT_SCROLLWIN_THUMBTRACK;
        }

        GetEventHandler()->ProcessEvent(wevent);
    }
}

// Get the window with the focus
wxWindowMac *wxWindowBase::FindFocus()
{
    return gFocusWindow ;
}

#if WXWIN_COMPATIBILITY
// If nothing defined for this, try the parent.
// E.g. we may be a button loaded from a resource, with no callback function
// defined.
void wxWindowMac::OnCommand(wxWindowMac& win, wxCommandEvent& event)
{
    if ( GetEventHandler()->ProcessEvent(event)  )
        return;
    if ( m_parent )
        m_parent->GetEventHandler()->OnCommand(win, event);
}
#endif // WXWIN_COMPATIBILITY_2

#if WXWIN_COMPATIBILITY
wxObject* wxWindowMac::GetChild(int number) const
{
    // Return a pointer to the Nth object in the Panel
    wxNode *node = GetChildren().First();
    int n = number;
    while (node && n--)
        node = node->Next();
    if ( node )
    {
        wxObject *obj = (wxObject *)node->Data();
        return(obj);
    }
    else
        return NULL;
}
#endif // WXWIN_COMPATIBILITY

void wxWindowMac::OnSetFocus(wxFocusEvent& event)
{
    // panel wants to track the window which was the last to have focus in it,
    // so we want to set ourselves as the window which last had focus
    //
    // notice that it's also important to do it upwards the tree becaus
    // otherwise when the top level panel gets focus, it won't set it back to
    // us, but to some other sibling

    // CS:don't know if this is still needed:
    //wxChildFocusEvent eventFocus(this);
    //(void)GetEventHandler()->ProcessEvent(eventFocus);

    event.Skip();
}

void wxWindowMac::Clear()
{
    wxClientDC dc(this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

// Setup background and foreground colours correctly
void wxWindowMac::SetupColours()
{
    if ( GetParent() )
        SetBackgroundColour(GetParent()->GetBackgroundColour());
}

void wxWindowMac::OnIdle(wxIdleEvent& event)
{
/*
   // Check if we need to send a LEAVE event
   if (m_mouseInWindow)
   {
       POINT pt;
       ::GetCursorPos(&pt);
       if (::WindowFromPoint(pt) != (HWND) GetHWND())
       {
           // Generate a LEAVE event
           m_mouseInWindow = FALSE;
           MSWOnMouseLeave(pt.x, pt.y, 0);
       }
    }
*/

    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
    UpdateWindowUI();
}

// Raise the window to the top of the Z order
void wxWindowMac::Raise()
{
}

// Lower the window to the bottom of the Z order
void wxWindowMac::Lower()
{
}

void wxWindowMac::DoSetClientSize(int width, int height)
{
    if ( width != -1 || height != -1 )
    {

        if ( width != -1 && m_vScrollBar )
            width += MAC_SCROLLBAR_SIZE ;
        if ( height != -1 && m_vScrollBar )
            height += MAC_SCROLLBAR_SIZE ;

        width += MacGetLeftBorderSize(  ) + MacGetRightBorderSize( ) ;
        height += MacGetTopBorderSize(  ) + MacGetBottomBorderSize( ) ;

        DoSetSize( -1 , -1 , width , height ) ;
    }
}


wxWindowMac* wxWindowMac::s_lastMouseWindow = NULL ;

bool wxWindowMac::MacGetWindowFromPointSub( const wxPoint &point , wxWindowMac** outWin )
{
    if ( IsTopLevel() )
    {
      if ((point.x < 0) || (point.y < 0) ||
          (point.x > (m_width)) || (point.y > (m_height)))
          return FALSE;
    }
    else
    {
      if ((point.x < m_x) || (point.y < m_y) ||
          (point.x > (m_x + m_width)) || (point.y > (m_y + m_height)))
          return FALSE;
    }

    WindowRef window = (WindowRef) MacGetRootWindow() ;

    wxPoint newPoint( point ) ;

    if ( !IsTopLevel() )
    {
      newPoint.x -= m_x;
      newPoint.y -= m_y;
    }

    for (wxNode *node = GetChildren().First(); node; node = node->Next())
    {
        wxWindowMac *child = (wxWindowMac*)node->Data();
        // added the m_isShown test --dmazzoni
        if ( child->MacGetRootWindow() == window && child->m_isShown )
        {
            if (child->MacGetWindowFromPointSub(newPoint , outWin ))
                return TRUE;
        }
    }

    *outWin = this ;
    return TRUE;
}

bool wxWindowMac::MacGetWindowFromPoint( const wxPoint &screenpoint , wxWindowMac** outWin )
{
    WindowRef window ;

    Point pt = { screenpoint.y , screenpoint.x } ;
    if ( ::FindWindow( pt , &window ) == 3 )
    {

        wxWindowMac* win = wxFindWinFromMacWindow( window ) ;
        if ( win )
        {
            // No, this yields the CLIENT are, we need the whole frame. RR.
            // point = win->ScreenToClient( point ) ;

            GrafPtr     port;
            ::GetPort( &port ) ;
            ::SetPort( UMAGetWindowPort( window ) ) ;
            ::GlobalToLocal( &pt ) ;
            ::SetPort( port ) ;

            wxPoint point( pt.h, pt.v ) ;

            return win->MacGetWindowFromPointSub( point , outWin ) ;
        }
    }
    return FALSE ;
}

static wxWindow *gs_lastWhich = NULL;

bool wxWindowMac::MacSetupCursor( const wxPoint& pt)
{
    // first trigger a set cursor event

    wxPoint clientorigin = GetClientAreaOrigin() ;
    wxSize clientsize = GetClientSize() ;
    wxCursor cursor ;
    if ( wxRect2DInt( clientorigin.x , clientorigin.y , clientsize.x , clientsize.y ).Contains( wxPoint2DInt( pt ) ) )
    {
        wxSetCursorEvent event( pt.x , pt.y );

        bool processedEvtSetCursor = GetEventHandler()->ProcessEvent(event);
        if ( processedEvtSetCursor && event.HasCursor() )
        {
           cursor = event.GetCursor() ;
        }
        else
        {

            // the test for processedEvtSetCursor is here to prevent using m_cursor
            // if the user code caught EVT_SET_CURSOR() and returned nothing from
            // it - this is a way to say that our cursor shouldn't be used for this
            // point
            if ( !processedEvtSetCursor && m_cursor.Ok() )
            {
                cursor = m_cursor ;
            }
            if ( wxIsBusy() )
            {
            }
            else
            {
                if ( !GetParent() )
                    cursor = *wxSTANDARD_CURSOR  ;
            }
        }
        if ( cursor.Ok() )
            cursor.MacInstall() ;
    }
    return cursor.Ok() ;
}

bool wxWindowMac::MacDispatchMouseEvent(wxMouseEvent& event)
{
    if ((event.m_x < m_x) || (event.m_y < m_y) ||
        (event.m_x > (m_x + m_width)) || (event.m_y > (m_y + m_height)))
        return FALSE;


    if ( IsKindOf( CLASSINFO ( wxStaticBox ) ) /* || IsKindOf( CLASSINFO( wxSpinCtrl ) ) */)
        return FALSE ;

    WindowRef window = (WindowRef) MacGetRootWindow() ;

    event.m_x -= m_x;
    event.m_y -= m_y;

    int x = event.m_x ;
    int y = event.m_y ;

    for (wxNode *node = GetChildren().First(); node; node = node->Next())
    {
        wxWindowMac *child = (wxWindowMac*)node->Data();
        if ( child->MacGetRootWindow() == window && child->IsShown() && child->IsEnabled() )
        {
            if (child->MacDispatchMouseEvent(event))
                return TRUE;
        }
    }

    wxWindow* cursorTarget = this ;
    wxPoint cursorPoint( x , y ) ;

    while( cursorTarget && !cursorTarget->MacSetupCursor( cursorPoint ) )
    {
        cursorTarget = cursorTarget->GetParent() ;
        if ( cursorTarget )
            cursorPoint += cursorTarget->GetPosition() ;
    }
    event.m_x = x ;
    event.m_y = y ;
    event.SetEventObject( this ) ;

    if ( event.GetEventType() == wxEVT_LEFT_DOWN )
    {
        // set focus to this window
        if (AcceptsFocus() && FindFocus()!=this)
            SetFocus();
    }

#if wxUSE_TOOLTIPS
    if ( event.GetEventType() == wxEVT_MOTION
            || event.GetEventType() == wxEVT_ENTER_WINDOW
            || event.GetEventType() == wxEVT_LEAVE_WINDOW )
        wxToolTip::RelayEvent( this , event);
#endif // wxUSE_TOOLTIPS

    if (gs_lastWhich != this)
    {
        gs_lastWhich = this;

        // Double clicks must always occur on the same window
        if (event.GetEventType() == wxEVT_LEFT_DCLICK)
            event.SetEventType( wxEVT_LEFT_DOWN );
        if (event.GetEventType() == wxEVT_RIGHT_DCLICK)
            event.SetEventType( wxEVT_RIGHT_DOWN );

        // Same for mouse up events
        if (event.GetEventType() == wxEVT_LEFT_UP)
            return TRUE;
        if (event.GetEventType() == wxEVT_RIGHT_UP)
            return TRUE;
    }

    GetEventHandler()->ProcessEvent( event ) ;

    return TRUE;
}

wxString wxWindowMac::MacGetToolTipString( wxPoint &pt )
{
    if ( m_tooltip )
    {
        return m_tooltip->GetTip() ;
    }
    return "" ;
}

void wxWindowMac::Update()
{
    wxTopLevelWindowMac* win = MacGetTopLevelWindow(  ) ;
    if ( win )
    {
      win->MacUpdate( 0 ) ;
#if TARGET_API_MAC_CARBON
        if ( QDIsPortBuffered( GetWindowPort( (WindowRef) win->MacGetWindowRef() ) ) )
        {
                QDFlushPortBuffer( GetWindowPort( (WindowRef) win->MacGetWindowRef() ) , NULL ) ;
        }
#endif
      }
}

wxTopLevelWindowMac* wxWindowMac::MacGetTopLevelWindow() const
{
    wxTopLevelWindowMac* win = NULL ;
    WindowRef window = (WindowRef) MacGetRootWindow() ;
    if ( window )
    {
        win = wxFindWinFromMacWindow( window ) ;
    }
    return win ;
}

const wxRegion& wxWindowMac::MacGetVisibleRegion( bool respectChildrenAndSiblings )
{
  RgnHandle visRgn = NewRgn() ;
  RgnHandle tempRgn = NewRgn() ;
  RgnHandle tempStaticBoxRgn = NewRgn() ;

  SetRectRgn( visRgn , 0 , 0 , m_width , m_height ) ;

  //TODO : as soon as the new scheme has proven to work correctly, move this to wxStaticBox
  if ( IsKindOf( CLASSINFO( wxStaticBox ) ) )
  {
    int borderTop = 14 ;
    int borderOther = 4 ;

    SetRectRgn( tempStaticBoxRgn , borderOther , borderTop , m_width - borderOther , m_height - borderOther ) ;
    DiffRgn( visRgn , tempStaticBoxRgn , visRgn ) ;
  }

  if ( !IsTopLevel() )
  {
    wxWindow* parent = GetParent() ;
    while( parent )
    {
      wxSize size = parent->GetSize() ;
      int x , y ;
      x = y = 0 ;
      parent->MacWindowToRootWindow( &x, &y ) ;
      MacRootWindowToWindow( &x , &y ) ;

      SetRectRgn( tempRgn ,
      	x + parent->MacGetLeftBorderSize() , y + parent->MacGetTopBorderSize() ,
      	x + size.x - parent->MacGetRightBorderSize(),
      	y + size.y - parent->MacGetBottomBorderSize()) ;

      SectRgn( visRgn , tempRgn , visRgn ) ;
      if ( parent->IsTopLevel() )
        break ;
      parent = parent->GetParent() ;
    }
  }
  if ( respectChildrenAndSiblings )
  {
      if ( GetWindowStyle() & wxCLIP_CHILDREN )
      {
        for (wxNode *node = GetChildren().First(); node; node = node->Next())
        {
            wxWindowMac *child = (wxWindowMac*)node->Data();

            if ( !child->IsTopLevel() && child->IsShown() )
            {
                SetRectRgn( tempRgn , child->m_x , child->m_y , child->m_x + child->m_width ,  child->m_y + child->m_height ) ;
                if ( child->IsKindOf( CLASSINFO( wxStaticBox ) ) )
                {
                    int borderTop = 14 ;
                    int borderOther = 4 ;

                    SetRectRgn( tempStaticBoxRgn , child->m_x + borderOther , child->m_y + borderTop , child->m_x + child->m_width - borderOther , child->m_y + child->m_height - borderOther ) ;
                    DiffRgn( tempRgn , tempStaticBoxRgn , tempRgn ) ;
                }
                DiffRgn( visRgn , tempRgn , visRgn ) ;
            }
        }
      }

      if ( (GetWindowStyle() & wxCLIP_SIBLINGS) && GetParent() )
      {
        bool thisWindowThrough = false ;
        for (wxNode *node = GetParent()->GetChildren().First(); node; node = node->Next())
        {
            wxWindowMac *sibling = (wxWindowMac*)node->Data();
            if ( sibling == this )
            {
              thisWindowThrough = true ;
              continue ;
            }
            if( !thisWindowThrough )
            {
              continue ;
            }

            if ( !sibling->IsTopLevel() && sibling->IsShown() )
            {
                SetRectRgn( tempRgn , sibling->m_x - m_x , sibling->m_y - m_y , sibling->m_x + sibling->m_width - m_x ,  sibling->m_y + sibling->m_height - m_y ) ;
                if ( sibling->IsKindOf( CLASSINFO( wxStaticBox ) ) )
                {
                    int borderTop = 14 ;
                    int borderOther = 4 ;

                    SetRectRgn( tempStaticBoxRgn , sibling->m_x - m_x + borderOther , sibling->m_y - m_y + borderTop , sibling->m_x + sibling->m_width - m_x - borderOther , sibling->m_y + sibling->m_height - m_y - borderOther ) ;
                    DiffRgn( tempRgn , tempStaticBoxRgn , tempRgn ) ;
                }
                DiffRgn( visRgn , tempRgn , visRgn ) ;
            }
        }
      }
  }
  m_macVisibleRegion = visRgn ;
  DisposeRgn( visRgn ) ;
  DisposeRgn( tempRgn ) ;
  DisposeRgn( tempStaticBoxRgn ) ;
  return m_macVisibleRegion ;
}

void wxWindowMac::MacRedraw( WXHRGN updatergnr , long time, bool erase)
{
    RgnHandle updatergn = (RgnHandle) updatergnr ;
    // updatergn is always already clipped to our boundaries
    // it is in window coordinates, not in client coordinates

    WindowRef window = (WindowRef) MacGetRootWindow() ;

    {
        // ownUpdateRgn is the area that this window has to repaint, it is in window coordinates
        RgnHandle ownUpdateRgn = NewRgn() ;
        CopyRgn( updatergn , ownUpdateRgn ) ;

        SectRgn( ownUpdateRgn , (RgnHandle) MacGetVisibleRegion().GetWXHRGN() , ownUpdateRgn ) ;

        // newupdate is the update region in client coordinates
        RgnHandle newupdate = NewRgn() ;
        wxSize point = GetClientSize() ;
        wxPoint origin = GetClientAreaOrigin() ;
        SetRectRgn( newupdate , origin.x , origin.y , origin.x + point.x , origin.y+point.y ) ;
        SectRgn( newupdate , ownUpdateRgn , newupdate ) ;
        OffsetRgn( newupdate , -origin.x , -origin.y ) ;
        m_updateRegion = newupdate ;
        DisposeRgn( newupdate ) ; // it's been cloned to m_updateRegion

        if ( erase && !EmptyRgn(ownUpdateRgn) )
        {
          wxWindowDC dc(this);
          dc.SetClippingRegion(wxRegion(ownUpdateRgn));
          wxEraseEvent eevent( GetId(), &dc );
          eevent.SetEventObject( this );
          GetEventHandler()->ProcessEvent( eevent );

          wxNcPaintEvent eventNc( GetId() );
          eventNc.SetEventObject( this );
          GetEventHandler()->ProcessEvent( eventNc );
        }
        DisposeRgn( ownUpdateRgn ) ;
        if ( !m_updateRegion.Empty() )
        {
          wxPaintEvent event;
          event.m_timeStamp = time ;
          event.SetEventObject(this);
          GetEventHandler()->ProcessEvent(event);
        }
    }

    // now intersect for each of the children their rect with the updateRgn and call MacRedraw recursively

    RgnHandle childupdate = NewRgn() ;
    for (wxNode *node = GetChildren().First(); node; node = node->Next())
    {
        // calculate the update region for the child windows by intersecting the window rectangle with our own
        // passed in update region and then offset it to be client-wise window coordinates again
        wxWindowMac *child = (wxWindowMac*)node->Data();
        SetRectRgn( childupdate , child->m_x , child->m_y , child->m_x + child->m_width ,  child->m_y + child->m_height ) ;
        SectRgn( childupdate , updatergn , childupdate ) ;
        OffsetRgn( childupdate , -child->m_x , -child->m_y ) ;
        if ( child->MacGetRootWindow() == window && child->IsShown() && !EmptyRgn( childupdate ) )
        {
            // because dialogs may also be children
            child->MacRedraw( childupdate , time , erase ) ;
        }
    }
    DisposeRgn( childupdate ) ;
    // eventually a draw grow box here

}

WXHWND wxWindowMac::MacGetRootWindow() const
{
    wxWindowMac *iter = (wxWindowMac*)this ;

    while( iter )
    {
        if ( iter->IsTopLevel() )
            return ((wxTopLevelWindow*)iter)->MacGetWindowRef() ;

        iter = iter->GetParent() ;
    }
    wxASSERT_MSG( 1 , "No valid mac root window" ) ;
    return NULL ;
}

void wxWindowMac::MacCreateScrollBars( long style )
{
    wxASSERT_MSG( m_vScrollBar == NULL && m_hScrollBar == NULL , "attempt to create window twice" ) ;

    bool hasBoth = ( style & wxVSCROLL ) && ( style & wxHSCROLL ) ;
    int adjust = hasBoth ? MAC_SCROLLBAR_SIZE - 1: 0 ;
    int width, height ;
    GetClientSize( &width , &height ) ;

    wxPoint vPoint(width-MAC_SCROLLBAR_SIZE, 0) ;
    wxSize vSize(MAC_SCROLLBAR_SIZE, height - adjust) ;
    wxPoint hPoint(0 , height-MAC_SCROLLBAR_SIZE ) ;
    wxSize hSize( width - adjust, MAC_SCROLLBAR_SIZE) ;

    m_vScrollBar = new wxScrollBar(this, wxWINDOW_VSCROLL, vPoint,
        vSize , wxVERTICAL);

    if ( style & wxVSCROLL )
    {

    }
    else
    {
        m_vScrollBar->Show(false) ;
    }
    m_hScrollBar = new wxScrollBar(this, wxWINDOW_HSCROLL, hPoint,
        hSize , wxHORIZONTAL);
    if ( style  & wxHSCROLL )
    {
    }
    else
    {
        m_hScrollBar->Show(false) ;
    }

    // because the create does not take into account the client area origin
    MacRepositionScrollBars() ; // we might have a real position shift
}

void wxWindowMac::MacRepositionScrollBars()
{
    bool hasBoth = ( m_hScrollBar && m_hScrollBar->IsShown()) && ( m_vScrollBar && m_vScrollBar->IsShown()) ;
    int adjust = hasBoth ? MAC_SCROLLBAR_SIZE - 1 : 0 ;

    // get real client area

    int width = m_width ;
    int height = m_height ;

    width -= MacGetLeftBorderSize() + MacGetRightBorderSize();
    height -= MacGetTopBorderSize() + MacGetBottomBorderSize();

    wxPoint vPoint(width-MAC_SCROLLBAR_SIZE, 0) ;
    wxSize vSize(MAC_SCROLLBAR_SIZE, height - adjust) ;
    wxPoint hPoint(0 , height-MAC_SCROLLBAR_SIZE ) ;
    wxSize hSize( width - adjust, MAC_SCROLLBAR_SIZE) ;

    int x = 0 ;
    int y = 0 ;
    int w = m_width ;
    int h = m_height ;

    MacClientToRootWindow( &x , &y ) ;
    MacClientToRootWindow( &w , &h ) ;

    wxWindowMac *iter = (wxWindowMac*)this ;

    int totW = 10000 , totH = 10000;
    while( iter )
    {
        if ( iter->IsTopLevel() )
        {
            totW = iter->m_width ;
            totH = iter->m_height ;
            break ;
        }

        iter = iter->GetParent() ;
    }

    if ( x == 0 )
    {
        hPoint.x = -1 ;
        hSize.x += 1 ;
    }
    if ( y == 0 )
    {
        vPoint.y = -1 ;
        vSize.y += 1 ;
    }

    if ( w-x >= totW )
    {
        hSize.x += 1 ;
        vPoint.x += 1 ;
    }

    if ( h-y >= totH )
    {
        vSize.y += 1 ;
        hPoint.y += 1 ;
    }

    if ( m_vScrollBar )
    {
        m_vScrollBar->SetSize( vPoint.x , vPoint.y, vSize.x, vSize.y , wxSIZE_ALLOW_MINUS_ONE);
    }
    if ( m_hScrollBar )
    {
        m_hScrollBar->SetSize( hPoint.x , hPoint.y, hSize.x, hSize.y, wxSIZE_ALLOW_MINUS_ONE);
    }
}

bool wxWindowMac::AcceptsFocus() const
{
    return MacCanFocus() && wxWindowBase::AcceptsFocus();
}

WXWidget wxWindowMac::MacGetContainerForEmbedding()
{
    return GetParent()->MacGetContainerForEmbedding() ;
}

void wxWindowMac::MacSuperChangedPosition()
{
    // only window-absolute structures have to be moved i.e. controls

    wxNode *node = GetChildren().First();
    while ( node )
    {
        wxWindowMac *child = (wxWindowMac *)node->Data();
        child->MacSuperChangedPosition() ;
        node = node->Next();
    }
}

void wxWindowMac::MacTopLevelWindowChangedPosition()
{
    // only screen-absolute structures have to be moved i.e. glcanvas

    wxNode *node = GetChildren().First();
    while ( node )
    {
        wxWindowMac *child = (wxWindowMac *)node->Data();
        child->MacTopLevelWindowChangedPosition() ;
        node = node->Next();
    }
}
long wxWindowMac::MacGetLeftBorderSize( ) const
{
    if( IsTopLevel() )
        return 0 ;

    if (m_windowStyle & wxRAISED_BORDER || m_windowStyle & wxSUNKEN_BORDER )
    {
        SInt32 border = 3 ;
#if wxMAC_USE_THEME_BORDER
#if TARGET_CARBON
          GetThemeMetric( kThemeMetricListBoxFrameOutset , &border ) ;
#endif
#endif
          return border ;
    }
    else if (  m_windowStyle &wxDOUBLE_BORDER)
    {
          SInt32 border = 3 ;
#if wxMAC_USE_THEME_BORDER
#if TARGET_CARBON
          GetThemeMetric( kThemeMetricListBoxFrameOutset , &border ) ;
#endif
#endif
          return border ;
    }
    else if (m_windowStyle &wxSIMPLE_BORDER)
    {
        return 1 ;
    }
    return 0 ;
}

long wxWindowMac::MacGetRightBorderSize( ) const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacGetTopBorderSize( ) const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacGetBottomBorderSize( ) const
{
    // they are all symmetric in mac themes
    return MacGetLeftBorderSize() ;
}

long wxWindowMac::MacRemoveBordersFromStyle( long style )
{
    return style & ~( wxDOUBLE_BORDER | wxSUNKEN_BORDER | wxRAISED_BORDER | wxBORDER | wxSTATIC_BORDER ) ;
}

// Find the wxWindowMac at the current mouse position, returning the mouse
// position.
wxWindowMac* wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxGetMousePosition();
    wxWindowMac* found = wxFindWindowAtPoint(pt);
    return found;
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    int x, y;
    wxGetMousePosition(& x, & y);
    return wxPoint(x, y);
}

