/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dir.cpp
// Purpose:     wxDir implementation for Mac
// Author:      Stefan Csomor
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Stefan Csomor <csomor@advanced.ch>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dir.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/dir.h"
#include "wx/filefn.h"          // for wxPathExists()

#ifndef __DARWIN__
  #include <windows.h>
#endif

#include "wx/mac/private.h"

#include "MoreFiles.h"
#include "MoreFilesExtras.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifndef MAX_PATH
    #define MAX_PATH 260        // from VC++ headers
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define M_DIR       ((wxDirData *)m_data)

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this class stores everything we need to enumerate the files
class wxDirData
{
public:
    wxDirData(const wxString& dirname);
    ~wxDirData();

    void SetFileSpec(const wxString& filespec) { m_filespec = filespec; }
    void SetFlags(int flags) { m_flags = flags; }

    bool Read(wxString *filename); // reads the next 
    void Rewind() ;

    const wxString& GetName() const { return m_dirname; }

private:
	CInfoPBRec			m_CPB ;
	wxInt16				m_index ;
	long				m_dirId ;
	Str255				m_name ;
	Boolean				m_isDir ;

    wxString m_dirname;
    wxString m_filespec;

    int      m_flags;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDirData
// ----------------------------------------------------------------------------

wxDirData::wxDirData(const wxString& dirname)
         : m_dirname(dirname)
{
    // throw away the trailing slashes
    size_t n = m_dirname.length();
    wxCHECK_RET( n, _T("empty dir name in wxDir") );

    while ( n > 0 && wxIsPathSeparator(m_dirname[--n]) )
        ;

    m_dirname.Truncate(n + 1);

	FSSpec fsspec ;

	wxMacFilename2FSSpec( m_dirname , &fsspec ) ;
	m_CPB.hFileInfo.ioVRefNum = fsspec.vRefNum ;
	m_CPB.hFileInfo.ioNamePtr = m_name ;
	m_index = 0 ;

	OSErr err = FSpGetDirectoryID( &fsspec , &m_dirId , &m_isDir ) ;
        wxASSERT_MSG( err == noErr , "Error accessing directory") ;
}

wxDirData::~wxDirData()
{
}

void wxDirData::Rewind() 
{
	m_index = 0 ;
}

bool wxDirData::Read(wxString *filename)
{
    if ( !m_isDir )
        return FALSE ;
		
#if TARGET_CARBON
	char c_name[256] ;
#endif
    wxString result;

	short err = noErr ;
	
	while ( err == noErr )
	{
		m_index++ ;
		m_CPB.dirInfo.ioFDirIndex = m_index;
		m_CPB.dirInfo.ioDrDirID = m_dirId;	/* we need to do this every time */
		err = PBGetCatInfoSync((CInfoPBPtr)&m_CPB);
		if ( err != noErr )
			break ;

#if TARGET_CARBON
		p2cstrcpy( c_name, m_name ) ;
		strcpy( (char *)m_name, c_name);
#else
		p2cstr( m_name ) ;
#endif
#if TARGET_CARBON
                // under X thats the way the mounting points look like
                if ( ( m_CPB.dirInfo.ioDrDirID == 0 ) && ( m_flags & wxDIR_DIRS) )
                        break ;
#endif
		if ( ( m_CPB.dirInfo.ioFlAttrib & ioDirMask) != 0 && (m_flags & wxDIR_DIRS) ) //  we have a directory
			break ;
			
		if ( ( m_CPB.dirInfo.ioFlAttrib & ioDirMask) == 0 && !(m_flags & wxDIR_FILES ) ) // its a file but we don't want it
			continue ;
			 
        if ( ( m_CPB.hFileInfo.ioFlFndrInfo.fdFlags & kIsInvisible ) && !(m_flags & wxDIR_HIDDEN) ) // its hidden but we don't want it
			continue ;

		wxString file( m_name ) ;
		if ( m_filespec.IsEmpty() || m_filespec == "*.*" || m_filespec == "*" )
		{
		}
		else if ( m_filespec.Length() > 1 && m_filespec.Left(1) =="*" )
		{
			if ( file.Right( m_filespec.Length() - 1 ).Upper() != m_filespec.Mid(1).Upper() )
			{
				continue ;
			}
		}
		else if ( m_filespec.Length() > 1 && m_filespec.Right(1) == "*" )
		{
			if ( file.Left( m_filespec.Length() - 1 ).Upper() != m_filespec.Left( m_filespec.Length() - 1 ).Upper() )
			{
				continue ;
			}
		}
		else if ( file.Upper() != m_filespec.Upper() )
		{
			continue ;
		}

		break ;
	}
	if ( err != noErr )
	{
		return FALSE ;
	}

	*filename = (char*) m_name ;

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxDir helpers
// ----------------------------------------------------------------------------

/* static */
bool wxDir::Exists(const wxString& dir)
{
    return wxPathExists(dir);
}

// ----------------------------------------------------------------------------
// wxDir construction/destruction
// ----------------------------------------------------------------------------

wxDir::wxDir(const wxString& dirname)
{
    m_data = NULL;

    (void)Open(dirname);
}

bool wxDir::Open(const wxString& dirname)
{
    delete M_DIR;
    m_data = new wxDirData(dirname);

    return TRUE;
}

bool wxDir::IsOpened() const
{
    return m_data != NULL;
}

wxString wxDir::GetName() const
{
    wxString name;
    if ( m_data )
    {
	name = M_DIR->GetName();
	if ( !name.empty() && (name.Last() == _T('/')) )
	{
	    // chop off the last (back)slash
	    name.Truncate(name.length() - 1);
	}
    }

    return name;
}

wxDir::~wxDir()
{
    if (M_DIR != NULL) {
        delete M_DIR;
        m_data = NULL;
    }
}

// ----------------------------------------------------------------------------
// wxDir enumerating
// ----------------------------------------------------------------------------

bool wxDir::GetFirst(wxString *filename,
                     const wxString& filespec,
                     int flags) const
{
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );

    M_DIR->Rewind();

    M_DIR->SetFileSpec(filespec);
    M_DIR->SetFlags(flags);

    return GetNext(filename);
}

bool wxDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );

    wxCHECK_MSG( filename, FALSE, _T("bad pointer in wxDir::GetNext()") );

    return M_DIR->Read(filename);
}
