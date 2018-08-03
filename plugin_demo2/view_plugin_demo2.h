/**
 *
 * 文 件 名 : view_plugin_demo2.h
 * 创建日期 : 2018-03-24 09:34
 * 作    者 : SspAssist(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 界面平台测试插件2
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-03-24	SspAssist　创建文件
 *
 **/

#ifndef __VIEW_PLUGIN_DEMO2_H__
#define __VIEW_PLUGIN_DEMO2_H__

#include "cbaseview.h"
#include "sk_database.h"
#include "SMdb.h"
#include "ui_view_plugin_demo2.h"

class view_plugin_demo2 : public CBaseView
{
	Q_OBJECT

public:
	view_plugin_demo2(QWidget *parent = 0);
	~view_plugin_demo2();

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 230)
	{
		setStyleSheet(tr("QWidget#%1{background:rgb(%2,%3,%4,%5);}").arg(objectName()).arg(red).arg(yellow).arg(blue).arg(alpha));
	}

	//此接口函数须快速操作回调完毕，不可阻塞
	static BYTE* OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);
	
private:
	Ui::view_plugin_demo2 ui;

	CMdbClient *m_pMdbTrgClient;

private:
	void RegisterMdbTrigger()
	{
		m_pMdbTrgClient = ((SMdb*)MDB->GetDatabasePool()->GetDatabaseByIdx(MDB->GetDatabasePool()->GetPoolSize()-1))->GetMdbClient();
		m_pMdbTrgClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_oe_element_state",MDB_TRGFLG_UPDATE);
	}

	void RemoveMdbTrigger()
	{
		m_pMdbTrgClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_oe_element_state",MDB_TRGFLG_UPDATE);
	}
};

#endif // __VIEW_PLUGIN_DEMO2_H__

