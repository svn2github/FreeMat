/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "menu.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#include "wx/os2/private.h"

// other standard headers
#include <string.h>

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

extern wxMenu*                      wxCurrentPopupMenu;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

//
// The (popup) menu title has this special id
//
static const int                    idMenuTitle = -2;

//
// The unique ID for Menus
//
#ifdef __VISAGECPP__
USHORT                              wxMenu::m_nextMenuId = 0;
#else
static USHORT                       wxMenu::m_nextMenuId = 0;
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)

// ----------------------------------------------------------------------------
// static function for translating menu labels
// ----------------------------------------------------------------------------

static wxString TextToLabel(
  const wxString&                   rsTitle
)
{
    wxString                        sTitle = "";
    const wxChar*                   zPc;

    if (rsTitle.IsEmpty())
        return sTitle;
    for (zPc = rsTitle.c_str(); *zPc != wxT('\0'); zPc++ )
    {
        if (*zPc == wxT('&') )
        {
            if (*(zPc + 1) == wxT('&'))
            {
                zPc++;
                sTitle << wxT('&');
            }
            else
                sTitle << wxT('~');
        }
        else
        {
            if ( *zPc == wxT('~') )
            {
                //
                // Tildes must be doubled to prevent them from being
                // interpreted as accelerator character prefix by PM ???
                //
                sTitle << *zPc;
            }
            sTitle << *zPc;
        }
    }
    return sTitle;
} // end of TextToLabel

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxMenu construction, adding and removing menu items
// ---------------------------------------------------------------------------

//
// Construct a menu with optional title (then use append)
//
void wxMenu::Init()
{
    m_bDoBreak = FALSE;
    m_nStartRadioGroup = -1;

    //
    // Create the menu (to be used as a submenu or a popup)
    //
    if ((m_hMenu =  ::WinCreateWindow( HWND_DESKTOP
                                      ,WC_MENU
                                      ,"Menu"
                                      ,0L
                                      ,0L
                                      ,0L
                                      ,0L
                                      ,0L
                                      ,NULLHANDLE
                                      ,HWND_TOP
                                      ,0L
                                      ,NULL
                                      ,NULL
                                     )) == 0)
    {
        wxLogLastError("WinLoadMenu");
    }
    m_vMenuData.iPosition   = 0;
    m_vMenuData.afStyle     = MIS_SUBMENU | MIS_TEXT;
    m_vMenuData.afAttribute = (USHORT)0;
    m_vMenuData.id          = m_nextMenuId++;
    m_vMenuData.hwndSubMenu = m_hMenu;
    m_vMenuData.hItem       = NULLHANDLE;

    //
    // If we have a title, insert it in the beginning of the menu
    //
    if (!m_title.IsEmpty())
    {
        Append( idMenuTitle
               ,m_title
               ,wxEmptyString
               ,wxITEM_NORMAL
              );
        AppendSeparator();
    }
} // end of wxMenu::Init

//
// The wxWindow destructor will take care of deleting the submenus.
//
wxMenu::~wxMenu()
{
    //
    // We should free PM resources only if PM doesn't do it for us
    // which happens if we're attached to a menubar or a submenu of another
    // menu
    if (!IsAttached() && !GetParent())
    {
        if (!::WinDestroyWindow((HWND)GetHmenu()) )
        {
            wxLogLastError("WinDestroyWindow");
        }
    }

#if wxUSE_ACCEL
    //
    // Delete accels
    //
    WX_CLEAR_ARRAY(m_vAccels);
#endif // wxUSE_ACCEL
} // end of wxMenu::~wxMenu

void wxMenu::Break()
{
    // this will take effect during the next call to Append()
    m_bDoBreak = TRUE;
} // end of wxMenu::Break

void wxMenu::Attach(
  wxMenuBarBase*                    pMenubar
)
{
    wxMenuBase::Attach(pMenubar);
    EndRadioGroup();
} // end of wxMenu::Break;

#if wxUSE_ACCEL

int wxMenu::FindAccel(
  int                               nId
) const
{
    size_t                          n;
    size_t                          nCount = m_vAccels.GetCount();

    for (n = 0; n < nCount; n++)
        if (m_vAccels[n]->m_command == nId)
            return n;
    return wxNOT_FOUND;
} // end of wxMenu::FindAccel

void wxMenu::UpdateAccel(
  wxMenuItem*                       pItem
)
{
    if (pItem->IsSubMenu())
    {
        wxMenu*                     pSubmenu = pItem->GetSubMenu();
        wxMenuItemList::Node*       pNode = pSubmenu->GetMenuItems().GetFirst();

        while (pNode)
        {
            UpdateAccel(pNode->GetData());
            pNode = pNode->GetNext();
        }
    }
    else if (!pItem->IsSeparator())
    {
        //
        // Find the (new) accel for this item
        //
        wxAcceleratorEntry*         pAccel = wxGetAccelFromString(pItem->GetText());

        if (pAccel)
            pAccel->m_command = pItem->GetId();

        //
        // Find the old one
        //
        size_t                      n = FindAccel(pItem->GetId());

        if (n == wxNOT_FOUND)
        {
            //
            // No old, add new if any
            //
            if (pAccel)
                m_vAccels.Add(pAccel);
            else
                return;
        }
        else
        {
            //
            // Replace old with new or just remove the old one if no new
            //
            delete m_vAccels[n];
            if (pAccel)
                m_vAccels[n] = pAccel;
            else
                m_vAccels.RemoveAt(n);
        }

        if (IsAttached())
        {
            m_menuBar->RebuildAccelTable();
        }
    }
} // wxMenu::UpdateAccel

#endif // wxUSE_ACCEL

//
// Append a new item or submenu to the menu
//
bool wxMenu::DoInsertOrAppend(
  wxMenuItem*                       pItem
, size_t                            nPos
)
{
    wxMenu*                         pSubmenu = pItem->GetSubMenu();
    MENUITEM&                       rItem = (pSubmenu != NULL)?pSubmenu->m_vMenuData:
                                            pItem->m_vMenuData;

    ERRORID                         vError;
    wxString                        sError;
    char                            zMsg[128];

#if wxUSE_ACCEL
    UpdateAccel(pItem);
#endif // wxUSE_ACCEL

    //
    // If "Break" has just been called, insert a menu break before this item
    // (and don't forget to reset the flag)
    //
    if (m_bDoBreak)
    {
        rItem.afStyle |= MIS_BREAK;
        m_bDoBreak = FALSE;
    }

    if (pItem->IsSeparator())
    {
        rItem.afStyle |= MIS_SEPARATOR;
    }

    //
    // Id is the numeric id for normal menu items and HMENU for submenus as
    // required by ::MM_INSERTITEM message API
    //

    if (pSubmenu != NULL)
    {
        wxASSERT_MSG(pSubmenu->GetHMenu(), wxT("invalid submenu"));
        pSubmenu->SetParent(this);

        rItem.iPosition = 0; // submenus have a 0 position
        rItem.id        = (USHORT)pSubmenu->GetHMenu();
        rItem.afStyle  |= MIS_SUBMENU | MIS_TEXT;
    }
    else
    {
        rItem.id = pItem->GetId();
    }

    BYTE*                           pData;

#if wxUSE_OWNER_DRAWN
    if (pItem->IsOwnerDrawn())
    {
        //
        // Want to get {Measure|Draw}Item messages?
        // item draws itself, passing pointer to data doesn't work in OS/2
        // Will eventually need to set the image handle somewhere into vItem.hItem
        //
        rItem.afStyle             |= MIS_OWNERDRAW;
        pData                      = (BYTE*)NULL;
        rItem.hItem                = (HBITMAP)pItem->GetBitmap().GetHBITMAP();
        pItem->m_vMenuData.afStyle = rItem.afStyle;
        pItem->m_vMenuData.hItem   = rItem.hItem;
    }
    else
#endif
    {
        //
        // Menu is just a normal string (passed in data parameter)
        //
        rItem.afStyle |= MIS_TEXT;
        pData = (char*)pItem->GetText().c_str();
    }

    if (nPos == (size_t)-1)
    {
        rItem.iPosition = MIT_END;
    }
    else
    {
        rItem.iPosition = nPos;
    }

    APIRET                          rc;

    rc = (APIRET)::WinSendMsg( GetHmenu()
                              ,MM_INSERTITEM
                              ,(MPARAM)&rItem
                              ,(MPARAM)pData
                             );
#if wxUSE_OWNER_DRAWN
    if (pItem->IsOwnerDrawn())
    {
        BOOL                       rc;
        MENUITEM                   vMenuItem;

        ::WinSendMsg( GetHmenu()
                     ,MM_QUERYITEM
                     ,MPFROM2SHORT( (USHORT)pItem->GetId()
                                   ,(USHORT)(FALSE)
                                  )
                     ,&vMenuItem
                    );
    }
#endif
    if (rc == MIT_MEMERROR || rc == MIT_ERROR)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error inserting or appending a menuitem. Error: %s\n", sError.c_str());
        wxLogLastError("Insert or AppendMenu");
        return FALSE;
    }
    else
    {
        //
        // If we're already attached to the menubar, we must update it
        //
        if (IsAttached() && m_menuBar->IsAttached())
        {
            m_menuBar->Refresh();
        }
        return TRUE;
    }
    return FALSE;
} // end of wxMenu::DoInsertOrAppend

void wxMenu::EndRadioGroup()
{
    //
    // We're not inside a radio group any longer
    //
    m_nStartRadioGroup = -1;
} // end of wxMenu::EndRadioGroup

bool wxMenu::DoAppend(
  wxMenuItem*                       pItem
)
{
    wxCHECK_MSG( pItem, FALSE, _T("NULL item in wxMenu::DoAppend") );

    bool                            bCheck = FALSE;

    if (pItem->GetKind() == wxITEM_RADIO)
    {
        int                         nCount = GetMenuItemCount();

        if (m_nStartRadioGroup == -1)
        {
            //
            // Start a new radio group
            //
            m_nStartRadioGroup = nCount;

            //
            // For now it has just one element
            //
            pItem->SetAsRadioGroupStart();
            pItem->SetRadioGroupEnd(m_nStartRadioGroup);

            //
            // Ensure that we have a checked item in the radio group
            //
            bCheck = TRUE;
        }
        else // extend the current radio group
        {
            //
            // We need to update its end item
            //
            pItem->SetRadioGroupStart(m_nStartRadioGroup);

            wxMenuItemList::Node*   pNode = GetMenuItems().Item(m_nStartRadioGroup);

            if (pNode)
            {
                pNode->GetData()->SetRadioGroupEnd(nCount);
            }
            else
            {
                wxFAIL_MSG( _T("where is the radio group start item?") );
            }
        }
    }
    else // not a radio item
    {
        EndRadioGroup();
    }

    if (!wxMenuBase::DoAppend(pItem) || !DoInsertOrAppend(pItem))
    {
        return FALSE;
    }
    if (bCheck)
    {
        //
        // Check the item initially
        //
        pItem->Check(TRUE);
    }
    return TRUE;
} // end of wxMenu::DoAppend

bool wxMenu::DoInsert(
  size_t                            nPos
, wxMenuItem*                       pItem
)
{
    return ( wxMenuBase::DoInsert( nPos
                                  ,pItem) &&
             DoInsertOrAppend( pItem
                              ,nPos
                             )
           );
} // end of wxMenu::DoInsert

wxMenuItem* wxMenu::DoRemove(
  wxMenuItem*                       pItem
)
{
    //
    // We need to find the items position in the child list
    //
    size_t                          nPos;
    wxMenuItemList::Node*           pNode = GetMenuItems().GetFirst();

    for (nPos = 0; pNode; nPos++)
    {
        if (pNode->GetData() == pItem)
            break;
        pNode = pNode->GetNext();
    }

    //
    // DoRemove() (unlike Remove) can only be called for existing item!
    //
    wxCHECK_MSG(pNode, NULL, wxT("bug in wxMenu::Remove logic"));

#if wxUSE_ACCEL
    //
    // Remove the corresponding accel from the accel table
    //
    int                             n = FindAccel(pItem->GetId());

    if (n != wxNOT_FOUND)
    {
        delete m_vAccels[n];
        m_vAccels.RemoveAt(n);
    }

#endif // wxUSE_ACCEL
    //
    // Remove the item from the menu
    //
    ::WinSendMsg( GetHmenu()
                 ,MM_REMOVEITEM
                 ,MPFROM2SHORT(pItem->GetId(), TRUE)
                 ,(MPARAM)0
                );
    if (IsAttached() && m_menuBar->IsAttached())
    {
        //
        // Otherwise, the chane won't be visible
        //
        m_menuBar->Refresh();
    }

    //
    // And from internal data structures
    //
    return wxMenuBase::DoRemove(pItem);
} // end of wxMenu::DoRemove

// ---------------------------------------------------------------------------
// accelerator helpers
// ---------------------------------------------------------------------------

#if wxUSE_ACCEL

//
// Create the wxAcceleratorEntries for our accels and put them into provided
// array - return the number of accels we have
//
size_t wxMenu::CopyAccels(
  wxAcceleratorEntry*               pAccels
) const
{
    size_t                          nCount = GetAccelCount();

    for (size_t n = 0; n < nCount; n++)
    {
        *pAccels++ = *m_vAccels[n];
    }
    return nCount;
} // end of wxMenu::CopyAccels

#endif // wxUSE_ACCEL

// ---------------------------------------------------------------------------
// set wxMenu title
// ---------------------------------------------------------------------------

void wxMenu::SetTitle(
  const wxString&                   rLabel
)
{
    bool                            bHasNoTitle = m_title.IsEmpty();
    HWND                            hMenu = GetHmenu();

    m_title = rLabel;
    if (bHasNoTitle)
    {
        if (!rLabel.IsEmpty())
        {
            if (!::WinSetWindowText(hMenu, rLabel.c_str()))
            {
                wxLogLastError("SetMenuTitle");
            }
        }
    }
    else
    {
        if (rLabel.IsEmpty() )
        {
            ::WinSendMsg( GetHmenu()
                         ,MM_REMOVEITEM
                         ,MPFROM2SHORT(hMenu, TRUE)
                         ,(MPARAM)0
                        );
        }
        else
        {
            //
            // Modify the title
            //
            if (!::WinSetWindowText(hMenu, rLabel.c_str()))
            {
                wxLogLastError("SetMenuTitle");
            }
        }
    }
} // end of wxMenu::SetTitle

// ---------------------------------------------------------------------------
// event processing
// ---------------------------------------------------------------------------

bool wxMenu::OS2Command(
  WXUINT                            WXUNUSED(uParam)
, WXWORD                            vId
)
{
    //
    // Ignore commands from the menu title
    //

    if (vId != (WXWORD)idMenuTitle)
    {
        SendEvent( vId
                  ,(int)::WinSendMsg( GetHmenu()
                                     ,MM_QUERYITEMATTR
                                     ,(MPARAM)vId
                                     ,(MPARAM)MIA_CHECKED
                                    )
                 );
    }
    return TRUE;
} // end of wxMenu::OS2Command

// ---------------------------------------------------------------------------
// other
// ---------------------------------------------------------------------------

wxWindow* wxMenu::GetWindow() const
{
    if (m_invokingWindow != NULL)
        return m_invokingWindow;
    else if ( m_menuBar != NULL)
        return m_menuBar->GetFrame();

    return NULL;
} // end of wxMenu::GetWindow

// recursive search for item by id
wxMenuItem* wxMenu::FindItem(
  int                               nItemId
, ULONG                             hItem
, wxMenu**                          ppItemMenu
) const
{
    if ( ppItemMenu )
        *ppItemMenu = NULL;

    wxMenuItem*                     pItem = NULL;

    for ( wxMenuItemList::Node *node = m_items.GetFirst();
          node && !pItem;
          node = node->GetNext() )
    {
        pItem = node->GetData();

        if ( pItem->GetId() == nItemId && pItem->m_vMenuData.hItem == hItem)
        {
            if ( ppItemMenu )
                *ppItemMenu = (wxMenu *)this;
        }
        else if ( pItem->IsSubMenu() )
        {
            pItem = pItem->GetSubMenu()->FindItem( nItemId
                                                  ,hItem
                                                  ,ppItemMenu
                                                 );
            if (pItem)
                break;
        }
        else
        {
            // don't exit the loop
            pItem = NULL;
        }
    }
    return pItem;
} // end of wxMenu::FindItem

// ---------------------------------------------------------------------------
// Menu Bar
// ---------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_eventHandler = this;
    m_menuBarFrame = NULL;
    m_hMenu = 0;
} // end of wxMenuBar::Init

wxMenuBar::wxMenuBar()
{
    Init();
} // end of wxMenuBar::wxMenuBar

wxMenuBar::wxMenuBar(
 long                               WXUNUSED(lStyle)
)
{
    Init();
} // end of wxMenuBar::wxMenuBar

wxMenuBar::wxMenuBar(
  int                               nCount
, wxMenu*                           vMenus[]
, const wxString                    sTitles[]
)
{
    Init();

    m_titles.Alloc(nCount);
    for ( int i = 0; i < nCount; i++ )
    {
        m_menus.Append(vMenus[i]);
        m_titles.Add(sTitles[i]);
        vMenus[i]->Attach(this);
    }
} // end of wxMenuBar::wxMenuBar

wxMenuBar::~wxMenuBar()
{
    //
    // We should free PM's resources only if PM doesn't do it for us
    // which happens if we're attached to a frame
    //
    if (m_hMenu && !IsAttached())
    {
        ::WinDestroyWindow((HMENU)m_hMenu);
        m_hMenu = (WXHMENU)NULL;
    }
} // end of wxMenuBar::~wxMenuBar

// ---------------------------------------------------------------------------
// wxMenuBar helpers
// ---------------------------------------------------------------------------

void wxMenuBar::Refresh()
{
    wxCHECK_RET( IsAttached(), wxT("can't refresh unatteched menubar") );

    WinSendMsg(GetWinHwnd(m_menuBarFrame), WM_UPDATEFRAME, (MPARAM)FCF_MENU, (MPARAM)0);
} // end of wxMenuBar::Refresh

WXHMENU wxMenuBar::Create()
{
    MENUITEM                        vItem;
    HWND                            hFrame;

    if (m_hMenu != 0 )
        return m_hMenu;

    wxCHECK_MSG(!m_hMenu, TRUE, wxT("menubar already created"));

    //
    // Menubars should be associated with a frame otherwise they are popups
    //
    if (m_menuBarFrame != NULL)
        hFrame = GetWinHwnd(m_menuBarFrame);
    else
        hFrame = HWND_DESKTOP;
    //
    // Create an empty menu and then fill it with insertions
    //
    if ((m_hMenu =  ::WinCreateWindow( hFrame
                                      ,WC_MENU
                                      ,(PSZ)NULL
                                      ,MS_ACTIONBAR | WS_SYNCPAINT | WS_VISIBLE
                                      ,0L
                                      ,0L
                                      ,0L
                                      ,0L
                                      ,hFrame
                                      ,HWND_TOP
                                      ,FID_MENU
                                      ,NULL
                                      ,NULL
                                     )) == 0)
    {
        wxLogLastError("WinLoadMenu");
    }
    else
    {
        size_t                      nCount = GetMenuCount();

        for (size_t i = 0; i < nCount; i++)
        {
            APIRET                  rc;
            ERRORID                 vError;
            wxString                sError;
            HWND                    hSubMenu;

            //
            // Set the parent and owner of the submenues to be the menubar, not the desktop
            //
            hSubMenu = m_menus[i]->m_vMenuData.hwndSubMenu;
            if (!::WinSetParent(m_menus[i]->m_vMenuData.hwndSubMenu, m_hMenu, FALSE))
            {
                vError = ::WinGetLastError(vHabmain);
                sError = wxPMErrorToStr(vError);
                wxLogError("Error setting parent for submenu. Error: %s\n", sError.c_str());
                return NULLHANDLE;
            }

            if (!::WinSetOwner(m_menus[i]->m_vMenuData.hwndSubMenu, m_hMenu))
            {
                vError = ::WinGetLastError(vHabmain);
                sError = wxPMErrorToStr(vError);
                wxLogError("Error setting parent for submenu. Error: %s\n", sError.c_str());
                return NULLHANDLE;
            }

            m_menus[i]->m_vMenuData.iPosition = i;

            rc = (APIRET)::WinSendMsg(m_hMenu, MM_INSERTITEM, (MPARAM)&m_menus[i]->m_vMenuData, (MPARAM)m_titles[i].c_str());
            if (rc == MIT_MEMERROR || rc == MIT_ERROR)
            {
                vError = ::WinGetLastError(vHabmain);
                sError = wxPMErrorToStr(vError);
                wxLogError("Error inserting or appending a menuitem. Error: %s\n", sError.c_str());
                return NULLHANDLE;
            }
        }
    }
    return m_hMenu;
} // end of wxMenuBar::Create

// ---------------------------------------------------------------------------
// wxMenuBar functions to work with the top level submenus
// ---------------------------------------------------------------------------

//
// NB: we don't support owner drawn top level items for now, if we do these
//     functions would have to be changed to use wxMenuItem as well
//
void wxMenuBar::EnableTop(
  size_t                            nPos
, bool                              bEnable
)
{
    wxCHECK_RET(IsAttached(), wxT("doesn't work with unattached menubars"));
    USHORT                          uFlag = 0;
    SHORT                           nId;

    if(!bEnable)
       uFlag = MIA_DISABLED;

    nId = SHORT1FROMMR(::WinSendMsg((HWND)m_hMenu, MM_ITEMIDFROMPOSITION, MPFROMSHORT(nPos), (MPARAM)0));
    if (nId == MIT_ERROR)
    {
        wxLogLastError("LogLastError");
        return;
    }
    ::WinSendMsg((HWND)m_hMenu, MM_SETITEMATTR, MPFROM2SHORT(nId, TRUE), MPFROM2SHORT(MIA_DISABLED, uFlag));
    Refresh();
} // end of wxMenuBar::EnableTop

void wxMenuBar::SetLabelTop(
  size_t                            nPos
, const wxString&                   rLabel
)
{
    SHORT                           nId;
    MENUITEM                        vItem;

    wxCHECK_RET(nPos < GetMenuCount(), wxT("invalid menu index"));
    m_titles[nPos] = rLabel;

    if (!IsAttached())
    {
        return;
    }

    nId = SHORT1FROMMR(::WinSendMsg((HWND)m_hMenu, MM_ITEMIDFROMPOSITION, MPFROMSHORT(nPos), (MPARAM)0));
    if (nId == MIT_ERROR)
    {
        wxLogLastError("LogLastError");
        return;
    }
    if(!::WinSendMsg( (HWND)m_hMenu
                     ,MM_QUERYITEM
                     ,MPFROM2SHORT(nId, TRUE)
                     ,MPARAM(&vItem)
                    ))
    {
        wxLogLastError("QueryItem");
    }
    nId = vItem.id;

    if (::WinSendMsg(GetHmenu(), MM_SETITEMTEXT, MPFROMSHORT(nId), (MPARAM)rLabel.c_str()));
    {
        wxLogLastError("ModifyMenu");
    }
    Refresh();
} // end of wxMenuBar::SetLabelTop

wxString wxMenuBar::GetLabelTop(
  size_t                            nPos
) const
{
    wxCHECK_MSG( nPos < GetMenuCount(), wxEmptyString,
                 wxT("invalid menu index in wxMenuBar::GetLabelTop") );
    return m_titles[nPos];
} // end of wxMenuBar::GetLabelTop

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

wxMenu* wxMenuBar::Replace(
  size_t                             nPos
, wxMenu*                            pMenu
, const wxString&                    rTitle
)
{
    SHORT                            nId;
    wxString                         sTitle = TextToLabel(rTitle);
    wxMenu*                          pMenuOld = wxMenuBarBase::Replace( nPos
                                                                       ,pMenu
                                                                       ,sTitle
                                                                      );


    nId = SHORT1FROMMR(::WinSendMsg((HWND)m_hMenu, MM_ITEMIDFROMPOSITION, MPFROMSHORT(nPos), (MPARAM)0));
    if (nId == MIT_ERROR)
    {
        wxLogLastError("LogLastError");
        return NULL;
    }
    if (!pMenuOld)
        return NULL;
    m_titles[nPos] = sTitle;
    if (IsAttached())
    {
        ::WinSendMsg((HWND)m_hMenu, MM_REMOVEITEM, MPFROM2SHORT(nId, TRUE), (MPARAM)0);
        ::WinSendMsg((HWND)m_hMenu, MM_INSERTITEM, (MPARAM)&pMenu->m_vMenuData, (MPARAM)sTitle.c_str());

#if wxUSE_ACCEL
        if (pMenuOld->HasAccels() || pMenu->HasAccels())
        {
            //
            // Need to rebuild accell table
            //
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL
        Refresh();
    }
    return pMenuOld;
} // end of wxMenuBar::Replace

bool wxMenuBar::Insert(
  size_t                            nPos
, wxMenu*                           pMenu
, const wxString&                   rTitle
)
{
    wxString                        sTitle = TextToLabel(rTitle);

    if (!wxMenuBarBase::Insert( nPos
                               ,pMenu
                               ,sTitle
                              ))
        return FALSE;

    m_titles.Insert( sTitle
                    ,nPos
                   );

    if (IsAttached())
    {
        pMenu->m_vMenuData.iPosition = nPos;
        ::WinSendMsg( (HWND)m_hMenu
                     ,MM_INSERTITEM
                     ,(MPARAM)&pMenu->m_vMenuData
                     ,(MPARAM)sTitle.c_str()
                    );
#if wxUSE_ACCEL
        if (pMenu->HasAccels())
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL
        Refresh();
    }
    return TRUE;
} // end of wxMenuBar::Insert

bool wxMenuBar::Append(
  wxMenu*                           pMenu
, const wxString&                   rsTitle
)
{
    WXHMENU                         hSubmenu = pMenu ? pMenu->GetHMenu() : 0;

    wxCHECK_MSG(hSubmenu, FALSE, wxT("can't append invalid menu to menubar"));

    wxString                        sTitle = TextToLabel(rsTitle);

    if (!wxMenuBarBase::Append(pMenu, sTitle))
        return FALSE;

    m_titles.Add(sTitle);

    if ( IsAttached() )
    {
        pMenu->m_vMenuData.iPosition = MIT_END;
        ::WinSendMsg((HWND)m_hMenu, MM_INSERTITEM, (MPARAM)&pMenu->m_vMenuData, (MPARAM)sTitle.c_str());
#if wxUSE_ACCEL
        if (pMenu->HasAccels())
        {
            //
            // Need to rebuild accell table
            //
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL
        Refresh();
    }
    return TRUE;
} // end of wxMenuBar::Append

wxMenu* wxMenuBar::Remove(
  size_t                            nPos
)
{
    wxMenu*                         pMenu = wxMenuBarBase::Remove(nPos);
    SHORT                           nId;

    if (!pMenu)
        return NULL;

    nId = SHORT1FROMMR(::WinSendMsg( (HWND)GetHmenu()
                                    ,MM_ITEMIDFROMPOSITION
                                    ,MPFROMSHORT(nPos)
                                    ,(MPARAM)0)
                                   );
    if (nId == MIT_ERROR)
    {
        wxLogLastError("LogLastError");
        return NULL;
    }
    if (IsAttached())
    {
        ::WinSendMsg( (HWND)GetHmenu()
                     ,MM_REMOVEITEM
                     ,MPFROM2SHORT(nId, TRUE)
                     ,(MPARAM)0
                    );

#if wxUSE_ACCEL
        if (pMenu->HasAccels())
        {
            //
            // Need to rebuild accell table
            //
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL
        Refresh();
    }
    m_titles.Remove(nPos);
    return pMenu;
} // end of wxMenuBar::Remove

#if wxUSE_ACCEL

void wxMenuBar::RebuildAccelTable()
{
    //
    // Merge the accelerators of all menus into one accel table
    //
    size_t                          nAccelCount = 0;
    size_t                          i;
    size_t                          nCount = GetMenuCount();

    for (i = 0; i < nCount; i++)
    {
        nAccelCount += m_menus[i]->GetAccelCount();
    }

    if (nAccelCount)
    {
        wxAcceleratorEntry*         pAccelEntries = new wxAcceleratorEntry[nAccelCount];

        nAccelCount = 0;
        for (i = 0; i < nCount; i++)
        {
            nAccelCount += m_menus[i]->CopyAccels(&pAccelEntries[nAccelCount]);
        }
        m_vAccelTable = wxAcceleratorTable( nAccelCount
                                           ,pAccelEntries
                                          );
        delete [] pAccelEntries;
    }
} // end of wxMenuBar::RebuildAccelTable

#endif // wxUSE_ACCEL

void wxMenuBar::Attach(
  wxFrame*                          pFrame
)
{
    wxMenuBarBase::Attach(pFrame);

#if wxUSE_ACCEL
    RebuildAccelTable();
    //
    // Ensure the accelerator table is set to the frame (not the client!)
    //
    if (!::WinSetAccelTable( vHabmain
                            ,m_vAccelTable.GetHACCEL()
                            ,(HWND)pFrame->GetFrame()
                           ))
        wxLogLastError("WinSetAccelTable");
#endif // wxUSE_ACCEL
} // end of wxMenuBar::Attach

void wxMenuBar::Detach()
{
    ::WinDestroyWindow((HWND)m_hMenu);
    m_hMenu = (WXHMENU)NULL;
    m_menuBarFrame = NULL;
} // end of wxMenuBar::Detach

// ---------------------------------------------------------------------------
// wxMenuBar searching for menu items
// ---------------------------------------------------------------------------

//
// Find the itemString in menuString, and return the item id or wxNOT_FOUND
//
int wxMenuBar::FindMenuItem(
  const wxString&                   rMenuString
, const wxString&                   rItemString
) const
{
    wxString                        sMenuLabel = wxStripMenuCodes(rMenuString);
    size_t                          nCount = GetMenuCount();

    for (size_t i = 0; i < nCount; i++)
    {
        wxString                    sTitle = wxStripMenuCodes(m_titles[i]);

        if (rMenuString == sTitle)
            return m_menus[i]->FindItem(rItemString);
    }
    return wxNOT_FOUND;
} // end of wxMenuBar::FindMenuItem

wxMenuItem* wxMenuBar::FindItem(
  int                               nId
, wxMenu**                          ppItemMenu
) const
{
    if (ppItemMenu)
        *ppItemMenu = NULL;

    wxMenuItem*                     pItem = NULL;
    size_t                          nCount = GetMenuCount();

    for (size_t i = 0; !pItem && (i < nCount); i++)
    {
        pItem = m_menus[i]->FindItem( nId
                                     ,ppItemMenu
                                    );
    }
    return pItem;
} // end of wxMenuBar::FindItem

wxMenuItem* wxMenuBar::FindItem(
  int                               nId
, ULONG                             hItem
, wxMenu**                          ppItemMenu
) const
{
    if (ppItemMenu)
        *ppItemMenu = NULL;

    wxMenuItem*                     pItem = NULL;
    size_t                          nCount = GetMenuCount();

    for (size_t i = 0; !pItem && (i < nCount); i++)
    {
        pItem = m_menus[i]->FindItem( nId
                                     ,hItem
                                     ,ppItemMenu
                                    );
    }
    return pItem;
} // end of wxMenuBar::FindItem

