/****************************************************************************
** Meta object code from reading C++ file 'skbasewidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/SKBaseWidget/skbasewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'skbasewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SKBaseWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      26,   13,   13,   13, 0x0a,
      40,   13,   13,   13, 0x0a,
      54,   13,   13,   13, 0x0a,
      71,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SKBaseWidget[] = {
    "SKBaseWidget\0\0SlotClose()\0SlotShowMax()\0"
    "SlotShowMin()\0SlotShowNormal()\0"
    "SlotFullScreen()\0"
};

void SKBaseWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SKBaseWidget *_t = static_cast<SKBaseWidget *>(_o);
        switch (_id) {
        case 0: _t->SlotClose(); break;
        case 1: _t->SlotShowMax(); break;
        case 2: _t->SlotShowMin(); break;
        case 3: _t->SlotShowNormal(); break;
        case 4: _t->SlotFullScreen(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SKBaseWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SKBaseWidget::staticMetaObject = {
    { &AbsFrameLessAutoSize::staticMetaObject, qt_meta_stringdata_SKBaseWidget,
      qt_meta_data_SKBaseWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SKBaseWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SKBaseWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SKBaseWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SKBaseWidget))
        return static_cast<void*>(const_cast< SKBaseWidget*>(this));
    return AbsFrameLessAutoSize::qt_metacast(_clname);
}

int SKBaseWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbsFrameLessAutoSize::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
