#include "dlgmanufacturer.h"
#include "qmessagebox.h"

DlgManufacturer::DlgManufacturer(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.radioButton_select,SIGNAL(clicked()),this,SLOT(slotRadioMafSelect()));
	connect(ui.radioButton_new,SIGNAL(clicked()),this,SLOT(slotRadioMafNew()));
	connect(ui.pushButton_ok,SIGNAL(clicked()), this, SLOT(slotOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()), this, SLOT(slotClose()));

	init();
}

DlgManufacturer::~DlgManufacturer()
{

}

void DlgManufacturer::init()
{
	max_maf_no = 0;
	ui.radioButton_select->click();
}

void DlgManufacturer::start()
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select maf_no,maf_name,maf_fullname,wildcard from t_oe_manufacturer_def");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			int m_maf_no = rs.GetValue(i,0).toInt();
			ui.comboBox_maf->addItem(
				QIcon(":/image/station"),
				tr("%1:%2").arg(m_maf_no).arg(rs.GetValue(i,2).data()),
				rs.GetValue(i,3).data());

			if (m_maf_no > max_maf_no)
				max_maf_no = m_maf_no;
		}
	}

	ui.label_type->setText(m_model);
	ui.label_maf->setText(m_wildcard);
}

void DlgManufacturer::slotRadioMafSelect()
{
	ui.comboBox_maf->setEnabled(true);
	ui.lineEdit_maf->setEnabled(false);
}

void DlgManufacturer::slotRadioMafNew()
{
	ui.comboBox_maf->setEnabled(false);
	ui.lineEdit_maf->setEnabled(true);
}

void DlgManufacturer::slotOk()
{
	m_error = QString::null;

	if (ui.radioButton_select->isChecked())
	{
		QString text = ui.comboBox_maf->currentText();
		maf_no = text.split(":").at(0).toInt();
		updateDb();
	}
	else if (ui.radioButton_new->isChecked())
	{
		if (ui.lineEdit_maf->text().isEmpty())
		{
			QMessageBox::warning(this,tr("告警"),tr("请输入厂家名称。"));
			return;
		}
		
		insertDb();
	}

	accept();
}

void DlgManufacturer::slotClose()
{
	int ret = QMessageBox::question(this,tr("询问"),tr("确认不导入此装置厂家信息？"),tr("是"),tr("否"));
	if (ret == 0)
		close();
}

void DlgManufacturer::insertDb()
{
	SString sql;
	SRecordset rs;

	QString maf = ui.lineEdit_maf->text();
	max_maf_no++;
	sql.sprintf("insert into t_oe_manufacturer_def(maf_no,maf_name,maf_fullname,wildcard) values(%d,'%s','%s','%s')",
		max_maf_no,maf.toStdString().data(),maf.toStdString().data(),m_wildcard.toStdString().data());
	if (!DB->Execute(sql))
		m_error = "数据库SQL语句执行错误：" + sql;

	maf_no = max_maf_no;
}

void DlgManufacturer::updateDb()
{
	if (m_wildcard.isEmpty())
		return;

	QString wildcard = ui.comboBox_maf->itemData(ui.comboBox_maf->currentIndex()).toString();
	if (wildcard.isEmpty())
	{
		wildcard += m_wildcard;
	}
	else
	{
		wildcard += tr("|%1").arg(m_wildcard);
	}

	SString sql;
	sql.sprintf("update t_oe_manufacturer_def set wildcard='%s' where maf_no=%d",wildcard.toStdString().data(),maf_no);
	if (!DB->Execute(sql))
		m_error = "数据库SQL语句执行错误：" + sql;
}