/////////////////////////////////////////////////////////////////////////////
// Name:        wave.cpp
// Purpose:     wxWave class implementation: optional
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wave.h"
#endif

#include "wx/object.h"
#include "wx/string.h"
#include "wx/motif/wave.h"

wxWave::wxWave()
  : m_waveData(NULL), m_waveLength(0), m_isResource(FALSE)
{
}

wxWave::wxWave(const wxString& sFileName, bool isResource)
  : m_waveData(NULL), m_waveLength(0), m_isResource(FALSE)
{
    Create(sFileName, isResource);
}


wxWave::~wxWave()
{
    Free();
}

bool wxWave::Create(const wxString& fileName, bool isResource)
{
    Free();

    // TODO

    return FALSE;
}

bool wxWave::Play(bool async, bool looped) const
{
    if (!IsOk())
        return FALSE;

    // TODO
    return FALSE;
}

bool wxWave::Free()
{
    // TODO
    return FALSE;
}


