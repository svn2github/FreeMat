/////////////////////////////////////////////////////////////////////////////
// Name:        colordlg.h
// Purpose:     wxColourDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLORDLG_H_
#define _WX_COLORDLG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "colordlg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * Platform-specific colour dialog implementation
 */

class WXDLLEXPORT wxColourDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxColourDialog)
public:
    wxColourDialog();
    wxColourDialog(wxWindow *parent, wxColourData *data = NULL);

    bool Create(wxWindow *parent, wxColourData *data = NULL);

    int ShowModal();
    wxColourData& GetColourData() { return m_colourData; }

protected:
    wxColourData        m_colourData;
    wxWindow*           m_dialogParent;
};

#endif
    // _WX_COLORDLG_H_
