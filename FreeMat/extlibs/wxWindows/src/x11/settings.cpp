/////////////////////////////////////////////////////////////////////////////
// Name:        settings.cpp
// Purpose:     wxSettings
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// TODO: these settings should probably be configurable from some central or
// per-user file, which can be edited using a Windows-control-panel clone.
// Also they should be documented better. Some are very MS Windows-ish.

#ifdef __GNUG__
#pragma implementation "settings.h"
#endif

#include "wx/settings.h"
#include "wx/gdicmn.h"
#include "wx/x11/private.h"

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    switch (index)
    {
        case wxSYS_COLOUR_APPWORKSPACE:
            return wxColour( 0xc0c0c0 );
            
        default:
            break;
    }
    
    // Overridden mostly by wxSystemSettings::GetColour in wxUniversal
    return *wxWHITE;
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    switch (index)
    {
        case wxSYS_SYSTEM_FIXED_FONT:
        {
            return wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, FALSE);
            break;
        }
        case wxSYS_DEVICE_DEFAULT_FONT:
        case wxSYS_SYSTEM_FONT:
        case wxSYS_DEFAULT_GUI_FONT:
        default:
        {
            return wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, FALSE);
            break;
        }
    }

    return wxFont();
}

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index)
{
	switch ( index)
	{
        case wxSYS_MOUSE_BUTTONS:
            // TODO
            return 0;
        case wxSYS_BORDER_X:
            // TODO
            return 0;
        case wxSYS_BORDER_Y:
            // TODO
            return 0;
        case wxSYS_CURSOR_X:
            // TODO
            return 0;
        case wxSYS_CURSOR_Y:
            // TODO
            return 0;
        case wxSYS_DCLICK_X:
            // TODO
            return 0;
        case wxSYS_DCLICK_Y:
            // TODO
            return 0;
        case wxSYS_DRAG_X:
            // TODO
            return 0;
        case wxSYS_DRAG_Y:
            // TODO
            return 0;
        case wxSYS_EDGE_X:
            // TODO
            return 0;
        case wxSYS_EDGE_Y:
            // TODO
            return 0;
        case wxSYS_HSCROLL_ARROW_X:
            // TODO
            return 0;
        case wxSYS_HSCROLL_ARROW_Y:
            // TODO
            return 0;
        case wxSYS_HTHUMB_X:
            // TODO
            return 0;
        case wxSYS_ICON_X:
            // TODO
            return 0;
        case wxSYS_ICON_Y:
            // TODO
            return 0;
        case wxSYS_ICONSPACING_X:
            // TODO
            return 0;
        case wxSYS_ICONSPACING_Y:
            // TODO
            return 0;
        case wxSYS_WINDOWMIN_X:
            // TODO
            return 0;
        case wxSYS_WINDOWMIN_Y:
            // TODO
            return 0;
        case wxSYS_SCREEN_X:
            return DisplayWidth( wxGlobalDisplay(), 0 );
            
        case wxSYS_SCREEN_Y:
            return DisplayHeight( wxGlobalDisplay(), 0 );
            
        case wxSYS_FRAMESIZE_X:
            // TODO
            return 0;
        case wxSYS_FRAMESIZE_Y:
            // TODO
            return 0;
        case wxSYS_SMALLICON_X:
            // TODO
            return 0;
        case wxSYS_SMALLICON_Y:
            // TODO
            return 0;
        case wxSYS_HSCROLL_Y:
            // TODO
            return 0;
        case wxSYS_VSCROLL_X:
            // TODO
            return 0;
        case wxSYS_VSCROLL_ARROW_X:
            // TODO
            return 0;
        case wxSYS_VSCROLL_ARROW_Y:
            // TODO
            return 0;
        case wxSYS_VTHUMB_Y:
            // TODO
            return 0;
        case wxSYS_CAPTION_Y:
            // TODO
            return 0;
        case wxSYS_MENU_Y:
            // TODO
            return 0;
        case wxSYS_NETWORK_PRESENT:
            // TODO
            return 0;
        case wxSYS_PENWINDOWS_PRESENT:
            // TODO
            return 0;
        case wxSYS_SHOW_SOUNDS:
            // TODO
            return 0;
        case wxSYS_SWAP_BUTTONS:
            // TODO
            return 0;
        default:
            return 0;
    }
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    switch (index)
    {
        case wxSYS_CAN_ICONIZE_FRAME: 
        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return TRUE;
            
        default:
            return FALSE;
    }
}
