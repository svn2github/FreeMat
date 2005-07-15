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

#ifndef QT_NO_PRINTER

#include "qpsprinter_p.h"
#include "qpsprinter.h"
#include "qprintengine.h"
#include "qlist.h"
#include <qprintdialog.h>
#include <qpagesetupdialog.h>

#include <qprintengine_ps_p.h>

#ifdef QT3_SUPPORT
#  include "qprintdialog.h"
#endif // QT3_SUPPORT

#define ABORT_IF_ACTIVE(location) \
    if (d->printEngine->printerState() == QPSPrinter::Active) { \
        qWarning("%s, cannot be changed while printer is active", location); \
        return; \
    }

QPSPrinter::QPSPrinter(PrinterMode mode)  : QPaintDevice() {
  printEngine = new QPSPrintEngine(mode);
}

QPSPrinter::~QPSPrinter() {
  delete printEngine;
}

int QPSPrinter::devType() const
{
  return QInternal::Printer;
}

QString QPSPrinter::outputFileName() const
{
  return printEngine->property(QPrintEngine::PPK_OutputFileName).toString();
}

void QPSPrinter::setOutputFileName(const QString &fileName)
{
  printEngine->setProperty(QPrintEngine::PPK_OutputFileName, fileName);
}

void QPSPrinter::setColorMode(ColorMode newColorMode)
{
  printEngine->setProperty(QPrintEngine::PPK_ColorMode, newColorMode);
}

QPSPrinter::ColorMode QPSPrinter::colorMode() const
{
  return QPSPrinter::ColorMode(printEngine->property(QPrintEngine::PPK_ColorMode).toInt());
}


QPaintEngine *QPSPrinter::paintEngine() const

{
  // Being a bit safe, since we have multiple inheritance...
  return static_cast<QPSPrintEngine *>(printEngine);
}

QList<int> QPSPrinter::supportedResolutions() const
{
  QList<QVariant> varlist
    = printEngine->property(QPrintEngine::PPK_SupportedResolutions).toList();
  QList<int> intlist;
  for (int i=0; i<varlist.size(); ++i)
    intlist << varlist.at(i).toInt();
  return intlist;
}


#endif // QT_NO_PRINTER

QPrinter::PrinterState QPSPrinter::printerState() const 
{
  return (QPrinter::PrinterState) printEngine->printerState();
 }

int QPSPrinter::metric(PaintDeviceMetric id) const
{
  return printEngine->metric(id);
}


