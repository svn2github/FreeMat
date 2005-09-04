/****************************************************************************
** TextEdit meta object code from reading C++ file 'textedit.h'
**
** Created: Fri Sep 2 10:13:30 2005
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "textedit.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *TextEdit::className() const
{
    return "TextEdit";
}

QMetaObject *TextEdit::metaObj = 0;
static QMetaObjectCleanUp cleanUp_TextEdit( "TextEdit", &TextEdit::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString TextEdit::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TextEdit", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString TextEdit::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TextEdit", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* TextEdit::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"fileNew", 0, 0 };
    static const QUMethod slot_1 = {"fileOpen", 0, 0 };
    static const QUMethod slot_2 = {"fileSave", 0, 0 };
    static const QUMethod slot_3 = {"fileSaveAs", 0, 0 };
    static const QUMethod slot_4 = {"filePrint", 0, 0 };
    static const QUMethod slot_5 = {"fileClose", 0, 0 };
    static const QUMethod slot_6 = {"fileExit", 0, 0 };
    static const QUMethod slot_7 = {"editUndo", 0, 0 };
    static const QUMethod slot_8 = {"editRedo", 0, 0 };
    static const QUMethod slot_9 = {"editCut", 0, 0 };
    static const QUMethod slot_10 = {"editCopy", 0, 0 };
    static const QUMethod slot_11 = {"editPaste", 0, 0 };
    static const QUMethod slot_12 = {"textBold", 0, 0 };
    static const QUMethod slot_13 = {"textUnderline", 0, 0 };
    static const QUMethod slot_14 = {"textItalic", 0, 0 };
    static const QUParameter param_slot_15[] = {
	{ "f", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_15 = {"textFamily", 1, param_slot_15 };
    static const QUParameter param_slot_16[] = {
	{ "p", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_16 = {"textSize", 1, param_slot_16 };
    static const QUMethod slot_17 = {"textColor", 0, 0 };
    static const QUParameter param_slot_18[] = {
	{ "a", &static_QUType_ptr, "QAction", QUParameter::In }
    };
    static const QUMethod slot_18 = {"textAlign", 1, param_slot_18 };
    static const QUParameter param_slot_19[] = {
	{ "f", &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_19 = {"fontChanged", 1, param_slot_19 };
    static const QUParameter param_slot_20[] = {
	{ "c", &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_20 = {"colorChanged", 1, param_slot_20 };
    static const QUParameter param_slot_21[] = {
	{ "a", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_21 = {"alignmentChanged", 1, param_slot_21 };
    static const QUParameter param_slot_22[] = {
	{ 0, &static_QUType_ptr, "QWidget", QUParameter::In }
    };
    static const QUMethod slot_22 = {"editorChanged", 1, param_slot_22 };
    static const QMetaData slot_tbl[] = {
	{ "fileNew()", &slot_0, QMetaData::Private },
	{ "fileOpen()", &slot_1, QMetaData::Private },
	{ "fileSave()", &slot_2, QMetaData::Private },
	{ "fileSaveAs()", &slot_3, QMetaData::Private },
	{ "filePrint()", &slot_4, QMetaData::Private },
	{ "fileClose()", &slot_5, QMetaData::Private },
	{ "fileExit()", &slot_6, QMetaData::Private },
	{ "editUndo()", &slot_7, QMetaData::Private },
	{ "editRedo()", &slot_8, QMetaData::Private },
	{ "editCut()", &slot_9, QMetaData::Private },
	{ "editCopy()", &slot_10, QMetaData::Private },
	{ "editPaste()", &slot_11, QMetaData::Private },
	{ "textBold()", &slot_12, QMetaData::Private },
	{ "textUnderline()", &slot_13, QMetaData::Private },
	{ "textItalic()", &slot_14, QMetaData::Private },
	{ "textFamily(const QString&)", &slot_15, QMetaData::Private },
	{ "textSize(const QString&)", &slot_16, QMetaData::Private },
	{ "textColor()", &slot_17, QMetaData::Private },
	{ "textAlign(QAction*)", &slot_18, QMetaData::Private },
	{ "fontChanged(const QFont&)", &slot_19, QMetaData::Private },
	{ "colorChanged(const QColor&)", &slot_20, QMetaData::Private },
	{ "alignmentChanged(int)", &slot_21, QMetaData::Private },
	{ "editorChanged(QWidget*)", &slot_22, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"TextEdit", parentObject,
	slot_tbl, 23,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_TextEdit.setMetaObject( metaObj );
    return metaObj;
}

void* TextEdit::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "TextEdit" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool TextEdit::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: fileNew(); break;
    case 1: fileOpen(); break;
    case 2: fileSave(); break;
    case 3: fileSaveAs(); break;
    case 4: filePrint(); break;
    case 5: fileClose(); break;
    case 6: fileExit(); break;
    case 7: editUndo(); break;
    case 8: editRedo(); break;
    case 9: editCut(); break;
    case 10: editCopy(); break;
    case 11: editPaste(); break;
    case 12: textBold(); break;
    case 13: textUnderline(); break;
    case 14: textItalic(); break;
    case 15: textFamily((const QString&)static_QUType_QString.get(_o+1)); break;
    case 16: textSize((const QString&)static_QUType_QString.get(_o+1)); break;
    case 17: textColor(); break;
    case 18: textAlign((QAction*)static_QUType_ptr.get(_o+1)); break;
    case 19: fontChanged((const QFont&)*((const QFont*)static_QUType_ptr.get(_o+1))); break;
    case 20: colorChanged((const QColor&)*((const QColor*)static_QUType_ptr.get(_o+1))); break;
    case 21: alignmentChanged((int)static_QUType_int.get(_o+1)); break;
    case 22: editorChanged((QWidget*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool TextEdit::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool TextEdit::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool TextEdit::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
