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
			m_app->m_pSvgView->RefreshStateByKey(key,stu.stuState.current_val);
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

	m_pSvgView = new CSvgView();
	m_mdbThred = new MDBThread(this);
	m_mdbThred->start();
}

view_plugin_drawer::~view_plugin_drawer()
{
	RemoveMdbTrigger();

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
		m_pSvgView->Load((char*)pBuf);
		ui.gridLayoutCentral->addWidget(m_pSvgView);
		delete [] pBuf;
	}

	return 0;
}
