/**
 *
 * �� �� �� : view_plugin_demo1.cpp
 * �������� : 2018-03-24 09:34
 * ��    �� : SspAssist(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ����ƽ̨���Բ��1
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-03-24	SspAssist�������ļ�
 *
 **/
#include "view_plugin_demo1.h"
#include "mdb_def.h"

view_plugin_demo1::view_plugin_demo1(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	SetBackgroundColor();
	RegisterMdbTrigger();

	TestCommandSend();
	Start();
}

view_plugin_demo1::~view_plugin_demo1()
{
	RemoveMdbTrigger();
}

int view_plugin_demo1::OnCommand(SString sCmd,SString &sResult)
{
	return 0;
}

BYTE* view_plugin_demo1::OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	t_oe_element_state stuElemState;
	if (sTable == "t_oe_element_state")
	{
		int size = sizeof(t_oe_element_state);
		if (size != iRowSize)
		{
			LOGWARN("�ṹ��С��һ��(%s:%d|%d)������mdb_def.h��mdb.xml�Ƿ��Ӧ.",sTable.data(),size,iRowSize);
			return 0;
		}

		for (int i = 0; i < iTrgRows; i++)
		{
			memset(&stuElemState,0,sizeof(t_oe_element_state));
			memcpy(&stuElemState,pTrgData+i*sizeof(t_oe_element_state),sizeof(t_oe_element_state));
		}
	}
	
	return 0;
}

bool view_plugin_demo1::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return true;
}

void view_plugin_demo1::TestCommandSend()
{
	stuSpUnitAgentProcessId dst_id;
	dst_id.m_iUnitId = 0;
	dst_id.SetApplicationId(SP_UA_APPNO_AGENT);

	bool ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,SP_UA_MSG_REQ_GET_UNIT,SApplication::NewMsgSn());
	if (!ret)
	{
		int b = 0;
	}
}

void view_plugin_demo1::Start()
{
	CreatSvgXml();
	SString sContent;
	m_svgXml.SaveToXml(sContent);

	m_pSvgView = new DrawView;
	m_pSvgView->Load(sContent.data());
	ui.gridLayoutCentral->addWidget(m_pSvgView);
}

void view_plugin_demo1::CreatSvgXml()
{
	stuPoint point;
	//point.m_x = 100;
	//point.m_y = 300;
	//m_svgXml.CreatRhombus(point,100,150);

	point.m_x = 200;
	point.m_y = 100;
	CSKTextItem *item = m_svgXml.CreatTextTime(point,200,80);
	item->SetFamily("΢���ź�");
	item->SetPointSize(16);
}
