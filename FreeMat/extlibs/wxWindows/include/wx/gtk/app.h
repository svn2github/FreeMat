/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/app.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKAPPH__
#define __GTKAPPH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/frame.h"
#include "wx/icon.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxApp;
class wxLog;

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

class wxApp: public wxAppBase
{
public:
    wxApp();
    virtual ~wxApp();

    /* override for altering the way wxGTK intializes the GUI
     * (palette/visual/colorcube). under wxMSW, OnInitGui() does nothing by
     * default. when overriding this method, the code in it is likely to be
     * platform dependent, otherwise use OnInit(). */
    virtual bool OnInitGui();

    // override base class (pure) virtuals
    virtual int MainLoop();
    virtual void ExitMainLoop();
    virtual bool Initialized();
    virtual bool Pending();
    virtual void Dispatch();
    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual bool ProcessIdle();

    // implementation only from now on
    void OnIdle( wxIdleEvent &event );
    bool SendIdleEvents();
    bool SendIdleEvents( wxWindow* win );

    static bool Initialize();
    static bool InitialzeVisual();
    static void CleanUp();

    void DeletePendingObjects();

#ifdef __WXDEBUG__
    virtual void OnAssert(const wxChar *file, int line, const wxChar *cond, const wxChar *msg);

    bool IsInAssert() const { return m_isInAssert; }
#endif // __WXDEBUG__

    bool            m_initialized;

    gint            m_idleTag;
#if wxUSE_THREADS
    gint            m_wakeUpTimerTag;
#endif
    unsigned char  *m_colorCube;

    // Used by the the wxGLApp and wxGLCanvas class for GL-based X visual
    // selection; this is actually an XVisualInfo*
    void           *m_glVisualInfo;
    // This returns the current visual: either that used by wxRootWindow
    // or the XVisualInfo* for SGI.
    GdkVisual      *GetGdkVisual();

private:
    // true if we're inside an assert modal dialog
#ifdef __WXDEBUG__
    bool m_isInAssert;
#endif // __WXDEBUG__

    bool CallInternalIdle( wxWindow* win );

    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

int WXDLLEXPORT wxEntry( int argc, char *argv[] );

#endif // __GTKAPPH__
