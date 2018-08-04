#include "skgui.h"
#include "cinitwidget.h"
#include "sk_unitconfig.h"
#include "cpluginmgr.h"
#include "cfunpoint.h"
#include "cusers.h"

#define HMI_NAME		"HMI界面主程序"
#define HMI_VERSION		"1.0.0"
#define HMI_DATE		"2014-05-20"	
#define HMI_TIME		"12:36:00"
#define MAX_SKIN		9

SKGui *g_gui=NULL;
SKGui::SKGui()
{
	Init();
}

SKGui::~SKGui()
{
	if (w)
		delete ((SKBaseWidget*)w);

	foreach (CFunPoint *p, m_lstFunPoint)
		delete p;
	foreach (CUsers *u, m_lstUsers)
		delete u;

	if (m_iSettings)
		delete m_iSettings;
}

SKGui* SKGui::GetPtr()
{
	if(g_gui == NULL)
		g_gui = new SKGui();
	return g_gui;
}

void SKGui::Init()
{
	m_iSettings = NULL;
	m_pPluginMgr = new CPluginMgr;
	m_pListBaseView = new QList<CBaseView*>;
	m_sHmiName = HMI_NAME;
	m_sHmiVersion = HMI_VERSION;

	QTime t= QTime::currentTime();
	qsrand(t.msec()+t.second()*1000);
	m_iSkinNo = qrand() % MAX_SKIN;

	m_iLoginOutTime = 0;
}

void SKGui::InitSettings(QString name)
{
	if (m_iSettings)
	{
		delete m_iSettings;
		m_iSettings = NULL;
	}

#ifdef WIN32
	bool isExist = Common::FileExists(QCoreApplication::applicationDirPath()+ "\\..\\conf\\" + name);
	m_iSettings = new QSettings(QCoreApplication::applicationDirPath()+ "\\..\\conf\\" + name, QSettings::IniFormat);
#else
	bool isExist = Common::FileExists(QCoreApplication::applicationDirPath()+ "/../conf/" + name);
	m_iSettings = new QSettings(QCoreApplication::applicationDirPath()+ "/../conf/" + name, QSettings::IniFormat);
#endif

	if (!isExist)
	{
		SetSettingsValue(SG_SETTING,SV_NAME,"Unknown");
		SetSettingsValue(SG_SETTING,SV_LOGINTIME,0);
	}
}

bool SKGui::Begin(char *argv[])
{
	SString sModule = argv[0];
	if(sModule.findRev('/') >= 0)
		sModule = sModule.mid(sModule.findRev('/')+1);
	if(sModule.findRev('\\') >= 0)
		sModule = sModule.mid(sModule.findRev('\\')+1);

	SString err;
	SString sModuleCrc = SApi::GenerateCRCByFile(sModule,err);
	SString sModuleTime;
	sModuleTime.sprintf("%s %s",HMI_DATE,HMI_TIME);

	SetApplicationId(SP_UA_APPNO_USER);
	SString sInfo;
	sInfo.sprintf("name=%s;ver=%s;crc=%s;date=%s;",sModule.data(),m_sHmiVersion.data(),sModuleCrc.data(),sModuleTime.data());
	SetModuleInfo(sInfo);
	SetModuleDesc(m_sHmiName);

	CInitWidget *wiget = new CInitWidget();
	if (wiget)
	{
		w = new SKBaseWidget(NULL,wiget);
		((SKBaseWidget*)w)->SetWindowFixSize(550,320);
		((SKBaseWidget*)w)->SetWindowBackgroundImage(QPixmap(":/images/load"));
		((SKBaseWidget*)w)->HideTopFrame();
		((SKBaseWidget*)w)->Show();
	}
	else
		return false;

	return true;
}

bool SKGui::BeginAgent()
{
#ifdef WIN32
	if (SK_UNITCONFIG->Load(QCoreApplication::applicationDirPath().toStdString()+"\\..\\conf\\sys_unitconfig.xml"))
#else
	if (SK_UNITCONFIG->Load(QCoreApplication::applicationDirPath().toStdString()+"/../conf/sys_unitconfig.xml"))
#endif
	{
		m_iUnitId = SK_UNITCONFIG->GetUnitId();
		m_sUnitName = SK_UNITCONFIG->GetUnitName();
		m_sUnitDesc = SK_UNITCONFIG->GetUnitDesc();
		m_sSoftwareType = SK_UNITCONFIG->GetSystemType();

		StartAgent(false);
		return true;
	}

	return false;
}

void SKGui::End()
{
	while (m_pListBaseView->count())
		delete m_pListBaseView->at(0);
	delete m_pListBaseView;

	m_pPluginMgr->Exit();
	delete m_pPluginMgr;

	CSpAgentClient::Stop();
	StopAndWait(10);
	delete this;
}

bool SKGui::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	for (int i = 0; i < m_pListBaseView->count(); i++)
		m_pListBaseView->at(i)->ProcessAgentMsg(wMsgType,pMsgHead,sHeadStr,pBuffer,iLength);

	return true;
}

CBaseView* SKGui::NewView(QString sPluginName,QWidget* parent)
{
	CBaseView *view = NULL;

	view = m_pPluginMgr->NewView(sPluginName,parent);
	if (view)
		return view;

	return NULL;
}

void SKGui::SetFunPoint(CFunPoint *fpoint)
{
	SString sql;
	SRecordset rs;
	if (fpoint)
		sql.sprintf("select fun_key,name,auth,type from t_ssp_fun_point where p_fun_key='%s' order by idx asc",fpoint->GetKey().toStdString().data());
	else
		sql.sprintf("select fun_key,name,auth,type from t_ssp_fun_point where p_fun_key='%s' order by idx asc","top");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			CFunPoint *p = new CFunPoint(fpoint);
			p->SetKey(rs.GetValue(i,0).data());
			p->SetDesc(rs.GetValue(i,1).data());
			p->SetAuth((bool)rs.GetValue(i,2).toInt());
			p->SetType(rs.GetValue(i,3).toInt());
			
			SString sWhere = SString::toFormat("fun_key='%s'",p->GetKey().toStdString().data());
			DB->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,p->m_pImageBuffer,p->m_iImageLen);

			if (fpoint)
				fpoint->m_lstChilds.append(p);
			else
				m_lstFunPoint.append(p);

			SetFunPoint(p);
		}
	}
}

void SKGui::SetUsersAuth(QString code)
{
	SString sql;
	SRecordset rs,rs1;
	if (code.isEmpty())
		sql.sprintf("select grp_code,name,dsc from t_ssp_user_group");
	else
		sql.sprintf("select grp_code,name,dsc from t_ssp_user_group where grp_code='%s'",code.toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			CUsers *grp = new CUsers();
			grp->SetCode(rs.GetValue(i,0).data());
			grp->SetName(rs.GetValue(i,1).data());
			grp->SetDesc(rs.GetValue(i,2).data());
			sql.sprintf("select fun_key,auth from t_ssp_usergroup_auth where grp_code='%s'",grp->GetCode().toStdString().data());
			int cnt1 = DB->Retrieve(sql,rs1);
			for (int j = 0; j < cnt1; j++)
			{
				stuAuth *auth = new stuAuth;
				auth->fun_key = rs1.GetValue(j,0).data();
				auth->auth = (bool)rs1.GetValue(j,1).toInt();
				grp->m_lstAuth.append(auth);
			}

			SetUserAuth(grp,-1);
			m_lstUsers.append(grp);
		}
	}
}

CUser* SKGui::SetUserAuth(CUsers *grp, int sn)
{
	SString sql;
	SRecordset rs,rs1;
	if (sn == -1)
		sql.sprintf("select usr_sn,usr_code,grp_code,name,pwd,dsc,email,mobile,create_time,login_timeout from t_ssp_user where grp_code='%s'",grp->GetCode().toStdString().data());
	else
		sql.sprintf("select usr_sn,usr_code,grp_code,name,pwd,dsc,email,mobile,create_time,login_timeout from t_ssp_user where grp_code='%s' and usr_sn=%d",grp->GetCode().toStdString().data(),sn);
	int cnt = DB->Retrieve(sql,rs);
	CUser *user = 0;
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			user = new CUser(grp);
			user->SetSn(rs.GetValue(i,0).toInt());
			user->SetCode(rs.GetValue(i,1).data());
			user->SetGrpCode(rs.GetValue(i,2).data());
			user->SetName(rs.GetValue(i,3).data());
			user->SetPassword(rs.GetValue(i,4).data());
			user->SetDesc(rs.GetValue(i,5).data());
			user->SetEmail(rs.GetValue(i,6).data());
			user->SetMobile(rs.GetValue(i,7).data());
			user->SetLoginTime(rs.GetValue(i,8).toUInt());
			user->SetLoginTimeout(rs.GetValue(i,9).toUInt());
			sql.sprintf("select fun_key,auth from t_ssp_user_auth where usr_sn=%d",user->GetSn());
			int cnt1 = DB->Retrieve(sql,rs1);
			for (int j = 0; j < cnt1; j++)
			{
				stuAuth *auth = new stuAuth;
				auth->fun_key = rs1.GetValue(j,0).data();
				auth->auth = (bool)rs1.GetValue(j,1).toInt();
				user->m_lstAuth.append(auth);
			}

			grp->m_lstUser.append(user);
		}
	}

	return user;
}

void SKGui::SetRunPoints(QList<CFunPoint*> lstFunPoint)
{
	foreach (CFunPoint *p, lstFunPoint)
	{
		if (p->m_lstChilds.count() == 0 && p->GetType() != 1)
		{
			m_lstRunFunPoint.append(p);
		}

		SetRunPoints(p->m_lstChilds);
	}
}

void SKGui::SetSettingsValue(const QString &group, const QString &key, const QVariant &value)
{
	m_iSettings->beginGroup(group);
	m_iSettings->setValue(key, value);
	m_iSettings->endGroup();
	m_iSettings->sync();
}

QVariant SKGui::GetSettingsValue(const QString &group, const QString &key, const QVariant &defaultValue)
{
	m_iSettings->beginGroup(group);
	QVariant value = m_iSettings->value(key, defaultValue);
	m_iSettings->endGroup();
	return value;
}

bool SKGui::ContaintsSettingsValue(const QString &group, const QString &key)
{
	m_iSettings->beginGroup(group);
	bool bcontain = m_iSettings->contains(key);
	m_iSettings->endGroup();
	return bcontain;
}

void SKGui::RemoveSettingsValue(const QString &group, const QString &key)
{
	m_iSettings->beginGroup(group);
	m_iSettings->remove(key);
	m_iSettings->endGroup();
	m_iSettings->sync();
}

void SKGui::CheckFunPoint()
{
	SString sql;
	SRecordset rs;

	sql.sprintf("select fun_key from t_ssp_fun_point order by fun_key asc");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			QString key = rs.GetValue(i,0).data();
			if (!IsExistKey(key) && key != "top")
			{
				sql.sprintf("delete from t_ssp_fun_point where fun_key='%s'",key.toStdString().data());
				DB->Execute(sql);
			}
		}
	}
}

void SKGui::CheckUserAuth(QList<CFunPoint*> lstFunPoint)
{
	SString sql;
	foreach (CFunPoint *p, lstFunPoint)
	{
		foreach (CUsers *users, SK_GUI->m_lstUsers)
		{
			if (!users->IsExistKey(p->GetKey()))
			{
				sql.sprintf("insert into t_ssp_usergroup_auth values ('%s','%s',1)",
					users->GetCode().toStdString().data(),p->GetKey().toStdString().data());
				DB->Execute(sql);
			}

			foreach (CUser *user, users->m_lstUser)
			{
				if (!user->IsExistKey(p->GetKey()))
				{
					sql.sprintf("insert into t_ssp_user_auth values (%d,'%s',1)",user->GetSn(),p->GetKey().toStdString().data());
					DB->Execute(sql);
				}
			}
		}

		CheckUserAuth(p->m_lstChilds);
	}
}

void SKGui::DeleteUserAuth()
{
	SString sql;
	foreach (CUsers *users, SK_GUI->m_lstUsers)
	{
		foreach (stuAuth *auth, users->m_lstAuth)
		{
			if (!IsExistKey(auth->fun_key))
			{
				sql.sprintf("delete from t_ssp_usergroup_auth where grp_code='%s' and fun_key='%s'",
					users->GetCode().toStdString().data(),auth->fun_key.toStdString().data());
				DB->Execute(sql);
			}
		}

		foreach (CUser *user, users->m_lstUser)
		{
			foreach (stuAuth *auth, user->m_lstAuth)
			{
				if (!IsExistKey(auth->fun_key))
				{
					sql.sprintf("delete from t_ssp_user_auth where usr_sn=%d and fun_key='%s'",
						user->GetSn(),auth->fun_key.toStdString().data());
					DB->Execute(sql);
				}
			}
		}
	}
}

bool SKGui::IsExistKey(QString key)
{
	bool bFind = false;

	foreach (CFunPoint *p, SK_GUI->m_lstFunPoint)
	{
		if (p->IsExist(key))
			bFind = true;
	}

	return bFind;
}
