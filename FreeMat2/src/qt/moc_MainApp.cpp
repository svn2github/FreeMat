/****************************************************************************
** MainApp meta object code from reading C++ file 'MainApp.hpp'
**
** Created: Fri Jun 10 09:38:31 2005
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "MainApp.hpp"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MainApp::className() const
{
    return "MainApp";
}

QMetaObject *MainApp::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MainApp( "MainApp", &MainApp::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MainApp::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainApp", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MainApp::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainApp", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MainApp::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod slot_0 = {"Run", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "Run()", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"MainApp", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MainApp.setMetaObject( metaObj );
    return metaObj;
}

void* MainApp::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MainApp" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool MainApp::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: static_QUType_int.set(_o,Run()); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MainApp::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MainApp::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool MainApp::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
