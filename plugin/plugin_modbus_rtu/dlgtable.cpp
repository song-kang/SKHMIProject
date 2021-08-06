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
		this->setWindowTitle("������Ŀ");
		ui.label->setText("���Ӹ���");
		ui.lineEdit->setValidator(validator);
		break;
	case eOperCommon:
		this->setWindowTitle("��ͬһֵ");
		ui.label->setText("��ֵ");
		break;
	case eOperSequence:
		this->setWindowTitle("��˳��ֵ");
		ui.label->setText("��ʼ��ֵ");
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
				QMessageBox::warning(this, "�澯", "���Ӹ���һ�β��ܳ���1000");
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
