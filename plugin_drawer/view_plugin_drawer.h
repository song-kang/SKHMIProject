/**
 *
 * �� �� �� : view_plugin_drawer.h
 * �������� : 2018-08-11 09:03
 * ��    �� : SspAssist(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �Ի�ͼ��չʾ���
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-08-11	SspAssist�������ļ�
 *
 **/

#ifndef __VIEW_PLUGIN_DRAWER_H__
#define __VIEW_PLUGIN_DRAWER_H__

#include "cbaseview.h"
#include "sk_database.h"
#include "SMdb.h"
#include "ui_view_plugin_drawer.h"
#include "drawview.h"
#include "drawscene.h"
#include "mdb_def.h"

struct stuOeElementStateQueue
{
	stuOeElementStateQueue()
	{
		soc = 0;
		usec = 0;
		memset(&stuState,0,sizeof(t_oe_element_state));
	}
	int soc;
	int usec;
	t_oe_element_state stuState;
};

//================= MDBThread ===================
class MDBThread : public QThread
{
	Q_OBJECT

public:
	MDBThread(QObject *parent);
	~MDBThread();

	bool m_isRun;
	bool m_isQuit;

protected:
	virtual void run();

private:
	view_plugin_drawer *m_app;

};

//================= view_plugin_drawer ===================
class view_plugin_drawer : public CBaseView
{
	Q_OBJECT

	friend class MDBThread;

public:
	view_plugin_drawer(QWidget *parent = 0);
	~view_plugin_drawer();

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 230)
	{
		setStyleSheet(tr("QWidget#%1{background:rgb(%2,%3,%4,%5);}").arg(objectName()).arg(red).arg(yellow).arg(blue).arg(alpha));
	}

	virtual int OnCommand(SString sCmd,SString &sResult);
	//�˴��������뾡����С�����ٷ��أ���������
	static BYTE* OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);
	//������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	void InsertMapLinkDB(QString key, GraphicsItem *item);
	void InitDrawobj();

private:
	Ui::view_plugin_drawer ui;

	CMdbClient *m_pMdbTrgClient;
	DrawView *m_pView;
	DrawScene *m_pScene;
	MDBThread *m_mdbThred;

public:
	QMutex m_queMutex;
	QQueue<stuOeElementStateQueue> m_qOeElementState;
	QMap<QString, QList<GraphicsItem *>*> m_mapLinkDB;

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

	DrawView* CreateView();
	bool Load(char *content);
	bool Load(QString fileName);

private slots:
	void SlotItemSelected();
	void SlotPositionChanged(int,int);
	void SlotMouseRightButton(QPoint);
	
};

#endif // __VIEW_PLUGIN_DRAWER_H__

