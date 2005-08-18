#include "QPSPaintEngine.h"

QPSPaintEngine::QPSPaintEngine() : 
  QPaintEngine(PrimitiveTransform | PatternTransform | PixmapTransform | PainterPaths) {
}

QPSPaintEngine::~QPSPaintEngine() {
  // Close file
}

static void ps_r7(QTextStream& stream, const char * s, int l)
{
    int i = 0;
    uchar line[80];
    int col = 0;

    while(i < l) {
        line[col++] = s[i++];
        if (i < l - 1 && col >= 76) {
            line[col++] = '\n';
            line[col++] = '\0';
            stream << (const char *)line;
            col = 0;
        }
    }
    if (col > 0) {
        while((col&3) != 0)
            line[col++] = '%'; // use a comment as padding
        line[col++] = '\n';
        line[col++] = '\0';
        stream << (const char *)line;
    }
}

static QByteArray runlengthEncode(const QByteArray &input)
{
    if (!input.length())
        return input;

    const char *data = input.constData();

    QByteArray out;
    int start = 0;
    char last = *data;

    enum State {
        Undef,
        Equal,
        Diff
    };
    State state = Undef;

    int i = 1;
    int written = 0;
    while (1) {
        bool flush = (i == input.size());
        if (!flush) {
            switch(state) {
            case Undef:
                state = (last == data[i]) ? Equal : Diff;
                break;
            case Equal:
                if (data[i] != last)
                    flush = true;
                break;
            case Diff:
                if (data[i] == last) {
                    --i;
                    flush = true;
                }
            }
        }
        if (flush || i - start == 128) {
            int size = i - start;
            if (state == Equal) {
                out.append((char)(uchar)(257-size));
                out.append(last);
                written += size;
            } else {
                out.append((char)(uchar)size-1);
                while (start < i)
                    out.append(data[start++]);
                written += size;
            }
            state = Undef;
            start = i;
            if (i == input.size())
                break;
        }
        last = data[i];
        ++i;
    };
    out.append((char)(uchar)128);
    return out;
}

static QByteArray ascii85Encode(const QByteArray &input)
{
    int isize = input.size()/4*4;
    QByteArray output;
    output.resize(input.size()*5/4+7);
    char *out = output.data();
    const uchar *in = (const uchar *)input.constData();
    for (int i = 0; i < isize; i += 4) {
        uint val = (((uint)in[i])<<24) + (((uint)in[i+1])<<16) + (((uint)in[i+2])<<8) + (uint)in[i+3];
        if (val == 0) {
            *out = 'z';
            ++out;
        } else {
            char base[5];
            base[4] = val % 85;
            val /= 85;
            base[3] = val % 85;
            val /= 85;
            base[2] = val % 85;
            val /= 85;
            base[1] = val % 85;
            val /= 85;
            base[0] = val % 85;
            *(out++) = base[0] + '!';
            *(out++) = base[1] + '!';
            *(out++) = base[2] + '!';
            *(out++) = base[3] + '!';
            *(out++) = base[4] + '!';
        }
    }
    //write the last few bytes
    int remaining = input.size() - isize;
    if (remaining) {
        uint val = 0;
        for (int i = isize; i < input.size(); ++i)
            val = (val << 8) + in[i];
        val <<= 8*(4-remaining);
        char base[5];
        base[4] = val % 85;
        val /= 85;
        base[3] = val % 85;
        val /= 85;
        base[2] = val % 85;
        val /= 85;
        base[1] = val % 85;
        val /= 85;
        base[0] = val % 85;
        for (int i = 0; i < remaining+1; ++i)
            *(out++) = base[i] + '!';
    }
    *(out++) = '~';
    *(out++) = '>';
    output.resize(out-output.data());
    return output;
}

static QByteArray compress(const QImage &img, bool gray) {
    // we can't use premultiplied here
    QImage image = img;
    if (image.format() == QImage::Format_ARGB32_Premultiplied)
        image = image.convertToFormat(QImage::Format_ARGB32);
    int width = image.width();
    int height = image.height();
    int depth = image.depth();
    int size = width*height;

    if (depth == 1)
        size = (width+7)/8*height;
    else if (!gray)
        size = size*3;

    QByteArray pixelData;
    pixelData.resize(size);
    uchar *pixel = (uchar *)pixelData.data();
    int i = 0;
    if (depth == 1) {
        QImage::Format format = image.format();
        memset(pixel, 0xff, size);
        for(int y=0; y < height; y++) {
            const uchar * s = image.scanLine(y);
            for(int x=0; x < width; x++) {
                // need to copy bit for bit...
                bool b = (format == QImage::Format_MonoLSB) ?
                         (*(s + (x >> 3)) >> (x & 7)) & 1 :
                         (*(s + (x >> 3)) << (x & 7)) & 0x80 ;
                if (b)
                    pixel[i >> 3] ^= (0x80 >> (i & 7));
                i++;
            }
            // we need to align to 8 bit here
            i = (i+7) & 0xffffff8;
        }
    } else if (depth == 8) {
        for(int y=0; y < height; y++) {
            const uchar * s = image.scanLine(y);
            for(int x=0; x < width; x++) {
                QRgb rgb = image.color(s[x]);
                if (gray) {
                    pixel[i] = (unsigned char) qGray(rgb);
                    i++;
                } else {
                    pixel[i] = (unsigned char) qRed(rgb);
                    pixel[i+1] = (unsigned char) qGreen(rgb);
                    pixel[i+2] = (unsigned char) qBlue(rgb);
                    i += 3;
                }
            }
        }
    } else {
        for(int y=0; y < height; y++) {
            QRgb * s = (QRgb*)(image.scanLine(y));
            for(int x=0; x < width; x++) {
                QRgb rgb = (*s++);
                if (gray) {
                    pixel[i] = (unsigned char) qGray(rgb);
                    i++;
                } else {
                    pixel[i] = (unsigned char) qRed(rgb);
                    pixel[i+1] = (unsigned char) qGreen(rgb);
                    pixel[i+2] = (unsigned char) qBlue(rgb);
                    i += 3;
                }
            }
        }
    }

    QByteArray runlength = runlengthEncode(pixelData);
    QByteArray outarr = ascii85Encode(runlength);
    return outarr;
}

static QByteArray color(const QColor &c)
{
    QByteArray retval;
    retval += '[';
    retval += QByteArray::number(c.red()/255.);
    retval += ' ';
    retval += QByteArray::number(c.green()/255.);
    retval += ' ';
    retval += QByteArray::number(c.blue()/255.);
    retval += ']';
    return retval;
}


static const char * psCap(Qt::PenCapStyle p)
{
    if (p == Qt::SquareCap)
        return "2 ";
    else if (p == Qt::RoundCap)
        return "1 ";
    return "0 ";
}


static const char * psJoin(Qt::PenJoinStyle p) {
    if (p == Qt::BevelJoin)
        return "2 ";
    else if (p == Qt::RoundJoin)
        return "1 ";
    return "0 ";
}



#undef POINT
#undef RECT
#undef INT_ARG

#define POINT(p) p.x() << ' ' << p.y() << ' '
#define RECT(r) r.x()  << ' ' << r.y()  << ' ' <<     \
                        r.width() << ' ' << r.height() << ' '
#define INT_ARG(x)  x << ' '

bool QPSPaintEngine::begin(QPaintDevice *pdev) {
  // Open the output stream here...
  return;
}

bool QPSPaintEngine::end() {
  // Close the output stream here...
}

void QPSPrintEnginePrivate::drawImage(qreal x, qreal y, qreal w, qreal h,
				      const QImage &img, const QImage &mask)
{
  if (!w || !h || img.isNull()) return;
  
  int width  = img.width();
  int height = img.height();
  qreal scaleX = width/w;
  qreal scaleY = height/h;
  
  bool gray = (colorMode == QPSPrinter::GrayScale) ||
    img.allGray();
  int splitSize = 21830 * (gray ? 3 : 1);
  if (width * height > splitSize) { // 65535/3, tolerance for broken printers
    int images, subheight;
    images = (width * height + splitSize - 1) / splitSize;
    subheight = (height + images-1) / images;
    while (subheight * width > splitSize) {
      images++;
      subheight = (height + images-1) / images;
    }
    int suby = 0;
    while(suby < height) {
      drawImage(x, y + suby/scaleY, w, qMin(subheight, height-suby)/scaleY,
		img.copy(0, suby, width, qMin(subheight, height-suby)),
		mask.isNull() ? mask : mask.copy(0, suby, width, qMin(subheight, height-suby)));
      suby += subheight;
    }
  } else {
    QByteArray out;
    int size = 0;
    const char *bits;
    
    if (!mask.isNull()) {
      out = ::compress(mask, true);
      size = (width+7)/8*height;
      pageStream << "/mask currentfile/ASCII85Decode filter/RunLengthDecode filter "
		 << size << " string readstring\n";
      ps_r7( pageStream, out, out.size() );
      pageStream << " pop d\n";
    }
    if (img.depth() == 1) {
      size = (width+7)/8*height;
      bits = "1 ";
    } else if (gray) {
      size = width*height;
      bits = "8 ";
    } else {
      size = width*height*3;
      bits = "24 ";
    }
    
    out = ::compress(img, gray);
    pageStream << "/sl currentfile/ASCII85Decode filter/RunLengthDecode filter "
	       << size << " string readstring\n";
    ps_r7( pageStream, out, out.size() );
    pageStream << " pop d\n";
    pageStream << width << ' ' << height << "[" << scaleX << " 0 0 " << scaleY << " 0 0]sl "
	       << bits << (!mask.isNull() ? "mask " : "false ")
	       << x << ' ' << y << " di\n";
  }
}

void QPSPrintEnginePrivate::emitHeader(bool finished)
{
    QString title = docName;
    QString creator = this->creator;
    if (creator.count() == 0)                             // default creator
        creator = QLatin1String("Qt " QT_VERSION_STR);
    outDevice = new QFile();
    static_cast<QFile *>(outDevice)->open(fd, QIODevice::WriteOnly);
    outStream.setDevice(outDevice);
    outStream << "%!PS-Adobe-1.0";
    QPSPrintEngine *q = static_cast<QPSPrintEngine *>(q_ptr);
    scale = 72. / ((qreal) q->metric(QPaintDevice::PdmDpiY));
    QRect pageRect = this->pageRect();
    QRect paperRect = this->paperRect();
    uint mtop = pageRect.top() - paperRect.top();
    uint mleft = pageRect.left() - paperRect.left();
    uint mbottom = paperRect.bottom() - pageRect.bottom();
    uint mright = paperRect.right() - pageRect.right();
    int width = pageRect.width();
    int height = pageRect.height();
    if (finished && pageCount == 1 && copies == 1 &&
         ((fullPage && qt_gen_epsf) || (outputFileName.endsWith(".eps")))
       ) {
        if (!boundingBox.isValid())
            boundingBox.setRect(0, 0, width, height);
        if (orientation == QPSPrinter::Landscape) {
            if (!fullPage)
                boundingBox.translate(-mleft, -mtop);
            outStream << " EPSF-3.0\n%%BoundingBox: "
                      << (int)(printer->height() - boundingBox.bottom())*scale << " " // llx
                      << (int)(printer->width() - boundingBox.right())*scale - 1 << " " // lly
                      << (int)(printer->height() - boundingBox.top())*scale + 1 << " " // urx
                      << (int)(printer->width() - boundingBox.left())*scale; // ury
        } else {
            if (!fullPage)
                boundingBox.translate(mleft, -mtop);
            outStream << " EPSF-3.0\n%%BoundingBox: "
                      << (int)(boundingBox.left())*scale << " "
                      << (int)(printer->height() - boundingBox.bottom())*scale - 1 << " "
                      << (int)(boundingBox.right())*scale + 1 << " "
                      << (int)(printer->height() - boundingBox.top())*scale;
        }
    } else {
        int w = width + (fullPage ? 0 : mleft + mright);
        int h = height + (fullPage ? 0 : mtop + mbottom);
        w = (int)(w*scale);
        h = (int)(h*scale);
        // set a bounding box according to the DSC
        if (orientation == QPSPrinter::Landscape)
            outStream << "\n%%BoundingBox: 0 0 " << h << " " << w;
        else
            outStream << "\n%%BoundingBox: 0 0 " << w << " " << h;
    }
    outStream << "\n" << wrapDSC("%%Creator: " + creator);
    if (title.count() == 0)
        outStream << wrapDSC("%%Title: " + title);
    outStream << "%%CreationDate: " << QDateTime::currentDateTime().toString();
    outStream << "\n%%Orientation: ";
    if (orientation == QPSPrinter::Landscape)
        outStream << "Landscape";
    else
        outStream << "Portrait";
    if (finished)
        outStream << "\n%%Pages: " << pageCount << "\n"
                  << wrapDSC("%%DocumentFonts: " + fontsUsed);
    else
        outStream << "%%Pages: (atend)"
               << "\n%%DocumentFonts: (atend)";
    outStream << "\n%%EndComments\n";

    outStream << "%%BeginProlog\n";
    const char prologLicense[] = "% Prolog copyright 1994-2003 Trolltech. "
                                 "You may copy this prolog in any way\n"
                                 "% that is directly related to this "
                                 "document. For other use of this prolog,\n"
                                 "% see your licensing agreement for Qt.\n";
    outStream << prologLicense << ps_header << "\n";

    // we have to do this here, as scaling can affect this.
    QString lineStyles = "/LArr["                                       // Pen styles:
                         " [] []"                       //   solid line
                         " [w s] [s w]"                 //   dash line
                         " [s s] [s s]"                  //   dot line
                         " [m s s s] [s m s s]"      //   dash dot line
                         " [m s s s s] [s m s s s s]"         //   dash dot dot line
                         "] d\n";
    lineStyles.replace(QLatin1String("w"), toString(10./scale));
    lineStyles.replace(QLatin1String("m"), toString(5./scale));
    lineStyles.replace(QLatin1String("s"), toString(3./scale));

    outStream << lineStyles;

    outStream << "/pageinit {\n";
    if (!fullPage) {
        if (orientation == QPSPrinter::Portrait)
            outStream << mleft*scale << " "
                   << mbottom*scale << " translate\n";
        else
            outStream << mtop*scale << " "
                   << mleft*scale << " translate\n";
    }
    if (orientation == QPSPrinter::Portrait) {
        outStream << "% " << printer->widthMM() << "*" << printer->heightMM()
               << "mm (portrait)\n0 " << height*scale
               << " translate " << scale << " -" << scale << " scale/defM matrix CM d } d\n";
    } else {
        outStream << "% " << printer->heightMM() << "*" << printer->widthMM()
               << " mm (landscape)\n 90 rotate " << scale << " -" << scale << " scale/defM matrix CM d } d\n";
    }
    outStream << "%%EndProlog\n";


    outStream << "%%BeginSetup\n";
    if (copies > 1) {
        outStream << "/#copies " << copies << " def\n";
        outStream << "/NumCopies " << copies << " SPD\n";
        outStream << "/Collate " << (collate ? "true" : "false") << " SPD\n";
    }
    fontStream.flush();
    if (fontBuffer->buffer().size()) {
        if (pageCount == 1 || finished)
            outStream << "% Fonts and encodings used\n";
        else
            outStream << "% Fonts and encodings used on pages 1-"
                   << pageCount << "\n";
        for (QHash<QString, QPSPrintEngineFont *>::Iterator it = fonts.begin(); it != fonts.end(); ++it)
            (*it)->download(outStream, true); // true means its global
        fontStream.flush();
        outStream << fontBuffer->buffer();
    }
    outStream << "%%EndSetup\n";
    outStream << buffer->buffer();

    delete buffer;
    buffer = 0;
    fontStream.flush();
    delete fontBuffer;
    fontBuffer = 0;
}

static void putRect(QTextStream &stream, const QRect &r)
{
    stream << r.x() << " "
           << r.y() << " "
           << r.width() << " "
           << r.height() << " ";
}




#define QT_PATH_ELEMENT(elm) elm.x << ' ' << elm.y << ' '

void QPSPrintEngine::drawPath(const QPainterPath &p)
{
  bool winding = (p.fillRule() == Qt::WindingFill);
  
  if (winding)
    pageStream << "/WFi true d\n";
  pageStream << "NP\n";
 
  int start = -1;
  for (int i=0; i<p.elementCount(); ++i) {
    const QPainterPath::Element &elm = p.elementAt(i);
    switch (elm.type) {
    case QPainterPath::MoveToElement:
      if (start >= 0
	  && p.elementAt(start).x == p.elementAt(i-1).x
	  && p.elementAt(start).y == p.elementAt(i-1).y)
	pageStream << "CP\n";
      pageStream << QT_PATH_ELEMENT(elm) << "MT\n";
      start = i;
      break;
    case QPainterPath::LineToElement:
      pageStream << QT_PATH_ELEMENT(elm) << "LT\n";
      break;
    case QPainterPath::CurveToElement:
      Q_ASSERT(p.elementAt(i+1).type == QPainterPath::CurveToDataElement);
      Q_ASSERT(p.elementAt(i+2).type == QPainterPath::CurveToDataElement);
      pageStream << QT_PATH_ELEMENT(elm)
		    << QT_PATH_ELEMENT(p.elementAt(i+1))
		    << QT_PATH_ELEMENT(p.elementAt(i+2))
		    << "curveto\n";
      i += 2;
      break;
    default:
      qFatal("QPSPrintEngine::drawPath(), unhandled type: %d", elm.type);
    }
  }
  if (start >= 0
      && p.elementAt(start).x == p.elementAt(p.elementCount()-1).x
      && p.elementAt(start).y == p.elementAt(p.elementCount()-1).y)
    pageStream << "CP\n";
  
  pageStream << "BF QS\n";
  
  if (winding)
    pageStream << "/WFi false d\n";
}


void QPSPrintEngine::drawRect(const QRectF &r)
{
    Q_D(QPSPrintEngine);
    d->pageStream << RECT(r) << "R\n";
}

void QPSPrintEngine::drawPoint(const QPointF &p)
{
    Q_D(QPSPrintEngine);
    d->pageStream << POINT(p) << "P\n";
}

void QPSPrintEngine::drawEllipse(const QRectF &r)
{
    Q_D(QPSPrintEngine);
    d->pageStream << RECT(r) << "E\n";
}


void QPSPaintEngine::drawLine(const QLineF &line) {
  pageStream << POINT(line.p2())
	     << POINT(line.p1()) << "DL\n";
}

void QPSPaintEngine::drawLines(const QLineF *lines, int lineCount)
{
  pageStream << "NP\n";
  for (int i = 0; i < lineCount; ++i) {
    pageStream << POINT(lines[i].p1()) << "MT "
	       << POINT(lines[i].p2()) << "LT\n";
  }
  pageStream << "QS\n";
}

void QPSPrintEngine::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
  if (mode == WindingMode)
    pageStream << "/WFi true d\n";
  pageStream << "NP\n";
  pageStream << POINT(points[0]) << "MT\n";
  for(int i = 1; i < pointCount; i++) {
    pageStream << POINT(points[i]) << "LT\n";
  }
  if (mode == PolylineMode)
    pageStream << "QS\n";
  else
    pageStream << "CP BF QS\n";
  if (mode == WindingMode)
    pageStream << "/WFi false d\n";
}

void QPSPrintEngine::drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode)
{
  QPolygonF p;
  p.reserve(pointCount);
  for (int i=0; i<pointCount; ++i)
    p << points[i];
  drawPolygon(p.data(), pointCount, mode);
}

void QPSPrintEngine::drawImage(const QRectF &r, const QImage &img, const QRectF &sr,
                               Qt::ImageConversionFlags)
{
  QImage image = img.copy(sr.toRect());
  QImage mask;
  if (image.hasAlphaChannel()) {
    // get better alpha dithering
    int xscale = image.width();
    xscale *= xscale <= 800 ? 4 : (xscale <= 1600 ? 2 : 1);
    int yscale = image.height();
    yscale *= yscale <= 800 ? 4 : (yscale <= 1600 ? 2 : 1);
    image = image.scaled(xscale, yscale);
    mask = image.createAlphaMask(Qt::OrderedAlphaDither);
  }
  drawImage(r.x(), r.y(), r.width(), r.height(), image, mask);
}

void QPSPrintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
  QImage img = pm.toImage();
  drawImage(r, img, sr, Qt::AutoColor);
}

void QPSPrintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
  const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);
  setFont(ti.fontEngine);
  Q_ASSERT(currentPSFont);
  if(currentPSFont) // better not crash in case somethig goes wrong.
    currentPSFont->drawText(pageStream, d, p, ti);
}

void QPSPrintEngine::drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &p)
{
  // ### Optimise implementation!
  qreal yPos = r.y();
  qreal yOff = p.y();
  while( yPos < r.y() + r.height() ) {
    qreal drawH = pixmap.height() - yOff;    // Cropping first row
    if ( yPos + drawH > r.y() + r.height() )        // Cropping last row
      drawH = r.y() + r.height() - yPos;
    qreal xPos = r.x();
    qreal xOff = p.x();
    while( xPos < r.x() + r.width() ) {
      qreal drawW = pixmap.width() - xOff; // Cropping first column
      if ( xPos + drawW > r.x() + r.width() )    // Cropping last column
	drawW = r.x() + r.width() - xPos;
      // ########
      painter()->drawPixmap( QPointF(xPos, yPos).toPoint(), pixmap,
			     QRectF(xOff, yOff, drawW, drawH).toRect());
      xPos += drawW;
      xOff = 0;
    }
    yPos += drawH;
    yOff = 0;
  }
}

void QPSPrintEngine::updateClipRegion(const QRegion &region, Qt::ClipOperation op)
{
    Q_D(QPSPrintEngine);
    if (d->clipOn) {
        d->pageStream << "CLO\n";              // clipping off, includes a restore
        setDirty(QFlag(AllDirty & ~DirtyClipRegion & ~DirtyClipPath));
        // the next line is needed again in case we set clipping
        updateMatrix(d->matrix);
    }
    if (op == Qt::NoClip) {
        d->clip = QRegion();
        d->clipOn = false;
        return;
    }

    switch (op) {
    case Qt::IntersectClip:
        if (!d->clip.isEmpty()) {
            d->clip  &= region & d->getClip();
            d->clip_matrix = d->matrix;
            break;
        }
        // fall through
    case Qt::ReplaceClip:
        d->clip = region;
        d->clip_matrix = d->matrix;
        break;
    case Qt::UniteClip:
        d->clip = region | d->getClip();
        d->clip_matrix = d->matrix;
        break;
    default:
        break;
    }

    QVector<QRect> rects = d->clip.rects();
    d->pageStream<< "CLS\n";           // start clipping
    for(int i = 0 ; i < rects.size() ; i++) {
        putRect(d->pageStream, rects[i]);
        d->pageStream << "ACR\n";          // add clip rect
        if (d->pageCount == 1)
            d->boundingBox = d->boundingBox.unite(rects[i]);
    }
    d->pageStream << "clip\n";            // end clipping
    d->clipOn = true;
}


void QPSPrintEngine::updatePen(const QPen &pen)
{
    Q_D(QPSPrintEngine);
    d->cpen = pen;
    // we special-case for narrow solid lines with the default
    // cap and join styles
    if (d->cpen.style() == Qt::SolidLine && d->cpen.width() == 0 &&
        d->cpen.capStyle() == Qt::FlatCap &&
        d->cpen.joinStyle() == Qt::MiterJoin)
        d->pageStream << color(d->cpen.color()) << "P1\n";
    else
        d->pageStream << (int)d->cpen.style() << ' ' << d->cpen.width()
                      << ' ' << color(d->cpen.color())
                      << psCap(d->cpen.capStyle())
                      << psJoin(d->cpen.joinStyle()) << "PE\n";
}

void QPSPrintEngine::updateBrush(const QBrush &brush, const QPointF &/*origin*/)
{
    Q_D(QPSPrintEngine);
    // ### use brush origin!
    if (brush.style() == Qt::TexturePattern) {
#if defined(CHECK_RANGE)
        qWarning("QPSPrinter: Pixmap brush not supported");
#endif
        return;
    }
    d->cbrush = brush;
    // we special-case for nobrush since this is a very common case
    if (d->cbrush.style() == Qt::NoBrush)
        d->pageStream << "NB\n";
    else if (brush.style() == Qt::LinearGradientPattern) {
        const QLinearGradient *lg = static_cast<const QLinearGradient *>(brush.gradient());
        d->pageStream << POINT(lg->start())
                      << POINT(lg->finalStop())
                      << color(lg->stops().first().second)
                      << color(lg->stops().last().second) << " SetLinGrad\n";
    } else {
        d->pageStream << (int)d->cbrush.style() << ' '
                      << color(d->cbrush.color()) << "BR\n";
    }
}

void QPSPrintEngine::updateFont(const QFont &)
{
    // no need to do anything here, as we use the font engines.
}

void QPSPrintEngine::updateBackground(Qt::BGMode bgMode, const QBrush &bgBrush)
{
    Q_D(QPSPrintEngine);
    d->bkColor = bgBrush.color();
    d->bkMode = bgMode;

    d->pageStream << color(d->bkColor) << "BC\n";

    if (d->bkMode == Qt::TransparentMode)
        d->pageStream << "/OMo false d\n";
    else
        d->pageStream << "/OMo true d\n";
}

void QPSPrintEngine::updateMatrix(const QMatrix &matrix)
{
    Q_D(QPSPrintEngine);
    d->pageStream << "["
                  << matrix.m11() << ' ' << matrix.m12() << ' '
                  << matrix.m21() << ' ' << matrix.m22() << ' '
                  << matrix.dx()  << ' ' << matrix.dy()
                  << "]ST\n";
    d->matrix = matrix;
}

QRegion QPSPrintEnginePrivate::getClip()
{
    return clip * clip_matrix * matrix.inverted();
}


void QPSPaintEngine::updateState(const QPaintEngineState &state) {
  QPaintEngine::DirtyFlags flags = state.state();
  if (flags & DirtyTransform) updateMatrix(state.matrix());
  if (flags & DirtyClipPath) {
    updateClipRegion(QRegion(state.clipPath().toFillPolygon().toPolygon(),
			     state.clipPath().fillRule()),
		     state.clipOperation());
  }
  if (flags & DirtyClipRegion) updateClipRegion(state.clipRegion(), state.clipOperation());
  flags = state.state(); // updateClipRegion modifies the state
  if (flags & DirtyPen) updatePen(state.pen());
  if (flags & DirtyBrush) updateBrush(state.brush(), state.brushOrigin());
  if (flags & DirtyBackground) updateBackground(state.backgroundMode(), state.backgroundBrush());
  if (flags & DirtyFont) updateFont(state.font());
}
