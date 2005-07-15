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

#ifndef QPSPRINTER_H
#define QPSPRINTER_H

#include "QtGui/qpaintdevice.h"
#include "QtCore/qstring.h"
#include "qprinter.h"

#ifndef QT_NO_PRINTER

#if defined(B0)
#undef B0 // Terminal hang-up.  We assume that you do not want that.
#endif

class QPSPrinterPrivate;
class QPrintEngine;

class QPSPrinter : public QPaintDevice
{
public:
  QPrintEngine *printEngine;

    enum PrinterMode { ScreenResolution, PrinterResolution, HighResolution };

    QPSPrinter(PrinterMode mode = ScreenResolution);
   ~QPSPrinter();

    int devType() const;

    enum Orientation { Portrait, Landscape };

    enum PageSize    { A4, B5, Letter, Legal, Executive,
		       A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1,
		       B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E,
		       DLE, Folio, Ledger, Tabloid, Custom, NPageSize = Custom };

    enum PageOrder   { FirstPageFirst,
		       LastPageFirst };

    enum ColorMode   { GrayScale,
		       Color };

    enum PaperSource { OnlyOne,
		       Lower,
		       Middle,
		       Manual,
		       Envelope,
                       EnvelopeManual,
		       Auto,
		       Tractor,
		       SmallFormat,
                       LargeFormat,
		       LargeCapacity,
		       Cassette,
		       FormSource };

    enum PrinterState { Idle,
                        Active,
                        Aborted,
                        Error };

    void setOutputFileName(const QString &);
    QString outputFileName()const;

    void setColorMode(ColorMode);

    ColorMode colorMode() const;

    QList<int> supportedResolutions() const;

    QRect paperRect() const;
    QRect pageRect() const;

    bool newPage();
    bool abort();

    QPrinter::PrinterState printerState() const;

    QPaintEngine *paintEngine() const;


protected:
    int         metric(PaintDeviceMetric) const;

private:
    Q_DISABLE_COPY(QPSPrinter)
};


#endif // QT_NO_PRINTER

#endif // QPSPRINTER_H
