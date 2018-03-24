/********************************************************************************
** Form generated from reading UI file 'cinitwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CINITWIDGET_H
#define UI_CINITWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CInitWidget
{
public:
    QLabel *label_2;
    QLabel *label;
    QLabel *label_load;
    QLabel *label_msg;
    QPushButton *btnClose;

    void setupUi(QWidget *CInitWidget)
    {
        if (CInitWidget->objectName().isEmpty())
            CInitWidget->setObjectName(QString::fromUtf8("CInitWidget"));
        CInitWidget->resize(550, 320);
        label_2 = new QLabel(CInitWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 10, 140, 48));
        label_2->setPixmap(QPixmap(QString::fromUtf8(":/images/project_logo")));
        label = new QLabel(CInitWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 90, 376, 38));
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/project_name")));
        label_load = new QLabel(CInitWidget);
        label_load->setObjectName(QString::fromUtf8("label_load"));
        label_load->setGeometry(QRect(30, 150, 34, 34));
        label_msg = new QLabel(CInitWidget);
        label_msg->setObjectName(QString::fromUtf8("label_msg"));
        label_msg->setGeometry(QRect(80, 160, 301, 16));
        btnClose = new QPushButton(CInitWidget);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));
        btnClose->setGeometry(QRect(510, 0, 23, 25));
        btnClose->setFocusPolicy(Qt::NoFocus);
        btnClose->setStyleSheet(QString::fromUtf8("QPushButton#btnClose{\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_close_normal);\n"
"}\n"
"\n"
"QPushButton#btnClose:hover {\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_close_hover);\n"
"}\n"
"\n"
"QPushButton#btnClose:pressed {\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_close_down);\n"
"}"));
        btnClose->setFlat(true);

        retranslateUi(CInitWidget);

        QMetaObject::connectSlotsByName(CInitWidget);
    } // setupUi

    void retranslateUi(QWidget *CInitWidget)
    {
        CInitWidget->setWindowTitle(QApplication::translate("CInitWidget", "CInitWidget", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        label->setText(QString());
        label_load->setText(QApplication::translate("CInitWidget", "load", 0, QApplication::UnicodeUTF8));
        label_msg->setText(QApplication::translate("CInitWidget", "\345\220\257\345\212\250\344\270\255......", 0, QApplication::UnicodeUTF8));
        btnClose->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CInitWidget: public Ui_CInitWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CINITWIDGET_H
