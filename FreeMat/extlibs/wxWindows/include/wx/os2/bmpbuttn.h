/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTN_H_
#define _WX_BMPBUTTN_H_

#include "wx/button.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonNameStr;

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLEXPORT wxBitmapButton: public wxBitmapButtonBase
{
public:
    inline wxBitmapButton()
    {
        m_marginX = wxDEFAULT_BUTTON_MARGIN;
        m_marginY = wxDEFAULT_BUTTON_MARGIN;
    }
    inline wxBitmapButton( wxWindow*          pParent
                          ,wxWindowID         vId
                          ,const wxBitmap&    rBitmap
                          ,const wxPoint&     rPos = wxDefaultPosition
                          ,const wxSize&      rSize = wxDefaultSize
                          ,long               lStyle = wxBU_AUTODRAW
                          ,const wxValidator& rValidator = wxDefaultValidator
                          ,const wxString&    rsName = wxButtonNameStr
                         )
    {
        Create( pParent
               ,vId
               ,rBitmap
               ,rPos
               ,rSize
               ,lStyle
               ,rValidator
               ,rsName
              );
    }

    bool Create( wxWindow*          pParent
                ,wxWindowID         vId
                ,const wxBitmap&    rBitmap
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,long               lStyle = wxBU_AUTODRAW
                ,const wxValidator& rValidator = wxDefaultValidator
                ,const wxString&    rsName = wxButtonNameStr
               );

    virtual void SetLabel(const wxBitmap& rBitmap)
    {
        SetBitmapLabel(rBitmap);
    }

#if WXWIN_COMPATIBILITY
    wxBitmap* GetBitmap(void) const
    {
        return (wxBitmap *)&m_buttonBitmap;
    }
#endif

    //
    // Implementation
    //
    virtual void SetDefault();
    virtual bool OS2OnDraw(WXDRAWITEMSTRUCT* pItem);

private:

    virtual void DrawFace( wxClientDC& rDC
                          ,bool        bSel
                         );
    virtual void DrawButtonFocus(wxClientDC& rDC);
    virtual void DrawButtonDisable( wxClientDC& rDC
                                   ,wxBitmap&   rBmp
                                  );
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)

    virtual void SetLabel(const wxString& rsString)
    {
        wxButton::SetLabel(rsString);
    }
}; // end of CLASS wxBitmapButton

#endif // _WX_BMPBUTTN_H_

