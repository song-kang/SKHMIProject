/********************************************************************************
** Form generated from reading UI file 'example.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXAMPLE_H
#define UI_EXAMPLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class Ui_exampleClass
{
public:
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QGridLayout *gridLayout;
    QWebView *webView_pie;
    QWebView *webView_bar;
    QTableWidget *tableWidget_ia_warn;
    QHBoxLayout *horizontalLayout;
    QWebView *webView_line;
    QWidget *widget_map;
    QTableWidget *tableWidget_warn;

    void setupUi(QWidget *exampleClass)
    {
        if (exampleClass->objectName().isEmpty())
            exampleClass->setObjectName(QString::fromUtf8("exampleClass"));
        exampleClass->resize(893, 600);
        gridLayout_2 = new QGridLayout(exampleClass);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(-1, 120, -1, -1);
        webView_pie = new QWebView(exampleClass);
        webView_pie->setObjectName(QString::fromUtf8("webView_pie"));
        webView_pie->setUrl(QUrl(QString::fromUtf8("about:blank")));

        gridLayout->addWidget(webView_pie, 0, 0, 1, 1);


        horizontalLayout_2->addLayout(gridLayout);

        webView_bar = new QWebView(exampleClass);
        webView_bar->setObjectName(QString::fromUtf8("webView_bar"));
        webView_bar->setUrl(QUrl(QString::fromUtf8("about:blank")));

        horizontalLayout_2->addWidget(webView_bar);

        tableWidget_ia_warn = new QTableWidget(exampleClass);
        tableWidget_ia_warn->setObjectName(QString::fromUtf8("tableWidget_ia_warn"));

        horizontalLayout_2->addWidget(tableWidget_ia_warn);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 2);
        horizontalLayout_2->setStretch(2, 1);

        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        webView_line = new QWebView(exampleClass);
        webView_line->setObjectName(QString::fromUtf8("webView_line"));
        webView_line->setUrl(QUrl(QString::fromUtf8("about:blank")));

        horizontalLayout->addWidget(webView_line);

        widget_map = new QWidget(exampleClass);
        widget_map->setObjectName(QString::fromUtf8("widget_map"));

        horizontalLayout->addWidget(widget_map);

        tableWidget_warn = new QTableWidget(exampleClass);
        tableWidget_warn->setObjectName(QString::fromUtf8("tableWidget_warn"));

        horizontalLayout->addWidget(tableWidget_warn);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 2);
        horizontalLayout->setStretch(2, 1);

        gridLayout_2->addLayout(horizontalLayout, 1, 0, 1, 1);


        retranslateUi(exampleClass);

        QMetaObject::connectSlotsByName(exampleClass);
    } // setupUi

    void retranslateUi(QWidget *exampleClass)
    {
        exampleClass->setWindowTitle(QApplication::translate("exampleClass", "example", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class exampleClass: public Ui_exampleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXAMPLE_H
