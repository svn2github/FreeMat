/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/image.h"
#include "wx/app.h"
#if wxUSE_NANOX
#include "wx/dcmemory.h"
#endif

#include "wx/x11/private.h"

/* No point in using libXPM for NanoX */
#if wxUSE_NANOX
#undef wxHAVE_LIB_XPM
#define wxHAVE_LIB_XPM 0

// Copy from the drawable to the wxImage
bool wxGetImageFromDrawable(GR_DRAW_ID drawable, int srcX, int srcY, int width, int height, wxImage& image);
#endif

#if wxUSE_XPM
#if wxHAVE_LIB_XPM
#include <X11/xpm.h>
#else
#include "wx/xpmdecod.h"
#include "wx/wfstream.h"
#endif
#endif
#include <math.h>

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask()
{
    m_bitmap = NULL;
    m_display = NULL;
}

wxMask::wxMask( const wxBitmap& bitmap, const wxColour& colour )
{
    m_bitmap = NULL;
    Create( bitmap, colour );
}

wxMask::wxMask( const wxBitmap& bitmap, int paletteIndex )
{
    m_bitmap = NULL;
    Create( bitmap, paletteIndex );
}

wxMask::wxMask( const wxBitmap& bitmap )
{
    m_bitmap = NULL;
    Create( bitmap );
}

wxMask::~wxMask()
{
    if (m_bitmap)
        XFreePixmap( (Display*) m_display, (Pixmap) m_bitmap );
}

bool wxMask::Create( const wxBitmap& bitmap,
                     const wxColour& colour )
{
#if !wxUSE_NANOX
    if (m_bitmap)
    {
        XFreePixmap( (Display*) m_display, (Pixmap) m_bitmap );
        m_bitmap = NULL;
    }

    m_display = bitmap.GetDisplay();

    wxImage image = bitmap.ConvertToImage();
    if (!image.Ok()) return FALSE;
    
    m_display = bitmap.GetDisplay();
    
    Display *xdisplay = (Display*) m_display;
    int xscreen = DefaultScreen( xdisplay );
    Window xroot = RootWindow( xdisplay, xscreen );
    
    m_bitmap = (WXPixmap) XCreatePixmap( xdisplay, xroot, image.GetWidth(), image.GetHeight(), 1 );
    GC gc = XCreateGC( xdisplay, (Pixmap) m_bitmap, 0, NULL );

    XSetForeground( xdisplay, gc, WhitePixel(xdisplay,xscreen) );
    XSetFillStyle( xdisplay, gc, FillSolid );
    XFillRectangle( xdisplay, (Pixmap) m_bitmap, gc, 0, 0, image.GetWidth(), image.GetHeight() );

    unsigned char *data = image.GetData();
    int index = 0;

    unsigned char red = colour.Red();
    unsigned char green = colour.Green();
    unsigned char blue = colour.Blue();

    int bpp = wxTheApp->m_visualDepth;
    
    if (bpp == 15)
    {
        red = red & 0xf8;
        green = green & 0xf8;
        blue = blue & 0xf8;
    } else
    if (bpp == 16)
    {
        red = red & 0xf8;
        green = green & 0xfc;
        blue = blue & 0xf8;
    } else
    if (bpp == 12)
    {
        red = red & 0xf0;
        green = green & 0xf0;
        blue = blue & 0xf0;
    }

    XSetForeground( xdisplay, gc, BlackPixel(xdisplay,xscreen) );

    int width = image.GetWidth();
    int height = image.GetHeight();
    for (int j = 0; j < height; j++)
    {
        int start_x = -1;
        int i;
        for (i = 0; i < width; i++)
        {
            if ((data[index] == red) &&
                (data[index+1] == green) &&
                (data[index+2] == blue))
            {
                if (start_x == -1)
                start_x = i;
            }
            else
            {
                if (start_x != -1)
                {
                    XDrawLine( xdisplay, (Pixmap) m_bitmap, gc, start_x, j, i-1, j );
                    start_x = -1;
                }
            }
            index += 3;
        }
        if (start_x != -1)
            XDrawLine( xdisplay, (Pixmap) m_bitmap, gc, start_x, j, i, j );
    }
    
    XFreeGC( xdisplay, gc );

    return TRUE;
#else
    return FALSE;
#endif
    // wxUSE_NANOX
}

bool wxMask::Create( const wxBitmap& bitmap, int paletteIndex )
{
    unsigned char r,g,b;
    wxPalette *pal = bitmap.GetPalette();

    wxCHECK_MSG( pal, FALSE, wxT("Cannot create mask from bitmap without palette") );

    pal->GetRGB(paletteIndex, &r, &g, &b);

    return Create(bitmap, wxColour(r, g, b));
}

bool wxMask::Create( const wxBitmap& bitmap )
{
#if !wxUSE_NANOX
    if (m_bitmap)
    {
        XFreePixmap( (Display*) m_display, (Pixmap) m_bitmap );
        m_bitmap = NULL;
    }

    if (!bitmap.Ok()) return FALSE;

    wxCHECK_MSG( bitmap.GetBitmap(), FALSE, wxT("Cannot create mask from colour bitmap") );
    
    m_display = bitmap.GetDisplay();

    int xscreen = DefaultScreen( (Display*) m_display );
    Window xroot = RootWindow( (Display*) m_display, xscreen );
    
    m_bitmap = (WXPixmap) XCreatePixmap( (Display*) m_display, xroot, bitmap.GetWidth(), bitmap.GetHeight(), 1 );

    if (!m_bitmap) return FALSE;
    
    GC gc = XCreateGC( (Display*) m_display, (Pixmap) m_bitmap, 0, NULL );

    XCopyPlane( (Display*) m_display, (Pixmap) bitmap.GetBitmap(), (Pixmap) m_bitmap,
       gc, 0, 0, bitmap.GetWidth(), bitmap.GetHeight(), 0, 0, 1 );
       
    XFreeGC( (Display*) m_display, gc );

    return TRUE;
#else
    return FALSE;
#endif
    // wxUSE_NANOX
}

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxObjectRefData
{
public:
    wxBitmapRefData();
    ~wxBitmapRefData();

    WXPixmap        m_pixmap;
    WXPixmap        m_bitmap;
    WXDisplay      *m_display;
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
    wxPalette      *m_palette;
};

wxBitmapRefData::wxBitmapRefData()
{
    m_pixmap = NULL;
    m_bitmap = NULL;
    m_display = NULL;
    m_mask = (wxMask *) NULL;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
    m_palette = (wxPalette *) NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap) XFreePixmap( (Display*) m_display, (Pixmap) m_pixmap );
    if (m_bitmap) XFreePixmap( (Display*) m_display, (Pixmap) m_bitmap );
    if (m_mask) delete m_mask;
    if (m_palette) delete m_palette;
}

//-----------------------------------------------------------------------------

#define M_BMPDATA ((wxBitmapRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap()
{
}

wxBitmap::wxBitmap( int width, int height, int depth )
{
    Create( width, height, depth );
}

bool wxBitmap::Create( int width, int height, int depth )
{
    UnRef();

    wxCHECK_MSG( (width > 0) && (height > 0), FALSE, wxT("invalid bitmap size") )

    m_refData = new wxBitmapRefData();
    
    M_BMPDATA->m_display = wxGlobalDisplay();
    
    wxASSERT_MSG( M_BMPDATA->m_display, wxT("No display") );
    
    int xscreen = DefaultScreen( (Display*) M_BMPDATA->m_display );
    Window xroot = RootWindow( (Display*) M_BMPDATA->m_display, xscreen );
    
    int bpp = DefaultDepth( (Display*) M_BMPDATA->m_display, xscreen );
    if (depth == -1) depth = bpp;

    wxCHECK_MSG( (depth == bpp) ||
                 (depth == 1), FALSE, wxT("invalid bitmap depth") )

    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;

#if wxUSE_NANOX
    M_BMPDATA->m_pixmap = (WXPixmap) GrNewPixmap(width, height, NULL);
    M_BMPDATA->m_bpp = bpp;

    wxASSERT_MSG( M_BMPDATA->m_pixmap, wxT("Bitmap creation failed") );
#else
    if (depth == 1)
    {
        M_BMPDATA->m_bitmap = (WXPixmap) XCreatePixmap( (Display*) M_BMPDATA->m_display, xroot, width, height, 1 );
        
        wxASSERT_MSG( M_BMPDATA->m_bitmap, wxT("Bitmap creation failed") );
        
        M_BMPDATA->m_bpp = 1;
    }
    else
    {
        M_BMPDATA->m_pixmap = (WXPixmap) XCreatePixmap( (Display*) M_BMPDATA->m_display, xroot, width, height, depth );
        
        wxASSERT_MSG( M_BMPDATA->m_pixmap, wxT("Pixmap creation failed") );
        
        M_BMPDATA->m_bpp = depth;
    }
#endif
    return Ok();
}

bool wxBitmap::CreateFromXpm( const char **bits )
{
#if wxUSE_XPM
#if wxHAVE_LIB_XPM
    UnRef();

    wxCHECK_MSG( bits != NULL, FALSE, wxT("invalid bitmap data") )

    m_refData = new wxBitmapRefData();

    M_BMPDATA->m_display = wxGlobalDisplay();
    
    Display *xdisplay = (Display*) M_BMPDATA->m_display;
    
    int xscreen = DefaultScreen( xdisplay );
    Window xroot = RootWindow( xdisplay, xscreen );
    
    int bpp = DefaultDepth( xdisplay, xscreen );

    XpmAttributes xpmAttr;
    xpmAttr.valuemask = XpmReturnInfos;    // nothing yet, but get infos back

    Pixmap pixmap;
    Pixmap mask = 0;
    
    int ErrorStatus = XpmCreatePixmapFromData( xdisplay, xroot, (char**) bits, &pixmap, &mask, &xpmAttr );
    
    if (ErrorStatus == XpmSuccess)
    {
        M_BMPDATA->m_width = xpmAttr.width;
        M_BMPDATA->m_height = xpmAttr.height;

        M_BMPDATA->m_bpp = bpp;  // mono as well?

#if __WXDEBUG__
        unsigned int depthRet;
        int xRet, yRet;
        unsigned int widthRet, heightRet, borderWidthRet;
        XGetGeometry( xdisplay, pixmap, &xroot, &xRet, &yRet,
            &widthRet, &heightRet, &borderWidthRet, &depthRet);

        wxASSERT_MSG( bpp == (int)depthRet, wxT("colour depth mismatch") );
#endif

        XpmFreeAttributes(&xpmAttr);
        
        M_BMPDATA->m_pixmap = (WXPixmap) pixmap;
        
        if (mask)
        {
            M_BMPDATA->m_mask = new wxMask;
            M_BMPDATA->m_mask->SetBitmap( (WXPixmap) mask );
            M_BMPDATA->m_mask->SetDisplay( xdisplay );
        }
	return TRUE;
    }
    else
    {
        UnRef();
        
        return FALSE;
    }
#else
    wxXPMDecoder decoder;
    wxImage image(decoder.ReadData(bits));
    if (image.Ok())
	return CreateFromImage(image);
    else
	return FALSE;
#endif
#endif
    return FALSE;
}

bool wxBitmap::CreateFromImage( const wxImage& image, int depth )
{
#if wxUSE_NANOX
    if (!image.Ok())
    {
        wxASSERT_MSG(image.Ok(), "Invalid wxImage passed to wxBitmap::CreateFromImage.");
        return FALSE;
    }
    
    int w = image.GetWidth();
    int h = image.GetHeight();
    
    if (!Create(w, h, depth))
        return FALSE;

    // Unfortunately the mask has to be screen-depth since
    // 1-bpp bitmaps don't seem to be supported
    // TODO: implement transparent drawing, presumably
    // by doing several blits as per the Windows
    // implementation because Nano-X doesn't support
    // XSetClipMask.
    // TODO: could perhaps speed this function up
    // by making a buffer of pixel values,
    // and then calling GrArea to write that to the
    // pixmap. See demos/nxroach.c.

    bool hasMask = image.HasMask();
    
    GC pixmapGC = GrNewGC();
    Pixmap pixmap = (Pixmap) GetPixmap();

    GC maskGC = 0;
    Pixmap maskPixmap = 0;

    unsigned char maskR = 0;
    unsigned char maskG = 0;
    unsigned char maskB = 0;

    if (hasMask)
    {
        maskR = image.GetMaskRed();
        maskG = image.GetMaskGreen();
        maskB = image.GetMaskBlue();
        
        maskGC = GrNewGC();
        maskPixmap = GrNewPixmap(w, h, 0);
        if (!maskPixmap)
            hasMask = FALSE;
        else
        {
            wxMask* mask = new wxMask;
            mask->SetBitmap((WXPixmap) maskPixmap);
            SetMask(mask);
        }
    }

    GR_COLOR lastPixmapColour = 0;
    GR_COLOR lastMaskColour = 0;
    
    int i, j;
    for (i = 0; i < w; i++)
    {
        for (j = 0; j < h; j++)
        {
            unsigned char red = image.GetRed(i, j);
            unsigned char green = image.GetGreen(i, j);
            unsigned char blue = image.GetBlue(i, j);

            GR_COLOR colour = GR_RGB(red, green, blue);

            // Efficiency measure
            if (colour != lastPixmapColour || (i == 0 && j == 0))
            {
                GrSetGCForeground(pixmapGC, colour);
                lastPixmapColour = colour;
            }
            
            GrPoint(pixmap, pixmapGC, i, j);
            
            if (hasMask)
            {
                // scan the bitmap for the transparent colour and set the corresponding
                // pixels in the mask to BLACK and the rest to WHITE
                if (maskR == red && maskG == green && maskB == blue)
                {
                    colour = GR_RGB(0, 0, 0);
                }
                else
                {
                    colour = GR_RGB(255, 255, 255);
                }
                if (colour != lastMaskColour || (i == 0 && j == 0))
                {
                    GrSetGCForeground(maskGC, colour);
                    lastMaskColour = colour;
                }
                GrPoint(maskPixmap, maskGC, i, j);
            }
        }
    }

    GrDestroyGC(pixmapGC);
    if (hasMask)
        GrDestroyGC(maskGC);
    
    return TRUE;
#else
    // !wxUSE_NANOX
    
    UnRef();

    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") )
    wxCHECK_MSG( depth == -1, FALSE, wxT("invalid bitmap depth") )

    m_refData = new wxBitmapRefData();

    M_BMPDATA->m_display = wxGlobalDisplay();
    
    Display *xdisplay = (Display*) M_BMPDATA->m_display;
    
    int xscreen = DefaultScreen( xdisplay );
    Window xroot = RootWindow( xdisplay, xscreen );
    Visual* xvisual = DefaultVisual( xdisplay, xscreen );
    
    int bpp = wxTheApp->m_visualDepth;
    
    int width = image.GetWidth();
    int height = image.GetHeight();
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;

    if (depth != 1) depth = bpp;
    M_BMPDATA->m_bpp = depth;
    
    if (depth == 1)
    {
        wxFAIL_MSG( "mono images later" );
    }
    else
    {
        // Create image
    
        XImage *data_image = XCreateImage( xdisplay, xvisual, bpp, ZPixmap, 0, 0, width, height, 32, 0 );
        data_image->data = (char*) malloc( data_image->bytes_per_line * data_image->height );
    
        if (data_image->data == NULL)
        {
            wxLogError( wxT("Out of memory.") );  // TODO clean
            return FALSE;
        }

        M_BMPDATA->m_pixmap = (WXPixmap) XCreatePixmap( xdisplay, xroot, width, height, depth );

        // Create mask

        XImage *mask_image = (XImage*) NULL;
        if (image.HasMask())
        { 
            mask_image = XCreateImage( xdisplay, xvisual, 1, ZPixmap, 0, 0, width, height, 32, 0 );
            mask_image->data = (char*) malloc( mask_image->bytes_per_line * mask_image->height );
        
            if (mask_image->data == NULL)
            {
                wxLogError( wxT("Out of memory.") ); // TODO clean
                return FALSE;
            }
            
            wxMask *mask = new wxMask();
            mask->SetDisplay( xdisplay );
            mask->SetBitmap( (WXPixmap) XCreatePixmap( xdisplay, xroot, width, height, 1 ) );

            SetMask( mask );
        }

        if (bpp < 8) bpp = 8;

        // Render

        enum byte_order { RGB, RBG, BRG, BGR, GRB, GBR };
        byte_order b_o = RGB;

        if (bpp > 8)
        {
            if ((wxTheApp->m_visualRedMask > wxTheApp->m_visualGreenMask) && (wxTheApp->m_visualGreenMask > wxTheApp->m_visualBlueMask))      b_o = RGB;
            else if ((wxTheApp->m_visualRedMask > wxTheApp->m_visualBlueMask) && (wxTheApp->m_visualBlueMask > wxTheApp->m_visualGreenMask))  b_o = RBG;
            else if ((wxTheApp->m_visualBlueMask > wxTheApp->m_visualRedMask) && (wxTheApp->m_visualRedMask > wxTheApp->m_visualGreenMask))   b_o = BRG;
            else if ((wxTheApp->m_visualBlueMask > wxTheApp->m_visualGreenMask) && (wxTheApp->m_visualGreenMask > wxTheApp->m_visualRedMask)) b_o = BGR;
            else if ((wxTheApp->m_visualGreenMask > wxTheApp->m_visualRedMask) && (wxTheApp->m_visualRedMask > wxTheApp->m_visualBlueMask))   b_o = GRB;
            else if ((wxTheApp->m_visualGreenMask > wxTheApp->m_visualBlueMask) && (wxTheApp->m_visualBlueMask > wxTheApp->m_visualRedMask))  b_o = GBR;
        }

        int r_mask = image.GetMaskRed();
        int g_mask = image.GetMaskGreen();
        int b_mask = image.GetMaskBlue();

        unsigned char* data = image.GetData();
        wxASSERT_MSG( data, "No image data" );
        
        unsigned char *colorCube = wxTheApp->m_colorCube;

        bool hasMask = image.HasMask();

        int index = 0;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int r = data[index];
                index++;
                int g = data[index];
                index++;
                int b = data[index];
                index++;

                if (hasMask)
                {
                    if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                        XPutPixel( mask_image, x, y, 0 );
                    else
                        XPutPixel( mask_image, x, y, 1 );
                }

                switch (bpp)
                {
                    case 8:
                    {
                        int pixel = 0;
                        pixel = colorCube[ ((r & 0xf8) << 7) + ((g & 0xf8) << 2) + ((b & 0xf8) >> 3) ];
                        XPutPixel( data_image, x, y, pixel );
                        break;
                    }
                    case 12:  // SGI only
                    {
                        int pixel = 0;
                        switch (b_o)
                        {
                            case RGB: pixel = ((r & 0xf0) << 4) | (g & 0xf0) | ((b & 0xf0) >> 4); break;
                            case RBG: pixel = ((r & 0xf0) << 4) | (b & 0xf0) | ((g & 0xf0) >> 4); break;
                            case GRB: pixel = ((g & 0xf0) << 4) | (r & 0xf0) | ((b & 0xf0) >> 4); break;
                            case GBR: pixel = ((g & 0xf0) << 4) | (b & 0xf0) | ((r & 0xf0) >> 4); break;
                            case BRG: pixel = ((b & 0xf0) << 4) | (r & 0xf0) | ((g & 0xf0) >> 4); break;
                            case BGR: pixel = ((b & 0xf0) << 4) | (g & 0xf0) | ((r & 0xf0) >> 4); break;
                        }
                        XPutPixel( data_image, x, y, pixel );
                        break;
                    }
                    case 15:
                    {
                        int pixel = 0;
                        switch (b_o)
                        {
                            case RGB: pixel = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3); break;
                            case RBG: pixel = ((r & 0xf8) << 7) | ((b & 0xf8) << 2) | ((g & 0xf8) >> 3); break;
                            case GRB: pixel = ((g & 0xf8) << 7) | ((r & 0xf8) << 2) | ((b & 0xf8) >> 3); break;
                            case GBR: pixel = ((g & 0xf8) << 7) | ((b & 0xf8) << 2) | ((r & 0xf8) >> 3); break;
                            case BRG: pixel = ((b & 0xf8) << 7) | ((r & 0xf8) << 2) | ((g & 0xf8) >> 3); break;
                            case BGR: pixel = ((b & 0xf8) << 7) | ((g & 0xf8) << 2) | ((r & 0xf8) >> 3); break;
                        }
                        XPutPixel( data_image, x, y, pixel );
                        break;
                    }
                    case 16:
                    {
                        // I actually don't know if for 16-bit displays, it is alway the green
                        // component or the second component which has 6 bits.
                        int pixel = 0;
                        switch (b_o)
                        {
                            case RGB: pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3); break;
                            case RBG: pixel = ((r & 0xf8) << 8) | ((b & 0xfc) << 3) | ((g & 0xf8) >> 3); break;
                            case GRB: pixel = ((g & 0xf8) << 8) | ((r & 0xfc) << 3) | ((b & 0xf8) >> 3); break;
                            case GBR: pixel = ((g & 0xf8) << 8) | ((b & 0xfc) << 3) | ((r & 0xf8) >> 3); break;
                            case BRG: pixel = ((b & 0xf8) << 8) | ((r & 0xfc) << 3) | ((g & 0xf8) >> 3); break;
                            case BGR: pixel = ((b & 0xf8) << 8) | ((g & 0xfc) << 3) | ((r & 0xf8) >> 3); break;
                        }
                        XPutPixel( data_image, x, y, pixel );
                        break;
                    }
                    case 32:
                    case 24:
                    {
                        int pixel = 0;
                        switch (b_o)
                        {
                            case RGB: pixel = (r << 16) | (g << 8) | b; break;
                            case RBG: pixel = (r << 16) | (b << 8) | g; break;
                            case BRG: pixel = (b << 16) | (r << 8) | g; break;
                            case BGR: pixel = (b << 16) | (g << 8) | r; break;
                            case GRB: pixel = (g << 16) | (r << 8) | b; break;
                            case GBR: pixel = (g << 16) | (b << 8) | r; break;
                        }
                        XPutPixel( data_image, x, y, pixel );
                    }
                    default: break;
                }
            } // for
        }  // for

        // Blit picture

        GC gc = XCreateGC( xdisplay, (Pixmap) M_BMPDATA->m_pixmap, 0, NULL );
        XPutImage( xdisplay, (Pixmap) M_BMPDATA->m_pixmap, gc, data_image, 0, 0, 0, 0, width, height );
        XDestroyImage( data_image );
        XFreeGC( xdisplay, gc );

        // Blit mask
        
        if (image.HasMask())
        {
            GC gc = XCreateGC( xdisplay, (Pixmap) GetMask()->GetBitmap(), 0, NULL );
            XPutImage( xdisplay, (Pixmap) GetMask()->GetBitmap(), gc, mask_image, 0, 0, 0, 0, width, height );

            XDestroyImage( mask_image );
            XFreeGC( xdisplay, gc );
        }
    }

    return TRUE;
#endif
    // wxUSE_NANOX
}

wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;

    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    Display *xdisplay = (Display*) M_BMPDATA->m_display;
    wxASSERT_MSG( xdisplay, wxT("No display") );

#if wxUSE_NANOX
    //int bpp = DefaultDepth(xdisplay, xscreen);
    wxGetImageFromDrawable((Pixmap) GetPixmap(), 0, 0, GetWidth(), GetHeight(), image);
    return image;
#else
    // !wxUSE_NANOX
    int bpp = wxTheApp->m_visualDepth;
    XImage *x_image = NULL;
    if (GetPixmap())
    {
        x_image = XGetImage( xdisplay, (Pixmap) GetPixmap(),
            0, 0,
            GetWidth(), GetHeight(),
            AllPlanes, ZPixmap );
    } else
    if (GetBitmap())
    {
        x_image = XGetImage( xdisplay, (Pixmap) GetBitmap(),
            0, 0,
            GetWidth(), GetHeight(),
            AllPlanes, ZPixmap );
    } else
    {
        wxFAIL_MSG( wxT("Ill-formed bitmap") );
    }

    wxCHECK_MSG( x_image, wxNullImage, wxT("couldn't create image") );

    image.Create( GetWidth(), GetHeight() );
    char unsigned *data = image.GetData();

    if (!data)
    {
        XDestroyImage( x_image );
        wxFAIL_MSG( wxT("couldn't create image") );
        return wxNullImage;
    }

    XImage *x_image_mask = NULL;
    if (GetMask())
    {
        x_image_mask = XGetImage( xdisplay, (Pixmap) GetMask()->GetBitmap(),
            0, 0,
            GetWidth(), GetHeight(), 
            AllPlanes, ZPixmap );

        image.SetMaskColour( 16, 16, 16 );  // anything unlikely and dividable
    }

    int red_shift_right = 0;
    int green_shift_right = 0;
    int blue_shift_right = 0;
    int red_shift_left = 0;
    int green_shift_left = 0;
    int blue_shift_left = 0;
    bool use_shift = FALSE;

    if (GetPixmap())
    {
        red_shift_right = wxTheApp->m_visualRedShift;
        red_shift_left = 8-wxTheApp->m_visualRedPrec;
        green_shift_right = wxTheApp->m_visualGreenShift;
        green_shift_left = 8-wxTheApp->m_visualGreenPrec;
        blue_shift_right = wxTheApp->m_visualBlueShift;
        blue_shift_left = 8-wxTheApp->m_visualBluePrec;
        
        use_shift = (wxTheApp->m_visualType == GrayScale) || (wxTheApp->m_visualType != PseudoColor);
    }
    
    if (GetBitmap())
    {
        bpp = 1;
    }

    XColor *colors = (XColor*) wxTheApp->m_visualColormap;

    int width = GetWidth();
    int height = GetHeight();
    long pos = 0;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned long pixel = XGetPixel( x_image, i, j );
            if (bpp == 1)
            {
                if (pixel == 0)
                {
                    data[pos]   = 0;
                    data[pos+1] = 0;
                    data[pos+2] = 0;
                }
                else
                {
                    data[pos]   = 255;
                    data[pos+1] = 255;
                    data[pos+2] = 255;
                }
            }
            else if (use_shift)
            {
                data[pos] =   (pixel >> red_shift_right)   << red_shift_left;
                data[pos+1] = (pixel >> green_shift_right) << green_shift_left;
                data[pos+2] = (pixel >> blue_shift_right)  << blue_shift_left;
            }
            else if (colors)
            {
                data[pos] =   colors[pixel].red   >> 8;
                data[pos+1] = colors[pixel].green >> 8;
                data[pos+2] = colors[pixel].blue  >> 8;
            }
            else
            {
                wxFAIL_MSG( wxT("Image conversion failed. Unknown visual type.") );
            }

            if (x_image_mask)
            {
                int mask_pixel = XGetPixel( x_image_mask, i, j );
                if (mask_pixel == 0)
                {
                    data[pos] = 16;
                    data[pos+1] = 16;
                    data[pos+2] = 16;
                }
            }

            pos += 3;
        }
    }

    XDestroyImage( x_image );
    if (x_image_mask) XDestroyImage( x_image_mask );
    return image;
#endif
    // wxUSE_NANOX
}

wxBitmap::wxBitmap( const wxBitmap& bmp )
{
    Ref( bmp );
}

wxBitmap::wxBitmap( const wxString &filename, int type )
{
    LoadFile( filename, type );
}

wxBitmap::wxBitmap( const char bits[], int width, int height, int WXUNUSED(depth) )
{
#if !wxUSE_NANOX
    m_refData = new wxBitmapRefData();

    M_BMPDATA->m_display = wxGlobalDisplay();

    Display *xdisplay = (Display*) M_BMPDATA->m_display;
    
    int xscreen = DefaultScreen( xdisplay );
    Window xroot = RootWindow( xdisplay, xscreen );
    
    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_bitmap = (WXPixmap) XCreateBitmapFromData( xdisplay, xroot, (char *) bits, width, height );
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    M_BMPDATA->m_bpp = 1;
#endif
    wxCHECK_RET( M_BMPDATA->m_bitmap, wxT("couldn't create bitmap") );
}

wxBitmap::~wxBitmap()
{
}

wxBitmap& wxBitmap::operator = ( const wxBitmap& bmp )
{
    if ( m_refData != bmp.m_refData )
        Ref( bmp );

    return *this;
}

bool wxBitmap::operator == ( const wxBitmap& bmp ) const
{
    return m_refData == bmp.m_refData;
}

bool wxBitmap::operator != ( const wxBitmap& bmp ) const
{
    return m_refData != bmp.m_refData;
}

bool wxBitmap::Ok() const
{
    return (m_refData != NULL);
}

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_bpp;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( Ok(), (wxMask *) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );

    if (M_BMPDATA->m_mask) delete M_BMPDATA->m_mask;

    M_BMPDATA->m_mask = mask;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    *this = icon;
    return TRUE;
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= M_BMPDATA->m_width) && (rect.y+rect.height <= M_BMPDATA->m_height),
                 wxNullBitmap, wxT("invalid bitmap or bitmap region") );

    wxBitmap ret( rect.width, rect.height, M_BMPDATA->m_bpp );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

    
    wxFAIL_MSG( "wxBitmap::GetSubBitmap not yet implemented" );
    
#if 0
    if (ret.GetPixmap())
    {
        GdkGC *gc = gdk_gc_new( ret.GetPixmap() );
        gdk_draw_pixmap( ret.GetPixmap(), gc, GetPixmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
        gdk_gc_destroy( gc );
    }
    else
    {
        GdkGC *gc = gdk_gc_new( ret.GetBitmap() );
        gdk_wx_draw_bitmap( ret.GetBitmap(), gc, GetBitmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
        gdk_gc_destroy( gc );
    }

    if (GetMask())
    {
        wxMask *mask = new wxMask;
        mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, rect.width, rect.height, 1 );

        GdkGC *gc = gdk_gc_new( mask->m_bitmap );
        gdk_wx_draw_bitmap( mask->m_bitmap, gc, M_BMPDATA->m_mask->m_bitmap, 0, 0, rect.x, rect.y, rect.width, rect.height );
        gdk_gc_destroy( gc );

        ret.SetMask( mask );
    }
#endif

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, int type, wxPalette *WXUNUSED(palette) )
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid bitmap") );

    // Try to save the bitmap via wxImage handlers:
    {
        wxImage image(this->ConvertToImage()) ;
        if (image.Ok()) return image.SaveFile( name, type );
    }

    return FALSE;
}

bool wxBitmap::LoadFile( const wxString &name, int type )
{
    UnRef();

    if (!wxFileExists(name)) return FALSE;


    if (type == wxBITMAP_TYPE_XPM)
    {
#if wxUSE_XPM
#if wxHAVE_LIB_XPM
        m_refData = new wxBitmapRefData();

        M_BMPDATA->m_display = wxGlobalDisplay();
    
        Display *xdisplay = (Display*) M_BMPDATA->m_display;
    
        int xscreen = DefaultScreen( xdisplay );
        Window xroot = RootWindow( xdisplay, xscreen );
    
        int bpp = DefaultDepth( xdisplay, xscreen );

        XpmAttributes xpmAttr;
        xpmAttr.valuemask = XpmReturnInfos;    // nothing yet, but get infos back

        Pixmap pixmap;
        Pixmap mask = 0;
    
        int ErrorStatus = XpmReadFileToPixmap( xdisplay, xroot, (char*) name.c_str(), &pixmap, &mask, &xpmAttr);
    
        if (ErrorStatus == XpmSuccess)
        {
            M_BMPDATA->m_width = xpmAttr.width;
            M_BMPDATA->m_height = xpmAttr.height;

            M_BMPDATA->m_bpp = bpp;  // mono as well?

            XpmFreeAttributes(&xpmAttr);
        
            M_BMPDATA->m_bitmap = (WXPixmap) pixmap;
        
            if (mask)
            {
                M_BMPDATA->m_mask = new wxMask;
                M_BMPDATA->m_mask->SetBitmap( (WXPixmap) mask );
                M_BMPDATA->m_mask->SetDisplay( xdisplay );
            }
        }
        else
        {
            UnRef();
            
            return FALSE;
        }
#else
#if wxUSE_STREAMS
	wxXPMDecoder decoder;
	wxFileInputStream stream(name);
	if (stream.Ok())
	{
            wxImage image(decoder.ReadFile(stream));
	    if (image.Ok())
		return CreateFromImage(image);
	    else
		return FALSE;
	}
	else
	    return FALSE;
#else
	return FALSE;
#endif
	// wxUSE_STREAMS
#endif
	// wxHAVE_LIB_XPM
#endif
	// wxUSE_XPM
	return FALSE;
    }
    else // try if wxImage can load it
    {
        wxImage image;
        if (!image.LoadFile( name, type )) return FALSE;
        if (image.Ok()) *this = wxBitmap(image);
        else return FALSE;
    }

    return TRUE;
}

wxPalette *wxBitmap::GetPalette() const
{
    if (!Ok()) return (wxPalette *) NULL;

    return M_BMPDATA->m_palette;
}

void wxBitmap::SetHeight( int height )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth( int width )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth( int depth )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_bpp = depth;
}

void wxBitmap::SetPixmap( WXPixmap pixmap )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_pixmap = pixmap;
}

void wxBitmap::SetBitmap( WXPixmap bitmap )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_bitmap = bitmap;
}

WXPixmap wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( Ok(), (WXPixmap) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixmap;
}

WXPixmap wxBitmap::GetBitmap() const
{
    wxCHECK_MSG( Ok(), (WXPixmap) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_bitmap;
}

WXDisplay *wxBitmap::GetDisplay() const
{
    wxCHECK_MSG( Ok(), (WXDisplay*) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_display;
}

#if wxUSE_NANOX
// Copy from the drawable to the wxImage
bool wxGetImageFromDrawable(GR_DRAW_ID drawable, int srcX, int srcY, int width, int height, wxImage& image)
{
    GR_SCREEN_INFO sinfo;
    int x, y;
    GR_PIXELVAL *pixels;
    GR_PALETTE* palette = NULL;
    unsigned char rgb[3], *pp;

    GrGetScreenInfo(&sinfo);

    if (sinfo.pixtype == MWPF_PALETTE) {
		if(!(palette = (GR_PALETTE*) malloc(sizeof(GR_PALETTE)))) {
			return FALSE;
		}
		GrGetSystemPalette(palette);
	}

    if(!(pixels = (GR_PIXELVAL*) malloc(sizeof(GR_PIXELVAL) * width * height)))
    {
        return FALSE;
    }

    image.Create(width, height);

	GrReadArea(drawable, srcX, srcY, width, height,
						pixels);


		for(x = 0; x < sinfo.cols; x++) {

			pp = (unsigned char *)pixels +
				((x + (y * sinfo.cols)) *
				sizeof(GR_PIXELVAL));

			switch(sinfo.pixtype) {
			/* FIXME: These may need modifying on big endian. */
				case MWPF_TRUECOLOR0888:
				case MWPF_TRUECOLOR888:
					rgb[0] = pp[2];
					rgb[1] = pp[1];
					rgb[2] = pp[0];
					break;
				case MWPF_PALETTE:
					rgb[0] = palette->palette[pp[0]].r;
					rgb[1] = palette->palette[pp[0]].g;
					rgb[2] = palette->palette[pp[0]].b;
					break;
				case MWPF_TRUECOLOR565:
					rgb[0] = pp[1] & 0xf8;
					rgb[1] = ((pp[1] & 0x07) << 5) |
						((pp[0] & 0xe0) >> 3);
					rgb[2] = (pp[0] & 0x1f) << 3;
					break;
				case MWPF_TRUECOLOR555:
					rgb[0] = (pp[1] & 0x7c) << 1;
					rgb[1] = ((pp[1] & 0x03) << 6) |
						((pp[0] & 0xe0) >> 2);
					rgb[2] = (pp[0] & 0x1f) << 3;
					break;
				case MWPF_TRUECOLOR332:
					rgb[0] = pp[0] & 0xe0;
					rgb[1] = (pp[0] & 0x1c) << 3;
					rgb[2] = (pp[0] & 0x03) << 6;
					break;
				default:
					fprintf(stderr, "Unsupported pixel "
							"format\n");
					return 1;
			}

            image.SetRGB(x, y, rgb[0], rgb[1], rgb[2]);

	}

    free(pixels);
    if(palette) free(palette);

    return TRUE;
}

#if 0
int GrGetPixelColor(GR_SCREEN_INFO* sinfo, GR_PALETTE* palette, GR_PIXELVAL pixel,
    unsigned char* red, unsigned char* green, unsigned char* blue)
{
	unsigned char rgb[3], *pp;

    pp = (unsigned char*) & pixel ;

    switch (sinfo.pixtype)
    {
			/* FIXME: These may need modifying on big endian. */
				case MWPF_TRUECOLOR0888:
				case MWPF_TRUECOLOR888:
					rgb[0] = pp[2];
					rgb[1] = pp[1];
					rgb[2] = pp[0];
					break;
				case MWPF_PALETTE:
					rgb[0] = palette->palette[pp[0]].r;
					rgb[1] = palette->palette[pp[0]].g;
					rgb[2] = palette->palette[pp[0]].b;
					break;
				case MWPF_TRUECOLOR565:
					rgb[0] = pp[1] & 0xf8;
					rgb[1] = ((pp[1] & 0x07) << 5) |
						((pp[0] & 0xe0) >> 3);
					rgb[2] = (pp[0] & 0x1f) << 3;
					break;
				case MWPF_TRUECOLOR555:
					rgb[0] = (pp[1] & 0x7c) << 1;
					rgb[1] = ((pp[1] & 0x03) << 6) |
						((pp[0] & 0xe0) >> 2);
					rgb[2] = (pp[0] & 0x1f) << 3;
					break;
				case MWPF_TRUECOLOR332:
					rgb[0] = pp[0] & 0xe0;
					rgb[1] = (pp[0] & 0x1c) << 3;
					rgb[2] = (pp[0] & 0x03) << 6;
					break;
				default:
					fprintf(stderr, "Unsupported pixel format\n");
					return 0;
	}


    *(red) = rgb[0];
    *(green) = rgb[1];
    *(blue) = rgb[2];
    return 1;
}
#endif

#endif
  // wxUSE_NANOX

