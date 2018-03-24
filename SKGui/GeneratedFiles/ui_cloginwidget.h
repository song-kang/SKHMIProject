/********************************************************************************
** Form generated from reading UI file 'cloginwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLOGINWIDGET_H
#define UI_CLOGINWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CLoginWidget
{
public:
    QLineEdit *lineEdit_password;
    QLabel *label;
    QLabel *label_tip;
    QComboBox *comboBox_user;
    QPushButton *btnCancel;
    QPushButton *btnLogin;
    QLabel *label_2;
    QPushButton *btnClose;

    void setupUi(QWidget *CLoginWidget)
    {
        if (CLoginWidget->objectName().isEmpty())
            CLoginWidget->setObjectName(QString::fromUtf8("CLoginWidget"));
        CLoginWidget->resize(400, 300);
        lineEdit_password = new QLineEdit(CLoginWidget);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(96, 140, 261, 20));
        label = new QLabel(CLoginWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 100, 60, 18));
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/username")));
        label_tip = new QLabel(CLoginWidget);
        label_tip->setObjectName(QString::fromUtf8("label_tip"));
        label_tip->setGeometry(QRect(98, 169, 241, 23));
        label_tip->setPixmap(QPixmap(QString::fromUtf8(":/images/login_tip")));
        comboBox_user = new QComboBox(CLoginWidget);
        comboBox_user->setObjectName(QString::fromUtf8("comboBox_user"));
        comboBox_user->setGeometry(QRect(96, 100, 261, 20));
        comboBox_user->setEditable(true);
        btnCancel = new QPushButton(CLoginWidget);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
        btnCancel->setGeometry(QRect(220, 219, 84, 38));
        btnCancel->setStyleSheet(QString::fromUtf8("QPushButton#btnCancel{\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_cancel);\n"
"}\n"
"\n"
"QPushButton#btnCancel:hover {\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_cancel);\n"
"}\n"
"\n"
"QPushButton#btnCancel:pressed {\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_cancel);\n"
"}"));
        btnLogin = new QPushButton(CLoginWidget);
        btnLogin->setObjectName(QString::fromUtf8("btnLogin"));
        btnLogin->setGeometry(QRect(100, 219, 84, 38));
        btnLogin->setStyleSheet(QString::fromUtf8("QPushButton#btnLogin{\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_login);\n"
"}\n"
"\n"
"QPushButton#btnLogin:hover {\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_login);\n"
"}\n"
"\n"
"QPushButton#btnLogin:pressed {\n"
"  padding: -1;\n"
"  border-image:url(:/images/btn_login);\n"
"}"));
        label_2 = new QLabel(CLoginWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 140, 60, 18));
        label_2->setPixmap(QPixmap(QString::fromUtf8(":/images/password")));
        btnClose = new QPushButton(CLoginWidget);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));
        btnClose->setGeometry(QRect(370, 0, 23, 25));
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
        QWidget::setTabOrder(comboBox_user, lineEdit_password);
        QWidget::setTabOrder(lineEdit_password, btnLogin);
        QWidget::setTabOrder(btnLogin, btnCancel);
        QWidget::setTabOrder(btnCancel, btnClose);

        retranslateUi(CLoginWidget);

        QMetaObject::connectSlotsByName(CLoginWidget);
    } // setupUi

    void retranslateUi(QWidget *CLoginWidget)
    {
        CLoginWidget->setWindowTitle(QApplication::translate("CLoginWidget", "CLoginWidget", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        label_tip->setText(QString());
        btnCancel->setText(QString());
        btnLogin->setText(QString());
        label_2->setText(QString());
        btnClose->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CLoginWidget: public Ui_CLoginWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLOGINWIDGET_H
