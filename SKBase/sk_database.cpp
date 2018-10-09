#include "sk_database.h"
#include <math.h>

static CSKDatabase* g_pDatabase = NULL;

CSKDatabase::CSKDatabase()
{
	m_pHisMasterDbPools = NULL;
	m_pHisSlaveDbPools = NULL;
	m_pMdbDbPools = NULL;
	m_iPoolSize = 3;
	m_MasterDbType = m_SlaveDbType = DB_UNKNOWN;
}

CSKDatabase::~CSKDatabase()
{
	if (m_pHisMasterDbPools != NULL)
		delete m_pHisMasterDbPools;
	if (m_pHisSlaveDbPools != NULL)
		delete m_pHisSlaveDbPools;
	if (m_pMdbDbPools != NULL)
		delete m_pMdbDbPools;
}

CSKDatabase* CSKDatabase::GetPtr()
{
	if (g_pDatabase == NULL)
		g_pDatabase = new CSKDatabase();

	return g_pDatabase;
}

#define UKDB_PWD_MASK "yqqlm^gsycl.1978"
bool CSKDatabase::Load(SString sPathFile)
{
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
		return false;

	SString hdb_ip;
	int hdb_port=0;
	SBaseConfig *pCfg = xml.SearchChild("master");
	if (pCfg)
	{
		m_bMaster = true;
		m_sMasterType		= pCfg->GetAttribute("type");//mysql/pgsql/oracle/dameng/mdb
		m_sMasterHostAddr	= pCfg->GetAttribute("hostaddr");
		m_iMasterPort		= pCfg->GetAttributeI("port");
		m_sMasterUser		= pCfg->GetAttribute("user");
		m_sMasterPassword	= SApi::Decrypt_String(pCfg->GetAttribute("password"),UKDB_PWD_MASK);
		m_sMasterDbName		= pCfg->GetAttribute("dbname");
	}
	else
	{
		m_bMaster = false;
	}

	pCfg = xml.SearchChild("slave");
	if (pCfg)
	{
		m_bSlave = true;
		m_sSlaveType		= pCfg->GetAttribute("type");//mysql/pgsql/oracle/dameng/mdb
		m_sSlaveHostAddr	= pCfg->GetAttribute("hostaddr");
		m_iSlavePort		= pCfg->GetAttributeI("port");
		m_sSlaveUser		= pCfg->GetAttribute("user");
		m_sSlavePassword	= SApi::Decrypt_String(pCfg->GetAttribute("password"),UKDB_PWD_MASK);
		m_sSlaveDbName		= pCfg->GetAttribute("dbname");
	}
	else
	{
		m_bSlave = false;
	}

	pCfg = xml.SearchChild("mdb");
	if (pCfg)
	{
		m_bMdb = true;
		m_sMdbHostAddr		= pCfg->GetAttribute("hostaddr");
		m_iMdbPort			= pCfg->GetAttributeI("port");
		m_sMdbUser			= pCfg->GetAttribute("user");
		m_sMdbPassword		= SApi::Decrypt_String(pCfg->GetAttribute("password"),UKDB_PWD_MASK);
		m_sMdbDbName		= pCfg->GetAttribute("dbname");
	}
	else
	{
		m_bMdb = false;
	}

	bool bReload = false;
	if (m_pHisMasterDbPools != NULL)
		bReload = true;

	SDatabasePool<SDatabase> *pHisMaster,*pHisSlave,*pMdb;
	pHisMaster = pHisSlave = pMdb = NULL;
	if (m_bMaster == false)
	{
		LOGFAULT("主数据库未配置!");
		goto err;
	}

#ifdef SSP_DBUSED_MYSQL
	if (m_sMasterType.toLower() == "mysql")
	{
		m_MasterDbType = DB_MYSQL;
		SDatabasePool<SMySQL> *pNewPool = new SDatabasePool<SMySQL>;
		pHisMaster = (SDatabasePool<SDatabase> *) pNewPool;
		pNewPool->SetParams(GetMasterConnectString());
		if (!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pHisMaster = NULL;
			LOGERROR("创建MYSQL主数据库连接池时失败");
			goto err;
		}
	}
#endif
#ifdef SSP_DBUSED_ORACLE
	else if (m_sMasterType.toLower() == "oracle")
	{
		m_MasterDbType = DB_ORACLE;
		SDatabasePool<SOracle> *pNewPool = new SDatabasePool<SOracle>;
		pHisMaster = (SDatabasePool<SDatabase> *) pNewPool;
		pNewPool->SetParams(GetMasterConnectString());
		if (!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pHisMaster = NULL;
			LOGERROR("创建ORACLE主数据库连接池时失败");
			goto err;
		}
	}
#endif
	else
	{
		LOGFAULT("未知的主数据库类型:%s",m_sMasterType.data());
		goto err;
	}	

	if (m_bSlave)
	{
#ifdef SSP_DBUSED_MYSQL
		if (m_sSlaveType.toLower() == "mysql")
		{
			m_SlaveDbType = DB_MYSQL;
			SDatabasePool<SMySQL> *pNewPool = new SDatabasePool<SMySQL>;
			pHisSlave = (SDatabasePool<SDatabase> *) pNewPool;
			pNewPool->SetParams(GetMasterConnectString());
			if(!pNewPool->CreateDatabasePool(m_iPoolSize))
			{
				delete pNewPool;
				pHisSlave = NULL;
				LOGERROR("创建MYSQL备数据库连接池时失败");
				goto err;
			}
		}
#endif
#ifdef SSP_DBUSED_ORACLE
		else if (m_sSlaveType.toLower() == "oracle")
		{
			m_SlaveDbType = DB_ORACLE;
			SDatabasePool<SOracle> *pNewPool = new SDatabasePool<SOracle>;
			pHisSlave = (SDatabasePool<SDatabase> *) pNewPool;
			pNewPool->SetParams(GetMasterConnectString());
			if (!pNewPool->CreateDatabasePool(m_iPoolSize))
			{
				delete pNewPool;
				pHisSlave = NULL;
				LOGERROR("创建ORACLE备数据库连接池时失败");
				goto err;
			}
		}
#endif
		else
		{
			LOGFAULT("未知的备数据库类型:%s",m_sSlaveType.data());
			goto err;
		}
	}

	if (m_bMdb)
	{
		SDatabasePool<SMdb> *pNewPool = new SDatabasePool<SMdb>;
		pMdb = (SDatabasePool<SDatabase> *)pNewPool;		
		pNewPool->SetParams(GetMdbConnectString());
		if (!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pMdb = NULL;
			LOGERROR("创建内存数据库连接池时失败");
			goto err;
		}
	}

	m_HisDbOper.SetDatabasePool(pHisMaster);
	m_HisDbOper.SetSlaveDatabasePool(pHisSlave);
	if(pHisSlave != NULL)
		m_HisDbOper.Start();
	m_MdbOper.SetDatabasePool(pMdb);

	if (bReload)
	{
		m_pOldHisMasterDbPools	= m_pHisMasterDbPools;
		m_pOldHisSlaveDbPools	= m_pHisSlaveDbPools;
		m_pOldMdbDbPools		= m_pMdbDbPools;
		S_CREATE_THREAD(ThreadDelayFree,this);
	}
	m_pHisMasterDbPools = pHisMaster;
	m_pHisSlaveDbPools = pHisSlave;
	m_pMdbDbPools = pMdb;

	return true;

err:
	if (pHisMaster != NULL)
		delete pHisMaster;
	if (pHisSlave != NULL)
		delete pHisSlave;
	if (pMdb != NULL)
		delete pMdb;

	return false;
}

void* CSKDatabase::ThreadDelayFree(void *lp)
{
	CSKDatabase *pThis = (CSKDatabase *)lp;
	S_INTO_THREAD;
	SApi::UsSleep(1000000);//5秒后释放

	if (pThis->m_pOldHisMasterDbPools != NULL)
	{
		delete pThis->m_pOldHisMasterDbPools;
		pThis->m_pOldHisMasterDbPools = NULL;
	}

	if (pThis->m_pOldHisSlaveDbPools	!= NULL)
	{
		delete pThis->m_pOldHisSlaveDbPools;
		pThis->m_pOldHisSlaveDbPools = NULL;
	}

	if (pThis->m_pOldMdbDbPools != NULL)
	{
		delete pThis->m_pOldMdbDbPools;
		pThis->m_pOldMdbDbPools = NULL;
	}

	return NULL;
}

SString CSKDatabase::GetMasterConnectString()
{
	return SString::toFormat("hostaddr=%s;port=%d;user=%s;password=%s;dbname=%s;",
		m_sMasterHostAddr.data(),
		m_iMasterPort,
		m_sMasterUser.data(),
		m_sMasterPassword.data(),
		m_sMasterDbName.data());
}

SString CSKDatabase::GetSlaveConnectString()
{
	return SString::toFormat("hostaddr=%s;port=%d;user=%s;password=%s;dbname=%s;",
		m_sSlaveHostAddr.data(),
		m_iSlavePort,
		m_sSlaveUser.data(),
		m_sSlavePassword.data(),
		m_sSlaveDbName.data());
}

SString CSKDatabase::GetMdbConnectString()
{
	return SString::toFormat("hostaddr=%s;port=%d;user=%s;password=%s;dbname=%s;",
		m_sMdbHostAddr.data(),
		m_iMdbPort,
		m_sMdbUser.data(),
		m_sMdbPassword.data(),
		m_sMdbDbName.data());
}

void CSKDatabase::RemoveAllMdbTrigger()
{
	if (m_pMdbDbPools == NULL)
		return;

	for (int i = 0; i < m_pMdbDbPools->GetPoolSize(); i++)
	{
		SMdb *pDb = (SMdb*)m_pMdbDbPools->GetDatabaseByIdx(i);
		if (pDb != NULL)
			pDb->GetMdbClient()->RemoveAllTrigger();
	}
}

SString CSKDatabase::DecryptPwd(SString text)
{
	return SApi::Decrypt_String(text,UKDB_PWD_MASK);
}

SString CSKDatabase::EncryptPwd(SString text)
{
	return SApi::Encrypt_String(text,UKDB_PWD_MASK);
}
