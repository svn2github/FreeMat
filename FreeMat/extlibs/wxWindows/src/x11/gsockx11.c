/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockmot.c
 * Purpose: GSocket: X11 part
 * CVSID:   $Id$
 * ------------------------------------------------------------------------- */

#include "wx/setup.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/gsocket.h"
#include "wx/unix/gsockunx.h"

/*
 * FIXME: have these in a common header instead of being repeated
 * in evtloop.cpp and gsockx11.c
 */

typedef void (*wxSocketCallback) (int fd, void* data);

typedef enum
{ wxSocketTableInput, wxSocketTableOutput } wxSocketTableType ;

void wxRegisterSocketCallback(int fd, wxSocketTableType socketType, wxSocketCallback cback, void* data);
void wxUnregisterSocketCallback(int fd, wxSocketTableType socketType);


static void _GSocket_X11_Input(int *fid, void* data)
{
  GSocket *socket = (GSocket *)data;
  
  _GSocket_Detected_Read(socket);
}

static void _GSocket_X11_Output(int *fid, void* data)
{
  GSocket *socket = (GSocket *)data;

  _GSocket_Detected_Write(socket);
}

int _GSocket_GUI_Init(GSocket *socket)
{
  int *m_id;

  socket->m_gui_dependent = (char *)malloc(sizeof(int)*2);
  m_id = (int *)(socket->m_gui_dependent);

  m_id[0] = -1;
  m_id[1] = -1;

  return TRUE;
}

void _GSocket_GUI_Destroy(GSocket *socket)
{
  free(socket->m_gui_dependent);
}

void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event)
{
  int *m_id = (int *)(socket->m_gui_dependent);
  int c;

  if (socket->m_fd == -1)
    return;

  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

#if 0
  if (m_id[c] != -1)
      XtRemoveInput(m_id[c]);
#endif /* 0 */

  if (c == 0)
  {
      m_id[0] = socket->m_fd;

      wxRegisterSocketCallback(socket->m_fd, wxSocketTableInput,
                               (wxSocketCallback) _GSocket_X11_Input, (void*) socket);
  }
  else
  {
      m_id[1] = socket->m_fd;

      wxRegisterSocketCallback(socket->m_fd, wxSocketTableOutput,
                               (wxSocketCallback) _GSocket_X11_Output, (void*) socket);
  }
}

void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
  int *m_id = (int *)(socket->m_gui_dependent);
  int c;

  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

  if (m_id[c] != -1)
  {
      if (c == 0)
          wxUnregisterSocketCallback(m_id[c], wxSocketTableInput);
      else
          wxUnregisterSocketCallback(m_id[c], wxSocketTableOutput);
  }

  m_id[c] = -1;
}

void _GSocket_Enable_Events(GSocket *socket)
{
  _GSocket_Install_Callback(socket, GSOCK_INPUT);
  _GSocket_Install_Callback(socket, GSOCK_OUTPUT);
}

void _GSocket_Disable_Events(GSocket *socket)
{
  _GSocket_Uninstall_Callback(socket, GSOCK_INPUT);
  _GSocket_Uninstall_Callback(socket, GSOCK_OUTPUT);
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
