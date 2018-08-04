/**
 *
 * 文 件 名 : view_plugin_demo1.cpp
 * 创建日期 : 2018-03-24 09:34
 * 作    者 : SspAssist(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 界面平台测试插件1
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-03-24	SspAssist　创建文件
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
			LOGWARN("结构大小不一致(%s:%d|%d)，请检查mdb_def.h和mdb.xml是否对应.",sTable.data(),size,iRowSize);
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
