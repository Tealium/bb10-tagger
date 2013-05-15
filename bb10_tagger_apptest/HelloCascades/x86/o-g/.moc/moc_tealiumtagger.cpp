/****************************************************************************
** Meta object code from reading C++ file 'tealiumtagger.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/tealiumtagger.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tealiumtagger.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TealiumTagger[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      27,   15,   14,   14, 0x08,
      75,   14,   14,   14, 0x08,
     100,   14,   14,   14, 0x08,
     118,   14,   14,   14, 0x08,
     128,   14,   14,   14, 0x08,
     139,   14,   14,   14, 0x08,
     155,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TealiumTagger[] = {
    "TealiumTagger\0\0loadRequest\0"
    "onLoadingChanged(bb::cascades::WebLoadRequest*)\0"
    "initializeWebViewRetry()\0sendRequestItem()\0"
    "onAwake()\0onAsleep()\0onAboutToQuit()\0"
    "connectionChange()\0"
};

void TealiumTagger::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TealiumTagger *_t = static_cast<TealiumTagger *>(_o);
        switch (_id) {
        case 0: _t->onLoadingChanged((*reinterpret_cast< bb::cascades::WebLoadRequest*(*)>(_a[1]))); break;
        case 1: _t->initializeWebViewRetry(); break;
        case 2: _t->sendRequestItem(); break;
        case 3: _t->onAwake(); break;
        case 4: _t->onAsleep(); break;
        case 5: _t->onAboutToQuit(); break;
        case 6: _t->connectionChange(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TealiumTagger::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TealiumTagger::staticMetaObject = {
    { &bb::cascades::CustomControl::staticMetaObject, qt_meta_stringdata_TealiumTagger,
      qt_meta_data_TealiumTagger, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TealiumTagger::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TealiumTagger::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TealiumTagger::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TealiumTagger))
        return static_cast<void*>(const_cast< TealiumTagger*>(this));
    typedef bb::cascades::CustomControl QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int TealiumTagger::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef bb::cascades::CustomControl QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
