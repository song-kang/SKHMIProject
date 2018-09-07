/**
 *
 * 文 件 名 : view_plugin_drawer.cpp
 * 创建日期 : 2018-08-11 09:03
 * 作    者 : SspAssist(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 自画图形展示插件
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-08-11	SspAssist　创建文件
 *
 **/
#include "view_plugin_drawer.h"
#include "mdb_def.h"

view_plugin_drawer::view_plugin_drawer(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	SetBackgroundColor();
	RegisterMdbTrigger();
}

view_plugin_drawer::~view_plugin_drawer()
{
	RemoveMdbTrigger();

	QMap<QString, QList<GraphicsItem *>*>::const_iterator it;
	for (it = m_mapLinkDB.constBegin(); it != m_mapLinkDB.constEnd(); it++)
		delete it.value();
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

			QString key = tr("t_oe_element_state::%1,%2,%3,%4::%5")
				.arg(stuElemState.ied_no).arg(stuElemState.cpu_no).arg(stuElemState.group_no).arg(stuElemState.entry).arg(stuElemState.name);
			QMap<QString, QList<GraphicsItem *>*>::iterator it = pThis->m_mapLinkDB.find(key);
			if (it == pThis->m_mapLinkDB.constEnd())
				continue;

			QList<GraphicsItem *> *itemList = it.value();
			for (int i = 0; i < itemList->count(); i++)
			{
				GraphicsItem *item = itemList->at(i);
				item->SetIsFlash(true);
				item->SetShowState(stuElemState.current_val);
			}
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
#if 0
	SString sql;
	SRecordset rs;
	sql.sprintf("select ref_sn from t_ssp_fun_point where fun_key='%s'",sCmd.data());
	int ref_sn = DB->SelectIntoI(sql);

	unsigned char *pBuf;
	int iLen = 0;
	if (DB->ReadLobToMem("t_ssp_uicfg_wnd","svg_file",SString::toFormat("wnd_sn=%d",ref_sn),pBuf,iLen))
	{
		m_pView->Load((char*)pBuf);
		delete [] pBuf;
	}
#endif

	m_pView = CreateView();
	if (m_pView)
	{
		m_pView->Load(QString(sCmd.data()));
		ui.gridLayoutCentral->addWidget(m_pView);
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
	int a = 0;
}

void view_plugin_drawer::SlotPositionChanged(int,int)
{
	int a = 0;
}

void view_plugin_drawer::SlotMouseRightButton(QPoint)
{
	int a = 0;
}

void view_plugin_drawer::InsertMapLinkDB(QString key, GraphicsItem *item)
{
	QList<GraphicsItem *> *itemList;
	QMap<QString, QList<GraphicsItem *>*>::iterator it = m_mapLinkDB.find(key);
	if (it == m_mapLinkDB.constEnd())
	{
		itemList = new QList<GraphicsItem *>;
		itemList->append(item);
		m_mapLinkDB.insert(key, itemList);
	}
	else
	{
		itemList = it.value();
		itemList->append(item);
	}
}
