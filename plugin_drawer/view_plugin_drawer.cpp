/**
 *
 * 文 件 名 : view_plugin_drawer.cpp
 * 创建日期 : 2018-08-11 09:03
 * 修改日期 : $Date: $
 * 功能描述 : 自画图形展示插件
 * 修改记录 : 
 *
 **/

#include "view_plugin_drawer.h"
#include "skgui.h"

//================= MDBThread ===================
MDBThread::MDBThread(QObject *parent)
	: QThread(parent), m_isRun(true), m_isQuit(false)
{
	m_app = (view_plugin_drawer *)parent;
}

MDBThread::~MDBThread()
{

}

void MDBThread::run()
{
	while(m_isRun)
	{
		m_app->m_queMutex.lock();
		while (!m_app->m_qOeElementState.isEmpty())
		{
			stuOeElementStateQueue stu = m_app->m_qOeElementState.dequeue();
			QString key = tr("t_oe_element_state::%1,%2,%3,%4::%5")
				.arg(stu.stuState.ied_no).arg(stu.stuState.cpu_no).arg(stu.stuState.group_no).arg(stu.stuState.entry).arg(stu.stuState.name);
			QMap<QString, QList<GraphicsItem *>*>::iterator it = m_app->m_mapLinkDBState.find(key);
			if (it == m_app->m_mapLinkDBState.constEnd())
			{
				m_app->m_queMutex.unlock();
				continue;
			}

			QList<GraphicsItem *> *itemList = it.value();
			for (int i = 0; i < itemList->count(); i++)
			{
				GraphicsItem *item = itemList->at(i);
				item->SetIsFlash(true);
				item->SetShowState(stu.stuState.current_val);
				item->SetStyleFromState(item->GetShowState());
			}
		}
		m_app->m_queMutex.unlock();
		msleep(100);
	}

	m_isQuit = true;
}

//================= view_plugin_drawer ===================
view_plugin_drawer::view_plugin_drawer(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	SetBackgroundColor();
	RegisterMdbTrigger();

	m_mdbThred = new MDBThread(this);
	m_mdbThred->start();

	m_pCtrlWidget = NULL;
}

view_plugin_drawer::~view_plugin_drawer()
{
	RemoveMdbTrigger();

	QMap<QString, QList<GraphicsItem *>*>::const_iterator it;
	for (it = m_mapLinkDBState.constBegin(); it != m_mapLinkDBState.constEnd(); it++)
		delete it.value();
	for (it = m_mapLinkDBMeasure.constBegin(); it != m_mapLinkDBMeasure.constEnd(); it++)
		delete it.value();

	while(!m_mdbThred->m_isQuit)
	{
		SApi::UsSleep(10000);
		m_mdbThred->m_isRun = false;	
	}
	m_qOeElementState.clear();
}

BYTE* view_plugin_drawer::OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	view_plugin_drawer *pThis = (view_plugin_drawer*)cbParam;

	if (sTable == "t_oe_element_state")
	{
		t_oe_element_state stuElemState;
		int size = sizeof(t_oe_element_state);
		if (size != iRowSize)
		{
			LOGWARN("结构大小不一致(%s:%d|%d)，请检查mdb_def.h和mdb.xml是否对应.",sTable.data(),size,iRowSize);
			return 0;
		}

		for (int i = 0; i < iTrgRows; i++)
		{
			memset(&stuElemState,0,sizeof(t_oe_element_state));
			memcpy(&stuElemState,pTrgData+i*sizeof(t_oe_element_state),sizeof(t_oe_element_state));

			stuOeElementStateQueue state;
			SDateTime::getSystemTime(state.soc,state.usec);
			memcpy(&state.stuState,&stuElemState,sizeof(t_oe_element_state));
			pThis->m_queMutex.lock();
			pThis->m_qOeElementState.enqueue(state);
			pThis->m_queMutex.unlock();
		}
	}

	return 0;
}

bool view_plugin_drawer::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return true;
}

int view_plugin_drawer::OnCommand(SString sCmd,SString &sResult)
{
	int iLen = 0;
	unsigned char *pBuf = NULL;
	if (DB->ReadLobToMem("t_ssp_uicfg_wnd","svg_file",sCmd,pBuf,iLen) && pBuf && iLen > 0)
	{
		m_pView = CreateView();
		if (m_pView)
		{
			m_pView->Load((char*)pBuf);
			ui.gridLayoutCentral->addWidget(m_pView);
		}

		delete [] pBuf;
	}

	return 0;
}

DrawView* view_plugin_drawer::CreateView()
{
	m_pScene = new DrawScene(this);
	connect(m_pScene, SIGNAL(selectionChanged()), this, SLOT(SlotItemSelected()));

	m_pView = new DrawView(m_pScene);
	m_pView->SetApp(this);
	m_pScene->SetView(m_pView);
	connect(m_pView, SIGNAL(SigPositionChanged(int,int)), this, SLOT(SlotPositionChanged(int,int)));
	connect(m_pView, SIGNAL(SigMouseRightButton(QPoint)), this, SLOT(SlotMouseRightButton(QPoint)));

	return m_pView;
}

void view_plugin_drawer::SlotItemSelected()
{
	QList<QGraphicsItem*> list = m_pScene->selectedItems();
	if (list.count() != 1)
		return;

	QGraphicsItem *item = list.at(0);
	if (((GraphicsItem*)item)->GetShowType() == 2)	//遥控界面
	{
		QString desc;
		QIcon icon;
		if (!m_pCtrlWidget)
		{
			CBaseView *view = SK_GUI->GotoFunPoint("plugin_telecontrol",desc,icon);
			if (view)
			{
				m_pCtrlWidget = new SKBaseWidget(NULL,view);
				m_pCtrlWidget->SetWindowsFlagsDialog();
				m_pCtrlWidget->SetWindowsModal();
				m_pCtrlWidget->SetWindowTitle("遥控操作");
				m_pCtrlWidget->SetWindowIcon(icon);
				m_pCtrlWidget->SetWindowFlags(0);
				m_pCtrlWidget->SetWindowSize(420,365);
				m_pCtrlWidget->SetIsDrag(true);
				connect(m_pCtrlWidget, SIGNAL(SigClose()), this, SLOT(SlotCtrlClose()));

				SString sCmd;
				SString sRet;
				view->OnCommand(sCmd,sRet);
				m_pCtrlWidget->Show();
			}
			else
				QMessageBox::warning(this, "告警", "遥控类插件[plugin_telecontrol]加载失败");
		}
		else
			m_pCtrlWidget->Show();
	}
	else if (((GraphicsItem*)item)->GetShowType() == 3)	//遥调界面
	{
		
	}
	else	//场景跳转
	{
		QString scene = ((GraphicsItem*)item)->GetLinkScene();
		if (scene.isEmpty())
			return;

		SK_GUI->GotoFunPoint(scene.split("::").at(0));
		m_pScene->clearSelection();
	}
}

void view_plugin_drawer::SlotCtrlClose()
{
	connect(m_pCtrlWidget, SIGNAL(SigClose()), this, SLOT(SlotCtrlClose()));
	delete m_pCtrlWidget;
	m_pCtrlWidget = NULL;
	m_pScene->clearSelection();
}

void view_plugin_drawer::SlotPositionChanged(int,int)
{
	
}

void view_plugin_drawer::SlotMouseRightButton(QPoint)
{
	
}

void view_plugin_drawer::InsertMapLinkDBState(QString key, GraphicsItem *item)
{
	QList<GraphicsItem *> *itemList;

	QMap<QString, QList<GraphicsItem *>*>::iterator it = m_mapLinkDBState.find(key);
	if (it == m_mapLinkDBState.constEnd())
	{
		itemList = new QList<GraphicsItem *>;
		itemList->append(item);
		m_mapLinkDBState.insert(key, itemList);
	}
	else
	{
		itemList = it.value();
		itemList->append(item);
	}
}

void view_plugin_drawer::InsertMapLinkDBMeasure(QString key, GraphicsItem *item)
{
	QList<GraphicsItem *> *itemList;

	QMap<QString, QList<GraphicsItem *>*>::iterator it = m_mapLinkDBMeasure.find(key);
	if (it == m_mapLinkDBMeasure.constEnd())
	{
		itemList = new QList<GraphicsItem *>;
		itemList->append(item);
		m_mapLinkDBMeasure.insert(key, itemList);
	}
	else
	{
		itemList = it.value();
		itemList->append(item);
	}
}

void view_plugin_drawer::InitDrawobj()
{
	SString sql;
	QMap<QString, QList<GraphicsItem *>*>::const_iterator it;
	for (it = m_mapLinkDBState.constBegin(); it != m_mapLinkDBState.constEnd(); it++)
	{
		QStringList l = it.key().split("::");
		QStringList ll = l.at(1).split(",");
		sql.sprintf("select current_val from %s where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
			l.at(0).toStdString().data(),ll.at(0).toInt(),ll.at(1).toInt(),ll.at(2).toInt(),ll.at(3).toInt());
		int iVal = DB->SelectIntoI(sql);

		QList<GraphicsItem *> *listItem = it.value();
		for (int i = 0; i < listItem->count(); i++)
		{
			GraphicsItem *item = listItem->at(i);
			if (item->GetShowState() == -1) //如为数据库初始状态
				item->SetShowState(iVal);
			item->SetStyleFromState(item->GetShowState());
		}
	}

	for (it = m_mapLinkDBMeasure.constBegin(); it != m_mapLinkDBMeasure.constEnd(); it++)
	{
		QStringList l = it.key().split("::");
		QStringList ll = l.at(1).split(",");
		sql.sprintf("select current_val from %s where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
			l.at(0).toStdString().data(),ll.at(0).toInt(),ll.at(1).toInt(),ll.at(2).toInt(),ll.at(3).toInt());
		SString sVal = DB->SelectInto(sql);

		QList<GraphicsItem *> *listItem = it.value();
		for (int i = 0; i < listItem->count(); i++)
		{
			GraphicsItem *item = listItem->at(i);
			if (item->GetName() == "文字图元")
				((GraphicsTextItem*)item)->SetText(sVal.data());
		}
	}
}

void view_plugin_drawer::RefreshStateFromDB()
{
	SString sql;
	QMap<QString, QList<GraphicsItem *>*>::const_iterator it;
	for (it = m_mapLinkDBState.constBegin(); it != m_mapLinkDBState.constEnd(); it++)
	{
		QStringList l = it.key().split("::");
		QStringList ll = l.at(1).split(",");
		sql.sprintf("select current_val from %s where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
			l.at(0).toStdString().data(),ll.at(0).toInt(),ll.at(1).toInt(),ll.at(2).toInt(),ll.at(3).toInt());
		int iVal = DB->SelectIntoI(sql);

		QList<GraphicsItem *> *listItem = it.value();
		for (int i = 0; i < listItem->count(); i++)
		{
			GraphicsItem *item = listItem->at(i);
			item->SetShowState(iVal);
		}
	}
}

void view_plugin_drawer::RefreshMeasureFromDB()
{
	SString sql;
	QMap<QString, QList<GraphicsItem *>*>::const_iterator it;
	for (it = m_mapLinkDBMeasure.constBegin(); it != m_mapLinkDBMeasure.constEnd(); it++)
	{
		QStringList l = it.key().split("::");
		QStringList ll = l.at(1).split(",");
		sql.sprintf("select current_val from %s where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
			l.at(0).toStdString().data(),ll.at(0).toInt(),ll.at(1).toInt(),ll.at(2).toInt(),ll.at(3).toInt());
		SString sVal = DB->SelectInto(sql);

		QList<GraphicsItem *> *listItem = it.value();
		for (int i = 0; i < listItem->count(); i++)
		{
			GraphicsItem *item = listItem->at(i);
			if (item->GetName() == "文字图元")
				((GraphicsTextItem*)item)->SetText(sVal.data());
		}
	}
}
