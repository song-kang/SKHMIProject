/********************************************************************************
** Form generated from reading UI file 'view_plugin_demo1.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_PLUGIN_DEMO1_H
#define UI_VIEW_PLUGIN_DEMO1_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include "cbaseview.h"

QT_BEGIN_NAMESPACE

class Ui_view_plugin_demo1
{
public:
    QGridLayout *gridLayout;
    QLabel *label;

    void setupUi(CBaseView *view_plugin_demo1)
    {
        if (view_plugin_demo1->objectName().isEmpty())
            view_plugin_demo1->setObjectName(QString::fromUtf8("view_plugin_demo1"));
        view_plugin_demo1->resize(400, 300);
        gridLayout = new QGridLayout(view_plugin_demo1);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(view_plugin_demo1);
        label->setObjectName(QString::fromUtf8("label"));
        label->setStyleSheet(QString::fromUtf8("font: 75 36pt \"\345\256\213\344\275\223\";"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);


        retranslateUi(view_plugin_demo1);

        QMetaObject::connectSlotsByName(view_plugin_demo1);
    } // setupUi

    void retranslateUi(CBaseView *view_plugin_demo1)
    {
        view_plugin_demo1->setWindowTitle(QApplication::translate("view_plugin_demo1", "view_plugin_demo1", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("view_plugin_demo1", "\346\265\213\350\257\225\346\217\222\344\273\2661", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_plugin_demo1: public Ui_view_plugin_demo1 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_PLUGIN_DEMO1_H
