#include "qpaintengine.h"

class QPSPaintEngine : public QPaintEngine {
 public:
    virtual bool begin(QPaintDevice *pdev) ;
    virtual bool end();
    virtual void updateState(const QPaintEngineState &state) ;
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
};
