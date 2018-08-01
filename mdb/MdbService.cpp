/**
 *
 * �� �� �� : MdbService.cpp
 * �������� : 2014-4-22 13:43
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �ڴ����ݿ����˳���
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	�ۿ�������ļ�
 *
 **/

#include "MdbService.h"

//////////////////////////////////////////////////////////////////////////
// ��    ��:  CMdbService
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:24 15:51
// ��    ��:  �ڴ����ݿ����˳���
//////////////////////////////////////////////////////////////////////////

CMdbService::CMdbService()
{
	m_iPort = 1400;
	m_sDbUser = "skt_mdb";
	m_sDbPwd = "skt001@163.com";
	SetStartWord(0x518C);
}

CMdbService::~CMdbService()
{

}

////////////////////////////////////////////////////////////////////////
// ��    ��:  �����ʼ��
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:24 16:02
// ����˵��:  @sParamsΪ�������Դ�
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbService::Init(SString sParams)
{
	if(SString::IsAttribute(sParams,"port"))
	{
		m_iPort = SString::GetAttributeValueI(sParams,"port");
		if(m_iPort == 0)
			m_iPort = 1400;
	}
	if(SString::IsAttribute(sParams,"dbname"))
	{
		m_sDbUser = SString::GetAttributeValue(sParams,"dbname");
	}
	if(SString::IsAttribute(sParams,"dbpwd"))
	{
		m_sDbPwd = SString::GetAttributeValue(sParams,"dbpwd");
	}
	SString path = SString::GetAttributeValue(sParams, "home_path");
	if (path.length() == 0)
		path = SDir::currentDirPath();
	if (path.at(path.length()-1) == '\\' || path.at(path.length()-1) == '/')
		path = path.left(path.length()-1);
	if(SString::GetAttributeValue(sParams,"hisdb") == "true")
		m_MdbMgr.SetHisDbEnable(true);
	else
		m_MdbMgr.SetHisDbEnable(false);
	m_MdbMgr.SetMdbHomePath(path);
	if(m_MdbMgr.Init(sParams.data()) != MDB_RET_SUCCESS)
	{
		LOGERROR("��ʼ���ڴ����ݿ�ʱʧ��!%s",m_MdbMgr.GetLastError().data());
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ��������
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:24 15:51
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbService::StartService()
{
	LOGDEBUG("CMdbService::StartService");
	if(!STcpServerBase::Start(m_iPort))
		return false;
	while(!IsListened())
	{
		SApi::UsSleep(100000);
	}
	LOGDEBUG("�ڶ˿�(%d)����MDB����ɹ�!",m_iPort);
	return true;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ֹͣ����
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:24 15:52
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbService::StopService()
{
	if(!STcpServerBase::Stop())
		return false;
	m_MdbMgr.Quit();
	LOGDEBUG("ֹͣMDB����ɹ�!");
	return true;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ��¼�ص��麯��
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:24 16:13
// ����˵��:  @ip��¼�ͻ��˵�IP
//            @port��¼�ͻ��˵Ķ˿�
//            @sLoginHead��¼�ַ���
// �� �� ֵ:  true��ʾ�����¼,false��ʾ�ܾ���¼
//////////////////////////////////////////////////////////////////////////
bool CMdbService::OnLogin(SString ip,int port,SString &sLoginHead)
{
	SString dbuser = SString::GetAttributeValue(sLoginHead,"dbuser");
	SString dbpwd = SString::GetAttributeValue(sLoginHead,"dbpwd");
	if(m_sDbUser == dbuser && m_sDbPwd == dbpwd)
		return true;
	LOGDEBUG("�ܾ�����(%s:%d)���û���:%s������:%s",ip.data(),port,dbuser.data(),dbpwd.data());
	return false;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  �Ự�߳̽��յ����ĺ�Ļص��麯����������ͨ���˺�������ʱ��Ϣ
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:24 16:23
// ����˵��:  @pSessionΪ�Ựʵ��ָ�룬����ͨ��ָ����Զ˷�������
//            @pPackageΪ�ս��յ������ݰ�
// �� �� ֵ:  true��ʾ�Ѿ�������ϣ��˱��Ŀ����ͷ��ˣ�false��ʾδ�����˱���Ӧ������ն���
//////////////////////////////////////////////////////////////////////////
bool CMdbService::OnRecvFrame(STcpServerSession *pSession,stuSTcpPackage *pPackage)
{
	int ret;
	SString sSendHead;
	CMdbTable* pTable=NULL;
	switch(pPackage->m_wFrameType)
	{
	case MDB_FT_INSERT:
		{
			//table=%s;rowsize=%d;rows=%d;
			SString table = SString::GetAttributeValue(pPackage->m_sHead,"table");
			int rowsize = SString::GetAttributeValueI(pPackage->m_sHead,"rowsize");
			int rows = SString::GetAttributeValueI(pPackage->m_sHead,"rows");
			//LOGDEBUG("insert into %s (rows=%d)", table.data(), rows);
			ret = m_MdbMgr.Insert(table.data(),pPackage->m_pAsduBuffer,rowsize,rows,pTable);
			//LOGDEBUG("insert %s: ret=%d",table.data(),ret);
			if (ret < 0)
				sSendHead = m_MdbMgr.GetLastError();
			else if (pTable != NULL)
				SendTrigger(pTable);
			SKT_SWAP_DWORD(ret);
			pSession->SendFrame(sSendHead,MDB_FT_INSERT,(BYTE*)&ret,sizeof(ret));			
		}
		return true;
	case MDB_FT_UPDATE_INSERT:
		{
			//table=%s;rowsize=%d;rows=%d;
			SString table = SString::GetAttributeValue(pPackage->m_sHead,"table");
			int rowsize = SString::GetAttributeValueI(pPackage->m_sHead,"rowsize");
			int rows = SString::GetAttributeValueI(pPackage->m_sHead,"rows");
			//LOGDEBUG("insert into %s (rows=%d)", table.data(), rows);
			ret = m_MdbMgr.UpdateWithInsert(table.data(),pPackage->m_pAsduBuffer,rowsize,rows,pTable);
			//LOGDEBUG("update/insert %s: ret=%d",table.data(),ret);
			if (ret < 0)
				sSendHead = m_MdbMgr.GetLastError();
			else if (pTable != NULL)
				SendTrigger(pTable);
			SKT_SWAP_DWORD(ret);
			pSession->SendFrame(sSendHead,MDB_FT_UPDATE_INSERT,(BYTE*)&ret,sizeof(ret));			
		}
		return true;
	case MDB_FT_EXECUTE:
		//LOGDEBUG("execute %s: ret=%d",pPackage->m_sHead.left(1000).data(),ret);
		{
			char *pSql = pPackage->m_pAsduBuffer==NULL?pPackage->m_sHead.data():(char*)pPackage->m_pAsduBuffer;
			char *pEnd;
			SString sql;
			int allret = 0;
			while(1)
			{
				pEnd = strstr(pSql,"{$SQL_SEP$}");
				if(pEnd != NULL)
					*pEnd = '\0';
				sql = pSql;
				ret = m_MdbMgr.Execute(sql, pTable);
				if(ret < 0 || pEnd == NULL)
				{
					if(ret < 0)
						allret = ret;
					break;
				}
				allret += ret;
				pSql = pEnd+11;
			}
			ret = allret;
			//SString sql = pPackage->m_pAsduBuffer==NULL?pPackage->m_sHead.data():(char*)pPackage->m_pAsduBuffer;
			//ret = m_MdbMgr.Execute(sql, pTable);
		}
		//LOGDEBUG("Execute=%d, %s",ret,pPackage->m_sHead.data());
		if(ret < 0)
			sSendHead = m_MdbMgr.GetLastError();
		else if(pTable != NULL)
			SendTrigger(pTable);
		SKT_SWAP_DWORD(ret);
		pSession->SendFrame(sSendHead,MDB_FT_EXECUTE,(BYTE*)&ret,sizeof(ret));
		return true;
	case MDB_FT_SELECT:
		{
			int ret_val[2];
			CMdbTable* pTable = NULL;
			SPtrList<CMdbRecord> Result;
			SString slResult;
			pPackage->m_sHead.replace("\r\n"," ");
			pPackage->m_sHead.replace("\n"," ");
			pPackage->m_sHead.replace("\t"," ");
			ret = m_MdbMgr.Retrieve(pPackage->m_sHead, pTable, Result, &slResult);
			//LOGDEBUG("retrieve %s: ret=%d",pPackage->m_sHead.left(1000).data(),ret);
			int rowsize = pTable==NULL?0:pTable->GetRowSize();
			ret_val[0] = ret;
			ret_val[1] = rowsize;
			SKT_SWAP_DWORD(ret_val[0]);
			SKT_SWAP_DWORD(ret_val[1]);
			if(ret < 0 || pTable == NULL)
			{
				sSendHead = m_MdbMgr.GetLastError();
			}
			if (slResult.length() > 0)
			{
				ret_val[0] = 1;
				pSession->SendFrame(slResult, MDB_FT_SELECT, (BYTE*)ret_val, sizeof(ret_val));
			}
			else if (ret <= 0 || Result.count() == 0)
			{
				//error or empty recordset
				pSession->SendFrame(sSendHead,MDB_FT_SELECT,(BYTE*)ret_val,sizeof(ret_val));
			}
			else
			{
				pSession->SetSendBufferSize(sSendHead.length(),sizeof(ret_val)+ret*rowsize);
				BYTE *pBuf = pSession->GetAsduOffsetInBuffer(sSendHead.length());
				memcpy(pBuf,ret_val,sizeof(ret_val));
				pBuf += sizeof(ret_val);
				unsigned long pos;
				CMdbRecord *pRec = Result.FetchFirst(pos);
				while(pRec)
				{
					memcpy(pBuf,pRec->m_pRecordData,rowsize);
					pBuf += rowsize;
					pRec = Result.FetchNext(pos);
				}
				//����Ԥ��ķ��ͻ��������������ݰ�������ڴ����
				pSession->SendFrame(sSendHead,MDB_FT_SELECT,NULL,sizeof(ret_val)+ret*rowsize);
				Result.clear();
			}
			if(pTable != NULL)
				pTable->unlock();
		}
		return true;
	case MDB_FT_UPDATE_BLOB:
		{
			SString table = SString::GetAttributeValue(pPackage->m_sHead, "table");
			SString field = SString::GetAttributeValue(pPackage->m_sHead, "field");
			SString where = SString::GetAttributeValue(pPackage->m_sHead, "where");
			int ret = m_MdbMgr.UpdateBlob(pPackage->m_pAsduBuffer, pPackage->m_iAsduLen, table, field, where);
			//LOGDEBUG("update_blob %s: ret=%d",table.data(),ret);
			sSendHead = m_MdbMgr.GetLastError();
			SKT_SWAP_DWORD(ret);
			pSession->SendFrame(sSendHead, MDB_FT_UPDATE_BLOB,(BYTE*)&ret,sizeof(ret));
		}
		return true;
	case MDB_FT_READ_BLOB:
		{
			SString table = SString::GetAttributeValue(pPackage->m_sHead, "table");
			SString field = SString::GetAttributeValue(pPackage->m_sHead, "field");
			SString where = SString::GetAttributeValue(pPackage->m_sHead, "where");
			BYTE *pBuf = NULL;
			int iBufLen = 0;
			int ret = m_MdbMgr.ReadBlob(pBuf, iBufLen, table, field, where);
			//LOGDEBUG("read_blob %s: ret=%d",table.data(),ret);
			sSendHead.sprintf("ret=%d;size=%d;", ret, iBufLen);
			pSession->SendFrame(sSendHead, MDB_FT_READ_BLOB, (BYTE*)pBuf, iBufLen);
			if (pBuf != NULL)
				delete[] pBuf;
		}
		return true;
	case MDB_FT_CONNTRG://ע�ᴥ����
		{
			SString table = SString::GetAttributeValue(pPackage->m_sHead, "table");
			SString action = SString::GetAttributeValue(pPackage->m_sHead, "action");//insert/update/delete/truncate
			bool ret = this->m_MdbMgr.ConnectTrigger(pSession, table, action);
			LOGDEBUG("connect_trigger %s: ret=%d",table.data(),ret);
		}
		return true;
	case MDB_FT_DISCONNTRG://�Ͽ�������
		{
			SString table = SString::GetAttributeValue(pPackage->m_sHead, "table");
			SString action = SString::GetAttributeValue(pPackage->m_sHead, "action");//insert/update/delete/truncate
			bool ret = this->m_MdbMgr.RemoveTrigger((void*)pSession, table, action);
			LOGDEBUG("disconnect_trigger %s: ret=%d",table.data(),ret);
		}

		return true;
	case MDB_FT_FIELDS://ȡ�ֶ���Ϣ
		{
			int res = 0;
			SString head;
			CMdbTable *pTable = m_MdbMgr.SearchTableByName(pPackage->m_sHead.data());
			if (pTable == NULL)
			{
				res = MDB_RET_TABLE_NOTEXIST;
			}
			else
			{
				unsigned long pos;
				res = pTable->GetFieldPtr()->count();
				CMdbField *pF = pTable->GetFieldPtr()->FetchFirst(pos);
				while (pF)
				{
					head += SString::toFormat("%s,%d,%d,%d",
						pF->m_sName.data(), pF->m_Type, pF->m_iStartPos, pF->m_iBytes);
					pF = pTable->GetFieldPtr()->FetchNext(pos);
					if (pF != NULL)
						head += ";";
				}
			}
			SKT_SWAP_DWORD(ret);
			pSession->SendFrame(head, MDB_FT_FIELDS, (BYTE*)&res, sizeof(res));
			//LOGDEBUG("get_fields %s: %s",pPackage->m_sHead.data(),pTable==NULL?"Error":"Ok");
		}
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ɾ���Ự�Ļص��麯��
// ��    ��:  �ۿ���
// ����ʱ��:  2014-10-17 14:50
// ����˵��:  @pSessionΪ������ɾ���ĻỰʵ��
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CMdbService::OnDeleteSession(STcpServerSession *pSession)
{
	m_MdbMgr.ClearSessionTrigger((void*)pSession);
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���ʹ�����Ϣ
// ��    ��:  �ۿ���
// ����ʱ��:  2014-10-17 14:56
// ����˵��:  @pTableΪָ���ı�
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CMdbService::SendTrigger(CMdbTable *pTable)
{
	unsigned long pos=0,pos2=0;
	STcpServerSession *pSession;
	CTableTrigger *pTrg;
	stuSTcpPackage *pPackage;
	SString sHead;
	//insert trigger
	m_TrgLock.lock();
	if (pTable->m_InsertTrigger.count() > 0)
	{
		pTrg = pTable->m_InsertTrigger.FetchFirst(pos);
		sHead.sprintf("table=%s;rowsize=%d;trgrows=%d;act=1;", pTable->GetTableName().data(), pTable->GetRowSize(), pTrg->m_iTrgRows);
		while (pTrg)
		{
			pSession = GetSessionPtr()->FetchFirst(pos2);
			while (pSession)
			{
				if (m_MdbMgr.IsSessionTrgConnected(pSession,pTable,1))
				{
					pPackage = pSession->NewPackage(sHead.data(),MDB_FT_TRIGGER, 
						pTrg->m_pTrgRowsData,pTrg->m_iTrgRows*pTable->GetRowSize());
					pSession->AddToSend(pPackage);
				}
				pSession = GetSessionPtr()->FetchNext(pos2);
			}
		
			pTrg = pTable->m_InsertTrigger.FetchNext(pos);
			pTable->m_InsertTrigger.remove(0);
		}
	}
	
	//update trigger
	if (pTable->m_UpdateTrigger.count() > 0)
	{
		pTrg = pTable->m_UpdateTrigger.FetchFirst(pos);
		sHead.sprintf("table=%s;rowsize=%d;trgrows=%d;act=2;", pTable->GetTableName().data(), pTable->GetRowSize(), pTrg->m_iTrgRows);
		while (pTrg)
		{
			GetSessionPtr()->lock();
			pSession = GetSessionPtr()->FetchFirst(pos2);
			while (pSession)
			{
				if (m_MdbMgr.IsSessionTrgConnected(pSession, pTable, 2))
				{
					pPackage = pSession->NewPackage(sHead.data(), MDB_FT_TRIGGER,
						pTrg->m_pTrgRowsData, pTrg->m_iTrgRows*pTable->GetRowSize());
					pSession->AddToSend(pPackage);
				}
				pSession = GetSessionPtr()->FetchNext(pos2);
			}
			GetSessionPtr()->unlock();
			pTrg = pTable->m_UpdateTrigger.FetchNext(pos);
			pTable->m_UpdateTrigger.remove(0);
		}
	}

	//delete trigger
	if (pTable->m_DeleteTrigger.count() > 0)
	{
		pTrg = pTable->m_DeleteTrigger.FetchFirst(pos);
		sHead.sprintf("table=%s;rowsize=%d;trgrows=%d;act=3;", pTable->GetTableName().data(), pTable->GetRowSize(), pTrg->m_iTrgRows);
		while (pTrg)
		{
			GetSessionPtr()->lock();
			pSession = GetSessionPtr()->FetchFirst(pos2);
			while (pSession)
			{
				if (m_MdbMgr.IsSessionTrgConnected(pSession, pTable, 3))
				{
					pPackage = pSession->NewPackage(sHead.data(), MDB_FT_TRIGGER,
						pTrg->m_pTrgRowsData, pTrg->m_iTrgRows*pTable->GetRowSize());
					pSession->AddToSend(pPackage);
				}
				pSession = GetSessionPtr()->FetchNext(pos2);
			}
			GetSessionPtr()->unlock();
			pTrg = pTable->m_DeleteTrigger.FetchNext(pos);
			pTable->m_DeleteTrigger.remove(0);
		}
	}

	//truncate trigger
	if (pTable->m_TruncateTrigger.count() > 0)
	{
		pTrg = pTable->m_TruncateTrigger.FetchFirst(pos);
		sHead.sprintf("table=%s;rowsize=%d;trgrows=%d;act=4;", pTable->GetTableName().data(), pTable->GetRowSize(), pTrg->m_iTrgRows);
		while (pTrg)
		{
			GetSessionPtr()->lock();
			pSession = GetSessionPtr()->FetchFirst(pos2);
			while (pSession)
			{
				if (m_MdbMgr.IsSessionTrgConnected(pSession, pTable, 4))
				{
					pPackage = pSession->NewPackage(sHead.data(), MDB_FT_TRIGGER,
						pTrg->m_pTrgRowsData, pTrg->m_iTrgRows*pTable->GetRowSize());
					pSession->AddToSend(pPackage);
				}
				pSession = GetSessionPtr()->FetchNext(pos2);
			}
			GetSessionPtr()->unlock();
			pTrg = pTable->m_TruncateTrigger.FetchNext(pos);
			pTable->m_TruncateTrigger.remove(0);
		}
	}
	m_TrgLock.unlock();
}

