#ifndef DLGIMPORTWARNTEXT_H
#define DLGIMPORTWARNTEXT_H

#include <QDialog>
#include "ui_dlgimportwarntext.h"

class DlgImportWarnText : public QDialog
{
	Q_OBJECT

public:
	DlgImportWarnText(QWidget *parent = 0);
	~DlgImportWarnText();

	void setWarnText(QString text) { warnText = text; showWarnText(); }
 
private:
	Ui::DlgImportWarnText ui;

	QString warnText;

private:
	void showWarnText();

};

#endif // DLGIMPORTWARNTEXT_H
