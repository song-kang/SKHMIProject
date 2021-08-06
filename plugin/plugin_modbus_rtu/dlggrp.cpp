#include "dlggrp.h"
#include "sk_database.h"
#include <QMessageBox>

DlgGrp::DlgGrp(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

DlgGrp::~DlgGrp()
{

}

void DlgGrp::Init()
{
	m_isFix = false;
	m_mapType.insert(GROUP_TYPE_DGT,0);
	m_mapType.insert(GROUP_TYPE_ANA,1);
	m_mapType.insert(GROUP_TYPE_CTL,2);
	m_mapType.insert(GROUP_TYPE_SET,3);
}

void DlgGrp::InitUi()
{
	ui.lineEditNo->setReadOnly(true);
	ui.comboBoxType->addItem("开关量",GROUP_TYPE_DGT);
	ui.comboBoxType->addItem("量测量",GROUP_TYPE_ANA);
	ui.comboBoxType->addItem("遥控量",GROUP_TYPE_CTL);
	ui.comboBoxType->addItem("定值量",GROUP_TYPE_SET);
}

void DlgGrp::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void DlgGrp::Start(quint32 iedNo)
{
	m_iIedNo = iedNo;

	SString sql;
	SRecordset rs;
	sql.sprintf("select max(group_no) from t_oe_group where ied_no=%d and cpu_no=1",iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt == 1) {
		int no = rs.GetValue(0,0).toInt();
		ui.lineEditNo->setText(QString("%1").arg(++no));
	}
}

void DlgGrp::StartFix(quint32 iedNo, quint32 groupNo)
{
	m_iIedNo = iedNo;

	SString sql;
	SRecordset rs;
	sql.sprintf("select group_no,name,type from t_oe_group "
		"where ied_no=%d and cpu_no=1 and group_no=%d",iedNo, groupNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt == 1) {
		ui.lineEditNo->setText(rs.GetValue(0,0).data());
		ui.comboBoxType->setCurrentIndex(m_mapType.value(rs.GetValue(0,2).toUInt()));
		ui.lineEditName->setText(rs.GetValue(0,1).data());
	}

	m_isFix = true;
	this->setWindowTitle(tr("修改组"));
}

void DlgGrp::SlotOk()
{
	m_iNo = ui.lineEditNo->text().toInt();
	m_iType = ui.comboBoxType->itemData(ui.comboBoxType->currentIndex()).toInt();
	m_sName = ui.lineEditName->text();

	SString sql;
	SRecordset rs;
	if (!m_isFix) {
		sql.sprintf("insert into t_oe_group(ied_no,cpu_no,group_no,name,type) "
			"values (%d,%d,%d,'%s',%d)",
			m_iIedNo,1,m_iNo,
			m_sName.toStdString().data(),
			m_iType);
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
			return;
		}
	}
	else {
		sql.sprintf("update t_oe_group set name='%s',type=%d "
			"where ied_no=%d and cpu_no=1 and group_no=%d",
			m_sName.toStdString().data(),
			m_iType,
			m_iIedNo,
			m_iNo);
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
			return;
		}
	}

	accept();
}
