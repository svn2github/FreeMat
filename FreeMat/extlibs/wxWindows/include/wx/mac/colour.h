/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:     wxColour class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "colour.h"
#endif

#include "wx/object.h"
#include "wx/string.h"

// Colour
class WXDLLEXPORT wxColour: public wxObject
{
public:
  // ctors
    // default
  wxColour();
    // from RGB
  wxColour( unsigned char red, unsigned char green, unsigned char blue );
  wxColour( unsigned long colRGB )
      : m_isInit(FALSE), m_red(0), m_blue(0), m_green(0)
      { Set(colRGB); }
  
    // implicit conversion from the colour name
  wxColour( const wxString &colourName )
      : m_isInit(FALSE), m_red(0), m_blue(0), m_green(0)
      { InitFromName(colourName); }
  wxColour( const char *colourName )
      : m_isInit(FALSE), m_red(0), m_blue(0), m_green(0)
      { InitFromName(colourName); }

    // copy ctors and assignment operators
  wxColour( const wxColour& col );
  wxColour( const wxColour* col );
  wxColour& operator = ( const wxColour& col );

    // dtor
  ~wxColour();

  // Set() functions
  void Set( unsigned char red, unsigned char green, unsigned char blue );
  void Set( unsigned long colRGB )
  {
    // we don't need to know sizeof(long) here because we assume that the three
    // least significant bytes contain the R, G and B values
    Set((unsigned char)colRGB,
        (unsigned char)(colRGB >> 8),
        (unsigned char)(colRGB >> 16));
  }

  // accessors
  bool Ok() const {return m_isInit; }

  // Let's remove this inelegant function
#if WXWIN_COMPATIBILITY
  void Get(unsigned char *r, unsigned char *g, unsigned char *b) const;
#endif

  unsigned char Red() const { return m_red; }
  unsigned char Green() const { return m_green; }
  unsigned char Blue() const { return m_blue; }

  // comparison
  bool operator == (const wxColour& colour) const
  {
    return (m_isInit == colour.m_isInit &&
            m_red == colour.m_red && 
            m_green == colour.m_green && 
            m_blue == colour.m_blue);
  }
  bool operator != (const wxColour& colour) const { return !(*this == colour); }

  void InitFromName(const wxString& col);

  const WXCOLORREF& GetPixel() const { return m_pixel; };

private:
  bool          m_isInit;
  unsigned char m_red;
  unsigned char m_blue;
  unsigned char m_green;

public:
  WXCOLORREF m_pixel ;
  void Set( const WXCOLORREF* color ) ;

private:
  DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif
  // _WX_COLOUR_H_
