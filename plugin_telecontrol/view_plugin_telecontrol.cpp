/**
 *
 * 文 件 名 : view_plugin_telecontrol.cpp
 * 创建日期 : 2018-09-26 13:25
 * 修改日期 : $Date: $
 * 功能描述 : 遥控类界面插件
 * 修改记录 : 
 *
 **/
#include "view_plugin_telecontrol.h"
#include "mdb_def.h"

view_plugin_telecontrol::view_plugin_telecontrol(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

view_plugin_telecontrol::~view_plugin_telecontrol()
{
	RemoveMdbTrigger();
}

void view_plugin_telecontrol::Init()
{
	SetBackgroundColor();
	RegisterMdbTrigger();
}

void view_plugin_telecontrol::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void view_plugin_telecontrol::InitSlot()
{

}

int view_plugin_telecontrol::OnCommand(SString sCmd,SString &sResult)
{
	return 0;
}

BYTE* view_plugin_telecontrol::OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
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

bool view_plugin_telecontrol::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return true;
}

void view_plugin_telecontrol::TestCommandSend()
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
