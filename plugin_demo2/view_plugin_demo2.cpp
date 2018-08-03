/**
 *
 * �� �� �� : view_plugin_demo2.cpp
 * �������� : 2018-03-24 09:34
 * ��    �� : SspAssist(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ����ƽ̨���Բ��2
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-03-24	SspAssist�������ļ�
 *
 **/
#include "view_plugin_demo2.h"
#include "mdb_def.h"

view_plugin_demo2::view_plugin_demo2(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	SetBackgroundColor();
	RegisterMdbTrigger();
}

view_plugin_demo2::~view_plugin_demo2()
{
	RemoveMdbTrigger();
}

BYTE* view_plugin_demo2::OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	t_oe_element_state stuElemState;
	if (sTable == "t_oe_element_state")
	{
		if (sizeof(t_oe_element_state) != iRowSize)
		{
			LOGWARN("�ṹ��С��һ��(%s)������mdb_def.h��mdb.xml�Ƿ��Ӧ.",sTable.data());
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
