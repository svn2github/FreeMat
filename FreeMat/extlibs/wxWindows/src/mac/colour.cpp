/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:     wxColour class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "colour.h"
#endif

#include "wx/gdicmn.h"
#include "wx/colour.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)
#endif

// Colour

#include "wx/mac/private.h"

static void wxComposeRGBColor( WXCOLORREF* color , int red, int blue, int green ) ;
static void wxComposeRGBColor( WXCOLORREF* color , int red, int blue, int green ) 
{
    RGBColor* col = (RGBColor*) color ;
	col->red = (red << 8) + red;
	col->blue = (blue << 8) + blue;
	col->green = (green << 8) + green;
}

wxColour::wxColour ()
{
  m_isInit = FALSE;
  m_red = m_blue = m_green = 0;

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

wxColour::wxColour (unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = TRUE;

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

wxColour::wxColour (const wxColour& col)
    : wxObject()
{
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    m_isInit = col.m_isInit;

    memcpy( &m_pixel , &col.m_pixel , 6 ) ;
}

wxColour::wxColour (const wxColour* col)
{
    m_red = col->m_red;
    m_green = col->m_green;
    m_blue = col->m_blue;
    m_isInit = col->m_isInit;

    memcpy( &m_pixel , &col->m_pixel , 6 ) ;
}

wxColour& wxColour::operator =(const wxColour& col)
{
  m_red = col.m_red;
  m_green = col.m_green;
  m_blue = col.m_blue;
  m_isInit = col.m_isInit;
 
  memcpy( &m_pixel , &col.m_pixel , 6 ) ;

  return *this;
}

void wxColour::InitFromName(const wxString& col)
{
    wxColour *the_colour = wxTheColourDatabase->FindColour (col);
    if (the_colour)
    {
        m_red = the_colour->Red ();
        m_green = the_colour->Green ();
        m_blue = the_colour->Blue ();
        m_isInit = TRUE;
    }
    else
    {
        m_red = 0;
        m_green = 0;
        m_blue = 0;
        m_isInit = FALSE;
    }

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

wxColour::~wxColour ()
{
}

void wxColour::Set (unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = TRUE;

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

void wxColour::Set( const WXCOLORREF* color )
{ 
    RGBColor* col = (RGBColor*) color ;
    memcpy( &m_pixel , color , 6 ) ;
    m_red = col->red>>8 ;
    m_blue = col->blue>>8 ;
    m_green = col->green>>8 ;
}
