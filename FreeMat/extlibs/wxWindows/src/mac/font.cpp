/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "font.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"

#include "wx/fontutil.h"

#include "wx/mac/private.h"
#include "ATSUnicode.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         int family,
                         int style,
                         int weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_style = style;
    m_pointSize = pointSize;
    m_family = family;
    m_style = style;
    m_weight = weight;
    m_underlined = underlined;
    m_faceName = faceName;
    m_encoding = encoding;

    m_macFontNum = 0 ;
    m_macFontSize = 0;
    m_macFontStyle = 0;
    m_fontId = 0;
}

wxFontRefData::~wxFontRefData()
{
}

void wxFontRefData::MacFindFont()
{
	if( m_faceName == "" )
	{
		switch( m_family )
		{
			case wxDEFAULT :
				m_macFontNum = ::GetAppFont() ;
				break ;
			case wxDECORATIVE :
				::GetFNum( "\pTimes" , &m_macFontNum) ;
				break ;
			case wxROMAN :
				::GetFNum( "\pTimes" , &m_macFontNum) ;
				break ;
			case wxSCRIPT :
				::GetFNum( "\pTimes" , &m_macFontNum) ;
				break ;
			case wxSWISS :
				::GetFNum( "\pGeneva" , &m_macFontNum) ;
				break ;
			case wxMODERN :
				::GetFNum( "\pMonaco" , &m_macFontNum) ;
				break ;
		}
		Str255 name ;
		GetFontName( m_macFontNum , name ) ;
		CopyPascalStringToC( name , (char*) name ) ;
		m_faceName = (char*) name ;
	}
	else
	{
		if ( m_faceName == "systemfont" )
			m_macFontNum = ::GetSysFont() ;
		else if ( m_faceName == "applicationfont" )
			m_macFontNum = ::GetAppFont() ;
		else
		{
#if TARGET_CARBON
			c2pstrcpy( (StringPtr) wxBuffer, m_faceName ) ;
#else
			strcpy( (char *) wxBuffer, m_faceName ) ;
			c2pstr( (char *) wxBuffer ) ;
#endif
			::GetFNum( (StringPtr) wxBuffer, &m_macFontNum);
		}
	}

	m_macFontStyle = 0;
	if (m_weight == wxBOLD)
		 m_macFontStyle |= bold;
	if (m_style == wxITALIC || m_style == wxSLANT) 
		m_macFontStyle |= italic;
	if (m_underlined) 
		m_macFontStyle |= underline;
	m_macFontSize = m_pointSize ;
	
	//TODO:if we supply the style as an additional parameter we must make a testing
	//sequence in order to degrade gracefully while trying to maintain most of the style
	//information, meanwhile we just take the normal font and apply the features after
	OSStatus status = ::ATSUFONDtoFontID(m_macFontNum, normal /*qdStyle*/, (UInt32*)&m_macATSUFontID); 
    /*
    status = ATSUFindFontFromName ( (Ptr) m_faceName , strlen( m_faceName ) ,
        kFontFullName,	kFontMacintoshPlatform, kFontRomanScript , kFontNoLanguage  ,  (UInt32*)&m_macATSUFontID ) ;
    */
	wxASSERT_MSG( status == noErr , "couldn't retrieve font identifier" ) ;
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

void wxFont::Init()
{
}

bool wxFont::Create(const wxNativeFontInfo& info)
{
    return Create(info.pointSize, info.family, info.style, info.weight,
                  info.underlined, info.faceName, info.encoding);
}

wxFont::wxFont(const wxString& fontdesc)
{
    wxNativeFontInfo info;
    if ( info.FromString(fontdesc) )
        (void)Create(info);
}

bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& faceName,
                    wxFontEncoding encoding)
{
    UnRef();
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, faceName, encoding);

    RealizeResource();

    return TRUE;
}

wxFont::~wxFont()
{
}

bool wxFont::RealizeResource()
{
	M_FONTDATA->MacFindFont() ;
    return TRUE;
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->m_encoding = encoding;

    RealizeResource();
}

void wxFont::Unshare()
{
	// Don't change shared data
	if (!m_refData)
    {
		m_refData = new wxFontRefData();
	}
    else
    {
		wxFontRefData* ref = new wxFontRefData(*(wxFontRefData*)m_refData);
		UnRef();
		m_refData = ref;
	}
}

void wxFont::SetPointSize(int pointSize)
{
    Unshare();

    M_FONTDATA->m_pointSize = pointSize;

    RealizeResource();
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->m_family = family;

    RealizeResource();
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->m_style = style;

    RealizeResource();
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->m_weight = weight;

    RealizeResource();
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_faceName = faceName;

    RealizeResource();
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->m_underlined = underlined;

    RealizeResource();
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    return M_FONTDATA->m_pointSize;
}

int wxFont::GetFamily() const
{
    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const
{
    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA->m_underlined;
}

wxString wxFont::GetFaceName() const
{
    wxString str;
    if ( M_FONTDATA )
        str = M_FONTDATA->m_faceName ;
    return str;
}

wxFontEncoding wxFont::GetEncoding() const
{
    return M_FONTDATA->m_encoding;
}

