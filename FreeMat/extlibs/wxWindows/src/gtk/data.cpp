/////////////////////////////////////////////////////////////////////////////
// Name:        data.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/window.h"
#include "wx/dc.h"

#if wxUSE_ACCEL
#include "wx/accel.h"
#endif

#include "wx/dcps.h"
#include "wx/icon.h"

#define _MAXPATHLEN 500

/* Windows List */
wxWindowList wxTopLevelWindows;

/* List of windows pending deletion */
wxList wxPendingDelete;

/* Current cursor, in order to hang on to
 * cursor handle when setting the cursor globally */
wxCursor g_globalCursor;

/* Don't allow event propagation during drag */
bool g_blockEventsOnDrag = FALSE;

/* Don't allow mouse event propagation during scroll */
bool g_blockEventsOnScroll = FALSE;

/* Don't allow window closing if there are open dialogs */
int g_openDialogs = 0;

/* TRUE when the message queue is empty. this gets set to
   FALSE by all event callbacks before anything else is done */
bool g_isIdle = FALSE;

/* Message Strings for Internationalization */
char **wx_msg_str = (char**)NULL;

/* For printing several pages */
int wxPageNumber;

// Now in prntbase.cpp
// wxPrintPaperDatabase* wxThePrintPaperDatabase = (wxPrintPaperDatabase *) NULL;

/* GDI Object Lists */
wxBrushList      *wxTheBrushList = (wxBrushList *) NULL;
wxPenList        *wxThePenList = (wxPenList *) NULL;
wxFontList       *wxTheFontList = (wxFontList *) NULL;
wxColourDatabase *wxTheColourDatabase = (wxColourDatabase *) NULL;
wxBitmapList     *wxTheBitmapList = (wxBitmapList *) NULL;

/* X only font names */
/*
wxFontNameDirectory *wxTheFontNameDirectory;
*/

/* Stock objects */
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
wxColour *wxGREY;
wxColour *wxRED;
wxColour *wxBLUE;
wxColour *wxGREEN;
wxColour *wxCYAN;
wxColour *wxLIGHT_GREY;

wxCursor *wxSTANDARD_CURSOR = (wxCursor *) NULL;
wxCursor *wxHOURGLASS_CURSOR = (wxCursor *) NULL;
wxCursor *wxCROSS_CURSOR = (wxCursor *) NULL;

/* 'Null' objects */
#if wxUSE_ACCEL
    wxAcceleratorTable   wxNullAcceleratorTable;
#endif // wxUSE_ACCEL

wxBitmap   wxNullBitmap;
wxIcon     wxNullIcon;
wxCursor   wxNullCursor;
wxPen      wxNullPen;
wxBrush    wxNullBrush;
wxFont     wxNullFont;
wxColour   wxNullColour;
wxPalette  wxNullPalette;

/* Default window names */
const wxChar *wxControlNameStr = wxT("control");
const wxChar *wxButtonNameStr = wxT("button");
const wxChar *wxCanvasNameStr = wxT("canvas");
const wxChar *wxCheckBoxNameStr = wxT("check");
const wxChar *wxChoiceNameStr = wxT("choice");
const wxChar *wxComboBoxNameStr = wxT("comboBox");
const wxChar *wxDialogNameStr = wxT("dialog");
const wxChar *wxFrameNameStr = wxT("frame");
const wxChar *wxGaugeNameStr = wxT("gauge");
const wxChar *wxStaticBoxNameStr = wxT("groupBox");
const wxChar *wxListBoxNameStr = wxT("listBox");
const wxChar *wxStaticTextNameStr = wxT("message");
const wxChar *wxStaticBitmapNameStr = wxT("message");
const wxChar *wxMultiTextNameStr = wxT("multitext");
const wxChar *wxPanelNameStr = wxT("panel");
const wxChar *wxRadioBoxNameStr = wxT("radioBox");
const wxChar *wxRadioButtonNameStr = wxT("radioButton");
const wxChar *wxBitmapRadioButtonNameStr = wxT("radioButton");
const wxChar *wxScrollBarNameStr = wxT("scrollBar");
const wxChar *wxSliderNameStr = wxT("slider");
const wxChar *wxStaticNameStr = wxT("static");
const wxChar *wxTextCtrlWindowNameStr = wxT("textWindow");
const wxChar *wxTextCtrlNameStr = wxT("text");
const wxChar *wxVirtListBoxNameStr = wxT("virtListBox");
const wxChar *wxButtonBarNameStr = wxT("buttonbar");
const wxChar *wxEnhDialogNameStr = wxT("Shell");
const wxChar *wxToolBarNameStr = wxT("toolbar");
const wxChar *wxStatusLineNameStr = wxT("status_line");
const wxChar *wxGetTextFromUserPromptStr = wxT("Input Text");
const wxChar *wxMessageBoxCaptionStr = wxT("Message");
const wxChar *wxFileSelectorPromptStr = wxT("Select a file");
const wxChar *wxFileSelectorDefaultWildcardStr = wxT("*");
const wxChar *wxDirDialogNameStr = wxT("wxDirCtrl");
const wxChar *wxDirDialogDefaultFolderStr = wxT("/");
const wxChar *wxTreeCtrlNameStr = wxT("wxTreeCtrl");

/* See wx/utils.h */
const wxChar *wxFloatToStringStr = wxT("%.2f");
const wxChar *wxDoubleToStringStr = wxT("%.2f");

/* Dafaults for wxWindow etc. */
const wxSize wxDefaultSize(-1, -1);
const wxPoint wxDefaultPosition(-1, -1);
