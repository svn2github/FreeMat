/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

#include "wx/dcscreen.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)
#endif

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
#if TARGET_CARBON
	m_macPort = GetQDGlobalsThePort() ;
	GrafPtr port ;
	GetPort( &port ) ;
	SetPort( (GrafPtr) m_macPort ) ;
	Point pt = { 0,0 } ;
    LocalToGlobal( &pt ) ; 	
	SetPort( port ) ;
	m_macLocalOrigin.x = -pt.h ;
	m_macLocalOrigin.y = -pt.v ;
#else
	m_macPort = LMGetWMgrPort() ;
	m_macLocalOrigin.x = 0 ;
	m_macLocalOrigin.y = 0 ;
#endif
 	m_ok = TRUE ;
	BitMap screenBits;
	GetQDGlobalsScreenBits( &screenBits );
 	m_minX = screenBits.bounds.left ;
 #if TARGET_CARBON
 	SInt16 height ;
 	GetThemeMenuBarHeight( &height ) ;
 	m_minY = screenBits.bounds.top + height ;
 #else
 	m_minY = screenBits.bounds.top + LMGetMBarHeight() ;
 #endif
	m_maxX = screenBits.bounds.right  ;
	m_maxY = screenBits.bounds.bottom ;
	MacSetRectRgn( (RgnHandle) m_macBoundaryClipRgn , m_minX , m_minY , m_maxX , m_maxY ) ;
	OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
	CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
}

wxScreenDC::~wxScreenDC()
{
    // TODO
}

