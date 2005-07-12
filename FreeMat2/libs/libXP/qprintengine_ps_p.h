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

#ifndef QPRINTENGINE_PS_P_H
#define QPRINTENGINE_PS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qpsprinter.cpp and qprinter_x11.cpp.
// This header file may change from version to version without notice,
// or even be removed.
//
// We mean it.
//

#include "qpaintengine.h"
#include "qprintengine.h"

#ifndef QT_NO_PRINTER

class QPSPrinter;
class QPSPrintEnginePrivate;

class Q_GUI_EXPORT QPSPrintEngine : public QPaintEngine, public QPrintEngine
{
    Q_DECLARE_PRIVATE(QPSPrintEngine)
public:
    // QPrinter uses these
    explicit QPSPrintEngine(QPSPrinter::PrinterMode m);
    ~QPSPrintEngine();


    virtual bool begin(QPaintDevice *pdev);
    virtual bool end();

    void updateState(const QPaintEngineState &state);

    void updatePen(const QPen &pen);
    void updateBrush(const QBrush &brush, const QPointF &pt);
    void updateFont(const QFont &font);
    void updateBackground(Qt::BGMode bgmode, const QBrush &bgBrush);
    void updateMatrix(const QMatrix &matrix);
    void updateClipRegion(const QRegion &region, Qt::ClipOperation op);

    virtual void drawLine(const QLineF &line);
    virtual void drawLines(const QLineF *lines, int lineCount);
    virtual void drawRect(const QRectF &r);
    virtual void drawPoint(const QPointF &p);
    virtual void drawEllipse(const QRectF &r);
    virtual void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode);
    virtual void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);

    virtual void drawImage(const QRectF &r, const QImage &img, const QRectF &sr, Qt::ImageConversionFlags);
    virtual void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);
    virtual void drawTextItem(const QPointF &p, const QTextItem &textItem);
    virtual void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s);
    virtual void drawPath(const QPainterPath &);

    virtual QPaintEngine::Type type() const { return QPaintEngine::PostScript; }

    // Printer stuff...
    void setProperty(PrintEnginePropertyKey key, const QVariant &value);
    QVariant property(PrintEnginePropertyKey key) const;

    virtual bool newPage();
    virtual bool abort();

    virtual int metric(QPaintDevice::PaintDeviceMetric metricType) const;

    virtual QPrinter::PrinterState printerState() const;

    virtual Qt::HANDLE handle() const { return 0; };

private:
    Q_DISABLE_COPY(QPSPrintEngine)
};

#endif // QT_NO_PRINTER

#endif // QPRINTENGINE_PS_P_H
