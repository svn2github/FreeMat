/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:     wxIcon class
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// compatible (even if incorrect) behaviour by default: derive wxIcon from
// wxBitmap
#ifndef wxICON_IS_BITMAP
    #define wxICON_IS_BITMAP 1
#endif

#include "wx/bitmap.h"
#if wxICON_IS_BITMAP

    #define wxIconRefDataBase   wxBitmapRefData
    #define wxIconBase          wxBitmap
#else
    #include "wx/os2/gdiimage.h"

    #define wxIconRefDataBase   wxGDIImageRefData
    #define wxIconBase          wxGDIImage
#endif

class WXDLLEXPORT wxIconRefData: public wxIconRefDataBase
{
public:
    wxIconRefData() { };
    virtual ~wxIconRefData() { Free(); }

    virtual void Free();
}; // end of

// ---------------------------------------------------------------------------
// Icon
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxIcon: public wxIconBase
{
public:
    wxIcon();

    // Copy constructors
    inline wxIcon(const wxIcon& icon) { Ref(icon); }

    wxIcon( const char bits[]
           ,int        nWidth
           ,int        nHeight
          );
    inline wxIcon(const char** ppData) { CreateIconFromXpm(ppData); }
    inline wxIcon(char** ppData) { CreateIconFromXpm((const char**)ppData); }
    wxIcon( const wxString& rName
           ,long            lFlags = wxBITMAP_TYPE_ICO_RESOURCE
           ,int             nDesiredWidth = -1
           ,int             nDesiredHeight = -1
          );
    ~wxIcon();

    bool LoadFile( const wxString& rName
                  ,long            lFlags = wxBITMAP_TYPE_ICO_RESOURCE
                  ,int             nDesiredWidth = -1
                  ,int             nDesiredHeight = -1
                 );

    inline wxIcon& operator = (const wxIcon& rIcon)
       { if (*this != rIcon) Ref(rIcon); return *this; }
    inline bool operator == (const wxIcon& rIcon)
       { return m_refData == rIcon.m_refData; }
    inline bool operator != (const wxIcon& rIcon)
       { return m_refData != rIcon.m_refData; }

    wxIconRefData *GetIconData() const { return (wxIconRefData *)m_refData; }

    inline void SetHICON(WXHICON hIcon) { SetHandle((WXHANDLE)hIcon); }
    inline WXHICON GetHICON() const { return (WXHICON)GetHandle(); }
    inline bool    IsXpm(void) const { return m_bIsXpm; };
    inline const wxBitmap& GetXpmSrc(void) const { return m_vXpmSrc; }

    void CopyFromBitmap(const wxBitmap& rBmp);
protected:
    virtual wxGDIImageRefData* CreateData() const
    {
        return new wxIconRefData;
    }
    void    CreateIconFromXpm(const char **ppData);

private:
    bool                            m_bIsXpm;
    wxBitmap                        m_vXpmSrc;

    DECLARE_DYNAMIC_CLASS(wxIcon)
};

#endif
    // _WX_ICON_H_
