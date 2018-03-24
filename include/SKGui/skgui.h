#ifndef SKGUI_H
#define SKGUI_H

#include "skgui_global.h"
#include "skhead.h"
#include "cbaseview.h"
#include "SApplication.h"

#ifdef _DEBUG
#include <vld.h>
#endif

#define SK_GUI SKGui::GetPtr()

class CPluginMgr;
class SKGUI_EXPORT SKGui : public SApplication
{
public:
	SKGui();
	~SKGui();

	bool Begin();
	bool BeginAgent();
	void End();
	static SKGui* GetPtr();
	QList<CBaseView*>* GetListBaseView() { return m_pListBaseView; }
	QString GetHmiName() { return m_sHmiName.data(); }
	QString GetHmiVersion() { return m_sHmiVersion.data(); }
	CBaseView* NewView(QString sPluginName,QWidget* parent = 0);

public:
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);
	virtual bool Start(){ return true; }
	virtual bool Stop(){ return true; }
	
public:
	CPluginMgr *m_pPluginMgr;

private:
	void *w;
	SString m_sHmiName;
	SString m_sHmiVersion;
	QList<CBaseView*> *m_pListBaseView;

private:
	void Init();

};

#endif // SKGUI_H
