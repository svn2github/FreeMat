///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     declaration of the wxDropTarget class
// Author:      David Webster
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 David Webster
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////


#ifndef __OS2DNDH__
#define __OS2DNDH__

#if !wxUSE_DRAG_AND_DROP
    #error  "You should #define wxUSE_DRAG_AND_DROP to 1 to compile this file!"
#endif  //WX_DRAG_DROP

#define INCL_WINSTDDRAG
#include <os2.h>
#ifndef __EMX__
#include <pmstddlg.h>
#endif

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropSource: public wxDropSourceBase
{
public:
    /* constructor. set data later with SetData() */
    wxDropSource(wxWindow* pWin);

    /* constructor for setting one data object */
    wxDropSource( wxDataObject& rData,
                  wxWindow*     pWin
                );
    virtual ~wxDropSource();

    /* start drag action */
    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);

protected:
    void Init(void);
    bool                            m_bLazyDrag;

    DRAGIMAGE*                      m_pDragImage;
    DRAGINFO*                       m_pDragInfo;
    DRAGTRANSFER*                   m_pDragTransfer;
};

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropTarget: public wxDropTargetBase
{
public:
    wxDropTarget(wxDataObject *dataObject = (wxDataObject*)NULL);
    virtual ~wxDropTarget();

    void Register(WXHWND hwnd);
    void Revoke(WXHWND hwnd);

    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDrop(wxCoord x, wxCoord y);
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult vResult);
    virtual bool GetData();

  // implementation
protected:
    virtual bool IsAcceptable(DRAGINFO* pInfo);

    DRAGINFO*                       m_pDragInfo;
    DRAGTRANSFER*                   m_pDragTransfer;
};

#endif //__OS2DNDH__

