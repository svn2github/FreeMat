/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWindows under Macintosh
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "glcanvas.h"
#endif

#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/setup.h"

#if wxUSE_GLCANVAS

#ifndef WX_PRECOMP
#include "wx/frame.h"
#endif

#include "wx/settings.h"
#include "wx/log.h"

#include "wx/glcanvas.h"
#include "wx/mac/uma.h"

/*
 * GLContext implementation
 */

wxGLContext::wxGLContext(
               AGLPixelFormat fmt, wxGLCanvas *win,
               const wxPalette& palette,
               const wxGLContext *other        /* for sharing display lists */
)
{
    m_window = win;

    m_drawable = (AGLDrawable) UMAGetWindowPort(MAC_WXHWND(win->MacGetRootWindow()));

    m_glContext = aglCreateContext(fmt, other ? other->m_glContext : NULL);
    wxCHECK_RET( m_glContext, wxT("Couldn't create OpenGl context") );

	GLboolean b;
    b = aglSetDrawable(m_glContext, m_drawable);
    wxCHECK_RET( b, wxT("Couldn't bind OpenGl context") );
 	aglEnable(m_glContext , AGL_BUFFER_RECT ) ;
    b = aglSetCurrentContext(m_glContext);
    wxCHECK_RET( b, wxT("Couldn't activate OpenGl context") );
}

wxGLContext::~wxGLContext()
{
  if (m_glContext)
  {
    aglSetCurrentContext(NULL);
  	aglDestroyContext(m_glContext);
  }
}

void wxGLContext::SwapBuffers()
{
  if (m_glContext)
  {
    aglSwapBuffers(m_glContext);
  }
}

void wxGLContext::SetCurrent()
{
  if (m_glContext)
  {
    aglSetCurrentContext(m_glContext);
  }
}

void wxGLContext::Update()
{
  if (m_glContext)
  {
    aglUpdateContext(m_glContext);
  }
}

void wxGLContext::SetColour(const char *colour)
{
  float r = 0.0;
  float g = 0.0;
  float b = 0.0;
  wxColour *col = wxTheColourDatabase->FindColour(colour);
  if (col)
  {
    r = (float)(col->Red()/256.0);
    g = (float)(col->Green()/256.0);
    b = (float)(col->Blue()/256.0);
    glColor3f( r, g, b);
  }
}


/*
 * wxGLCanvas implementation
 */

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
END_EVENT_TABLE()

wxGLCanvas::wxGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name,
    int *attribList, const wxPalette& palette)
{
    Create(parent, NULL, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
              const wxGLContext *shared, wxWindowID id,
              const wxPoint& pos, const wxSize& size, long style, const wxString& name,
              int *attribList, const wxPalette& palette )
{
    Create(parent, shared, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::wxGLCanvas( wxWindow *parent, const wxGLCanvas *shared, wxWindowID id,
                        const wxPoint& pos, const wxSize& size, long style, const wxString& name,
                        int *attribList, const wxPalette& palette )
{
    Create(parent, shared ? shared->GetContext() : NULL, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::~wxGLCanvas()
{
    if (m_glContext != NULL) {
        delete m_glContext;
        m_glContext = NULL;
    }
}

static AGLPixelFormat ChoosePixelFormat(const int *attribList)
{
    GLint data[512];
    GLint defaultAttribs[] = { AGL_RGBA, 
		     AGL_DOUBLEBUFFER, 
		     AGL_MINIMUM_POLICY, 
		     AGL_DEPTH_SIZE, 1,  // use largest available depth buffer
		     AGL_RED_SIZE, 1, 
		     AGL_GREEN_SIZE, 1, 
		     AGL_BLUE_SIZE, 1, 
		     AGL_ALPHA_SIZE, 0, 
		     AGL_NONE };
    GLint *attribs;
    if (!attribList)
    {
	  attribs = defaultAttribs;
    }
    else
    {
      int arg=0, p=0;

      data[p++] = AGL_MINIMUM_POLICY; // make _SIZE tags behave more like GLX
      while( (attribList[arg]!=0) && (p<512) )
      {
        switch( attribList[arg++] )
        {
          case WX_GL_RGBA: data[p++] = AGL_RGBA; break;
          case WX_GL_BUFFER_SIZE:
            data[p++]=AGL_BUFFER_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_LEVEL:
            data[p++]=AGL_LEVEL; data[p++]=attribList[arg++]; break;
          case WX_GL_DOUBLEBUFFER: data[p++] = AGL_DOUBLEBUFFER; break;
          case WX_GL_STEREO: data[p++] = AGL_STEREO; break;
          case WX_GL_AUX_BUFFERS:
            data[p++]=AGL_AUX_BUFFERS; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_RED:
            data[p++]=AGL_RED_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_GREEN:
            data[p++]=AGL_GREEN_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_BLUE:
            data[p++]=AGL_BLUE_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ALPHA:
            data[p++]=AGL_ALPHA_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_DEPTH_SIZE: 
            data[p++]=AGL_DEPTH_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_STENCIL_SIZE: 
            data[p++]=AGL_STENCIL_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_RED:
            data[p++]=AGL_ACCUM_RED_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_GREEN:
            data[p++]=AGL_ACCUM_GREEN_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_BLUE:
            data[p++]=AGL_ACCUM_BLUE_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_ALPHA:
            data[p++]=AGL_ACCUM_ALPHA_SIZE; data[p++]=attribList[arg++]; break;
          default:
            break;
        }
      }       
      data[p] = 0; 

      attribs = data;
    }
    
    return aglChoosePixelFormat(NULL, 0, attribs);
}

bool wxGLCanvas::Create(wxWindow *parent, const wxGLContext *shared, wxWindowID id,
			const wxPoint& pos, const wxSize& size, long style, const wxString& name,
			int *attribList, const wxPalette& palette)
{
    wxWindow::Create( parent, id, pos, size, style, name );

    AGLPixelFormat fmt = ChoosePixelFormat(attribList);
    wxCHECK_MSG( fmt, false, wxT("Couldn't create OpenGl pixel format") );

    m_glContext = new wxGLContext(fmt, this, palette, shared);

    aglDestroyPixelFormat(fmt);
    
    return true;
}

void wxGLCanvas::SwapBuffers()
{
  if (m_glContext)
    m_glContext->SwapBuffers();
}

void wxGLCanvas::UpdateContext()
{
  if (m_glContext)
    m_glContext->Update();
}

void wxGLCanvas::SetViewport()
{
  // viewport is initially set to entire port
  // adjust glViewport to just this window
    int x = 0 ;
    int y = 0 ;
    
    MacClientToRootWindow( &x , &y ) ;
    int width, height;
    GetClientSize(& width, & height);
    Rect bounds ;
    GetWindowPortBounds( MAC_WXHWND(MacGetRootWindow()) , &bounds ) ;
    GLint parms[4] ;
    parms[0] = x ;
    parms[1] = bounds.bottom - bounds.top - ( y + height ) ;
    parms[2] = width ;
    parms[3] = height ;
  
   	aglSetInteger( m_glContext->m_glContext , AGL_BUFFER_RECT , parms ) ;
}

void wxGLCanvas::OnSize(wxSizeEvent& event)
{
  MacUpdateView() ;
}

void wxGLCanvas::MacUpdateView()
{
  if (m_glContext)
  {
    UpdateContext();
    m_glContext->SetCurrent();
    SetViewport();
  }
}

void wxGLCanvas::MacSuperChangedPosition()
{
  MacUpdateView() ;
  wxWindow::MacSuperChangedPosition() ;
}

void wxGLCanvas::MacTopLevelWindowChangedPosition()
{
  MacUpdateView() ;
  wxWindow::MacTopLevelWindowChangedPosition() ;
}

void wxGLCanvas::SetCurrent()
{
  if (m_glContext)
  {
    m_glContext->SetCurrent();
  }
}

void wxGLCanvas::SetColour(const char *colour)
{
  if (m_glContext)
    m_glContext->SetColour(colour);
}


//---------------------------------------------------------------------------
// wxGLApp
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLApp, wxApp)

bool wxGLApp::InitGLVisual(int *attribList)
{
    AGLPixelFormat fmt = ChoosePixelFormat(attribList);
    if (fmt != NULL) {
        aglDestroyPixelFormat(fmt);
        return true;
    } else
        return false;
}

wxGLApp::~wxGLApp(void)
{
}

#endif // wxUSE_GLCANVAS
