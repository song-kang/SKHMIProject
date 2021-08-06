/**
 *
 * �� �� �� : MdbApplication.cpp
 * �������� : 2014-4-22 12:00
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �ڴ����ݿ�������
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	�ۿ�������ļ�
 *
 **/

#include "MdbApplication.h"
#include "..\SKLic\slicense.h"

void* CMdbApplication::ThreadLic(void* lp)
{
	CMdbApplication *pThis = (CMdbApplication*)lp;
	pThis->BeginThread();
	while(!pThis->IsQuit())
	{
		SApi::UsSleep(3600000000);
		SLicense lic;
		if(!lic.CheckLicense()) {
			printf("Licsence error\n");
			break;
		}
	}
	pThis->EndThread();
	pThis->ExitByLic();
	return NULL;
}

CMdbApplication::CMdbApplication()
{

}

CMdbApplication::~CMdbApplication()
{

}

CMdbService *g_mdb_server_ptr = NULL;
bool CMdbApplication::Start()
{
	SString sFile = GetConfPath()+"sys_unitconfig.xml";
	if(!SK_UNITCONFIG->Load(sFile))
	{
		LOGFAULT("���ص�Ԫ�����ļ�(%s)ʱʧ��!",sFile.data());
		return false;
	}
	m_iUnitId = SK_UNITCONFIG->GetUnitId();
	m_sUnitName = SK_UNITCONFIG->GetUnitName();
	m_sUnitDesc = SK_UNITCONFIG->GetUnitDesc();
	m_sSoftwareType = SK_UNITCONFIG->GetSystemType();

	static bool first=true;
	if(first)
	{
		first = false;
		SString sParam="";
		if(GetParam("hisdb").length() > 0)
			sParam += "hisdb="+GetParam("hisdb")+";";
		if(IsParam("port"))
			sParam += "port="+GetParam("port")+";";
		if(IsParam("dbname"))
			sParam += "dbname="+GetParam("dbname")+";";
		if(IsParam("dbpwd"))
			sParam += "dbpwd="+GetParam("dbpwd")+";";
		if(IsParam("home_path"))
			sParam += "home_path="+GetParam("home_path")+";";
		if(!m_MdbService.Init(sParam))
		{
			LOGERROR("��ʼ��MDBʧ��!");
		}
		else
		{
			LOGDEBUG("׼�����ز��!Threads=%d",m_MdbService.GetThreadCount());
			g_mdb_server_ptr = &m_MdbService;
			if(m_PluginMgr.LoadPlugin())
			{
				LOGDEBUG("׼�����в��!Threads=%d",m_MdbService.GetThreadCount());
				m_PluginMgr.Do_Mdb_Plugin_Init(Mdb_Run_Sql_Fun,Mdb_Insert_Fun,Mdb_Retrieve_Fun,Mdb_GetTableFields_Fun,(char*)"");
				LOGDEBUG("������н���!Threads=%d",m_MdbService.GetThreadCount());
			}
		}
	}
	
	if(!m_MdbService.StartService())
	{
		return false;
	}
	while(!m_MdbService.IsListened())
		SApi::UsSleep(10000);
	LOGDEBUG("�������!Threads=%d",m_MdbService.GetThreadCount());
	
	SKT_CREATE_THREAD(ThreadLic,this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���г�ʼ��SQL�Ļص�����
// ��    ��:  �ۿ���
// ����ʱ��:  2015-11-24 11:33
// ����˵��:  @sqlΪSQL���
// �� �� ֵ:  bool
//////////////////////////////////////////////////////////////////////////
bool CMdbApplication::Mdb_Run_Sql_Fun(char* sql)
{
	SString s = sql;
	CMdbTable *pTable = NULL;
	return g_mdb_server_ptr->GetMdbMgrPtr()->Execute(s,pTable) >= 0;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �ڴ�����ص�����
// ��    ��:  �ۿ���
// ����ʱ��:  2015-11-24 13:13
// ����˵��:  
// �� �� ֵ:  int
//////////////////////////////////////////////////////////////////////////
int CMdbApplication::Mdb_Insert_Fun(char* sTableName, void *pRowData, int iRowSize, int iRows)
{
	CMdbTable *pTable = NULL;
	if(g_mdb_server_ptr->GetMdbMgrPtr()->IsHisDb())
	{
		//����ʷ�⣬��Ҫ������Ϊ��ʷ��¼������һ��
		BYTE *pBuf = new BYTE[iRowSize*iRows];
		memcpy(pBuf,pRowData,iRowSize*iRows);
		int ret = g_mdb_server_ptr->GetMdbMgrPtr()->Insert(sTableName,pBuf,iRowSize,iRows,pTable);
		if(pBuf != NULL)
			delete[] pBuf;
		return ret;
	}
	return g_mdb_server_ptr->GetMdbMgrPtr()->Insert(sTableName,(BYTE* &)pRowData,iRowSize,iRows,pTable);
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �ڴ���ѯ�ص�����
// ��    ��:  �ۿ���
// ����ʱ��:  2015-12-14 10:55
// ����˵��:  @sqlΪSQL���
//         :  @ppRecordsetΪCMdbTable*���͵�ָ��,��ΪNULLʱ������Ӧͨ��delete[]�ͷ�
//         :  @piRowSizeΪÿ�м�¼�Ĵ�С���ֽڣ�
//         :  @piRowsΪ���ص�����
//         :  @ppsRowResult��ʾ�ۺ�����µĽ���ַ�������ΪNULLʱ������Ӧͨ��delete[]�ͷ�
// �� �� ֵ:  int
//////////////////////////////////////////////////////////////////////////
int CMdbApplication::Mdb_Retrieve_Fun(char* sql, unsigned char** ppRecordset, int *piRowSize, int *piRows, char** ppsRowResult)
{
	SString s = sql;
	CMdbTable *pTable = NULL;
	int ret;
	SPtrList<CMdbRecord> Result;
	SString slResult;
	ret = g_mdb_server_ptr->GetMdbMgrPtr()->Retrieve(s,pTable, Result, &slResult);
	LOGDEBUG("retrieve %s: ret=%d",s.data(),ret);
	int rowsize = pTable==NULL?0:pTable->GetRowSize();
	*piRowSize = rowsize;
	*ppRecordset = NULL;
	*ppsRowResult = NULL;
	if(ret < 0 || pTable == NULL)
	{
		return ret;
	}
	if (slResult.length() > 0)
	{
		ret = 1;
		*ppsRowResult = new char[slResult.length()+1];
		memset(*ppsRowResult,0,slResult.length()+1);
	}
	else if (ret <= 0 || Result.count() == 0)
	{
		//error or empty recordset
	}
	else
	{
		*piRowSize = rowsize;
		*piRows = ret;
		*ppRecordset = new BYTE[ret*rowsize];
		BYTE *pBuf = (BYTE *)*ppRecordset;
		unsigned long pos;
		CMdbRecord *pRec = Result.FetchFirst(pos);
		while(pRec)
		{
			memcpy(pBuf,pRec->m_pRecordData,rowsize);
			pBuf += rowsize;
			pRec = Result.FetchNext(pos);
		}
		//����Ԥ��ķ��ͻ��������������ݰ�������ڴ����
		Result.clear();
	}
	if(pTable != NULL)
		pTable->unlock();
	return ret;

}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ���ֶ���Ϣ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-3-23 23:19
// ����˵��:  @plFields
// �� �� ֵ:  �ֶ�����
//////////////////////////////////////////////////////////////////////////
int CMdbApplication::Mdb_GetTableFields_Fun(char* tablename,SPtrList<stuTableField> *plFields)
{
	plFields->clear();
	plFields->setAutoDelete(true);
	CMdbTable *pTable = g_mdb_server_ptr->GetMdbMgrPtr()->SearchTableByName(tablename);
	if(pTable == NULL)
		return -1;
	stuTableField *fld;
	unsigned long pos;
	CMdbField *pF = pTable->GetFieldPtr()->FetchFirst(pos);
	while (pF)
	{
		fld = new stuTableField();
		strcpy(fld->name, pF->m_sName.left(sizeof(fld->name) - 1).data());
		fld->type		= pF->m_Type;
		fld->start_pos	= pF->m_iStartPos;
		fld->bytes		= pF->m_iBytes;
		fld->is_pk		= pF->m_bIsPK ? 1 : 0;
		fld->is_null	= pF->m_bIsNull ? 1 : 0;
		strcpy(fld->defval, pF->m_sDefaultVal.left(sizeof(fld->defval) - 1).data());
		plFields->append(fld);
		pF = pTable->GetFieldPtr()->FetchNext(pos);
	}
	return plFields->count();
}

bool CMdbApplication::Stop()
{
	LOGDEBUG("CMdbApplication::Stop");
	if(!m_MdbService.StopService())
	{
		return false;
	}
	SLog::quitLog();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �����ı�����
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-13 15:12
// ����˵��:  @sCmdΪ�������ݣ����ַ�����ʾȡ�������б�
//         :  @sResultΪ���ؽ�������ı�
// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
//////////////////////////////////////////////////////////////////////////
bool CMdbApplication::ProcessTxtCmd(SString &sCmd,SString &sResult)
{
	SString sText;
	int i;
	if(sCmd == "" || sCmd == " ")
	{
		sResult = "cmd                - help\r\n"
				  "cmd info           - client information\r\n"
				  "cmd units          - unit information\r\n"
				  "cmd procs          - process infomation\r\n";
	}
	if(sCmd == "info")
	{
		sResult += SString::toFormat("��%d�����ݿ�ͻ���������!\r\n", m_MdbService.GetSessionCount());
		for (i = 0; i < m_MdbService.GetSessionCount(); i++)
		{
			STcpServerSession *p = m_MdbService.GetSession(i);
			if (p == NULL)
				continue;
			sResult += SString::toFormat("    ��%d������:%s:%d -> MDB\r\n", i + 1, p->GetSessionSock()->GetPeerIp().data(), p->GetSessionSock()->GetPeerPort());
		}
		sResult += m_MdbService.GetMdbMgrPtr()->GetMdbMgrInfo();
	}
	else if(sCmd == "units")
	{
		SPtrList<stuSpUnitAgentUnitInfo> units;
		GetUnitInfo(units);
		sResult = "";
		for(int i=0;i<units.count();i++)
		{
			stuSpUnitAgentUnitInfo *p = units[i];
			sResult += SString::toFormat("ID:%d  Ip:%s  type:%s   name:%s  desc:%s \r\n",
				p->m_iUnitId,SSocket::Ipv4ToIpStr(p->m_iUnitIp[0]).data(),p->m_sSoftType,p->m_sUnitName,p->m_sUnitDesc);
		}
	}
	else if(sCmd == "procs")
	{
		SPtrList<stuSpUnitAgentProcInfo> procs;
		GetProcessInfo(2, procs);
		sResult = "";
		for(int i=0;i<procs.count();i++)
		{
			sResult += SString::toFormat("%d, %s - %s \r\n",procs[i]->pid, procs[i]->name,procs[i]->desc);
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 15:20
// ����˵��:  @wMsgType��ʾ��Ϣ����
//         :  @pMsgHeadΪ��Ϣͷ
//         :  @sHeadStr��Ϣͷ�ַ���
//         :  @pBufferΪ��ϢЯ���Ķ������������ݣ�NULL��ʾ�޶���������
//         :  @iLengthΪ���������ݳ���
// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ�ܻ�δ����
//////////////////////////////////////////////////////////////////////////
bool CMdbApplication::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return false;
}