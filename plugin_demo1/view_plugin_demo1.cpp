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
}

view_plugin_demo1::~view_plugin_demo1()
{
	RemoveMdbTrigger();
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
