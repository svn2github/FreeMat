/////////////////////////////////////////////////////////////////////////////
// Name:        data.cpp
// Purpose:     Various data
// Author:      David Webster
// Modified by:
// Created:     09/20/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/prntbase.h"

#define _MAXPATHLEN 500

// Useful buffer, initialized in wxCommonInit
wxChar *wxBuffer = NULL;

// Windows List
wxWindowList wxTopLevelWindows;

// List of windows pending deletion
wxList WXDLLEXPORT wxPendingDelete;

// Current cursor, in order to hang on to
// cursor handle when setting the cursor globally
wxCursor *g_globalCursor = NULL;

// Message Strings for Internationalization
char **wx_msg_str = (char**)NULL;

int wxPageNumber;

// GDI Object Lists
wxBrushList *wxTheBrushList = NULL;
wxPenList   *wxThePenList = NULL;
wxFontList   *wxTheFontList = NULL;
wxBitmapList   *wxTheBitmapList = NULL;

wxColourDatabase *wxTheColourDatabase = NULL;

// Stock objects
wxFont *wxNORMAL_FONT;
wxFont *wxSMALL_FONT;
wxFont *wxITALIC_FONT;
wxFont *wxSWISS_FONT;
wxPen *wxRED_PEN;

wxPen *wxCYAN_PEN;
wxPen *wxGREEN_PEN;
wxPen *wxBLACK_PEN;
wxPen *wxWHITE_PEN;
wxPen *wxTRANSPARENT_PEN;
wxPen *wxBLACK_DASHED_PEN;
wxPen *wxGREY_PEN;
wxPen *wxMEDIUM_GREY_PEN;
wxPen *wxLIGHT_GREY_PEN;

wxBrush *wxBLUE_BRUSH;
wxBrush *wxGREEN_BRUSH;
wxBrush *wxWHITE_BRUSH;
wxBrush *wxBLACK_BRUSH;
wxBrush *wxTRANSPARENT_BRUSH;
wxBrush *wxCYAN_BRUSH;
wxBrush *wxRED_BRUSH;
wxBrush *wxGREY_BRUSH;
wxBrush *wxMEDIUM_GREY_BRUSH;
wxBrush *wxLIGHT_GREY_BRUSH;

wxColour *wxBLACK;
wxColour *wxWHITE;
wxColour *wxRED;
wxColour *wxBLUE;
wxColour *wxGREEN;
wxColour *wxCYAN;
wxColour *wxLIGHT_GREY;

wxCursor *wxSTANDARD_CURSOR = NULL;
wxCursor *wxHOURGLASS_CURSOR = NULL;
wxCursor *wxCROSS_CURSOR = NULL;

// 'Null' objects
wxAcceleratorTable wxNullAcceleratorTable;
wxBitmap wxNullBitmap;
wxIcon   wxNullIcon;
wxCursor wxNullCursor;
wxPen    wxNullPen;
wxBrush  wxNullBrush;
wxPalette wxNullPalette;
wxFont   wxNullFont;
wxColour wxNullColour;
wxNativeFontInfo wxNullNativeFontInfo;

//
// Default window names
//
const wxChar*                       wxButtonNameStr                  = wxT("button");
const wxChar*                       wxCanvasNameStr                  = wxT("canvas");
const wxChar*                       wxCheckBoxNameStr                = wxT("check");
const wxChar*                       wxChoiceNameStr                  = wxT("choice");
const wxChar*                       wxComboBoxNameStr                = wxT("comboBox");
const wxChar*                       wxDialogNameStr                  = wxT("dialog");
const wxChar*                       wxFrameNameStr                   = wxT("frame");
const wxChar*                       wxGaugeNameStr                   = wxT("gauge");
const wxChar*                       wxStaticBoxNameStr               = wxT("groupBox");
const wxChar*                       wxListBoxNameStr                 = wxT("listBox");
const wxChar*                       wxStaticTextNameStr              = wxT("message");
const wxChar*                       wxStaticBitmapNameStr            = wxT("message");
const wxChar*                       wxMultiTextNameStr               = wxT("multitext");
const wxChar*                       wxPanelNameStr                   = wxT("panel");
const wxChar*                       wxRadioBoxNameStr                = wxT("radioBox");
const wxChar*                       wxRadioButtonNameStr             = wxT("radioButton");
const wxChar*                       wxBitmapRadioButtonNameStr       = wxT("radioButton");
const wxChar*                       wxScrollBarNameStr               = wxT("scrollBar");
const wxChar*                       wxSliderNameStr                  = wxT("slider");
const wxChar*                       wxStaticNameStr                  = wxT("static");
const wxChar*                       wxTextCtrlWindowNameStr          = wxT("textWindow");
const wxChar*                       wxTextCtrlNameStr                = wxT("text");
const wxChar*                       wxVirtListBoxNameStr             = wxT("virtListBox");
const wxChar*                       wxButtonBarNameStr               = wxT("buttonbar");
const wxChar*                       wxEnhDialogNameStr               = wxT("Shell");
const wxChar*                       wxToolBarNameStr                 = wxT("toolbar");
const wxChar*                       wxStatusLineNameStr              = wxT("status_line");
const wxChar*                       wxGetTextFromUserPromptStr       = wxT("Input Text");
const wxChar*                       wxMessageBoxCaptionStr           = wxT("Message");
const wxChar*                       wxFileSelectorPromptStr          = wxT("Select a file");
const wxChar*                       wxFileSelectorDefaultWildcardStr = wxT("*.*");
const wxChar*                       wxTreeCtrlNameStr                = wxT("treeCtrl");
const wxChar*                       wxDirDialogNameStr               = wxT("wxDirCtrl");
const wxChar*                       wxDirDialogDefaultFolderStr      = wxT("/");

//
// Class names
//
const wxChar*                       wxFrameClassName                 = wxT("wxFrameClass");
const wxChar*                       wxFrameClassNameNoRedraw         = wxT("wxFrameClassNR");
const wxChar*                       wxMDIFrameClassName              = wxT("wxMDIFrameClass");
const wxChar*                       wxMDIFrameClassNameNoRedraw      = wxT("wxMDIFrameClassNR");
const wxChar*                       wxMDIChildFrameClassName         = wxT("wxMDIChildFrameClass");
const wxChar*                       wxMDIChildFrameClassNameNoRedraw = wxT("wxMDIChildFrameClassNR");
const wxChar*                       wxPanelClassName                 = wxT("wxPanelClass");
const wxChar*                       wxPanelClassNameNR               = wxT("wxPanelClassNR");
const wxChar*                       wxCanvasClassName                = wxT("wxCanvasClass");
const wxChar*                       wxCanvasClassNameNR              = wxT("wxCanvasClassNR");

// See wx/utils.h
const wxChar *wxFloatToStringStr = wxT("%.2f");
const wxChar *wxDoubleToStringStr = wxT("%.2f");

const wxChar *wxUserResourceStr = wxT("TEXT");

const wxSize wxDefaultSize(-1, -1);
const wxPoint wxDefaultPosition(-1, -1);

