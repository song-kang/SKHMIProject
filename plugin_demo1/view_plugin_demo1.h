/**
 *
 * �� �� �� : view_plugin_demo1.h
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

#ifndef __VIEW_PLUGIN_DEMO1_H__
#define __VIEW_PLUGIN_DEMO1_H__

#include "cbaseview.h"
#include "sk_database.h"
#include "SMdb.h"
#include "drawview.h"
#include "sk_svg_xml.h"
#include "ui_view_plugin_demo1.h"

class view_plugin_demo1 : public CBaseView
{
	Q_OBJECT

public:
	view_plugin_demo1(QWidget *parent = 0);
	~view_plugin_demo1();

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 230)
	{
		setStyleSheet(tr("QWidget#%1{background:rgb(%2,%3,%4,%5);}").arg(objectName()).arg(red).arg(yellow).arg(blue).arg(alpha));
	}

	virtual int OnCommand(SString sCmd,SString &sResult);
	//�˴��������뾡����С�����ٷ��أ���������
	static BYTE* OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);
	//������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	void Start();
	void CreatSvgXml();

private:
	Ui::view_plugin_demo1 ui;

	CMdbClient *m_pMdbTrgClient;
	DrawView *m_pSvgView;
	CSKSvgXml m_svgXml;

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

#endif // __VIEW_PLUGIN_DEMO1_H__

