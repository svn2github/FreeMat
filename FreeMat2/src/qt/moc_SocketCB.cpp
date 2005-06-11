/****************************************************************************
** SocketCB meta object code from reading C++ file 'SocketCB.hpp'
**
** Created: Fri Jun 10 09:38:32 2005
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "SocketCB.hpp"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *SocketCB::className() const
{
    return "SocketCB";
}

QMetaObject *SocketCB::metaObj = 0;
static QMetaObjectCleanUp cleanUp_SocketCB( "SocketCB", &SocketCB::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString SocketCB::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SocketCB", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString SocketCB::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SocketCB", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* SocketCB::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"activated", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "activated(int)", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"SocketCB", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_SocketCB.setMetaObject( metaObj );
    return metaObj;
}

void* SocketCB::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "SocketCB" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool SocketCB::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: activated((int)static_QUType_int.get(_o+1)); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool SocketCB::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool SocketCB::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool SocketCB::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
