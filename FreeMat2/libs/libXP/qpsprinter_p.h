/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the painting module of the Qt Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QPSPRINTER_P_H
#define QPSPRINTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_PRINTER

#include <qglobal.h>
#ifdef QT3_SUPPORT
#include <qpointer.h>
#include <qprintdialog.h>
#endif

class QPrintEngine;

class QPSPrinterPrivate
{
public:
    QPSPrinterPrivate()
        : printEngine(0)
#ifdef QT3_SUPPORT
        , printDialog(0)
#endif
    {
    }

    QPrintEngine *printEngine;

#ifdef QT3_SUPPORT
    QPointer<QPrintDialog> printDialog;
#endif
};

#endif // QT_NO_PRINTER

#endif // QPSPRINTER_P_H
