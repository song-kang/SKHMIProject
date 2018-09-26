/**
 *
 * �� �� �� : view_plugin_telecontrol.h
 * �������� : 2018-09-26 13:25
 * �޸����� : $Date: $
 * �������� : ң���������
 * �޸ļ�¼ : 
 *
 **/

#ifndef __VIEW_PLUGIN_TELECONTROL_H__
#define __VIEW_PLUGIN_TELECONTROL_H__

#include "cbaseview.h"
#include "sk_database.h"
#include "SMdb.h"
#include "ui_view_plugin_telecontrol.h"

class view_plugin_telecontrol : public CBaseView
{
	Q_OBJECT

public:
	view_plugin_telecontrol(QWidget *parent = 0);
	~view_plugin_telecontrol();

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 230)
	{
		setStyleSheet(tr("QWidget#%1{background:rgb(%2,%3,%4,%5);}").arg(objectName()).arg(red).arg(yellow).arg(blue).arg(alpha));
	}

	virtual int OnCommand(SString sCmd,SString &sResult);
	//�˴��������뾡����С�����ٷ��أ���������
	static BYTE* OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);
	//������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

private:
	Ui::view_plugin_telecontrol ui;

	CMdbClient *m_pMdbTrgClient;

private:
	void Init();
	void InitUi();
	void InitSlot();

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

	void TestCommandSend();
};

#endif // __VIEW_PLUGIN_TELECONTROL_H__

