#include "dlgimportwarntext.h"

DlgImportWarnText::DlgImportWarnText(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.plainTextEdit->setReadOnly(true);
	ui.plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
}

DlgImportWarnText::~DlgImportWarnText()
{

}

void DlgImportWarnText::showWarnText()
{
	ui.plainTextEdit->setPlainText(warnText);
}
