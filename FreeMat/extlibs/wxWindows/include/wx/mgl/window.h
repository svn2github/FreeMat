/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mgl/window.h
// Purpose:     wxWindow class
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "window.h"
#endif

#include "wx/font.h"

struct window_t;
class MGLDevCtx;

// ---------------------------------------------------------------------------
// wxWindow declaration for MGL
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindowMGL : public wxWindowBase
{
public:
    wxWindowMGL() { Init(); }

    wxWindowMGL(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindowMGL();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    virtual void Raise();
    virtual void Lower();

    virtual bool Show(bool show = TRUE);

    virtual void SetFocus();

    virtual bool Reparent(wxWindowBase *newParent);

    virtual void WarpPointer(int x, int y);

    virtual void Refresh(bool eraseBackground = TRUE,
                         const wxRect *rect = (const wxRect *) NULL);
    virtual void Update();
    virtual void Clear();
    virtual void Freeze();
    virtual void Thaw();

    virtual bool SetCursor(const wxCursor &cursor);
    virtual bool SetFont(const wxFont &font) { m_font = font; return TRUE; }

    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const;

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget(wxDropTarget *dropTarget);
#endif // wxUSE_DRAG_AND_DROP

    // Accept files for dragging
    virtual void DragAcceptFiles(bool accept);

#if WXWIN_COMPATIBILITY
    // event handlers
        // Handle a control command
    virtual void OnCommand(wxWindow& win, wxCommandEvent& event);

        // Override to define new behaviour for default action (e.g. double
        // clicking on a listbox)
    virtual void OnDefaultAction(wxControl * WXUNUSED(initiatingItem)) { }
#endif // WXWIN_COMPATIBILITY

    virtual WXWidget GetHandle() const { return m_wnd; }
    
    void SetMGLwindow_t(struct window_t *wnd);

    // implementation from now on
    // --------------------------

protected:
    // the window handle
    struct window_t      *m_wnd;
    // whether there should be wxEraseEvent before wxPaintEvent or not
    // (see wxWindow::Refresh)
    bool                  m_frozen:1;
    bool                  m_refreshAfterThaw:1;
    int                   m_eraseBackground;

    // implement the base class pure virtuals
    virtual void DoClientToScreen( int *x, int *y ) const;
    virtual void DoScreenToClient( int *x, int *y ) const;
    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetClientSize( int *width, int *height ) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow(int x, int y, int width, int height);
    
    void OnIdle(wxIdleEvent& event);

private:
    // common part of all ctors
    void Init();
    // counterpart to SetFocus
    void KillFocus();
    
    MGLDevCtx *m_paintMGLDC;
    friend class wxPaintDC;

    DECLARE_DYNAMIC_CLASS(wxWindowMGL)
    DECLARE_NO_COPY_CLASS(wxWindowMGL)
    DECLARE_EVENT_TABLE()

public:
    void HandlePaint(MGLDevCtx *dc);
    // needed by wxWindowPainter
    MGLDevCtx *GetPaintMGLDC() const { return m_paintMGLDC; }
};


#endif
    // _WX_WINDOW_H_
