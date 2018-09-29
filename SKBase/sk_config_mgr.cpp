#include "sk_config_mgr.h"

static CConfigMgr* g_pConfigMgr = NULL;

//==================== CConfigBase =======================
CConfigBase::CConfigBase()
{
	m_ConfigType = SPCFG_UNKNOWN;
	m_bAutoRelaod = false;
	if(g_pConfigMgr == NULL)
		g_pConfigMgr = new CConfigMgr();
	g_pConfigMgr->AddConfig(this);
}

CConfigBase::~CConfigBase()
{
	if(g_pConfigMgr != NULL)
		g_pConfigMgr->RemoveConfig(this);
}

//==================== CConfigMgr =======================
CConfigMgr::CConfigMgr()
{
	m_bQuit = true;
	m_iReloadSecs = 5;
	m_Configs.setAutoDelete(false);
	m_RemovedConfigs.setAutoDelete(true);
	m_ppConfig = new CConfigBase*[SPCFG_MAX_SIZE];//最多支持100个配置
	memset(m_ppConfig,0,sizeof(CConfigBase*)*SPCFG_MAX_SIZE);
}

CConfigMgr::~CConfigMgr()
{
	delete[] m_ppConfig;
	m_Configs.clear();
	m_RemovedConfigs.clear();
}

CConfigMgr* CConfigMgr::GetPtr()
{
	return g_pConfigMgr;
}

void CConfigMgr::SetPtr(CConfigMgr *ptr)
{
	g_pConfigMgr = ptr;
}

void CConfigMgr::SetReloadSeconds(int iSec)
{
	g_pConfigMgr->m_iReloadSecs = iSec;
};

void CConfigMgr::StartReload()
{
	if(!g_pConfigMgr->m_bQuit)
		return;
	g_pConfigMgr->m_bQuit = false;
	S_CREATE_THREAD(ThreadReload,g_pConfigMgr);
}

void CConfigMgr::Quit()
{
	if(g_pConfigMgr != NULL)
	{
		if(g_pConfigMgr->m_bQuit == false)
		{
			g_pConfigMgr->m_bQuit = true;
			while(g_pConfigMgr->m_Configs.count() > 0)
				SApi::UsSleep(1000);
		}
		else
		{
			SApi::UsSleep(100000);
			g_pConfigMgr->m_Configs.clear();
		}
		delete g_pConfigMgr;
		g_pConfigMgr = NULL;
	}
}

bool CConfigMgr::AddConfig(CConfigBase *pCfg)
{
	g_pConfigMgr->m_Configs.append(pCfg);
	return true;
}

void CConfigMgr::RemoveConfig(CConfigBase *pCfg,bool bAddtoRemovedList/*=false*/)
{
	g_pConfigMgr->m_Configs.remove(pCfg);
	if(bAddtoRemovedList)
		g_pConfigMgr->m_RemovedConfigs.append(pCfg);
}

CConfigBase* CConfigMgr::GetConfig(eSpConfigType cfgType)
{
	unsigned long pos=0;
	CConfigBase *p ;
	if(cfgType <= 0 || cfgType >= SPCFG_MAX_SIZE || g_pConfigMgr == NULL)
		return NULL;
	p = g_pConfigMgr->m_ppConfig[(int)cfgType];
	if(p != NULL)
		return p;
	p = g_pConfigMgr->m_Configs.FetchFirst(pos);
	while(p)
	{
		if(p->GetConfigType() == cfgType)
		{
			g_pConfigMgr->m_ppConfig[(int)cfgType] = p;
			return p;
		}
		p = g_pConfigMgr->m_Configs.FetchNext(pos);
	}
	LOGWARN("指定配置文件类型[%d]不存在!",cfgType);
	return NULL;
}

void* CConfigMgr::ThreadReload(void* lp)
{
	CConfigMgr *pThis = (CConfigMgr*)lp;
	unsigned long pos;
	CConfigBase *p;
	bool ret;
	S_INTO_THREAD;
	while(!pThis->m_bQuit)
	{
		p = pThis->m_Configs.FetchFirst(pos);
		while(p)
		{
			if(p->GetAutoReload())
			{
				if(SFile::filetime(p->GetPathFile()) != p->m_LastModifyTime)
				{
					ret = p->ReLoad();
					if(ret == false)
					{
						LOGERROR("重新加载配置文件[%s]时失败!",p->GetPathFile().data());
						p->m_LastModifyTime = SFile::filetime(p->GetPathFile());
					}
					else
					{
						LOGDEBUG("配置文件[%s]变化,将重新加载!",p->GetPathFile().data());
					}
				}
			}
			p = pThis->m_Configs.FetchNext(pos);
		}
		SApi::UsSleep(pThis->m_iReloadSecs*1000000);
	}
	pThis->m_Configs.clear();
	return NULL;
}
