/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.cpp
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// Modified:    13.12.99 by VZ to derive from wxToolBarBase
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tbargtk.h"
#endif

#include "wx/toolbar.h"

#if wxUSE_TOOLBAR_NATIVE

#include "wx/frame.h"

#include <glib.h>
#include "wx/gtk/private.h"

extern GdkFont *GtkGetDefaultGuiFont();

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// idle system
extern void wxapp_install_idle_handler();
extern bool g_isIdle;

// data
extern bool       g_blockEventsOnDrag;
extern wxCursor   g_globalCursor;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// translate wxWindows toolbar style flags to GTK orientation and style
static void GetGtkStyle(long style,
                        GtkOrientation *orient, GtkToolbarStyle *gtkStyle)
{
    *orient = style & wxTB_VERTICAL ? GTK_ORIENTATION_VERTICAL
                                    : GTK_ORIENTATION_HORIZONTAL;


    if ( style & wxTB_TEXT )
    {
        *gtkStyle = style & wxTB_NOICONS ? GTK_TOOLBAR_TEXT : GTK_TOOLBAR_BOTH;
    }
    else // no text, hence we must have the icons or what would we show?
    {
        *gtkStyle = GTK_TOOLBAR_ICONS;
    }
}

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxString& label,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  wxItemKind kind,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, label, bitmap1, bitmap2, kind,
                            clientData, shortHelpString, longHelpString)
    {
        Init();
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        Init();
    }

    // is this a radio button?
    //
    // unlike GetKind(), can be called for any kind of tools, not just buttons
    bool IsRadio() const { return IsButton() && GetKind() == wxITEM_RADIO; }

    // this is only called for the normal buttons, i.e. not separators nor
    // controls
    GtkToolbarChildType GetGtkChildType() const
    {
        switch ( GetKind() )
        {
            case wxITEM_CHECK:
                return GTK_TOOLBAR_CHILD_TOGGLEBUTTON;

            case wxITEM_RADIO:
                return GTK_TOOLBAR_CHILD_RADIOBUTTON;

            default:
                wxFAIL_MSG( _T("unknown toolbar child type") );
                // fall through

            case wxITEM_NORMAL:
                return GTK_TOOLBAR_CHILD_BUTTON;
        }
    }

    GtkWidget            *m_item;
    GtkWidget            *m_pixmap;

protected:
    void Init();
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// "clicked" (internal from gtk_toolbar)
//-----------------------------------------------------------------------------

static void gtk_toolbar_callback( GtkWidget *WXUNUSED(widget),
                                  wxToolBarTool *tool )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    wxToolBar *tbar = (wxToolBar *)tool->GetToolBar();

    if (tbar->m_blockEvent) return;

    if (g_blockEventsOnDrag) return;
    if (!tool->IsEnabled()) return;

    if (tool->CanBeToggled())
    {
        tool->Toggle();

        wxBitmap bitmap = tool->GetBitmap();
        if ( bitmap.Ok() )
        {
            GtkPixmap *pixmap = GTK_PIXMAP( tool->m_pixmap );

            GdkBitmap *mask = bitmap.GetMask() ? bitmap.GetMask()->GetBitmap()
                                               : (GdkBitmap *)NULL;

            gtk_pixmap_set( pixmap, bitmap.GetPixmap(), mask );
        }

        if ( tool->IsRadio() && !tool->IsToggled() )
        {
            // radio button went up, don't report this as a wxWin event
            return;
        }
    }

    tbar->OnLeftClick( tool->GetId(), tool->IsToggled() );
}

//-----------------------------------------------------------------------------
// "enter_notify_event" / "leave_notify_event"
//-----------------------------------------------------------------------------

static gint gtk_toolbar_tool_callback( GtkWidget *WXUNUSED(widget),
                                       GdkEventCrossing *gdk_event,
                                       wxToolBarTool *tool )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return TRUE;

    wxToolBar *tb = (wxToolBar *)tool->GetToolBar();

    // emit the event
    if( gdk_event->type == GDK_ENTER_NOTIFY )
        tb->OnMouseEnter( tool->GetId() );
    else
        tb->OnMouseEnter( -1 );

    return FALSE;
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxToolBar
//-----------------------------------------------------------------------------

static void wxInsertChildInToolBar( wxToolBar* WXUNUSED(parent),
                                    wxWindow* WXUNUSED(child) )
{
    // we don't do anything here
}

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

void wxToolBarTool::Init()
{
    m_item =
    m_pixmap = (GtkWidget *)NULL;
}

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxString& text,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         wxItemKind kind,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool(this, id, text, bitmap1, bitmap2, kind,
                             clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    return new wxToolBarTool(this, control);
}

//-----------------------------------------------------------------------------
// wxToolBar construction
//-----------------------------------------------------------------------------

void wxToolBar::Init()
{
    m_fg =
    m_bg = (GdkColor *)NULL;
    m_toolbar = (GtkToolbar *)NULL;
    m_blockEvent = FALSE;
}

wxToolBar::~wxToolBar()
{
    delete m_fg;
    delete m_bg;
}

bool wxToolBar::Create( wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name )
{
    m_needParent = TRUE;
    m_insertCallback = (wxInsertChildFunction)wxInsertChildInToolBar;

    if ( !PreCreation( parent, pos, size ) ||
         !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxToolBar creation failed") );

        return FALSE;
    }

#ifdef __WXGTK20__
    m_toolbar = GTK_TOOLBAR( gtk_toolbar_new() );
    GtkSetStyle();
    
    // Doesn't work this way.
    // GtkToolbarSpaceStyle space_style = GTK_TOOLBAR_SPACE_EMPTY;
    // gtk_widget_style_set (GTK_WIDGET (m_toolbar), "space_style", &space_style, NULL);
#else
    GtkOrientation orient;
    GtkToolbarStyle gtkStyle;
    GetGtkStyle(style, &orient, &gtkStyle);

    m_toolbar = GTK_TOOLBAR( gtk_toolbar_new(orient, gtkStyle) );
#endif

    SetToolSeparation(7);

    if (style & wxTB_DOCKABLE)
    {
        m_widget = gtk_handle_box_new();
        gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_toolbar) );
        gtk_widget_show( GTK_WIDGET(m_toolbar) );

        if (style & wxTB_FLAT)
            gtk_handle_box_set_shadow_type( GTK_HANDLE_BOX(m_widget), GTK_SHADOW_NONE );
    }
    else
    {
        m_widget = gtk_event_box_new();
        gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_toolbar) );
        ConnectWidget( m_widget );
        gtk_widget_show(GTK_WIDGET(m_toolbar));
    }

    gtk_toolbar_set_tooltips( GTK_TOOLBAR(m_toolbar), TRUE );

    // FIXME: there is no such function for toolbars in 2.0
#ifndef __WXGTK20__
    if (style & wxTB_FLAT)
        gtk_toolbar_set_button_relief( GTK_TOOLBAR(m_toolbar), GTK_RELIEF_NONE );
#endif


    m_fg = new GdkColor;
    m_fg->red = 0;
    m_fg->green = 0;
    m_fg->blue = 0;
    wxColour fg(0,0,0);
    fg.CalcPixel( gtk_widget_get_colormap( GTK_WIDGET(m_toolbar) ) );
    m_fg->pixel = fg.GetPixel();

    m_bg = new GdkColor;
    m_bg->red = 65535;
    m_bg->green = 65535;
    m_bg->blue = 49980;
    wxColour bg(255,255,196);
    bg.CalcPixel( gtk_widget_get_colormap( GTK_WIDGET(m_toolbar) ) );
    m_bg->pixel = bg.GetPixel();

    gtk_tooltips_force_window( GTK_TOOLBAR(m_toolbar)->tooltips );

    GtkStyle *g_style =
        gtk_style_copy(
                gtk_widget_get_style(
                    GTK_TOOLBAR(m_toolbar)->tooltips->tip_window ) );

    g_style->bg[GTK_STATE_NORMAL] = *m_bg;

    gtk_widget_set_style( GTK_TOOLBAR(m_toolbar)->tooltips->tip_window, g_style );

    m_parent->DoAddChild( this );

    PostCreation();

    Show( TRUE );

    return TRUE;
}

void wxToolBar::GtkSetStyle()
{
    GtkOrientation orient;
    GtkToolbarStyle style;
    GetGtkStyle(GetWindowStyle(), &orient, &style);

    gtk_toolbar_set_orientation(m_toolbar, orient);
    gtk_toolbar_set_style(m_toolbar, style);
}

void wxToolBar::SetWindowStyleFlag( long style )
{
    wxToolBarBase::SetWindowStyleFlag(style);

    if ( m_toolbar )
        GtkSetStyle();
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *toolBase)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

#ifndef __WXGTK20__
    // if we have inserted a space before all the tools we must change the GTK
    // index by 1
    size_t posGtk = m_xMargin > 1 ? pos + 1 : pos;
#else
    size_t posGtk = pos;
#endif

    if ( tool->IsButton() )
    {
        if ( !HasFlag(wxTB_NOICONS) )
        {
            wxBitmap bitmap = tool->GetNormalBitmap();

            wxCHECK_MSG( bitmap.Ok(), FALSE,
                         wxT("invalid bitmap for wxToolBar icon") );

            wxCHECK_MSG( bitmap.GetBitmap() == NULL, FALSE,
                         wxT("wxToolBar doesn't support GdkBitmap") );

            wxCHECK_MSG( bitmap.GetPixmap() != NULL, FALSE,
                         wxT("wxToolBar::Add needs a wxBitmap") );

            GtkWidget *tool_pixmap = (GtkWidget *)NULL;

            GdkPixmap *pixmap = bitmap.GetPixmap();

            GdkBitmap *mask = (GdkBitmap *)NULL;
            if ( bitmap.GetMask() )
              mask = bitmap.GetMask()->GetBitmap();

            tool_pixmap = gtk_pixmap_new( pixmap, mask );
            gtk_pixmap_set_build_insensitive( GTK_PIXMAP(tool_pixmap), TRUE );

            gtk_misc_set_alignment( GTK_MISC(tool_pixmap), 0.5, 0.5 );

            tool->m_pixmap = tool_pixmap;
        }
    }

    switch ( tool->GetStyle() )
    {
        case wxTOOL_STYLE_BUTTON:
            // for a radio button we need the widget which starts the radio
            // group it belongs to, i.e. the first radio button immediately
            // preceding this one
            {
                GtkWidget *widget = NULL;

                if ( tool->IsRadio() )
                {
                    wxToolBarToolsList::Node *node = pos ? m_tools.Item(pos - 1)
                                                         : NULL;
                    while ( node )
                    {
                        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
                        if ( !tool->IsRadio() )
                            break;

                        widget = tool->m_item;

                        node = node->GetPrevious();
                    }

                    if ( !widget )
                    {
                        // this is the first button in the radio button group,
                        // it will be toggled automatically by GTK so bring the
                        // internal flag in sync
                        tool->Toggle(TRUE);
                    }
                }

                tool->m_item = gtk_toolbar_insert_element
                               (
                                  m_toolbar,
                                  tool->GetGtkChildType(),
                                  widget,
                                  tool->GetLabel().empty()
                                    ? NULL
                                    : (const char*) wxGTK_CONV( tool->GetLabel() ),
                                  tool->GetShortHelp().empty()
                                    ? NULL
                                    : (const char*) wxGTK_CONV( tool->GetShortHelp() ),
                                  "", // tooltip_private_text (?)
                                  tool->m_pixmap,
                                  (GtkSignalFunc)gtk_toolbar_callback,
                                  (gpointer)tool,
                                  posGtk
                               );

                if ( !tool->m_item )
                {
                    wxFAIL_MSG( _T("gtk_toolbar_insert_element() failed") );

                    return FALSE;
                }
                
                gtk_signal_connect( GTK_OBJECT(tool->m_item),
                                    "enter_notify_event",
                                    GTK_SIGNAL_FUNC(gtk_toolbar_tool_callback),
                                    (gpointer)tool );
                gtk_signal_connect( GTK_OBJECT(tool->m_item),
                                    "leave_notify_event",
                                    GTK_SIGNAL_FUNC(gtk_toolbar_tool_callback),
                                    (gpointer)tool );
            }
            break;

        case wxTOOL_STYLE_SEPARATOR:
            gtk_toolbar_insert_space( m_toolbar, posGtk );

            // skip the rest
            return TRUE;

        case wxTOOL_STYLE_CONTROL:
            gtk_toolbar_insert_widget(
                                       m_toolbar,
                                       tool->GetControl()->m_widget,
                                       (const char *) NULL,
                                       (const char *) NULL,
                                       posGtk
                                      );
            break;
    }

    GtkRequisition req;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request )
        (m_widget, &req );
    m_width = req.width + m_xMargin;
    m_height = req.height + 2*m_yMargin;

    return TRUE;
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *toolBase)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    switch ( tool->GetStyle() )
    {
        case wxTOOL_STYLE_CONTROL:
            tool->GetControl()->Destroy();
            break;

        case wxTOOL_STYLE_BUTTON:
            gtk_widget_destroy( tool->m_item );
            break;

        //case wxTOOL_STYLE_SEPARATOR: -- nothing to do
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxToolBar tools state
// ----------------------------------------------------------------------------

void wxToolBar::DoEnableTool(wxToolBarToolBase *toolBase, bool enable)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    if (tool->m_item)
    {
        gtk_widget_set_sensitive( tool->m_item, enable );
    }
}

void wxToolBar::DoToggleTool( wxToolBarToolBase *toolBase, bool toggle )
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    GtkWidget *item = tool->m_item;
    if ( item && GTK_IS_TOGGLE_BUTTON(item) )
    {
        wxBitmap bitmap = tool->GetBitmap();
        if ( bitmap.Ok() )
        {
            GtkPixmap *pixmap = GTK_PIXMAP( tool->m_pixmap );

            GdkBitmap *mask = bitmap.GetMask() ? bitmap.GetMask()->GetBitmap()
                                               : (GdkBitmap *)NULL;

            gtk_pixmap_set( pixmap, bitmap.GetPixmap(), mask );
        }

        m_blockEvent = TRUE;

        gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(item), toggle );

        m_blockEvent = FALSE;
    }
}

void wxToolBar::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // VZ: absolutely no idea about how to do it
    wxFAIL_MSG( _T("not implemented") );
}

// ----------------------------------------------------------------------------
// wxToolBar geometry
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord WXUNUSED(x),
                                                  wxCoord WXUNUSED(y)) const
{
    // VZ: GTK+ doesn't seem to have such thing
    wxFAIL_MSG( _T("wxToolBar::FindToolForPosition() not implemented") );

    return (wxToolBarToolBase *)NULL;
}

void wxToolBar::SetMargins( int x, int y )
{
    wxCHECK_RET( GetToolsCount() == 0,
                 wxT("wxToolBar::SetMargins must be called before adding tools.") );

#ifndef __WXGTK20__
    if (x > 1)
        gtk_toolbar_append_space( m_toolbar );  // oh well
#endif

    m_xMargin = x;
    m_yMargin = y;
}

void wxToolBar::SetToolSeparation( int separation )
{
    // FIXME: this function disappeared
#ifndef __WXGTK20__
    gtk_toolbar_set_space_size( m_toolbar, separation );
#endif

    m_toolSeparation = separation;
}

void wxToolBar::SetToolShortHelp( int id, const wxString& helpString )
{
    wxToolBarTool *tool = (wxToolBarTool *)FindById(id);

    if ( tool )
    {
        (void)tool->SetShortHelp(helpString);
        gtk_tooltips_set_tip(m_toolbar->tooltips, tool->m_item,
                             wxGTK_CONV( helpString ), "");
    }
}

// ----------------------------------------------------------------------------
// wxToolBar idle handling
// ----------------------------------------------------------------------------

void wxToolBar::OnInternalIdle()
{
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (cursor.Ok())
    {
        /* I now set the cursor the anew in every OnInternalIdle call
           as setting the cursor in a parent window also effects the
           windows above so that checking for the current cursor is
           not possible. */

        if (HasFlag(wxTB_DOCKABLE) && (m_widget->window))
        {
            /* if the toolbar is dockable, then m_widget stands for the
               GtkHandleBox widget, which uses its own window so that we
               can set the cursor for it. if the toolbar is not dockable,
               m_widget comes from m_toolbar which uses its parent's
               window ("windowless windows") and thus we cannot set the
               cursor. */
            gdk_window_set_cursor( m_widget->window, cursor.GetCursor() );
        }

        wxToolBarToolsList::Node *node = m_tools.GetFirst();
        while ( node )
        {
            wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
            node = node->GetNext();

            GtkWidget *item = tool->m_item;
            if ( item )
            {
                GdkWindow *window = item->window;

                if ( window )
                {
                    gdk_window_set_cursor( window, cursor.GetCursor() );
                }
            }
        }
    }

    UpdateWindowUI();
}

#endif // wxUSE_TOOLBAR_NATIVE
