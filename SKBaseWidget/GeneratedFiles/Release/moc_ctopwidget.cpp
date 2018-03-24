/****************************************************************************
** Meta object code from reading C++ file 'ctopwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ctopwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ctopwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CTopWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      21,   11,   11,   11, 0x05,
      30,   11,   11,   11, 0x05,
      41,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CTopWidget[] = {
    "CTopWidget\0\0SigMin()\0SigMax()\0SigClose()\0"
    "SigFullScreen()\0SlotMainMenu()\0"
};

void CTopWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CTopWidget *_t = static_cast<CTopWidget *>(_o);
        switch (_id) {
        case 0: _t->SigMin(); break;
        case 1: _t->SigMax(); break;
        case 2: _t->SigClose(); break;
        case 3: _t->SigFullScreen(); break;
        case 4: _t->SlotMainMenu(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CTopWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CTopWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CTopWidget,
      qt_meta_data_CTopWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CTopWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CTopWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CTopWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CTopWidget))
        return static_cast<void*>(const_cast< CTopWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CTopWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CTopWidget::SigMin()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CTopWidget::SigMax()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CTopWidget::SigClose()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CTopWidget::SigFullScreen()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
