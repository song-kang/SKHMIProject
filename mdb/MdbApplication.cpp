/**
 *
 * 文 件 名 : MdbApplication.cpp
 * 创建日期 : 2014-4-22 12:00
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 内存数据库服务程序
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	邵凯田　创建文件
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
		LOGFAULT("加载单元配置文件(%s)时失败!",sFile.data());
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
			LOGERROR("初始化MDB失败!");
		}
		else
		{
			LOGDEBUG("准备加载插件!Threads=%d",m_MdbService.GetThreadCount());
			g_mdb_server_ptr = &m_MdbService;
			if(m_PluginMgr.LoadPlugin())
			{
				LOGDEBUG("准备运行插件!Threads=%d",m_MdbService.GetThreadCount());
				m_PluginMgr.Do_Mdb_Plugin_Init(Mdb_Run_Sql_Fun,Mdb_Insert_Fun,Mdb_Retrieve_Fun,Mdb_GetTableFields_Fun,(char*)"");
				LOGDEBUG("插件运行结束!Threads=%d",m_MdbService.GetThreadCount());
			}
		}
	}
	
	if(!m_MdbService.StartService())
	{
		return false;
	}
	while(!m_MdbService.IsListened())
		SApi::UsSleep(10000);
	LOGDEBUG("启动完毕!Threads=%d",m_MdbService.GetThreadCount());
	
	SKT_CREATE_THREAD(ThreadLic,this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  运行初始化SQL的回调函数
// 作    者:  邵凯田
// 创建时间:  2015-11-24 11:33
// 参数说明:  @sql为SQL语句
// 返 回 值:  bool
//////////////////////////////////////////////////////////////////////////
bool CMdbApplication::Mdb_Run_Sql_Fun(char* sql)
{
	SString s = sql;
	CMdbTable *pTable = NULL;
	return g_mdb_server_ptr->GetMdbMgrPtr()->Execute(s,pTable) >= 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  内存库插入回调函数
// 作    者:  邵凯田
// 创建时间:  2015-11-24 13:13
// 参数说明:  
// 返 回 值:  int
//////////////////////////////////////////////////////////////////////////
int CMdbApplication::Mdb_Insert_Fun(char* sTableName, void *pRowData, int iRowSize, int iRows)
{
	CMdbTable *pTable = NULL;
	if(g_mdb_server_ptr->GetMdbMgrPtr()->IsHisDb())
	{
		//有历史库，需要数据作为历史记录，复制一份
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
// 描    述:  内存库查询回调函数
// 作    者:  邵凯田
// 创建时间:  2015-12-14 10:55
// 参数说明:  @sql为SQL语句
//         :  @ppRecordset为CMdbTable*类型的指针,不为NULL时调用者应通过delete[]释放
//         :  @piRowSize为每行记录的大小（字节）
//         :  @piRows为返回的行数
//         :  @ppsRowResult表示聚合情况下的结果字符串，不为NULL时调用者应通过delete[]释放
// 返 回 值:  int
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
		//借助预设的发送缓冲区，避免数据包重组的内存搬移
		Result.clear();
	}
	if(pTable != NULL)
		pTable->unlock();
	return ret;

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取表字段信息
// 作    者:  邵凯田
// 创建时间:  2016-3-23 23:19
// 参数说明:  @plFields
// 返 回 值:  字段数量
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
// 描    述:  处理文本命令
// 作    者:  邵凯田
// 创建时间:  2016-2-13 15:12
// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
//         :  @sResult为返回结果，纯文本
// 返 回 值:  true表示成功，false表示失败
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
		sResult += SString::toFormat("共%d个数据库客户端已连接!\r\n", m_MdbService.GetSessionCount());
		for (i = 0; i < m_MdbService.GetSessionCount(); i++)
		{
			STcpServerSession *p = m_MdbService.GetSession(i);
			if (p == NULL)
				continue;
			sResult += SString::toFormat("    第%d个连接:%s:%d -> MDB\r\n", i + 1, p->GetSessionSock()->GetPeerIp().data(), p->GetSessionSock()->GetPeerPort());
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
// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
// 作    者:  邵凯田
// 创建时间:  2016-2-3 15:20
// 参数说明:  @wMsgType表示消息类型
//         :  @pMsgHead为消息头
//         :  @sHeadStr消息头字符串
//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
//         :  @iLength为二进制数据长度
// 返 回 值:  true表示处理成功，false表示处理失败或未处理
//////////////////////////////////////////////////////////////////////////
bool CMdbApplication::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return false;
}