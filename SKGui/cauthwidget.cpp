#include "cauthwidget.h"
#include "skgui.h"
#include "cfunpoint.h"
#include "cuserswidget.h"

#define USER_GROUP		1
#define USER			2

#define COLUMN_CHECK	0
#define COLUMN_KEY		1
#define COLUMN_DESC		2

Q_DECLARE_METATYPE(CFunPoint*);

CAuthWidget::CAuthWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_pUsersWidget = (CUsersWidget *)parent;

	Init();
	InitUi();
	InitSlot();
}

CAuthWidget::~CAuthWidget()
{

}

void CAuthWidget::Init()
{
	ui.tableWidgetAuth->setColumnWidth(COLUMN_CHECK,50);
	ui.tableWidgetAuth->setColumnWidth(COLUMN_KEY,180);
	ui.tableWidgetAuth->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	//ui.subTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);		//不可编辑
	ui.tableWidgetAuth->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidgetAuth->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	ui.tableWidgetAuth->horizontalHeader()->setStretchLastSection(true);			//设置充满表宽度
	ui.tableWidgetAuth->setStyleSheet("selection-background-color:lightblue;");		//设置选中背景色
	ui.tableWidgetAuth->verticalHeader()->setDefaultSectionSize(22);				//设置行高
	ui.tableWidgetAuth->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidgetAuth->setAlternatingRowColors(true);								//设置交替行色
	ui.tableWidgetAuth->verticalHeader()->setVisible(false);						//去除最前列
}

void CAuthWidget::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void CAuthWidget::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
}

void CAuthWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CAuthWidget::Start()
{
	if (m_iType == USER_GROUP)
	{
		ui.widget_user->hide();
		ShowUsersAuth();
	}
	else if (m_iType == USER)
	{
		ui.widget_grp->hide();
		if (m_pUsers)
			ShowUserAuth(m_pUsers);
	}
}

void CAuthWidget::SlotOk()
{
	if (m_iType == USER_GROUP)
	{
		AffirmGroup();
	}
	else if (m_iType == USER)
	{
		AffirmUser();
	}
}

void CAuthWidget::SlotCancel()
{
	m_app->SlotClose();
}

void CAuthWidget::ShowUsersAuth()
{
	QList<CFunPoint*> lstFunPoint = SK_GUI->GetRunFunPoints();
	int row = 0;
	QTableWidgetItem * item;
	ui.tableWidgetAuth->setRowCount(lstFunPoint.count());
	foreach (CFunPoint *p, lstFunPoint)
	{
		item = new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setCheckState(Qt::Unchecked);
		QVariant var;
		var.setValue(p);
		item->setData(Qt::UserRole,var);
		ui.tableWidgetAuth->setItem(row,COLUMN_CHECK,item);

		item = new QTableWidgetItem(p->GetKey());
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
		ui.tableWidgetAuth->setItem(row,COLUMN_KEY,item);

		item = new QTableWidgetItem(p->GetDesc());
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
		ui.tableWidgetAuth->setItem(row,COLUMN_DESC,item);

		row++;
	}
}

void CAuthWidget::ShowUserAuth(CUsers *users)
{
	QList<CFunPoint*> lstFunPoint = SK_GUI->GetRunFunPoints();
	int row = 0;
	QTableWidgetItem * item;
	ui.tableWidgetAuth->setRowCount(lstFunPoint.count());
	foreach (CFunPoint *p, lstFunPoint)
	{
		foreach (stuAuth *auth, users->m_lstAuth)
		{
			if (auth->fun_key == p->GetKey() && auth->auth)
			{
				item = new QTableWidgetItem("");
				item->setTextAlignment(Qt::AlignCenter);
				item->setCheckState(auth->auth ? Qt::Checked : Qt::Unchecked);
				QVariant var;
				var.setValue(p);
				item->setData(Qt::UserRole,var);
				ui.tableWidgetAuth->setItem(row,COLUMN_CHECK,item);

				item = new QTableWidgetItem(auth->fun_key);
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
				ui.tableWidgetAuth->setItem(row,COLUMN_KEY,item);

				item = new QTableWidgetItem(p->GetDesc());
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
				ui.tableWidgetAuth->setItem(row,COLUMN_DESC,item);

				row++;
			}
		}
	}

	ui.tableWidgetAuth->setRowCount(row);
}

void CAuthWidget::AffirmGroup()
{
	if (ui.lineEdit_grp_code->text().trimmed().isEmpty())
	{
		QMessageBox::warning(NULL,"告警","请输入用户组代码");
		return;
	}

	if (ui.lineEdit_grp_name->text().trimmed().isEmpty())
	{
		QMessageBox::warning(NULL,"告警","请输入用户组名称");
		return;
	}

	SString sql;
	SRecordset rs;
	sql.sprintf("select count(*) from t_ssp_user_group where grp_code='%s'",ui.lineEdit_grp_code->text().trimmed().toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count > 0)
		{
			QMessageBox::warning(NULL,"告警","此用户组代码已存在");
			return;
		}
	}
	else if (cnt < 0)
	{
		QMessageBox::warning(NULL,"告警",tr("SQL语句执行失败！\n\n%1").arg(sql.data()));
		return;
	}

	sql.sprintf("insert into t_ssp_user_group (grp_code,name,dsc) values ('%s','%s','%s')",
		ui.lineEdit_grp_code->text().trimmed().toStdString().data(),
		ui.lineEdit_grp_name->text().trimmed().toStdString().data(),
		ui.lineEdit_grp_desc->text().trimmed().toStdString().data());
	if (!DB->ExecuteSQL(sql))
	{
		QMessageBox::warning(NULL,"告警",tr("SQL语句执行失败！\n\n%1").arg(sql.data()));
		return;
	}

	SetListFunPointAuth(ui.lineEdit_grp_code->text().trimmed(),SK_GUI->m_lstFunPoint);
	for (int i = 0; i < ui.tableWidgetAuth->rowCount(); i++)
	{
		QTableWidgetItem *item = ui.tableWidgetAuth->item(i,0);
		if (item->checkState() == Qt::Checked)
			SetFunPointAuth(ui.lineEdit_grp_code->text().trimmed(),item->data(Qt::UserRole).value<CFunPoint*>());
	}

	SK_GUI->SetUsersAuth(ui.lineEdit_grp_code->text().trimmed());
	SlotCancel();
}

void CAuthWidget::AffirmUser()
{
	if (ui.lineEdit_user_name->text().trimmed().isEmpty())
	{
		QMessageBox::warning(NULL,"告警","请输入用户名称");
		return;
	}

	if (ui.lineEdit_user_password->text().trimmed().isEmpty())
	{
		QMessageBox::warning(NULL,"告警","请输入用户密码");
		return;
	}

	SString sql;
	SRecordset rs;
	sql.sprintf("select count(*) from t_ssp_user where usr_code='%s'",ui.lineEdit_user_name->text().trimmed().toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count > 0)
		{
			QMessageBox::warning(NULL,"告警","此用户名称已存在");
			return;
		}
	}
	else if (cnt < 0)
	{
		QMessageBox::warning(NULL,"告警",tr("SQL语句执行失败！\n\n%1").arg(sql.data()));
		return;
	}

	int max_sn = 0;
	sql.sprintf("select max(usr_sn) from t_ssp_user");
	cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		max_sn = rs.GetValue(0,0).toInt();
		max_sn++;
	}
	else if (cnt < 0)
	{
		QMessageBox::warning(NULL,"告警",tr("SQL语句执行失败！\n\n%1").arg(sql.data()));
		return;
	}	

	sql.sprintf("insert into t_ssp_user (usr_sn,usr_code,grp_code,name,pwd,dsc,email,mobile) values (%d,'%s','%s','%s','%s','%s','%s','%s')",
		max_sn,
		ui.lineEdit_user_name->text().trimmed().toStdString().data(),
		m_pUsers->GetCode().toStdString().data(),
		ui.lineEdit_user_name->text().trimmed().toStdString().data(),
		ui.lineEdit_user_password->text().trimmed().toStdString().data(),
		ui.lineEdit_user_desc->text().trimmed().toStdString().data(),
		ui.lineEdit_user_email->text().trimmed().toStdString().data(),
		ui.lineEdit_user_mobile->text().trimmed().toStdString().data());
	if (!DB->ExecuteSQL(sql))
	{
		QMessageBox::warning(NULL,"告警",tr("SQL语句执行失败！\n\n%1").arg(sql.data()));
		return;
	}

	SetListFunPointAuth(max_sn,SK_GUI->m_lstFunPoint);
	for (int i = 0; i < ui.tableWidgetAuth->rowCount(); i++)
	{
		QTableWidgetItem *item = ui.tableWidgetAuth->item(i,0);
		if (item->checkState() == Qt::Checked)
			SetFunPointAuth(max_sn,item->data(Qt::UserRole).value<CFunPoint*>());
	}

	CUser *user = SK_GUI->SetUserAuth(m_pUsers,max_sn);
	if (user)
		m_pUsersWidget->m_lstUser.append(user);

	SlotCancel();
}

void CAuthWidget::SetListFunPointAuth(QString code, QList<CFunPoint*> lstFunPoint)
{
	SString sql;
	foreach (CFunPoint *p, lstFunPoint)
	{
		sql.sprintf("insert into t_ssp_usergroup_auth values ('%s','%s',0)",code.toStdString().data(),p->GetKey().toStdString().data());
		DB->Execute(sql);

		SetListFunPointAuth(code, p->m_lstChilds);
	}
}

void CAuthWidget::SetListFunPointAuth(int sn,QList<CFunPoint*> lstFunPoint)
{
	SString sql;
	foreach (CFunPoint *p, lstFunPoint)
	{
		sql.sprintf("insert into t_ssp_user_auth values (%d,'%s',0)",sn,p->GetKey().toStdString().data());
		DB->Execute(sql);

		SetListFunPointAuth(sn, p->m_lstChilds);
	}
}

void CAuthWidget::SetFunPointAuth(QString code, CFunPoint* funPoint)
{
	if (!funPoint->m_pParent)
		return;

	SString sql;
	sql.sprintf("update t_ssp_usergroup_auth set auth=1 where grp_code='%s' and fun_key='%s'",code.toStdString().data(),funPoint->GetKey().toStdString().data());
	DB->Execute(sql);

	SetFunPointAuth(code, funPoint->m_pParent);
}

void CAuthWidget::SetFunPointAuth(int sn, CFunPoint* funPoint)
{
	if (!funPoint->m_pParent)
		return;

	SString sql;
	sql.sprintf("update t_ssp_user_auth set auth=1 where usr_sn=%d and fun_key='%s'",sn,funPoint->GetKey().toStdString().data());
	DB->Execute(sql);

	SetFunPointAuth(sn, funPoint->m_pParent);
}
