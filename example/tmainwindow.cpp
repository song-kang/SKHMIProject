#include "tmainwindow.h"

TMainWindow::TMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);}").arg(objectName()));
}

TMainWindow::~TMainWindow()
{

}
