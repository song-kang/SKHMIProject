#ifndef TMAINWINDOW_H
#define TMAINWINDOW_H

#include <QMainWindow>
#include "ui_tmainwindow.h"

class TMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	TMainWindow(QWidget *parent = 0);
	~TMainWindow();

private:
	Ui::TMainWindow ui;
};

#endif // TMAINWINDOW_H
