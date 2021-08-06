#include "dlgtable.h"
#include <QMessageBox>

DlgTable::DlgTable(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

DlgTable::~DlgTable()
{

}

void DlgTable::Init()
{
	m_eOper = eOperAdd;
}

void DlgTable::InitUi()
{
	
}

void DlgTable::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void DlgTable::Start(eOper oper)
{
	QRegExp regx("[1-9][0-9]+$");
	QValidator *validator = new QRegExpValidator(regx, ui.lineEdit);

	m_eOper = oper;
	switch(oper) {
	case eOperAdd:
		this->setWindowTitle("增加条目");
		ui.label->setText("增加个数");
		ui.lineEdit->setValidator(validator);
		break;
	case eOperCommon:
		this->setWindowTitle("设同一值");
		ui.label->setText("数值");
		break;
	case eOperSequence:
		this->setWindowTitle("设顺序值");
		ui.label->setText("起始数值");
		ui.lineEdit->setValidator(validator);
		break;
	}
}

void DlgTable::SlotOk()
{
	m_sVal = ui.lineEdit->text().trimmed();
	switch (m_eOper) {
	case eOperAdd:
		{
			quint32 val = m_sVal.toUInt();
			if (val > 1000) {
				QMessageBox::warning(this, "告警", "增加个数一次不能超过1000");
				return;
			}
		}
		break;
	case eOperCommon:
	case eOperSequence:
		break;
	}

	accept();
}
