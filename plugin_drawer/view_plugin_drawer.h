/**
 *
 * 文 件 名 : view_plugin_drawer.h
 * 创建日期 : 2018-08-11 09:03
 * 修改日期 : $Date: $
 * 功能描述 : 自画图形展示插件
 * 修改记录 : 
 *
 **/

#ifndef __VIEW_PLUGIN_DRAWER_H__
#define __VIEW_PLUGIN_DRAWER_H__

#include "cbaseview.h"
#include "sk_database.h"
#include "SMdb.h"
#include "ui_view_plugin_drawer.h"
#include "csvgview.h"
#include "mdb_def.h"

#define LINKDB_NONE			0
#define LINKDB_STATE		1
#define LINKDB_MEASURE		2

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

class view_plugin_drawer;
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
	//此处理函数必须尽量短小，快速返回，不可阻塞
	static BYTE* OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);
	//代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

private:
	Ui::view_plugin_drawer ui;

	CMdbClient *m_pMdbTrgClient;
	MDBThread *m_mdbThred;

public:
	QMutex m_queMutex;
	QQueue<stuOeElementStateQueue> m_qOeElementState;
	CSvgView *m_pSvgView;

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

	bool Load(char *content);
	bool Load(QString fileName);
	
};

#endif // __VIEW_PLUGIN_DRAWER_H__

