/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/dcclient.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Markus Holzem, Chris Breeze
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#ifdef __VMS
#define XCopyPlane XCOPYPLANE
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/image.h"
#include "wx/module.h"
#include "wx/log.h"
#include "wx/fontutil.h"

#include "wx/gtk/win_gtk.h"

#include <math.h>               // for floating-point functions

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkprivate.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------

#define USE_PAINT_REGION 1

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

#include "bdiag.xbm"
#include "fdiag.xbm"
#include "cdiag.xbm"
#include "horiz.xbm"
#include "verti.xbm"
#include "cross.xbm"
#define  num_hatches 6

#define IS_15_PIX_HATCH(s) ((s)==wxCROSSDIAG_HATCH || (s)==wxHORIZONTAL_HATCH || (s)==wxVERTICAL_HATCH)
#define IS_16_PIX_HATCH(s) ((s)!=wxCROSSDIAG_HATCH && (s)!=wxHORIZONTAL_HATCH && (s)!=wxVERTICAL_HATCH)


static GdkPixmap  *hatches[num_hatches];
static GdkPixmap **hatch_bitmap = (GdkPixmap **) NULL;

extern GtkWidget *wxGetRootWindow();

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const double RAD2DEG  = 180.0 / M_PI;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

//-----------------------------------------------------------------------------
// temporary implementation of the missing GDK function
//-----------------------------------------------------------------------------

#include "gdk/gdkprivate.h"

void gdk_wx_draw_bitmap(GdkDrawable  *drawable,
                        GdkGC        *gc,
                        GdkDrawable  *src,
                        gint         xsrc,
                        gint         ysrc,
                        gint         xdest,
                        gint         ydest,
                        gint         width,
                        gint         height)
{
    gint src_width, src_height;
#ifndef __WXGTK20__
    GdkWindowPrivate *drawable_private;
    GdkWindowPrivate *src_private;
    GdkGCPrivate *gc_private;
#endif

    g_return_if_fail (drawable != NULL);
    g_return_if_fail (src != NULL);
    g_return_if_fail (gc != NULL);

#ifdef __WXGTK20__
    if (GDK_WINDOW_DESTROYED(drawable) || GDK_WINDOW_DESTROYED(src))
        return;

    gdk_drawable_get_size(src, &src_width, &src_height);
#else
    drawable_private = (GdkWindowPrivate*) drawable;
    src_private = (GdkWindowPrivate*) src;
    if (drawable_private->destroyed || src_private->destroyed)
        return;

    src_width = src_private->width;
    src_height = src_private->height;

    gc_private = (GdkGCPrivate*) gc;
#endif

    if (width == -1) width = src_width;
    if (height == -1) height = src_height;

#ifdef __WXGTK20__
    XCopyPlane( GDK_WINDOW_XDISPLAY(drawable),
                GDK_WINDOW_XID(src),
                GDK_WINDOW_XID(drawable),
                GDK_GC_XGC(gc),
                xsrc, ysrc,
                width, height,
                xdest, ydest,
                1 );
#else
    XCopyPlane( drawable_private->xdisplay,
                src_private->xwindow,
                drawable_private->xwindow,
                gc_private->xgc,
                xsrc, ysrc,
                width, height,
                xdest, ydest,
                1 );
#endif
}

//-----------------------------------------------------------------------------
// Implement Pool of Graphic contexts. Creating them takes too much time.
//-----------------------------------------------------------------------------

#define GC_POOL_SIZE 200

enum wxPoolGCType
{
   wxGC_ERROR = 0,
   wxTEXT_MONO,
   wxBG_MONO,
   wxPEN_MONO,
   wxBRUSH_MONO,
   wxTEXT_COLOUR,
   wxBG_COLOUR,
   wxPEN_COLOUR,
   wxBRUSH_COLOUR,
   wxTEXT_SCREEN,
   wxBG_SCREEN,
   wxPEN_SCREEN,
   wxBRUSH_SCREEN
};

struct wxGC
{
    GdkGC        *m_gc;
    wxPoolGCType  m_type;
    bool          m_used;
};

#define GC_POOL_ALLOC_SIZE 100

static int wxGCPoolSize = 0;

static wxGC *wxGCPool = NULL;

static void wxInitGCPool()
{
    // This really could wait until the first call to
    // wxGetPoolGC, but we will make the first allocation
    // now when other initialization is being performed.
    
    // Set initial pool size.
    wxGCPoolSize = GC_POOL_ALLOC_SIZE;
    
    // Allocate initial pool.
    wxGCPool = (wxGC *)malloc(wxGCPoolSize * sizeof(wxGC));
    if (wxGCPool == NULL)
    {
        // If we cannot malloc, then fail with error
        // when debug is enabled.  If debug is not enabled,
        // the problem will eventually get caught
		// in wxGetPoolGC.
        wxFAIL_MSG( wxT("Cannot allocate GC pool") );
        return;
    }
    
    // Zero initial pool.
    memset(wxGCPool, 0, wxGCPoolSize * sizeof(wxGC));
}

static void wxCleanUpGCPool()
{
    for (int i = 0; i < wxGCPoolSize; i++)
    {
        if (wxGCPool[i].m_gc)
            gdk_gc_unref( wxGCPool[i].m_gc );
    }

    free(wxGCPool);
    wxGCPool = NULL;
    wxGCPoolSize = 0;
}

static GdkGC* wxGetPoolGC( GdkWindow *window, wxPoolGCType type )
{
    wxGC *pptr;
    
    // Look for an available GC.
    for (int i = 0; i < wxGCPoolSize; i++)
    {
        if (!wxGCPool[i].m_gc)
        {
            wxGCPool[i].m_gc = gdk_gc_new( window );
            gdk_gc_set_exposures( wxGCPool[i].m_gc, FALSE );
            wxGCPool[i].m_type = type;
            wxGCPool[i].m_used = FALSE;
        }
        if ((!wxGCPool[i].m_used) && (wxGCPool[i].m_type == type))
        {
            wxGCPool[i].m_used = TRUE;
            return wxGCPool[i].m_gc;
        }
    }

    // We did not find an available GC.
    // We need to grow the GC pool.
    pptr = (wxGC *)realloc(wxGCPool,
		(wxGCPoolSize + GC_POOL_ALLOC_SIZE)*sizeof(wxGC));
    if (pptr != NULL)
    {
        // Initialize newly allocated pool.
        wxGCPool = pptr;
    	memset(&wxGCPool[wxGCPoolSize], 0,
			GC_POOL_ALLOC_SIZE*sizeof(wxGC));
    
        // Initialize entry we will return.    
        wxGCPool[wxGCPoolSize].m_gc = gdk_gc_new( window );
        gdk_gc_set_exposures( wxGCPool[wxGCPoolSize].m_gc, FALSE );
        wxGCPool[wxGCPoolSize].m_type = type;
        wxGCPool[wxGCPoolSize].m_used = TRUE;
        
        // Set new value of pool size.
        wxGCPoolSize += GC_POOL_ALLOC_SIZE;
        
        // Return newly allocated entry.
        return wxGCPool[wxGCPoolSize-GC_POOL_ALLOC_SIZE].m_gc;
    }
    
    // The realloc failed.  Fall through to error.
    wxFAIL_MSG( wxT("No GC available") );

    return (GdkGC*) NULL;
}

static void wxFreePoolGC( GdkGC *gc )
{
    for (int i = 0; i < wxGCPoolSize; i++)
    {
        if (wxGCPool[i].m_gc == gc)
        {
            wxGCPool[i].m_used = FALSE;
            return;
        }
    }

    wxFAIL_MSG( wxT("Wrong GC") );
}

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC()
{
    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
    m_isMemDC = FALSE;
    m_isScreenDC = FALSE;
    m_owner = (wxWindow *)NULL;
#ifdef __WXGTK20__
    m_context = (PangoContext *)NULL;
    m_fontdesc = (PangoFontDescription *)NULL;
#endif
}

wxWindowDC::wxWindowDC( wxWindow *window )
{
    wxASSERT_MSG( window, wxT("DC needs a window") );

    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
    m_owner = (wxWindow *)NULL;
    m_isMemDC = FALSE;
    m_isScreenDC = FALSE;
    m_font = window->GetFont();

    GtkWidget *widget = window->m_wxwindow;

    // some controls don't have m_wxwindow - like wxStaticBox, but the user
    // code should still be able to create wxClientDCs for them, so we will
    // use the parent window here then
    if ( !widget )
    {
        window = window->GetParent();
        widget = window->m_wxwindow;
    }

    wxASSERT_MSG( widget, wxT("DC needs a widget") );

#ifdef __WXGTK20__
    m_context = window->GtkGetPangoDefaultContext();
    m_fontdesc = widget->style->font_desc;
#endif

    GtkPizza *pizza = GTK_PIZZA( widget );
    m_window = pizza->bin_window;

    /* not realized ? */
    if (!m_window)
    {
         /* don't report problems */
         m_ok = TRUE;

         return;
    }

    m_cmap = gtk_widget_get_colormap( widget ? widget : window->m_widget );

    SetUpDC();

    /* this must be done after SetUpDC, bacause SetUpDC calls the
       repective SetBrush, SetPen, SetBackground etc functions
       to set up the DC. SetBackground call m_owner->SetBackground
       and this might not be desired as the standard dc background
       is white whereas a window might assume gray to be the
       standard (as e.g. wxStatusBar) */

    m_owner = window;
}

wxWindowDC::~wxWindowDC()
{
    Destroy();
}

void wxWindowDC::SetUpDC()
{
    m_ok = TRUE;

    wxASSERT_MSG( !m_penGC, wxT("GCs already created") );

    if (m_isScreenDC)
    {
        m_penGC = wxGetPoolGC( m_window, wxPEN_SCREEN );
        m_brushGC = wxGetPoolGC( m_window, wxBRUSH_SCREEN );
        m_textGC = wxGetPoolGC( m_window, wxTEXT_SCREEN );
        m_bgGC = wxGetPoolGC( m_window, wxBG_SCREEN );
    }
    else
    if (m_isMemDC && (((wxMemoryDC*)this)->m_selected.GetDepth() == 1))
    {
        m_penGC = wxGetPoolGC( m_window, wxPEN_MONO );
        m_brushGC = wxGetPoolGC( m_window, wxBRUSH_MONO );
        m_textGC = wxGetPoolGC( m_window, wxTEXT_MONO );
        m_bgGC = wxGetPoolGC( m_window, wxBG_MONO );
    }
    else
    {
        m_penGC = wxGetPoolGC( m_window, wxPEN_COLOUR );
        m_brushGC = wxGetPoolGC( m_window, wxBRUSH_COLOUR );
        m_textGC = wxGetPoolGC( m_window, wxTEXT_COLOUR );
        m_bgGC = wxGetPoolGC( m_window, wxBG_COLOUR );
    }

#ifdef __WXGTK20__
    if (m_isMemDC)
    {
        m_context = gdk_pango_context_get();
        m_fontdesc = pango_context_get_font_description(m_context);
    }
#endif

    /* background colour */
    m_backgroundBrush = *wxWHITE_BRUSH;
    m_backgroundBrush.GetColour().CalcPixel( m_cmap );
    GdkColor *bg_col = m_backgroundBrush.GetColour().GetColor();

    /* m_textGC */
    m_textForegroundColour.CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );

    m_textBackgroundColour.CalcPixel( m_cmap );
    gdk_gc_set_background( m_textGC, m_textBackgroundColour.GetColor() );

    gdk_gc_set_fill( m_textGC, GDK_SOLID );

    /* m_penGC */
    m_pen.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_penGC, m_pen.GetColour().GetColor() );
    gdk_gc_set_background( m_penGC, bg_col );

    gdk_gc_set_line_attributes( m_penGC, 0, GDK_LINE_SOLID, GDK_CAP_NOT_LAST, GDK_JOIN_ROUND );

    /* m_brushGC */
    m_brush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_brushGC, m_brush.GetColour().GetColor() );
    gdk_gc_set_background( m_brushGC, bg_col );

    gdk_gc_set_fill( m_brushGC, GDK_SOLID );

    /* m_bgGC */
    gdk_gc_set_background( m_bgGC, bg_col );
    gdk_gc_set_foreground( m_bgGC, bg_col );

    gdk_gc_set_fill( m_bgGC, GDK_SOLID );

    /* ROPs */
    gdk_gc_set_function( m_textGC, GDK_COPY );
    gdk_gc_set_function( m_brushGC, GDK_COPY );
    gdk_gc_set_function( m_penGC, GDK_COPY );

    /* clipping */
    gdk_gc_set_clip_rectangle( m_penGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_brushGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_textGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_bgGC, (GdkRectangle *) NULL );

    if (!hatch_bitmap)
    {
        hatch_bitmap    = hatches;
        hatch_bitmap[0] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, bdiag_bits, bdiag_width, bdiag_height );
        hatch_bitmap[1] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, cdiag_bits, cdiag_width, cdiag_height );
        hatch_bitmap[2] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, fdiag_bits, fdiag_width, fdiag_height );
        hatch_bitmap[3] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, cross_bits, cross_width, cross_height );
        hatch_bitmap[4] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, horiz_bits, horiz_width, horiz_height );
        hatch_bitmap[5] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, verti_bits, verti_width, verti_height );
    }
}

void wxWindowDC::DoGetSize( int* width, int* height ) const
{
    wxCHECK_RET( m_owner, _T("GetSize() doesn't work without window") );

    m_owner->GetSize(width, height);
}

extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y, 
                          const wxColour & col, int style);

bool wxWindowDC::DoFloodFill(wxCoord x, wxCoord y,
                             const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}

bool wxWindowDC::DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const
{
    // Generic (and therefore rather inefficient) method.
    // Could be improved.
    wxMemoryDC memdc;
    wxBitmap bitmap(1, 1);
    memdc.SelectObject(bitmap);
    memdc.Blit(0, 0, 1, 1, (wxDC*) this, x1, y1);
    memdc.SelectObject(wxNullBitmap);

    wxImage image = bitmap.ConvertToImage();
    col->Set(image.GetRed(0, 0), image.GetGreen(0, 0), image.GetBlue(0, 0));
    return TRUE;
}

void wxWindowDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        if (m_window)
            gdk_draw_line( m_window, m_penGC, XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2) );

        CalcBoundingBox(x1, y1);
        CalcBoundingBox(x2, y2);
    }
}

void wxWindowDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int w = 0;
        int h = 0;
        GetSize( &w, &h );
        wxCoord xx = XLOG2DEV(x);
        wxCoord yy = YLOG2DEV(y);
        if (m_window)
        {
            gdk_draw_line( m_window, m_penGC, 0, yy, XLOG2DEVREL(w), yy );
            gdk_draw_line( m_window, m_penGC, xx, 0, xx, YLOG2DEVREL(h) );
        }
    }
}

void wxWindowDC::DoDrawArc( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                            wxCoord xc, wxCoord yc )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx1 = XLOG2DEV(x1);
    wxCoord yy1 = YLOG2DEV(y1);
    wxCoord xx2 = XLOG2DEV(x2);
    wxCoord yy2 = YLOG2DEV(y2);
    wxCoord xxc = XLOG2DEV(xc);
    wxCoord yyc = YLOG2DEV(yc);
    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx*dx+dy*dy));
    wxCoord   r      = (wxCoord)radius;
    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2)
    {
        radius1 = 0.0;
        radius2 = 360.0;
    }
    else
    if (radius == 0.0)
    {
        radius1 = radius2 = 0.0;
    }
    else
    {
        radius1 = (xx1 - xxc == 0) ?
            (yy1 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy1-yyc), double(xx1-xxc)) * RAD2DEG;
        radius2 = (xx2 - xxc == 0) ?
            (yy2 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy2-yyc), double(xx2-xxc)) * RAD2DEG;
    }
    wxCoord alpha1 = wxCoord(radius1 * 64.0);
    wxCoord alpha2 = wxCoord((radius2 - radius1) * 64.0);
    while (alpha2 <= 0) alpha2 += 360*64;
    while (alpha1 > 360*64) alpha1 -= 360*64;

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                gdk_gc_set_ts_origin( m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_arc( m_window, m_textGC, TRUE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                gdk_gc_set_ts_origin( m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 15, m_deviceOriginY % 15 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 16, m_deviceOriginY % 16 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                gdk_gc_set_ts_origin( m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            }
            else
            {
                gdk_draw_arc( m_window, m_brushGC, TRUE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
            gdk_draw_arc( m_window, m_penGC, FALSE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );

            gdk_draw_line( m_window, m_penGC, xx1, yy1, xxc, yyc );
            gdk_draw_line( m_window, m_penGC, xxc, yyc, xx2, yy2 );
        }
    }

    CalcBoundingBox (x1, y1);
    CalcBoundingBox (x2, y2);
}

void wxWindowDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double sa, double ea )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        wxCoord start = wxCoord(sa * 64.0);
        wxCoord end = wxCoord((ea-sa) * 64.0);

        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                gdk_gc_set_ts_origin( m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_arc( m_window, m_textGC, TRUE, xx, yy, ww, hh, start, end );
                gdk_gc_set_ts_origin( m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 15, m_deviceOriginY % 15 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, start, end );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 16, m_deviceOriginY % 16 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, start, end );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                gdk_gc_set_ts_origin( m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, start, end );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            }
            else
            {
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, start, end );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, ww, hh, start, end );
    }

    CalcBoundingBox (x, y);
    CalcBoundingBox (x + width, y + height);
}

void wxWindowDC::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if ((m_pen.GetStyle() != wxTRANSPARENT) && m_window)
        gdk_draw_point( m_window, m_penGC, XLOG2DEV(x), YLOG2DEV(y) );

    CalcBoundingBox (x, y);
}

void wxWindowDC::DoDrawLines( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() == wxTRANSPARENT) return;
    if (n <= 0) return;

    CalcBoundingBox( points[0].x + xoffset, points[0].y + yoffset );

    for (int i = 0; i < n-1; i++)
    {
        wxCoord x1 = XLOG2DEV(points[i].x + xoffset);
        wxCoord x2 = XLOG2DEV(points[i+1].x + xoffset);
        wxCoord y1 = YLOG2DEV(points[i].y + yoffset);     // oh, what a waste
        wxCoord y2 = YLOG2DEV(points[i+1].y + yoffset);

        if (m_window)
            gdk_draw_line( m_window, m_penGC, x1, y1, x2, y2 );

        CalcBoundingBox( points[i+1].x + xoffset, points[i+1].y + yoffset );
    }
}

void wxWindowDC::DoDrawPolygon( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int WXUNUSED(fillStyle) )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (n <= 0) return;

    GdkPoint *gdkpoints = new GdkPoint[n+1];
    int i;
    for (i = 0 ; i < n ; i++)
    {
        gdkpoints[i].x = XLOG2DEV(points[i].x + xoffset);
        gdkpoints[i].y = YLOG2DEV(points[i].y + yoffset);

        CalcBoundingBox( points[i].x + xoffset, points[i].y + yoffset );
    }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                gdk_gc_set_ts_origin( m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_polygon( m_window, m_textGC, TRUE, gdkpoints, n );
                gdk_gc_set_ts_origin( m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 15, m_deviceOriginY % 15 );
                gdk_draw_polygon( m_window, m_brushGC, TRUE, gdkpoints, n );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 16, m_deviceOriginY % 16 );
                gdk_draw_polygon( m_window, m_brushGC, TRUE, gdkpoints, n );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                gdk_gc_set_ts_origin( m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_polygon( m_window, m_brushGC, TRUE, gdkpoints, n );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            }
            else
            {
                gdk_draw_polygon( m_window, m_brushGC, TRUE, gdkpoints, n );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
            for (i = 0 ; i < n ; i++)
            {
                gdk_draw_line( m_window, m_penGC,
                               gdkpoints[i%n].x,
                               gdkpoints[i%n].y,
                               gdkpoints[(i+1)%n].x,
                               gdkpoints[(i+1)%n].y);
            }
        }
    }

    delete[] gdkpoints;
}

void wxWindowDC::DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                gdk_gc_set_ts_origin( m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_rectangle( m_window, m_textGC, TRUE, xx, yy, ww, hh );
                gdk_gc_set_ts_origin( m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 15, m_deviceOriginY % 15 );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy, ww, hh );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 16, m_deviceOriginY % 16 );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy, ww, hh );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                gdk_gc_set_ts_origin( m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy, ww, hh );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            }
            else
            {
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy, ww, hh );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
            gdk_draw_rectangle( m_window, m_penGC, FALSE, xx, yy, ww-1, hh-1 );
    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (radius < 0.0) radius = - radius * ((width < height) ? width : height);

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);
    wxCoord rr = XLOG2DEVREL((wxCoord)radius);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    // CMB: if radius is zero use DrawRectangle() instead to avoid
    // X drawing errors with small radii
    if (rr == 0)
    {
        DrawRectangle( x, y, width, height );
        return;
    }

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: adjust size if outline is drawn otherwise the result is
    // 1 pixel too wide and high
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        ww--;
        hh--;
    }

    if (m_window)
    {
        // CMB: ensure dd is not larger than rectangle otherwise we
        // get an hour glass shape
        wxCoord dd = 2 * rr;
        if (dd > ww) dd = ww;
        if (dd > hh) dd = hh;
        rr = dd / 2;

        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                gdk_gc_set_ts_origin( m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_rectangle( m_window, m_textGC, TRUE, xx+rr, yy, ww-dd+1, hh );
                gdk_draw_rectangle( m_window, m_textGC, TRUE, xx, yy+rr, ww, hh-dd+1 );
                gdk_draw_arc( m_window, m_textGC, TRUE, xx, yy, dd, dd, 90*64, 90*64 );
                gdk_draw_arc( m_window, m_textGC, TRUE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
                gdk_draw_arc( m_window, m_textGC, TRUE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
                gdk_draw_arc( m_window, m_textGC, TRUE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
                gdk_gc_set_ts_origin( m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 15, m_deviceOriginY % 15 );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx+rr, yy, ww-dd+1, hh );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy+rr, ww, hh-dd+1 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, dd, dd, 90*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 16, m_deviceOriginY % 16 );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx+rr, yy, ww-dd+1, hh );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy+rr, ww, hh-dd+1 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, dd, dd, 90*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                gdk_gc_set_ts_origin( m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx+rr, yy, ww-dd+1, hh );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy+rr, ww, hh-dd+1 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, dd, dd, 90*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            }
            else
            {
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx+rr, yy, ww-dd+1, hh );
                gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy+rr, ww, hh-dd+1 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, dd, dd, 90*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
            gdk_draw_line( m_window, m_penGC, xx+rr+1, yy, xx+ww-rr, yy );
            gdk_draw_line( m_window, m_penGC, xx+rr+1, yy+hh, xx+ww-rr, yy+hh );
            gdk_draw_line( m_window, m_penGC, xx, yy+rr+1, xx, yy+hh-rr );
            gdk_draw_line( m_window, m_penGC, xx+ww, yy+rr+1, xx+ww, yy+hh-rr );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, dd, dd, 90*64, 90*64 );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
        }
    }

    // this ignores the radius
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                gdk_gc_set_ts_origin( m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_arc( m_window, m_textGC, TRUE, xx, yy, ww, hh, 0, 360*64 );
                gdk_gc_set_ts_origin( m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 15, m_deviceOriginY % 15 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, 0, 360*64 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                gdk_gc_set_ts_origin( m_brushGC, m_deviceOriginX % 16, m_deviceOriginY % 16 );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, 0, 360*64 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                gdk_gc_set_ts_origin( m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, 0, 360*64 );
                gdk_gc_set_ts_origin( m_brushGC, 0, 0 );
            }
            else
            {
                gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, 0, 360*64 );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, ww, hh, 0, 360*64 );
    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    // VZ: egcs 1.0.3 refuses to compile this without cast, no idea why
    DoDrawBitmap( (const wxBitmap&)icon, x, y, (bool)TRUE );
}

void wxWindowDC::DoDrawBitmap( const wxBitmap &bitmap,
                               wxCoord x, wxCoord y,
                               bool useMask )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCHECK_RET( bitmap.Ok(), wxT("invalid bitmap") );

    bool is_mono = (bitmap.GetBitmap() != NULL);

    /* scale/translate size and position */
    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);

    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + w, y + h );

    if (!m_window) return;

    int ww = XLOG2DEVREL(w);
    int hh = YLOG2DEVREL(h);

    /* compare to current clipping region */
    if (!m_currentClippingRegion.IsNull())
    {
        wxRegion tmp( xx,yy,ww,hh );
        tmp.Intersect( m_currentClippingRegion );
        if (tmp.IsEmpty())
            return;
    }

    /* scale bitmap if required */
    wxBitmap use_bitmap;
    if ((w != ww) || (h != hh))
    {
        wxImage image = bitmap.ConvertToImage();
        image.Rescale( ww, hh );
        if (is_mono)
            use_bitmap = wxBitmap(image.ConvertToMono(255,255,255), 1);
        else
            use_bitmap = wxBitmap(image);
    }
    else
    {
        use_bitmap = bitmap;
    }
    
    /* apply mask if any */
    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (use_bitmap.GetMask()) mask = use_bitmap.GetMask()->GetBitmap();

        if (useMask && mask)
        {
            GdkBitmap *new_mask = (GdkBitmap*) NULL;
#ifndef __WXGTK20__  // TODO fix crash
            if (!m_currentClippingRegion.IsNull())
            {
                GdkColor col;
                new_mask = gdk_pixmap_new( wxGetRootWindow()->window, ww, hh, 1 );
                GdkGC *gc = gdk_gc_new( new_mask );
                col.pixel = 0;
                gdk_gc_set_foreground( gc, &col );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, ww, hh );
                col.pixel = 0;
                gdk_gc_set_background( gc, &col );
                col.pixel = 1;
                gdk_gc_set_foreground( gc, &col );
                gdk_gc_set_clip_region( gc, m_currentClippingRegion.GetRegion() );
                gdk_gc_set_clip_origin( gc, -xx, -yy );
                gdk_gc_set_fill( gc, GDK_OPAQUE_STIPPLED );
                gdk_gc_set_stipple( gc, mask );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, ww, hh );
                gdk_gc_unref( gc );
            }
#endif
            if (is_mono)
            {
                if (new_mask)
                    gdk_gc_set_clip_mask( m_textGC, new_mask );
                else
                    gdk_gc_set_clip_mask( m_textGC, mask );
                gdk_gc_set_clip_origin( m_textGC, xx, yy );
            }
            else
            {
                if (new_mask)
                    gdk_gc_set_clip_mask( m_penGC, new_mask );
                else
                    gdk_gc_set_clip_mask( m_penGC, mask );
                gdk_gc_set_clip_origin( m_penGC, xx, yy );
            }
            
            if (new_mask)
                gdk_bitmap_unref( new_mask );
        }

    /* Draw XPixmap or XBitmap, depending on what the wxBitmap contains. For
       drawing a mono-bitmap (XBitmap) we use the current text GC */
    if (is_mono)
        gdk_wx_draw_bitmap( m_window, m_textGC, use_bitmap.GetBitmap(), 0, 0, xx, yy, -1, -1 );
    else
        gdk_draw_pixmap( m_window, m_penGC, use_bitmap.GetPixmap(), 0, 0, xx, yy, -1, -1 );

    /* remove mask again if any */
    if (useMask && mask)
    {
        if (is_mono)
        {
            gdk_gc_set_clip_mask( m_textGC, (GdkBitmap *) NULL );
            gdk_gc_set_clip_origin( m_textGC, 0, 0 );
            if (!m_currentClippingRegion.IsNull())
                gdk_gc_set_clip_region( m_textGC, m_currentClippingRegion.GetRegion() );
        }
        else
        {
            gdk_gc_set_clip_mask( m_penGC, (GdkBitmap *) NULL );
            gdk_gc_set_clip_origin( m_penGC, 0, 0 );
            if (!m_currentClippingRegion.IsNull())
                gdk_gc_set_clip_region( m_penGC, m_currentClippingRegion.GetRegion() );
        }
    }
}

bool wxWindowDC::DoBlit( wxCoord xdest, wxCoord ydest,
                         wxCoord width, wxCoord height,
                         wxDC *source,
                         wxCoord xsrc, wxCoord ysrc,
                         int logical_func,
                         bool useMask,
                         wxCoord xsrcMask, wxCoord ysrcMask )
{
   /* this is the nth try to get this utterly useless function to
      work. it now completely ignores the scaling or translation
      of the source dc, but scales correctly on the target dc and
      knows about possible mask information in a memory dc. */

    wxCHECK_MSG( Ok(), FALSE, wxT("invalid window dc") );

    wxCHECK_MSG( source, FALSE, wxT("invalid source dc") );

    if (!m_window) return FALSE;

#if 1
    // transform the source DC coords to the device ones
    xsrc = source->XLOG2DEV(xsrc);
    ysrc = source->YLOG2DEV(ysrc);
#endif

    wxClientDC *srcDC = (wxClientDC*)source;
    wxMemoryDC *memDC = (wxMemoryDC*)source;

    bool use_bitmap_method = FALSE;
    bool is_mono = FALSE;

    /* TODO: use the mask origin when drawing transparently */
    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc; ysrcMask = ysrc;
    }

    if (srcDC->m_isMemDC)
    {
        if (!memDC->m_selected.Ok()) return FALSE;

        /* we use the "XCopyArea" way to copy a memory dc into
           y different window if the memory dc BOTH
           a) doesn't have any mask or its mask isn't used
           b) it is clipped
           c) is not 1-bit */

        if (useMask && (memDC->m_selected.GetMask()))
        {
           /* we HAVE TO use the direct way for memory dcs
              that have mask since the XCopyArea doesn't know
              about masks */
            use_bitmap_method = TRUE;
        }
        else if (memDC->m_selected.GetDepth() == 1)
        {
           /* we HAVE TO use the direct way for memory dcs
              that are bitmaps because XCopyArea doesn't cope
              with different bit depths */
            is_mono = TRUE;
            use_bitmap_method = TRUE;
        }
        else if ((xsrc == 0) && (ysrc == 0) &&
                 (width == memDC->m_selected.GetWidth()) &&
                 (height == memDC->m_selected.GetHeight()))
        {
           /* we SHOULD use the direct way if all of the bitmap
              in the memory dc is copied in which case XCopyArea
              wouldn't be able able to boost performace by reducing
              the area to be scaled */
            use_bitmap_method = TRUE;
        }
        else
        {
            use_bitmap_method = FALSE;
        }
    }

    CalcBoundingBox( xdest, ydest );
    CalcBoundingBox( xdest + width, ydest + height );

    /* scale/translate size and position */
    wxCoord xx = XLOG2DEV(xdest);
    wxCoord yy = YLOG2DEV(ydest);

    wxCoord ww = XLOG2DEVREL(width);
    wxCoord hh = YLOG2DEVREL(height);

    /* compare to current clipping region */
    if (!m_currentClippingRegion.IsNull())
    {
        wxRegion tmp( xx,yy,ww,hh );
        tmp.Intersect( m_currentClippingRegion );
        if (tmp.IsEmpty())
            return TRUE;
    }

    int old_logical_func = m_logicalFunction;
    SetLogicalFunction( logical_func );

    if (use_bitmap_method)
    {
        /* scale/translate bitmap size */
        wxCoord bm_width = memDC->m_selected.GetWidth();
        wxCoord bm_height = memDC->m_selected.GetHeight();

        wxCoord bm_ww = XLOG2DEVREL( bm_width );
        wxCoord bm_hh = YLOG2DEVREL( bm_height );

        /* scale bitmap if required */
        wxBitmap use_bitmap;

        if ((bm_width != bm_ww) || (bm_height != bm_hh))
        {
            wxImage image = memDC->m_selected.ConvertToImage();
            image = image.Scale( bm_ww, bm_hh );

            if (is_mono)
                use_bitmap = wxBitmap(image.ConvertToMono(255,255,255), 1);
            else
                use_bitmap = wxBitmap(image);
        }
        else
        {
            use_bitmap = memDC->m_selected;
        }

        /* apply mask if any */
        GdkBitmap *mask = (GdkBitmap *) NULL;
        if (use_bitmap.GetMask()) mask = use_bitmap.GetMask()->GetBitmap();

        if (useMask && mask)
        {
            GdkBitmap *new_mask = (GdkBitmap*) NULL;
#ifndef __WXGTK20__  // TODO fix crash
            if (!m_currentClippingRegion.IsNull())
            {
                GdkColor col;
                new_mask = gdk_pixmap_new( wxGetRootWindow()->window, bm_ww, bm_hh, 1 );
                GdkGC *gc = gdk_gc_new( new_mask );
                col.pixel = 0;
                gdk_gc_set_foreground( gc, &col );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, bm_ww, bm_hh );
                col.pixel = 0;
                gdk_gc_set_background( gc, &col );
                col.pixel = 1;
                gdk_gc_set_foreground( gc, &col );
                gdk_gc_set_clip_region( gc, m_currentClippingRegion.GetRegion() );
                gdk_gc_set_clip_origin( gc, -xx, -yy );
                gdk_gc_set_fill( gc, GDK_OPAQUE_STIPPLED );
                gdk_gc_set_stipple( gc, mask );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, bm_ww, bm_hh );
                gdk_gc_unref( gc );
            }
#endif
            if (is_mono)
            {
                if (new_mask)
                    gdk_gc_set_clip_mask( m_textGC, new_mask );
                else
                    gdk_gc_set_clip_mask( m_textGC, mask );
                gdk_gc_set_clip_origin( m_textGC, xx, yy );
            }
            else
            {
                if (new_mask)
                    gdk_gc_set_clip_mask( m_penGC, new_mask );
                else
                    gdk_gc_set_clip_mask( m_penGC, mask );
                gdk_gc_set_clip_origin( m_penGC, xx, yy );
            }
            if (new_mask)
                gdk_bitmap_unref( new_mask );
        }

        /* Draw XPixmap or XBitmap, depending on what the wxBitmap contains. For
           drawing a mono-bitmap (XBitmap) we use the current text GC */

        if (is_mono)
            gdk_wx_draw_bitmap( m_window, m_textGC, use_bitmap.GetBitmap(), xsrc, ysrc, xx, yy, ww, hh );
        else
            gdk_draw_pixmap( m_window, m_penGC, use_bitmap.GetPixmap(), xsrc, ysrc, xx, yy, ww, hh );

        /* remove mask again if any */
        if (useMask && mask)
        {
            if (is_mono)
            {
                gdk_gc_set_clip_mask( m_textGC, (GdkBitmap *) NULL );
                gdk_gc_set_clip_origin( m_textGC, 0, 0 );
                if (!m_currentClippingRegion.IsNull())
                    gdk_gc_set_clip_region( m_textGC, m_currentClippingRegion.GetRegion() );
            }
            else
            {
                gdk_gc_set_clip_mask( m_penGC, (GdkBitmap *) NULL );
                gdk_gc_set_clip_origin( m_penGC, 0, 0 );
                if (!m_currentClippingRegion.IsNull())
                    gdk_gc_set_clip_region( m_penGC, m_currentClippingRegion.GetRegion() );
            }
        }
    }
    else /* use_bitmap_method */
    {
        if ((width != ww) || (height != hh))
        {
            /* draw source window into a bitmap as we cannot scale
               a window in contrast to a bitmap. this would actually
               work with memory dcs as well, but we'd lose the mask
               information and waste one step in this process since
               a memory already has a bitmap. all this is slightly
               inefficient as we could take an XImage directly from
               an X window, but we'd then also have to care that
               the window is not outside the screen (in which case
               we'd get a BadMatch or what not).
               Is a double XGetImage and combined XGetPixel and
               XPutPixel really faster? I'm not sure. look at wxXt
               for a different implementation of the same problem. */

            wxBitmap bitmap( width, height );

            /* copy including child window contents */
            gdk_gc_set_subwindow( m_penGC, GDK_INCLUDE_INFERIORS );
            gdk_window_copy_area( bitmap.GetPixmap(), m_penGC, 0, 0,
                                  srcDC->GetWindow(),
                                  xsrc, ysrc, width, height );
            gdk_gc_set_subwindow( m_penGC, GDK_CLIP_BY_CHILDREN );

            /* scale image */
            wxImage image = bitmap.ConvertToImage();
            image = image.Scale( ww, hh );

            /* convert to bitmap */
            bitmap = wxBitmap(image);

            /* draw scaled bitmap */
            gdk_draw_pixmap( m_window, m_penGC, bitmap.GetPixmap(), 0, 0, xx, yy, -1, -1 );

        }
        else
        {
            /* No scaling and not a memory dc with a mask either */

            /* copy including child window contents */
            gdk_gc_set_subwindow( m_penGC, GDK_INCLUDE_INFERIORS );
            gdk_window_copy_area( m_window, m_penGC, xx, yy,
                                  srcDC->GetWindow(),
                                  xsrc, ysrc, width, height );
            gdk_gc_set_subwindow( m_penGC, GDK_CLIP_BY_CHILDREN );
        }
    }

    SetLogicalFunction( old_logical_func );
    return TRUE;
}

void wxWindowDC::DoDrawText( const wxString &text, wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;
    
    if (text.empty()) return;

#ifndef __WXGTK20__
    GdkFont *font = m_font.GetInternalFont( m_scaleY );

    wxCHECK_RET( font, wxT("invalid font") );
#endif

#ifdef __WXGTK20__
    wxCHECK_RET( m_context, wxT("no Pango context") );
#endif

    x = XLOG2DEV(x);
    y = YLOG2DEV(y);

#ifdef __WXGTK20__
    // TODO: the layout engine should be abstracted at a higher level!
    PangoLayout *layout = pango_layout_new(m_context);
    
#if wxUSE_UNICODE
    const wxCharBuffer data = wxConvUTF8.cWC2MB( text );
#else
    const wxWCharBuffer wdata = wxConvLocal.cMB2WC( text );
    const wxCharBuffer data = wxConvUTF8.cWC2MB( wdata );
#endif

    pango_layout_set_text( layout, (const char*)data, strlen((const char*)data) );

    if (m_scaleY != 1.0)
    {
         // If there is a user or actually any scale applied to
         // the device context, scale the font.
         
         // scale font description
         gint oldSize = pango_font_description_get_size( m_fontdesc );
         double size = oldSize;
         size = size * m_scaleY;
         pango_font_description_set_size( m_fontdesc, (gint)size );
         
         // actually apply scaled font
         pango_layout_set_font_description( layout, m_fontdesc );
         
         // Draw layout.
         gdk_draw_layout( m_window, m_textGC, x, y, layout );
         
         // reset unscaled size
         pango_font_description_set_size( m_fontdesc, oldSize );
    }
    else
    {
         // actually apply font
         pango_layout_set_font_description( layout, m_fontdesc );
         
         // Draw layout.
         gdk_draw_layout( m_window, m_textGC, x, y, layout );
    }

#if 0
    // Measure layout
    int w,h;
    pango_layout_get_pixel_size( m_layout, &w, &h );
#else
    int w = 10;
    int h = 10;
#endif
    wxCoord width = w;
    wxCoord height = h;

    g_object_unref( G_OBJECT( layout ) );
#else // GTK+ 1.x
    wxCoord width = gdk_string_width( font, text.mbc_str() );
    wxCoord height = font->ascent + font->descent;

    if ( m_backgroundMode == wxSOLID )
    {
        gdk_gc_set_foreground( m_textGC, m_textBackgroundColour.GetColor() );
        gdk_draw_rectangle( m_window, m_textGC, TRUE, x, y, width, height );
        gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );
    }
    gdk_draw_string( m_window, font, m_textGC, x, y + font->ascent, text.mbc_str() );

    /* CMB 17/7/98: simple underline: ignores scaling and underlying
       X font's XA_UNDERLINE_POSITION and XA_UNDERLINE_THICKNESS
       properties (see wxXt implementation) */
    if (m_font.GetUnderlined())
    {
        wxCoord ul_y = y + font->ascent;
        if (font->descent > 0) ul_y++;
        gdk_draw_line( m_window, m_textGC, x, ul_y, x + width, ul_y);
    }
#endif // GTK+ 2.0/1.x


    width = wxCoord(width / m_scaleX);
    height = wxCoord(height / m_scaleY);
    CalcBoundingBox (x + width, y + height);
    CalcBoundingBox (x, y);
}

void wxWindowDC::DoDrawRotatedText( const wxString &text, wxCoord x, wxCoord y, double angle )
{
    if (angle == 0.0)
    {
        DrawText(text, x, y);
        return;
    }

    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    GdkFont *font = m_font.GetInternalFont( m_scaleY );

    wxCHECK_RET( font, wxT("invalid font") );

    // the size of the text
    wxCoord w = gdk_string_width( font, text.mbc_str() );
    wxCoord h = font->ascent + font->descent;

    // draw the string normally
    wxBitmap src(w, h);
    wxMemoryDC dc;
    dc.SelectObject(src);
    dc.SetFont(GetFont());
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.Clear();
    dc.DrawText(text, 0, 0);
    dc.SelectObject(wxNullBitmap);

    // Calculate the size of the rotated bounding box.
    double rad = DegToRad(angle);
    double dx = cos(rad),
           dy = sin(rad);

    // the rectngle vertices are counted clockwise with the first one being at
    // (0, 0) (or, rather, at (x, y))
    double x2 = w*dx,
           y2 = -w*dy;      // y axis points to the bottom, hence minus
    double x4 = h*dy,
           y4 = h*dx;
    double x3 = x4 + x2,
           y3 = y4 + y2;

    // calc max and min
    wxCoord maxX = (wxCoord)(dmax(x2, dmax(x3, x4)) + 0.5),
            maxY = (wxCoord)(dmax(y2, dmax(y3, y4)) + 0.5),
            minX = (wxCoord)(dmin(x2, dmin(x3, x4)) - 0.5),
            minY = (wxCoord)(dmin(y2, dmin(y3, y4)) - 0.5);

    // prepare to blit-with-rotate the bitmap to the DC
    wxImage image = src.ConvertToImage();

    GdkColor *colText = m_textForegroundColour.GetColor(),
             *colBack = m_textBackgroundColour.GetColor();

    bool textColSet = TRUE;

    unsigned char *data = image.GetData();

    // paint pixel by pixel
    for ( wxCoord srcX = 0; srcX < w; srcX++ )
    {
        for ( wxCoord srcY = 0; srcY < h; srcY++ )
        {
            // transform source coords to dest coords
            double r = sqrt((double)srcX*srcX + srcY*srcY);
            double angleOrig = atan2((double)srcY, (double)srcX) - rad;
            wxCoord dstX = (wxCoord)(r*cos(angleOrig) + 0.5),
                    dstY = (wxCoord)(r*sin(angleOrig) + 0.5);

            // black pixel?
            bool textPixel = data[(srcY*w + srcX)*3] == 0;
            if ( textPixel || (m_backgroundMode == wxSOLID) )
            {
                // change colour if needed
                if ( textPixel != textColSet )
                {
                    gdk_gc_set_foreground( m_textGC, textPixel ? colText
                                                               : colBack );

                    textColSet = textPixel;
                }

                // don't use DrawPoint() because it uses the current pen
                // colour, and we don't need it here
                gdk_draw_point( m_window, m_textGC,
                                XLOG2DEV(x) + dstX, YLOG2DEV(y) + dstY );
            }
        }
    }

    // it would be better to draw with non underlined font and draw the line
    // manually here (it would be more straight...)
#if 0
    if ( m_font.GetUnderlined() )
    {
        gdk_draw_line( m_window, m_textGC,
                       XLOG2DEV(x + x4), YLOG2DEV(y + y4 + font->descent),
                       XLOG2DEV(x + x3), YLOG2DEV(y + y3 + font->descent));
    }
#endif // 0

    // restore the font colour
    gdk_gc_set_foreground( m_textGC, colText );

    // update the bounding box
    CalcBoundingBox(x + minX, y + minY);
    CalcBoundingBox(x + maxX, y + maxY);
}

void wxWindowDC::DoGetTextExtent(const wxString &string,
                                 wxCoord *width, wxCoord *height,
                                 wxCoord *descent, wxCoord *externalLeading,
                                 wxFont *theFont) const
{
    if (string.IsEmpty())
    {
        if (width) (*width) = 0;
        if (height) (*height) = 0;
        return;
    }
    
#ifdef __WXGTK20__
    // Create layout and set font description
    PangoLayout *layout = pango_layout_new( m_context );
    if (theFont)
        pango_layout_set_font_description( layout, theFont->GetNativeFontInfo()->description );
    else
        pango_layout_set_font_description( layout, m_fontdesc );
        
    // Set layout's text
#if wxUSE_UNICODE
    const wxCharBuffer data = wxConvUTF8.cWC2MB( string );
#else
    const wxWCharBuffer wdata = wxConvLocal.cMB2WC( string );
    const wxCharBuffer data = wxConvUTF8.cWC2MB( wdata );
#endif
    pango_layout_set_text( layout, (const char*) data, strlen((const char*)data) );
 
    // Measure text.
    int w,h;
    pango_layout_get_pixel_size( layout, &w, &h );
    
    if (width) (*width) = (wxCoord) w; 
    if (height) (*height) = (wxCoord) h;
    if (descent)
    {
        // Do something about metrics here. TODO.
        (*descent) = 0;
    }
    if (externalLeading) (*externalLeading) = 0;  // ??
    
    g_object_unref( G_OBJECT( layout ) );
#else
    wxFont fontToUse = m_font;
    if (theFont) fontToUse = *theFont;
    
    GdkFont *font = fontToUse.GetInternalFont( m_scaleY );
    if (width) (*width) = wxCoord(gdk_string_width( font, string.mbc_str() ) / m_scaleX);
    if (height) (*height) = wxCoord((font->ascent + font->descent) / m_scaleY);
    if (descent) (*descent) = wxCoord(font->descent / m_scaleY);
    if (externalLeading) (*externalLeading) = 0;  // ??
#endif
}

wxCoord wxWindowDC::GetCharWidth() const
{
#ifdef __WXGTK20__
    // There should be an easier way.
    PangoLayout *layout = pango_layout_new(m_context);
    pango_layout_set_font_description(layout, m_fontdesc);
    pango_layout_set_text(layout, "H", 1 );
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);
    g_object_unref( G_OBJECT( layout ) );
    return w;
#else
    GdkFont *font = m_font.GetInternalFont( m_scaleY );
    wxCHECK_MSG( font, -1, wxT("invalid font") );

    return wxCoord(gdk_string_width( font, "H" ) / m_scaleX);
#endif
}

wxCoord wxWindowDC::GetCharHeight() const
{
#ifdef __WXGTK20__
    // There should be an easier way.
    PangoLayout *layout = pango_layout_new(m_context);
    pango_layout_set_font_description(layout, m_fontdesc);
    pango_layout_set_text(layout, "H", 1);
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);
    g_object_unref( G_OBJECT( layout ) );
    return h;
#else
    GdkFont *font = m_font.GetInternalFont( m_scaleY );
    wxCHECK_MSG( font, -1, wxT("invalid font") );

    return wxCoord((font->ascent + font->descent) / m_scaleY);
#endif
}

void wxWindowDC::Clear()
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    // VZ: the code below results in infinite recursion and crashes when
    //     dc.Clear() is done from OnPaint() so I disable it for now.
    //     I don't know what the correct fix is but Clear() surely should not
    //     reenter OnPaint()!
#if 0
    /* - we either are a memory dc or have a window as the
       owner. anything else shouldn't happen.
       - we don't use gdk_window_clear() as we don't set
       the window's background colour anymore. it is too
       much pain to keep the DC's and the window's back-
       ground colour in synch. */

    if (m_owner)
    {
        m_owner->Clear();
        return;
    }

    if (m_isMemDC)
    {
        int width,height;
        GetSize( &width, &height );
        gdk_draw_rectangle( m_window, m_bgGC, TRUE, 0, 0, width, height );
        return;
    }
#else // 1
    int width,height;
    GetSize( &width, &height );
    gdk_draw_rectangle( m_window, m_bgGC, TRUE, 0, 0, width, height );
#endif // 0/1
}

void wxWindowDC::SetFont( const wxFont &font )
{
    // It is common practice to set the font to wxNullFont, so
    // don't consider it to be an error
    //    wxCHECK_RET( font.Ok(), _T("invalid font in wxWindowDC::SetFont") );

    m_font = font;
#ifdef __WXGTK20__
    if (m_font.Ok())
    {
        m_fontdesc = m_font.GetNativeFontInfo()->description;
   
        if (m_owner)
        {
            if (m_font.GetNoAntiAliasing())
                m_context = m_owner->GtkGetPangoX11Context();
            else
                m_context = m_owner->GtkGetPangoDefaultContext();
        }
    }
#endif
}

void wxWindowDC::SetPen( const wxPen &pen )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen == pen) return;

    m_pen = pen;

    if (!m_pen.Ok()) return;

    if (!m_window) return;

    gint width = m_pen.GetWidth();
    if (width <= 0)
    {
        // CMB: if width is non-zero scale it with the dc
        width = 1;
    }
    else
    {
        // X doesn't allow different width in x and y and so we take
        // the average
        double w = 0.5 +
                   ( fabs((double) XLOG2DEVREL(width)) +
                     fabs((double) YLOG2DEVREL(width)) ) / 2.0;
        width = (int)w;
    }

    static const wxGTKDash dotted[] = {1, 1};
    static const wxGTKDash short_dashed[] = {2, 2};
    static const wxGTKDash wxCoord_dashed[] = {2, 4};
    static const wxGTKDash dotted_dashed[] = {3, 3, 1, 3};

    // We express dash pattern in pen width unit, so we are
    // independent of zoom factor and so on...
    int req_nb_dash;
    const wxGTKDash *req_dash;

    GdkLineStyle lineStyle = GDK_LINE_SOLID;
    switch (m_pen.GetStyle())
    {
        case wxUSER_DASH:
        {
            lineStyle = GDK_LINE_ON_OFF_DASH;
            req_nb_dash = m_pen.GetDashCount();
            req_dash = (wxGTKDash*)m_pen.GetDash();
            break;
        }
        case wxDOT:
        {
            lineStyle = GDK_LINE_ON_OFF_DASH;
            req_nb_dash = 2;
            req_dash = dotted;
            break;
        }
        case wxLONG_DASH:
        {
            lineStyle = GDK_LINE_ON_OFF_DASH;
            req_nb_dash = 2;
            req_dash = wxCoord_dashed;
            break;
        }
        case wxSHORT_DASH:
        {
            lineStyle = GDK_LINE_ON_OFF_DASH;
            req_nb_dash = 2;
            req_dash = short_dashed;
            break;
        }
        case wxDOT_DASH:
        {
//            lineStyle = GDK_LINE_DOUBLE_DASH;
            lineStyle = GDK_LINE_ON_OFF_DASH;
            req_nb_dash = 4;
            req_dash = dotted_dashed;
            break;
        }

        case wxTRANSPARENT:
        case wxSTIPPLE_MASK_OPAQUE:
        case wxSTIPPLE:
        case wxSOLID:
        default:
        {
            lineStyle = GDK_LINE_SOLID;
            req_dash = (wxGTKDash*)NULL;
            req_nb_dash = 0;
            break;
        }
    }

#if (GTK_MINOR_VERSION > 0) || (GTK_MAJOR_VERSION > 1)
    if (req_dash && req_nb_dash)
    {
        wxGTKDash *real_req_dash = new wxGTKDash[req_nb_dash];
        if (real_req_dash)
        {
            for (int i = 0; i < req_nb_dash; i++)
                real_req_dash[i] = req_dash[i] * width;
            gdk_gc_set_dashes( m_penGC, 0, real_req_dash, req_nb_dash );
            delete[] real_req_dash;
        }
        else
        {
            // No Memory. We use non-scaled dash pattern...
            gdk_gc_set_dashes( m_penGC, 0, (wxGTKDash*)req_dash, req_nb_dash );
        }
    }
#endif // GTK+ > 1.0

    GdkCapStyle capStyle = GDK_CAP_ROUND;
    switch (m_pen.GetCap())
    {
        case wxCAP_PROJECTING: { capStyle = GDK_CAP_PROJECTING; break; }
        case wxCAP_BUTT:       { capStyle = GDK_CAP_BUTT;       break; }
        case wxCAP_ROUND:
        default:
        {
            if (width <= 1)
            {
                width = 0;
                capStyle = GDK_CAP_NOT_LAST;
            }
            else
            {
                capStyle = GDK_CAP_ROUND;
            }
            break;
        }
    }

    GdkJoinStyle joinStyle = GDK_JOIN_ROUND;
    switch (m_pen.GetJoin())
    {
        case wxJOIN_BEVEL: { joinStyle = GDK_JOIN_BEVEL; break; }
        case wxJOIN_MITER: { joinStyle = GDK_JOIN_MITER; break; }
        case wxJOIN_ROUND:
        default:           { joinStyle = GDK_JOIN_ROUND; break; }
    }

    gdk_gc_set_line_attributes( m_penGC, width, lineStyle, capStyle, joinStyle );

    m_pen.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_penGC, m_pen.GetColour().GetColor() );
}

void wxWindowDC::SetBrush( const wxBrush &brush )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_brush == brush) return;

    m_brush = brush;

    if (!m_brush.Ok()) return;

    if (!m_window) return;

    m_brush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_brushGC, m_brush.GetColour().GetColor() );

    gdk_gc_set_fill( m_brushGC, GDK_SOLID );

    if ((m_brush.GetStyle() == wxSTIPPLE) && (m_brush.GetStipple()->Ok()))
    {
        if (m_brush.GetStipple()->GetPixmap())
        {
            gdk_gc_set_fill( m_brushGC, GDK_TILED );
            gdk_gc_set_tile( m_brushGC, m_brush.GetStipple()->GetPixmap() );
        }
        else
        {
            gdk_gc_set_fill( m_brushGC, GDK_STIPPLED );
            gdk_gc_set_stipple( m_brushGC, m_brush.GetStipple()->GetBitmap() );
        }
    }

    if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
    {
        gdk_gc_set_fill( m_textGC, GDK_OPAQUE_STIPPLED);
        gdk_gc_set_stipple( m_textGC, m_brush.GetStipple()->GetMask()->GetBitmap() );
    }

    if (IS_HATCH(m_brush.GetStyle()))
    {
        gdk_gc_set_fill( m_brushGC, GDK_STIPPLED );
        int num = m_brush.GetStyle() - wxBDIAGONAL_HATCH;
        gdk_gc_set_stipple( m_brushGC, hatches[num] );
    }
}

void wxWindowDC::SetBackground( const wxBrush &brush )
{
   /* CMB 21/7/98: Added SetBackground. Sets background brush
    * for Clear() and bg colour for shapes filled with cross-hatch brush */

    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_backgroundBrush == brush) return;

    m_backgroundBrush = brush;

    if (!m_backgroundBrush.Ok()) return;

    if (!m_window) return;

    m_backgroundBrush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_background( m_brushGC, m_backgroundBrush.GetColour().GetColor() );
    gdk_gc_set_background( m_penGC, m_backgroundBrush.GetColour().GetColor() );
    gdk_gc_set_background( m_bgGC, m_backgroundBrush.GetColour().GetColor() );
    gdk_gc_set_foreground( m_bgGC, m_backgroundBrush.GetColour().GetColor() );

    gdk_gc_set_fill( m_bgGC, GDK_SOLID );

    if ((m_backgroundBrush.GetStyle() == wxSTIPPLE) && (m_backgroundBrush.GetStipple()->Ok()))
    {
        if (m_backgroundBrush.GetStipple()->GetPixmap())
        {
            gdk_gc_set_fill( m_bgGC, GDK_TILED );
            gdk_gc_set_tile( m_bgGC, m_backgroundBrush.GetStipple()->GetPixmap() );
        }
        else
        {
            gdk_gc_set_fill( m_bgGC, GDK_STIPPLED );
            gdk_gc_set_stipple( m_bgGC, m_backgroundBrush.GetStipple()->GetBitmap() );
        }
    }

    if (IS_HATCH(m_backgroundBrush.GetStyle()))
    {
        gdk_gc_set_fill( m_bgGC, GDK_STIPPLED );
        int num = m_backgroundBrush.GetStyle() - wxBDIAGONAL_HATCH;
        gdk_gc_set_stipple( m_bgGC, hatches[num] );
    }
}

void wxWindowDC::SetLogicalFunction( int function )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_logicalFunction == function)
        return;

    // VZ: shouldn't this be a CHECK?
    if (!m_window)
        return;

    GdkFunction mode;
    switch (function)
    {
        case wxXOR:          mode = GDK_XOR;           break;
        case wxINVERT:       mode = GDK_INVERT;        break;
#if (GTK_MINOR_VERSION > 0) || (GTK_MAJOR_VERSION > 1)
        case wxOR_REVERSE:   mode = GDK_OR_REVERSE;    break;
        case wxAND_REVERSE:  mode = GDK_AND_REVERSE;   break;
        case wxCLEAR:        mode = GDK_CLEAR;         break;
        case wxSET:          mode = GDK_SET;           break;
        case wxOR_INVERT:    mode = GDK_OR_INVERT;     break;
        case wxAND:          mode = GDK_AND;           break;
        case wxOR:           mode = GDK_OR;            break;
        case wxEQUIV:        mode = GDK_EQUIV;         break;
        case wxNAND:         mode = GDK_NAND;          break;
        case wxAND_INVERT:   mode = GDK_AND_INVERT;    break;
        case wxCOPY:         mode = GDK_COPY;          break;
        case wxNO_OP:        mode = GDK_NOOP;          break;
        case wxSRC_INVERT:   mode = GDK_COPY_INVERT;   break;

        // unsupported by GTK
        case wxNOR:          mode = GDK_COPY;          break;
#endif // GTK+ > 1.0
        default:
           wxFAIL_MSG( wxT("unsupported logical function") );
           mode = GDK_COPY;
    }

    m_logicalFunction = function;

    gdk_gc_set_function( m_penGC, mode );
    gdk_gc_set_function( m_brushGC, mode );

    // to stay compatible with wxMSW, we don't apply ROPs to the text
    // operations (i.e. DrawText/DrawRotatedText).
    // True, but mono-bitmaps use the m_textGC and they use ROPs as well.
    gdk_gc_set_function( m_textGC, mode );
}

void wxWindowDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    // don't set m_textForegroundColour to an invalid colour as we'd crash
    // later then (we use m_textForegroundColour.GetColor() without checking
    // in a few places)
    if ( !col.Ok() || (m_textForegroundColour == col) )
        return;

    m_textForegroundColour = col;

    if ( m_window )
    {
        m_textForegroundColour.CalcPixel( m_cmap );
        gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );
    }
}

void wxWindowDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    // same as above
    if ( !col.Ok() || (m_textBackgroundColour == col) )
        return;

    m_textBackgroundColour = col;

    if ( m_window )
    {
        m_textBackgroundColour.CalcPixel( m_cmap );
        gdk_gc_set_background( m_textGC, m_textBackgroundColour.GetColor() );
    }
}

void wxWindowDC::SetBackgroundMode( int mode )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    m_backgroundMode = mode;

    if (!m_window) return;

    // CMB 21/7/98: fill style of cross-hatch brushes is affected by
    // transparent/solid background mode

    if (m_brush.GetStyle() != wxSOLID && m_brush.GetStyle() != wxTRANSPARENT)
    {
        gdk_gc_set_fill( m_brushGC,
          (m_backgroundMode == wxTRANSPARENT) ? GDK_STIPPLED : GDK_OPAQUE_STIPPLED);
    }
}

void wxWindowDC::SetPalette( const wxPalette& WXUNUSED(palette) )
{
    wxFAIL_MSG( wxT("wxWindowDC::SetPalette not implemented") );
}

void wxWindowDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    wxRect rect;
    rect.x = XLOG2DEV(x);
    rect.y = YLOG2DEV(y);
    rect.width = XLOG2DEVREL(width);
    rect.height = YLOG2DEVREL(height);

    if (!m_currentClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( rect );
    else
        m_currentClippingRegion.Union( rect );

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( m_paintClippingRegion );
#endif

    wxCoord xx, yy, ww, hh;
    m_currentClippingRegion.GetBox( xx, yy, ww, hh );
    wxDC::DoSetClippingRegion( xx, yy, ww, hh );

    gdk_gc_set_clip_region( m_penGC, m_currentClippingRegion.GetRegion() );
    gdk_gc_set_clip_region( m_brushGC, m_currentClippingRegion.GetRegion() );
    gdk_gc_set_clip_region( m_textGC, m_currentClippingRegion.GetRegion() );
    gdk_gc_set_clip_region( m_bgGC, m_currentClippingRegion.GetRegion() );
}

void wxWindowDC::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }

    if (!m_window) return;

    if (!m_currentClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( region );
    else
        m_currentClippingRegion.Union( region );

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( m_paintClippingRegion );
#endif

    wxCoord xx, yy, ww, hh;
    m_currentClippingRegion.GetBox( xx, yy, ww, hh );
    wxDC::DoSetClippingRegion( xx, yy, ww, hh );

    gdk_gc_set_clip_region( m_penGC, m_currentClippingRegion.GetRegion() );
    gdk_gc_set_clip_region( m_brushGC, m_currentClippingRegion.GetRegion() );
    gdk_gc_set_clip_region( m_textGC, m_currentClippingRegion.GetRegion() );
    gdk_gc_set_clip_region( m_bgGC, m_currentClippingRegion.GetRegion() );
}

void wxWindowDC::DestroyClippingRegion()
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxDC::DestroyClippingRegion();

    m_currentClippingRegion.Clear();

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsEmpty())
        m_currentClippingRegion.Union( m_paintClippingRegion );
#endif

    if (!m_window) return;

    if (m_currentClippingRegion.IsEmpty())
    {
        gdk_gc_set_clip_rectangle( m_penGC, (GdkRectangle *) NULL );
        gdk_gc_set_clip_rectangle( m_brushGC, (GdkRectangle *) NULL );
        gdk_gc_set_clip_rectangle( m_textGC, (GdkRectangle *) NULL );
        gdk_gc_set_clip_rectangle( m_bgGC, (GdkRectangle *) NULL );
    }
    else
    {
        gdk_gc_set_clip_region( m_penGC, m_currentClippingRegion.GetRegion() );
        gdk_gc_set_clip_region( m_brushGC, m_currentClippingRegion.GetRegion() );
        gdk_gc_set_clip_region( m_textGC, m_currentClippingRegion.GetRegion() );
        gdk_gc_set_clip_region( m_bgGC, m_currentClippingRegion.GetRegion() );
    }
}

void wxWindowDC::Destroy()
{
    if (m_penGC) wxFreePoolGC( m_penGC );
    m_penGC = (GdkGC*) NULL;
    if (m_brushGC) wxFreePoolGC( m_brushGC );
    m_brushGC = (GdkGC*) NULL;
    if (m_textGC) wxFreePoolGC( m_textGC );
    m_textGC = (GdkGC*) NULL;
    if (m_bgGC) wxFreePoolGC( m_bgGC );
    m_bgGC = (GdkGC*) NULL;
}

void wxWindowDC::ComputeScaleAndOrigin()
{
    /* CMB: copy scale to see if it changes */
    double origScaleX = m_scaleX;
    double origScaleY = m_scaleY;

    wxDC::ComputeScaleAndOrigin();

    /* CMB: if scale has changed call SetPen to recalulate the line width */
    if ((m_scaleX != origScaleX || m_scaleY != origScaleY) &&
        (m_pen.Ok()))
    {
      /* this is a bit artificial, but we need to force wxDC to think
         the pen has changed */
      wxPen pen = m_pen;
      m_pen = wxNullPen;
      SetPen( pen );
  }
}

// Resolution in pixels per logical inch
wxSize wxWindowDC::GetPPI() const
{
    return wxSize( (int) (m_mm_to_pix_x * 25.4 + 0.5), (int) (m_mm_to_pix_y * 25.4 + 0.5));
}

int wxWindowDC::GetDepth() const
{
    wxFAIL_MSG(wxT("not implemented"));

    return -1;
}


//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxClientDC)

wxPaintDC::wxPaintDC( wxWindow *win )
         : wxClientDC( win )
{
#if USE_PAINT_REGION
    if (!win->m_clipPaintRegion)
        return;

    m_paintClippingRegion = win->GetUpdateRegion();
    GdkRegion *region = m_paintClippingRegion.GetRegion();
    if ( region )
    {
        m_paintClippingRegion = win->GetUpdateRegion();
        GdkRegion *region = m_paintClippingRegion.GetRegion();
        if ( region )
        {
            m_currentClippingRegion.Union( m_paintClippingRegion );

            gdk_gc_set_clip_region( m_penGC, region );
            gdk_gc_set_clip_region( m_brushGC, region );
            gdk_gc_set_clip_region( m_textGC, region );
            gdk_gc_set_clip_region( m_bgGC, region );
        }
    }
#endif // USE_PAINT_REGION
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC( wxWindow *win )
          : wxWindowDC( win )
{
    wxCHECK_RET( win, _T("NULL window in wxClientDC::wxClientDC") );

#ifdef __WXUNIVERSAL__
    wxPoint ptOrigin = win->GetClientAreaOrigin();
    SetDeviceOrigin(ptOrigin.x, ptOrigin.y);
    wxSize size = win->GetClientSize();
    SetClippingRegion(wxPoint(0, 0), size);
#endif // __WXUNIVERSAL__
}

void wxClientDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_owner, _T("GetSize() doesn't work without window") );

    m_owner->GetClientSize( width, height );
}

// ----------------------------------------------------------------------------
// wxDCModule
// ----------------------------------------------------------------------------

class wxDCModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxDCModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDCModule, wxModule)

bool wxDCModule::OnInit()
{
    wxInitGCPool();
    return TRUE;
}

void wxDCModule::OnExit()
{
    wxCleanUpGCPool();
}
