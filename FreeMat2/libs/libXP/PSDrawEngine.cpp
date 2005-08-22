#include "PSDrawEngine.hpp"
#include <qdatetime.h>
#include <qtextstream.h>

#define POINT(p) p.x() << ' ' << p.y() << ' '


static const char *const ps_header =
"/d/def load def/D{bind d}bind d/d2{dup dup}D/ED{exch d}D/D0{0 ED}D/LT\n"
"{lineto}D/MT{moveto}D/S{stroke}D/F{setfont}D/SW{setlinewidth}D/CP{closepath}\n"
"D/RL{rlineto}D/NP{newpath}D/CM{currentmatrix}D/SM{setmatrix}D/TR{translate}D\n"
"/SD{setdash}D/SC{aload pop setrgbcolor}D/CR{currentfile read pop}D/i{index}D\n"
"/bs{bitshift}D/scs{setcolorspace}D/DB{dict dup begin}D/DE{end d}D/ie{ifelse}\n"
"D/BSt 0 d/LWi 0 d/PSt 1 d/WFi false d/OMo false d/BCol[1 1 1]d/PCol[0 0 0]d\n"
"/BkCol[1 1 1]d/BDArr[0.94 0.88 0.63 0.50 0.37 0.12 0.06]d/defM matrix d\n"
"/level3{/languagelevel where{pop languagelevel 3 ge}{false}ie}D/GPS{PSt 1 ge\n"
"PSt 5 le and{{LArr PSt 1 sub 2 mul get}{LArr PSt 2 mul 1 sub get}ie}{[]}ie}D\n"
"/QS{PSt 0 ne{gsave LWi SW true GPS 0 SD S OMo PSt 1 ne and{BkCol SC false\n"
"GPS dup 0 get SD S}if grestore}if}D/sl D0/QCIgray D0/QCIcolor D0/QCIindex D0\n"
"/QCI{/colorimage where{pop false 3 colorimage}{exec/QCIcolor ED/QCIgray\n"
"QCIcolor length 3 idiv string d 0 1 QCIcolor length 3 idiv 1 sub{/QCIindex\n"
"ED/x QCIindex 3 mul d QCIgray QCIindex QCIcolor x get 0.30 mul QCIcolor x 1\n"
"add get 0.59 mul QCIcolor x 2 add get 0.11 mul add add cvi put}for QCIgray\n"
"image}ie}D/di{gsave TR 1 i 1 eq{false eq{pop true 3 1 roll 4 i 4 i false 4 i\n"
"4 i imagemask BkCol SC imagemask}{pop false 3 1 roll imagemask}ie}{dup false\n"
"ne{level3}{false}ie{/ma ED 8 eq{/dc[0 1]d/DeviceGray}{/dc[0 1 0 1 0 1]d\n"
"/DeviceRGB}ie scs/im ED/mt ED/h ED/w ED/id <</ImageType 1/Width w/Height h\n"
"/ImageMatrix mt/DataSource im/BitsPerComponent 8/Decode dc >> d/md <<\n"
"/ImageType 1/Width w/Height h/ImageMatrix mt/DataSource ma/BitsPerComponent\n"
"1/Decode[0 1]>> d <</ImageType 3/DataDict id/MaskDict md/InterleaveType 3 >>\n"
"image}{pop 8 4 1 roll 8 eq{image}{QCI}ie}ie}ie grestore}d/SetLinGrad{level3{\n"
"/c2 ED/c1 ED/y2 ED/x2 ED/y1 ED/x1 ED/LinGrad <</PatternType 2/Shading <<\n"
"/ShadingType 2/ColorSpace[/DeviceRGB]/Coords[x1 y1 x2 y2]/Extend[true true]\n"
"/Function <</FunctionType 2/Domain[0 1]/C0 c1/C1 c2/N 1 >> >> >> matrix\n"
"makepattern d}{pop}ie/BSt 15 d}D/BF{gsave BSt 1 eq{BCol SC WFi{fill}{eofill}\n"
"ie}if BSt 2 ge BSt 8 le and{BDArr BSt 2 sub get/sc ED BCol{1. exch sub sc\n"
"mul 1. exch sub}forall 3 array astore SC WFi{fill}{eofill}ie}if BSt 9 ge BSt\n"
"14 le and{WFi{clip}{eoclip}ie defM SM pathbbox 3 i 3 i TR 4 2 roll 3 2 roll\n"
"exch sub/h ED sub/w ED OMo{NP 0 0 MT 0 h RL w 0 RL 0 h neg RL CP BkCol SC\n"
"fill}if BCol SC 0.3 SW NP BSt 9 eq BSt 11 eq or{0 4 h{dup 0 exch MT w exch\n"
"LT}for}if BSt 10 eq BSt 11 eq or{0 4 w{dup 0 MT h LT}for}if BSt 12 eq BSt 14\n"
"eq or{w h gt{0 6 w h add{dup 0 MT h sub h LT}for}{0 6 w h add{dup 0 exch MT\n"
"w sub w exch LT}for}ie}if BSt 13 eq BSt 14 eq or{w h gt{0 6 w h add{dup h MT\n"
"h sub 0 LT}for}{0 6 w h add{dup w exch MT w sub 0 exch LT}for}ie}if S}if BSt\n"
"15 eq{level3{LinGrad setpattern WFi{fill}{eofill}ie}if}if BSt 24 eq{}if\n"
"grestore}D/mat matrix d/ang1 D0/ang2 D0/w D0/h D0/x D0/y D0/ARC{/ang2 ED\n"
"/ang1 ED/h ED/w ED/y ED/x ED mat CM pop x w 2 div add y h 2 div add TR 1 h w\n"
"div neg scale ang2 0 ge{0 0 w 2 div ang1 ang1 ang2 add arc}{0 0 w 2 div ang1\n"
"ang1 ang2 add arcn}ie mat SM}D/C D0/P{NP MT 0.5 0.5 rmoveto 0 -1 RL -1 0 RL\n"
"0 1 RL CP fill}D/DL{NP MT LT QS}D/R{/h ED/w ED/y ED/x ED NP x y MT 0 h RL w\n"
"0 RL 0 h neg RL CP BF QS}D/xr D0/yr D0/rx D0/ry D0/rx2 D0/ry2 D0/E{/h ED/w\n"
"ED/y ED/x ED mat CM pop x w 2 div add y h 2 div add TR 1 h w div scale NP 0\n"
"0 w 2 div 0 360 arc mat SM BF QS}D/BC{/BkCol ED}D/BR{/BCol ED/BSt ED}D/NB{0\n"
"[0 0 0]BR}D/PE{setlinejoin setlinecap/PCol ED/LWi ED/PSt ED PCol SC}D/P1{1 0\n"
"3 2 roll 0 0 PE}D/ST{defM SM concat}D/MF{true exch true exch{exch pop exch\n"
"pop dup 0 get dup findfont dup/FontName get 3 -1 roll eq{exit}if}forall exch\n"
"dup 1 get/fxscale ED 2 get/fslant ED exch/fencoding ED[fxscale 0 fslant 1 0\n"
"0]makefont fencoding false eq{}{dup maxlength dict begin{1 i/FID ne{def}{pop\n"
"pop}ifelse}forall/Encoding fencoding d currentdict end}ie definefont pop}D\n"
"/MFEmb{findfont dup length dict begin{1 i/FID ne{d}{pop pop}ifelse}forall\n"
"/Encoding ED currentdict end definefont pop}D/DF{findfont/fs 3 -1 roll d[fs\n"
"0 0 fs -1 mul 0 0]makefont d}D/ty 0 d/Y{/ty ED}D/Tl{gsave SW NP 1 i exch MT\n"
"1 i 0 RL S grestore}D/XYT{ty MT/xyshow where{pop pop xyshow}{exch pop 1 i\n"
"dup length 2 div exch stringwidth pop 3 -1 roll exch sub exch div exch 0\n"
"exch ashow}ie}D/AT{ty MT 1 i dup length 2 div exch stringwidth pop 3 -1 roll\n"
"exch sub exch div exch 0 exch ashow}D/QI{/C save d pageinit/OMo false d}D/QP\n"
"{C restore showpage}D/SPD{/setpagedevice where{<< 3 1 roll >> setpagedevice}\n"
"{pop pop}ie}D/CLS{gsave NP}D/ACR{/h ED/w ED/y ED/x ED x y MT 0 h RL w 0 RL 0\n"
"h neg RL CP}D/CLO{grestore}D\n";


void PSDrawEngine::emitHeader()
{
  pageStream << "%!PS-Adobe-3.0";
  pageStream << " EPSF-3.0\n%%BoundingBox: ";
  pageStream << " 0 0 " << m_width << ' ' << m_height << "\n";
  pageStream << "\n" << "%%Creator: FreeMat";
  pageStream << "%%Title: " + m_title;
  pageStream << "%%CreationDate: " << QDateTime::currentDateTime().toString();
  pageStream << "\n%%Pages: 1";
  pageStream << "\n%% Page: 1 1";
  pageStream << "\n%%EndComments\n";
  pageStream << "%%BeginProlog\n";
  const char prologLicense[] = "% Prolog copyright 1994-2003 Trolltech. "
    "You may copy this prolog in any way\n"
    "% that is directly related to this "
    "document. For other use of this prolog,\n"
    "% see your licensing agreement for Qt.\n";
  pageStream << prologLicense << ps_header << "\n";
  
  // we have to do this here, as scaling can affect this.
  QString lineStyles = "/LArr["                                       // Pen styles:
    " [] []"                       //   solid line
    " [10 3] [10 3]"                 //   dash line
    " [3 3] [3 3]"                  //   dot line
    " [5 3 3 3] [3 5 3 3]"      //   dash dot line
    " [5 3 3 3 3] [3 5 3 3 3 3]"         //   dash dot dot line
    "] d\n";

  
  pageStream << lineStyles;
  
  pageStream << "%%EndProlog\n";
  pageStream << "0 " << m_height << " translate \n";
  pageStream << "1 -1 scale\n";
}


PSDrawEngine::PSDrawEngine(std::string filename, int width, int height) {
  m_title = QString(filename.c_str());
  m_width = width;
  m_height = height;
  outDevice = new QFile(m_title);
#ifdef QT3
  outDevice->open(IO_WriteOnly);
#else
  outDevice->open(QIODevice::WriteOnly);
#endif
  pageStream.setDevice(outDevice);
  emitHeader();
}

PSDrawEngine::~PSDrawEngine() {
  pageStream << "%%EOF\n";
  delete outDevice;
}

void PSDrawEngine::setPen(const QPen &pen) {
}

void PSDrawEngine::setPen(const QColor &color) {
}

void PSDrawEngine::setPen(Qt::PenStyle) {
}

void PSDrawEngine::save() {
  pageStream << "gsave\n";
}

void PSDrawEngine::translate(float dx, float dy) {
  pageStream << dx << ' ' << dy << " translate\n";
}

void PSDrawEngine::restore() {
  pageStream << "grestore\n";
}

QFontMetrics PSDrawEngine::fontMetrics() {
  return QFontMetrics(m_font);
}

void PSDrawEngine::rotate(float a) {
  pageStream << a << " rotate\n";
}

void PSDrawEngine::drawLine(int x1, int y1, int x2, int y2) {
  pageStream << x2 << ' ' << y2 << ' ' << x1 << ' ' << y1 << ' ' << "DL\n";
}

void PSDrawEngine::setBrush(const QBrush &brush) {
}

void PSDrawEngine::drawRect(int x1, int y1, int w, int h) {
  pageStream << x1 << ' ' << y1 << ' ' << w << ' ' << h << ' ' << "R\n";  
}

void PSDrawEngine::fillRect(int x, int y, int w, int h, const QBrush &b) {
  // FIXME
  pageStream << x << ' ' << y << ' ' << w << ' ' << h << ' ' << "R\n";  
}

QPoint PSDrawEngine::xForm(const QPoint &) {
  // FIXME
  return QPoint(0,0);
}

void PSDrawEngine::setClipRect(int x, int y, int w, int h) {
}

void PSDrawEngine::drawPoint(int x, int y) {
  pageStream << x << ' ' << y << "P\n";
}

void PSDrawEngine::drawEllipse(int x, int y, int w, int h) {
  pageStream << x << ' ' << y << ' ' << w << ' ' << h << ' ' << "E\n";
}

void PSDrawEngine::drawPolyline(const std::vector<QPoint> &pa) {
  if (pa.empty()) return;
  pageStream << "NP\n";
  pageStream << POINT(pa[0]) << "MT\n";
  for(int i = 1; i < pa.size(); i++) 
    pageStream << POINT(pa[i]) << "LT\n";
   pageStream << "QS\n";
}

void PSDrawEngine::setFont(const QFont &f) {
  m_font = f;
  pageStream << "/Helvetica findfont\n" << f.pointSize() << " scalefont\nsetfont\n";
}

static inline const char *toHex(uchar u)
{
    static char hexVal[3];
    int i = 1;
    while (i >= 0) {
        ushort hex = (u & 0x000f);
        if (hex < 0x0a)
            hexVal[i] = '0'+hex;
        else
            hexVal[i] = 'A'+(hex-0x0a);
        u = u >> 4;
        i--;
    }
    hexVal[2] = '\0';
    return hexVal;
}

static const char *toHex(ushort u)
{
    static char hexVal[5];
    int i = 3;
    while (i >= 0) {
        ushort hex = (u & 0x000f);
        if (hex < 0x0a)
            hexVal[i] = '0'+hex;
        else
            hexVal[i] = 'A'+(hex-0x0a);
        u = u >> 4;
        i--;
    }
    hexVal[4] = '\0';
    return hexVal;
}

void PSDrawEngine::drawText(int x, int y, const QString &s) {
  pageStream << "gsave\nNP\n";
  pageStream << x << ' ' << y << " MT\n";
  pageStream << "1 -1 scale\n";
  pageStream << "(" << s << ")\n";
  pageStream << "show\ngrestore\n";
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


void PSDrawEngine::drawImage(int x, int y, const QImage &img) {
  if (img.isNull()) return;
  
  int width  = img.width();
  int height = img.height();
  QImage mask;
  
  bool gray = img.allGray();
  int splitSize = 21830 * (gray ? 3 : 1);
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
  pageStream << width << ' ' << height << "[" << 1.0 << " 0 0 " << 1.0 << " 0 0]sl "
	     << bits << (!mask.isNull() ? "mask " : "false ")
	     << x << ' ' << y << " di\n";
}
