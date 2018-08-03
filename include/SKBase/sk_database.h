#ifndef __SK_DATABASE_H__
#define __SK_DATABASE_H__

#include "SApi.h"
#include "SDatabase.h"
#include "SDatabaseOper.h"
#include "SMdb.h"
#include "sk_base_inc.h"

#define SK_DATABASE CSKDatabase::GetPtr()
#define DB SK_DATABASE->GetHisDbOper()
#define MDB SK_DATABASE->GetMdbOper()

typedef e_SDatabase_Type eSsp_Database_Type;

class SK_BASE_EXPORT CSKDatabase
{
public:
	CSKDatabase();
	virtual ~CSKDatabase();

	static CSKDatabase* GetPtr();
	void SetPoolSize(int poolsize) { m_iPoolSize = poolsize; }
	bool Load(SString sPathFile);
	SString GetMasterConnectString();
	SString GetSlaveConnectString();
	SString GetMdbConnectString();
	inline SDatabaseOper* GetHisDbOper() { return &m_HisDbOper; }
	inline SDatabaseOper* GetMdbOper() { return &m_MdbOper; };
	inline eSsp_Database_Type GetMasterType() { return m_MasterDbType; }
	inline eSsp_Database_Type GetSlaveType() { return  m_SlaveDbType; }
	void RemoveAllMdbTrigger();

private:
	bool	m_bMaster;
	SString m_sMasterType;
	eSsp_Database_Type m_MasterDbType;
	SString m_sMasterHostAddr;
	int		m_iMasterPort;
	SString m_sMasterUser;
	SString m_sMasterPassword;
	SString m_sMasterDbName;

	bool	m_bSlave;
	SString m_sSlaveType;
	eSsp_Database_Type m_SlaveDbType;
	SString m_sSlaveHostAddr;
	int		m_iSlavePort;
	SString m_sSlaveUser;
	SString m_sSlavePassword;
	SString m_sSlaveDbName;

	bool	m_bMdb;
	SString m_sMdbHostAddr;
	int		m_iMdbPort;
	SString m_sMdbUser;
	SString m_sMdbPassword;
	SString m_sMdbDbName;

	SDatabaseOper m_HisDbOper;						 //��ʷ���ݿ������
	SDatabaseOper m_MdbOper;						 //�ڴ����ݿ������
	SDatabasePool<SDatabase> *m_pHisMasterDbPools;	 //����ʷ���ݿ����ӳ�
	SDatabasePool<SDatabase> *m_pHisSlaveDbPools;	 //����ʷ���ݿ����ӳ�
	SDatabasePool<SDatabase> *m_pMdbDbPools;		 //�ڴ����ݿ����ӳ�
	SDatabasePool<SDatabase> *m_pOldHisMasterDbPools;//��ɾ���Ĺ�������ʷ���ݿ����ӳ�
	SDatabasePool<SDatabase> *m_pOldHisSlaveDbPools; //��ɾ���Ĺ��ڱ���ʷ���ݿ����ӳ�
	SDatabasePool<SDatabase> *m_pOldMdbDbPools;		 //��ɾ���Ĺ����ڴ����ݿ����ӳ�
	int m_iPoolSize;								 //���ӳش�С

private:
	static void* ThreadDelayFree(void *lp);
};

#endif//__SK_DATABASE_H__
