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

#ifndef QPAINTER_P_H
#define QPAINTER_P_H

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

#include "qbrush.h"
#include "qfont.h"
#include "qpen.h"
#include "qregion.h"
#include "qvector.h"
#include "qmatrix.h"
#include "qpainter.h"
#include "qpainterpath.h"
#include "qpaintengine.h"

class QPaintEngine;

class QPainterClipInfo
{
public:
    enum ClipType { RegionClip, PathClip };

    QPainterClipInfo(const QPainterPath &p, Qt::ClipOperation op, const QMatrix &m) :
        clipType(PathClip), matrix(m), operation(op), path(p) { }

    QPainterClipInfo(const QRegion &r, Qt::ClipOperation op, const QMatrix &m) :
        clipType(RegionClip), matrix(m), operation(op), region(r) { }

    ClipType clipType;
    QMatrix matrix;
    Qt::ClipOperation operation;
    QPainterPath path;
    QRegion region;
};


class QPainterState : public QPaintEngineState
{
public:
    QPainterState();
    QPainterState(const QPainterState *s);
    ~QPainterState();
    void init(QPainter *p);

    QPointF bgOrigin;
    QFont font;
    QFont deviceFont;
    QPen pen;
    QBrush brush;
    QBrush bgBrush;             // background brush
    QRegion clipRegion;
    QPainterPath clipPath;
    Qt::ClipOperation clipOperation;
    QPainter::RenderHints renderHints;
    QList<QPainterClipInfo> clipInfo;
#ifndef QT_NO_TRANSFORMATIONS
    QMatrix worldMatrix;       // World transformation matrix, not window and viewport
    QMatrix matrix;            // Complete transformation matrix, including win and view.
    int txop;
#else
    int xlatex;
    int xlatey;
#endif
    int wx, wy, ww, wh;         // window rectangle
    int vx, vy, vw, vh;         // viewport rectangle

    uint WxF:1;                 // World transformation
    uint VxF:1;                 // View transformation

    Qt::BGMode bgMode;
    QPainter *painter;
    Qt::LayoutDirection layoutDirection;
    QPainter::CompositionMode composition_mode;
    uint emulationSpecifier;
    uint changeFlags;
};


class QPainterPrivate
{
    Q_DECLARE_PUBLIC(QPainter)
public:
    QPainterPrivate(QPainter *painter)
        : q_ptr(painter), txinv(0), device(0), engine(0)
    {
        states.push_back(new QPainterState());
        state = states.back();
    }

    ~QPainterPrivate()
    {
        for (int i=0; i<states.size(); ++i)
            delete states.at(i);
    }

    QPainter *q_ptr;

    QPoint redirection_offset;

    QPainterState *state;
    QVector<QPainterState*> states;

#ifndef QT_NO_TRANSFORMATIONS
    QMatrix invMatrix;
    uint txinv:1;
#endif

    enum TransformationCodes {
        TxNone = 0,
        TxTranslate = 1,
        TxScale = 2,
        TxRotShear = 3
    };

    enum DrawOperation { StrokeDraw        = 0x1,
                         FillDraw          = 0x2,
                         StrokeAndFillDraw = 0x3
    };

    void updateEmulationSpecifier(QPainterState *s);
    void updateState(QPainterState *state);

    void draw_helper(const QPainterPath &path, DrawOperation operation = StrokeAndFillDraw);

    void updateMatrix();
    void updateInvMatrix();
    void init();

    int rectSubtraction() const {
        return state->pen.style() != Qt::NoPen && state->pen.width() == 0 ? 1 : 0;
    }

    QPaintDevice *device;
    QPaintEngine *engine;
};

QString qt_generate_brush_key(const QBrush &brush);

#endif // QPAINTER_P_H