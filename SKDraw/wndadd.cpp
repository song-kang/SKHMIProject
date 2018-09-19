#include "wndadd.h"
#include "skdraw.h"
#include "sk_database.h"

WndAdd::WndAdd(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_app = (SKDraw *)parent;

	Init();
	InitUi();
	InitSlot();
}

WndAdd::~WndAdd()
{

}

void WndAdd::Init()
{
	m_mapIcon.insert(WND_TYPE_FOLDER,":/images/open");
	m_mapIcon.insert(WND_TYPE_SVG,":/images/logo");
	ui.labelLog->setText(QString::null);
}

void WndAdd::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);}").arg(objectName()));
}

void WndAdd::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
}

void WndAdd::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void WndAdd::Start()
{

}

void WndAdd::SlotOk()
{
	if (ui.lineEdit->text().isEmpty())
	{
		ui.labelLog->setText("输入不可为空");
		return;
	}

	QTreeWidgetItem *item = m_app->GetCurrentTreeWidgetItem();
	int wnd_sn = DB->SelectIntoI("select max(wnd_sn) from t_ssp_uicfg_wnd") + 1;
	int p_wnd_sn = 0;
	if (item)
		p_wnd_sn = item->data(0,Qt::UserRole).toInt();
	QString wnd_name = ui.lineEdit->text().trimmed();
	int create_time = SDateTime::getNowSoc();
	int modify_time = create_time;

	SString sql;
	sql.sprintf("insert into t_ssp_uicfg_wnd (wnd_sn,p_wnd_sn,wnd_name,create_time,modify_time,wnd_type) values (%d,%d,'%s',%d,%d,%d)",
		wnd_sn,p_wnd_sn,wnd_name.toStdString().data(),create_time,modify_time,m_iType);
	DB->Execute(sql);
	
	QTreeWidgetItem *_it;
	if (item)
		_it = new QTreeWidgetItem(item, m_iType);
	else
		_it = new QTreeWidgetItem(m_app->GetWndTreeWidget(), m_iType);
	_it->setIcon(0, QIcon(m_mapIcon.value(m_iType)));
	_it->setData(0, Qt::UserRole,wnd_sn);
	_it->setText(0, wnd_name);
	if (item)
		item->setExpanded(true);

	CWnd *pWnd = NULL;
	if (m_app->GetCurrentTreeWidgetItem())
		pWnd = m_app->GetWndFromSn(m_app->GetCurrentTreeWidgetItem()->data(0, Qt::UserRole).toInt(), m_app->GetListWnd());
	CWnd *wnd = new CWnd(pWnd);
	wnd->SetWndSn(wnd_sn);
	wnd->SetPWndSn(p_wnd_sn);
	wnd->SetWndName(wnd_name);
	wnd->SetCreateAuth("");
	wnd->SetCreateTime(create_time);
	wnd->SetModifyAuth("");
	wnd->SetModifyTime(modify_time);
	wnd->SetWndType(m_iType);
	if (pWnd)
		pWnd->m_lstChilds.append(wnd);
	else
		m_app->GetListWnd().append(wnd);

	emit SigClose();
}

void WndAdd::SlotCancel()
{
	emit SigClose();
}
