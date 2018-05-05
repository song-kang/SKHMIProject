#ifndef SKGUI_H
#define SKGUI_H

#include "skgui_global.h"
#include "skhead.h"
#include "sk_database.h"
#include "cbaseview.h"
#include "SApplication.h"

#ifdef _DEBUG
#include <vld.h>
#endif

#define SG_SETTING		"Setting"
#define SG_FUNPOINT		"FunPoint"

#define SV_NAME			"Name"
#define SV_LOGINTIME	"LoginTime"
#define SV_QUICKPOINT	"QuickPoint"

#define SK_GUI SKGui::GetPtr()

class CPluginMgr;
class CFunPoint;
class CUsers;
class CUser;
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
	void SetFunPoint(CFunPoint *fpoint);
	void SetUsersAuth(QString code);
	CUser* SetUserAuth(CUsers *grp, int sn);
	int GetSkinNo() { return m_iSkinNo; }
	QList<CFunPoint*> GetRunFunPoints() { m_lstRunFunPoint.clear(); SetRunPoints(m_lstFunPoint); return m_lstRunFunPoint; }
	void InitSettings(QString name);
	void SetSettingsValue(const QString &group, const QString &key, const QVariant &value);
	QVariant GetSettingsValue(const QString &group, const QString &key, const QVariant &defaultValue=QVariant());
	bool ContaintsSettingsValue(const QString &group, const QString &key);
	void RemoveSettingsValue(const QString &group, const QString &key);

	void CheckFunPoint();
	void CheckUserAuth(QList<CFunPoint*> lstFunPoint);
	void DeleteUserAuth();
	bool IsExistKey(QString key);

public:
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);
	virtual bool Start(){ return true; }
	virtual bool Stop(){ return true; }

public:
	CPluginMgr *m_pPluginMgr;
	QList<CFunPoint*> m_lstFunPoint;
	QList<CUsers*> m_lstUsers;
	QSettings *m_iSettings;
	quint32 m_iLoginOutTime;

private:
	void *w;
	int m_iSkinNo;
	SString m_sHmiName;
	SString m_sHmiVersion;
	QList<CBaseView*> *m_pListBaseView;
	QList<CFunPoint*> m_lstRunFunPoint;

private:
	void Init();
	void SetRunPoints(QList<CFunPoint*> lstFunPoint);

};

#endif // SKGUI_H
