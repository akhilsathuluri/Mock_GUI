/****************************************************************************
** Meta object code from reading C++ file 'torch_utils.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SampleQtQuickControls/torch_utils.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'torch_utils.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_torchUtils_t {
    QByteArrayData data[5];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_torchUtils_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_torchUtils_t qt_meta_stringdata_torchUtils = {
    {
QT_MOC_LITERAL(0, 0, 10), // "torchUtils"
QT_MOC_LITERAL(1, 11, 10), // "load_model"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 14), // "predict_output"
QT_MOC_LITERAL(4, 38, 4) // "text"

    },
    "torchUtils\0load_model\0\0predict_output\0"
    "text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_torchUtils[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x02 /* Public */,
       3,    1,   25,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::QString,
    QMetaType::QString, QMetaType::QString,    4,

       0        // eod
};

void torchUtils::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        torchUtils *_t = static_cast<torchUtils *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { QString _r = _t->load_model();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 1: { QString _r = _t->predict_output((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject torchUtils::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_torchUtils.data,
      qt_meta_data_torchUtils,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *torchUtils::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *torchUtils::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_torchUtils.stringdata0))
        return static_cast<void*>(const_cast< torchUtils*>(this));
    return QObject::qt_metacast(_clname);
}

int torchUtils::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
