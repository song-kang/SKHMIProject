/********************************************************************************
** Form generated from reading UI file 'cmainwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CMAINWIDGET_H
#define UI_CMAINWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CMainWidget
{
public:
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QPushButton *btnTest;

    void setupUi(QWidget *CMainWidget)
    {
        if (CMainWidget->objectName().isEmpty())
            CMainWidget->setObjectName(QString::fromUtf8("CMainWidget"));
        CMainWidget->resize(790, 476);
        gridLayout = new QGridLayout(CMainWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        stackedWidget = new QStackedWidget(CMainWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);

        btnTest = new QPushButton(CMainWidget);
        btnTest->setObjectName(QString::fromUtf8("btnTest"));

        gridLayout->addWidget(btnTest, 1, 0, 1, 1);


        retranslateUi(CMainWidget);

        stackedWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(CMainWidget);
    } // setupUi

    void retranslateUi(QWidget *CMainWidget)
    {
        CMainWidget->setWindowTitle(QApplication::translate("CMainWidget", "CMainWidget", 0, QApplication::UnicodeUTF8));
        btnTest->setText(QApplication::translate("CMainWidget", "\346\265\213\350\257\2251", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CMainWidget: public Ui_CMainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CMAINWIDGET_H
