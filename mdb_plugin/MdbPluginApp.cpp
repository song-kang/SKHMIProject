/**
 *
 * 文 件 名 : MdbPlugin.cpp
 * 创建日期 : 2015-11-7 16:50
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : MDB内存数据库的初始化插件APP类，用于创建内存数据库结构，并从历史库加载初始数据
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-7	邵凯田　创建文件
 *
 **/

#include "MdbPluginApp.h"

CMdbPluginApplication::CMdbPluginApplication()
{
	m_pDatabase = new CSKDatabase();
	m_pDatabase->SetPoolSize(1);
}

CMdbPluginApplication::~CMdbPluginApplication()
{
	Stop();
	if(m_pDatabase != NULL)
		delete m_pDatabase;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
// 作    者:  邵凯田
// 创建时间:  2010-7-1 9:15
// 参数说明:  
// 返 回 值:  null
//////////////////////////////////////////////////////////////////////////
bool CMdbPluginApplication::Start()
{
	//加载数据库配置
	LOGDEBUG("into CMdbPluginApplication::Start()");
	SString sFileName = GetConfPath()+"sys_database.xml";
	if(!m_pDatabase->Load(sFileName))
	{
		LOGFAULT("数据库配置文件打开失败!file:%s", sFileName.data());
		return false;
	}

	SString sql;
	SDatabaseOper *pDb = m_pDatabase->GetHisDbOper();

	int iRowsCopyed = 0;
	SXmlConfig xml;
	sFileName = GetConfPath()+"mdb.xml";
	LOGDEBUG("load config %s",sFileName.data());
	if(!xml.ReadConfig(sFileName))
	{
		LOGFAULT("内存库配置文件打开失败!file:%s", sFileName.data());
		return false;
	}
	LOGDEBUG("mdb.xml文件共%d个表待恢复!",xml.GetChildCount("table"));
	unsigned long pos1,pos2;
	int iRowBuffSize = 102400;
	BYTE *pRowBuff = new BYTE[iRowBuffSize];
	SBaseConfig *pTabNode = xml.GetChildPtr()->FetchFirst(pos1);
	while(pTabNode)
	{
		if(pTabNode->GetNodeName() != "table")
		{
			pTabNode = xml.GetChildPtr()->FetchNext(pos1);
			continue;
		}
		int iTableRowsCopyed = 0;
		SString tablename = pTabNode->GetAttribute("name");
		bool sync = pTabNode->GetAttribute("sync_record")=="true";
		sql.sprintf("drop table %s",tablename.data());
		LOGDEBUG("正在恢复表%s到内存库...",tablename.data());
		MdbExecute(sql);
		sql.sprintf("create table %s (",
			tablename.data(),tablename.data());
		SString sKeyName;
		SString sAllField;
		SBaseConfig *pFldNode = pTabNode->GetChildPtr()->FetchFirst(pos2);
		while(pFldNode)
		{
			if(pFldNode->GetNodeName() != "field")
			{
				pFldNode = pTabNode->GetChildPtr()->FetchNext(pos2);
				continue;
			}
			SString name = pFldNode->GetAttribute("name");
			SString type = pFldNode->GetAttribute("type");
			SString isnull = pFldNode->GetAttribute("isnull");
			SString key = pFldNode->GetAttribute("key");
			sql += SString::toFormat("  %s %s%s,",name.data(),type.data(),isnull=="NO"?" not null":"");
			if(key == "pk")
			{
				if(sKeyName.length() > 0)
					sKeyName += ",";
				sKeyName += name;
			}
			if(sAllField.length()>0)
				sAllField += ",";
			sAllField += name;
			pFldNode = pTabNode->GetChildPtr()->FetchNext(pos2);
		}
		
		//处理主键,如果没有明确指定主键，则将所有字段一起作为主健
		if(sKeyName.length() == 0)
		{
			pFldNode = pTabNode->GetChildPtr()->FetchFirst(pos2);
			while(pFldNode)
			{
				if(pFldNode->GetNodeName() != "field")
				{
					pFldNode = pTabNode->GetChildPtr()->FetchNext(pos2);
					continue;
				}
				SString name = pFldNode->GetAttribute("name");
				if(sKeyName.length() > 0)
					sKeyName += ",";
				sKeyName += name;
				pFldNode = pTabNode->GetChildPtr()->FetchNext(pos2);
			}
		}
		sql += SString::toFormat("  constraint pk_%s primary key(%s) )",tablename.data(),sKeyName.data());
		if(!MdbExecute(sql))
		{
			LOGERROR("创建内存表%s时失败!",tablename.data());
		}
		SPtrList<stuTableField> fields;
		if(MdbGetTableFields(tablename.data(),&fields) <= 0)
		{
			LOGERROR("取表%s的字段信息时失败!",tablename.data());
		}
		else
		{
			if(m_pDatabase->GetMasterType() == DB_MYSQL)
			{
				//#ifdef OMS_MYSQL_DB
				MYSQL *pMySQL = ((SMySQL*)pDb->GetDatabasePool()->GetDatabaseByIdx(0))->GetConnectHandle();
				if(pMySQL == NULL)
				{
					delete[] pRowBuff;
					LOGFAULT("MySQL数据库尚未连接!将中止主程序的运行!!!");
					SLog::WaitForLogEmpty(5);
					exit(0);
					abort();
					return false;
				}
				
				int offset=0;
				int len = 100000;
				int iRowSize = 0;
				unsigned long pos;
				stuTableField  *pF = fields.FetchFirst(pos);
				while(pF)
				{
					iRowSize += pF->bytes;
					pF = fields.FetchNext(pos);
				}
				while(1)
				{
					sql.sprintf("select %s from %s limit %d,%d",sAllField.data(),tablename.data(),offset,len);
					int ret = mysql_query(pMySQL,sql.data());
					if(ret != 0)
					{
						delete[] pRowBuff;
						LOGFAULT("Retrieve mysql_query error:%s! 将中止主程序的运行!!!",mysql_error(pMySQL));
							SLog::WaitForLogEmpty(5);
						exit(0);
						abort();
						return false;
					}

					MYSQL_RES *pRes;
					if (!(pRes=mysql_store_result(pMySQL)))
					{
						delete[] pRowBuff;
						LOGFAULT("Couldn't get result :%s! 将中止主程序的运行!!!",mysql_error(pMySQL));
							SLog::WaitForLogEmpty(5);
						exit(0);
						abort();

						return false;
					}

					int cols = (int)mysql_num_fields(pRes);
					int rows = (int)mysql_num_rows(pRes);
					int i,j;
					if(iRowBuffSize < iRowSize*rows)
					{
						delete[] pRowBuff;
						iRowBuffSize = iRowSize*rows;
						pRowBuff = new BYTE[iRowBuffSize];
					}
					memset(pRowBuff,0,iRowBuffSize);

					MYSQL_ROW row;
					stuTableField *pFld;
					row= mysql_fetch_row(pRes);
					for(i=0;row != NULL && i<rows;i++)
					{
						pFld = fields.FetchFirst(pos);
						for(j=0;pFld && j<cols;j++,pFld = fields.FetchNext(pos))
						{
							if(row[j] == NULL)
								continue;
							switch (pFld->type)
							{
							case MDB_INT:	//4 bytes
							case MDB_BLOB:
								{
#ifndef _ARM
									*((int*)(pRowBuff + i*iRowSize + pFld->start_pos)) = atoi(row[j]);

#else
									int val = atoi(row[j]);
									memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
#endif
								}
								break;
							case MDB_SHORT:	//2 bytes
								{
#ifndef _ARM
									*((short*)(pRowBuff + i*iRowSize + pFld->start_pos)) = (short)atoi(row[j]);
#else
									short val = (short)atoi(row[j]);
									memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
#endif
								}
								break;
							case MDB_BYTE:	//1 bytes
								pRowBuff[i*iRowSize + pFld->start_pos] = (BYTE)atoi(row[j]);
								break;
							case MDB_INT64:	//8 bytes
								{
#ifndef _ARM
									*((INT64*)(pRowBuff + i*iRowSize + pFld->start_pos)) = SString::toInt64(row[j]);

#else
									INT64 val = SString::toInt64(row[j]);
									memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
#endif
								}
								break;
							case MDB_FLOAT:	//4 bytes
								{
#ifndef _ARM
									*((float*)(pRowBuff + i*iRowSize + pFld->start_pos)) = (float)atof(row[j]);

#else
									float val = (float)atof(row[j]);
									memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
#endif
								}
								break;
							case MDB_STR:	//N bytes（由建表语法决定）
								SString::strncpy((char*)(pRowBuff + i*iRowSize + pFld->start_pos), pFld->bytes,row[j]);
								break;
							default:
								{
									LOGDEBUG("字段%s有无效的数据类型:%d", pFld->name, pFld->type);
								}
								break;
							}
						}
						row= mysql_fetch_row(pRes);
					}
					mysql_free_result(pRes);

					if((ret=MdbInsert(tablename.data(),pRowBuff,iRowSize,rows)) != rows)
					{
						LOGERROR("插入%s数据到内存库失败!rows=%d,ret=%d",tablename.data(),rows,ret);
					}
					else
					{
						iRowsCopyed += rows;
						iTableRowsCopyed += rows;
						LOGDEBUG("成功向%s数据表写入%d行...累计%d行...",tablename.data(),rows,iTableRowsCopyed);
					}
					break;
					if(rows < len)
						break;
					offset += len;
				}
			}

			if(m_pDatabase->GetMasterType() == DB_ORACLE)
			{
				COracleDataBase *pOracle = ((SOracle*)pDb->GetDatabasePool()->GetDatabaseByIdx(0))->GetConnectHandle();
				if(pOracle == NULL)
				{
					delete[] pRowBuff;
					LOGFAULT("ORACLE数据库尚未连接!将中止主程序的运行!!!");
					SLog::WaitForLogEmpty(5);
					exit(0);
					abort();
					return false;
				}

				int offset=0;
				int len = 100000;
				int iRowSize = 0;
				unsigned long pos;
				stuTableField  *pF = fields.FetchFirst(pos);
				while(pF)
				{
					iRowSize += pF->bytes;
					pF = fields.FetchNext(pos);
				}

				sql.sprintf("select /*+all_rows*/%s from %s",sAllField.data(),tablename.data());

				OdbRecordSet ors;
				long hStmt=0;
				string sSql(sql.data());
				string sLastError;
				sword status = pOracle->Select(sSql,ors,hStmt,pOracle->m_iSelectRowsOnce);
				if(status!=0) 
				{
					if(status!=0)
					{
						delete[] pRowBuff;
						sLastError=pOracle->GetErrorText();
						LOGFAULT("SQL语句错误:%s, err:%s!将中止主程序的运行!!!\n",sSql.c_str(),sLastError.data());
						SLog::WaitForLogEmpty(5);
						exit(0);
						abort();

						return false;
					}
				}

				try
				{
					int nFields=ors.GetFieldCount();
					int ret = pOracle->FetchNext(hStmt,ors,0);
					int ret2;
					int i,j;
					SString sValue;
					stuTableField *pFld;
					while(ret==OCI_SUCCESS || ret == OCI_NO_DATA) 
					{
						int cols = nFields;
						int rows = ors.m_nRows;
						if(rows<=0)
							break;
						if(iRowBuffSize < iRowSize*rows)
						{
							delete[] pRowBuff;
							iRowBuffSize = iRowSize*rows;
							pRowBuff = new BYTE[iRowBuffSize];
						}
						memset(pRowBuff,0,iRowBuffSize);

						for(i=0;i<rows;i++)
						{
							pFld = fields.FetchFirst(pos);
							for(j=0;pFld && j<cols;j++,pFld = fields.FetchNext(pos))
							{
								ors.GetFieldValueToStr(&sValue,j,i);
								if(sValue.length() == 0)
									continue;
								switch (pFld->type)
								{
								case MDB_INT:	//4 bytes
								case MDB_BLOB:
									{
	#ifndef _ARM
										*((int*)(pRowBuff + i*iRowSize + pFld->start_pos)) = atoi(sValue.data());

	#else
										int val = atoi(sValue.data());
										memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
	#endif
									}
									break;
								case MDB_SHORT:	//2 bytes
									{
	#ifndef _ARM
										*((short*)(pRowBuff + i*iRowSize + pFld->start_pos)) = (short)atoi(sValue.data());
	#else
										short val = (short)atoi(sValue.data());
										memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
	#endif
									}
									break;
								case MDB_BYTE:	//1 bytes
									pRowBuff[i*iRowSize + pFld->start_pos] = (BYTE)atoi(sValue.data());
									break;
								case MDB_INT64:	//8 bytes
									{
	#ifndef _ARM
										*((INT64*)(pRowBuff + i*iRowSize + pFld->start_pos)) = SString::toInt64(sValue.data());

	#else
										INT64 val = SString::toInt64(sValue.data());
										memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
	#endif
									}
									break;
								case MDB_FLOAT:	//4 bytes
									{
	#ifndef _ARM
										*((float*)(pRowBuff + i*iRowSize + pFld->start_pos)) = (float)atof(sValue.data());

	#else
										float val = (float)atof(sValue.data());
										memset(pRowBuff + i*iRowSize + pFld->start_pos,&val,sizeof(val));
	#endif
									}
									break;
								case MDB_STR:	//N bytes（由建表语法决定）
									SString::strncpy((char*)(pRowBuff + i*iRowSize + pFld->start_pos), pFld->bytes,sValue.data());
									break;
								default:
									{
										LOGDEBUG("字段%s有无效的数据类型:%d", pFld->name, pFld->type);
									}
									break;
								}
							}
						}
						if((ret2=MdbInsert(tablename.data(),pRowBuff,iRowSize,rows)) != rows)
						{
							LOGERROR("插入%s数据到内存库失败!rows=%d,ret=%d",tablename.data(),rows,ret2);
						}
						else
						{
							iRowsCopyed += rows;
							iTableRowsCopyed += rows;
							LOGDEBUG("成功向%s数据表写入%d行...累计%d行...",tablename.data(),rows,iTableRowsCopyed);
						}		


						if(ret == OCI_NO_DATA)
							break;
						ret = pOracle->FetchNext(hStmt,ors,0);
					}
					pOracle->ClearRecordSets(hStmt,ors);
				}
				catch(...)
				{
					delete[] pRowBuff;
					pOracle->ClearRecordSets(hStmt,ors);
					sLastError=pOracle->GetErrorText();
					LOGFAULT("SQL语句错误:%s,err:%s!将中止主程序的运行!!!\n",sql.left(1000).data(),sLastError.data());
					SLog::WaitForLogEmpty(5);
					exit(0);
					abort();
					return false;
				}
			}
		}
		
		pTabNode = xml.GetChildPtr()->FetchNext(pos1);
	}
	delete[] pRowBuff;
	LOGDEBUG("累计同步%d条记录!",iRowsCopyed);

	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，关闭所有应用的服务，
// 作    者:  邵凯田
// 创建时间:  2010-7-1 9:18
// 参数说明:  
// 返 回 值:  null
//////////////////////////////////////////////////////////////////////////
bool CMdbPluginApplication::Stop()
{
	if(m_pDatabase != NULL)
	{
		delete m_pDatabase;
		m_pDatabase = NULL;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  从.sql文件批量执行SQL
// 作    者:  邵凯田
// 创建时间:  2015-11-7 22:58
// 参数说明:  @filename为文件名称
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbPluginApplication::RunSqlFile(SString filename)
{
	SFile file(filename);
	if (file.open(IO_ReadOnly) == false)
	{
		LOGERROR("文件[%s]不存在！\n", filename.data());
		return false;
	}
	char buf[10240];
	int lines = 0;
	SString sLine;
	SString sql = "";
	bool bEnd = false;
	bool bSepClosed = true;
	int pos = 0;
	bool bComment = false;
	SString msg;
	bool bSector = false;
	bool bOk = true;

	while (file.readLine(buf, sizeof(buf)) >= 0)
	{
		sLine = buf;
		if (sLine.length() == 0)
		{
			if (file.atEnd())
				break;
			continue;
		}

		if (sLine.Find("/*") >= 0)
		{
			bComment = true;
		}
		if (bComment == true)
		{
			if (sLine.Find("*/") >= 0)
				bComment = false;
			continue;
		}
		if (sLine.Find("--[BEGIN]") >= 0)
		{
			bSector = true;
			sql = "";
			continue;
		}
		if (bSector == true && sLine.Find("--[END]") >= 0)
		{
			bSector = false;
			//tfile.writeString("\r\nsql="+sql);
			if (sql.left(5).toLower().find("drop ")!=0 && !MdbExecute(sql))
			{
				LOGERROR("SQL[%s]错误!\n", sql.data());
				bOk = false;
				//break;
			}
			lines++;
			sql = "";
			bEnd = false;
			continue;
		}
		if (bSector == true)
		{
			sql += sLine + "\r\n";
			continue;
		}
		if (sLine.Find("--") >= 0)
		{
			sLine = sLine.Left(sLine.Find("--"));
		}
		pos = -1;
		while (1)
		{
			if ((pos = sLine.Find("'", pos + 1)) < 0)
				break;
			bSepClosed = !bSepClosed;
		}

		if (bSepClosed && sLine.Find(";") >= 0)
		{
			sLine = sLine.Left(sLine.Find(";"));
			bEnd = true;
		}
		sLine.trim();
		if (bEnd == false && (sLine == "" || sLine.Left(2) == "--" || sLine.Left(1) == "*"))
			continue;
		sql += sLine + " ";
		if (bEnd)
		{
			//tfile.writeString("\r\nsql="+sql);
			if (sql.left(5).toLower().find("drop ")!=0 && !MdbExecute(sql))
			{
				LOGERROR("SQL[%s]错误!", sql.data());
				bOk = false;
				//break;
			}
			lines++;
			sql = "";
			bEnd = false;
		}
		memset(buf, 0, sizeof(buf));
	}
// 	if (bOk)
// 	{
// 		LOGERROR("SQL文件执行完毕!共%d个语句被成功执行!\n", lines);
// 	}
	return bOk;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从一个指定的字符串批量执行SQL
// 作    者:  邵凯田
// 创建时间:  2015-11-7 22:59
// 参数说明:  @sqls为字符串内容
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbPluginApplication::RunSqlStrings(char* sqls)
{
	int lines = 0;
	SString sLine;
	SString sql = "";
	bool bEnd = false;
	bool bSepClosed = true;
	int pos = 0;
	bool bComment = false;
	SString msg;
	bool bSector = false;
	bool bOk = true;
	char *psql = sqls;
	char *psql2 = sqls;
	while(*psql2 != '\0')
	{
		psql2 = strstr(psql,"\n");
		if(psql2 == NULL)
			psql2 = psql+strlen(psql);
		else
			psql2++;
		sLine = SString::toString(psql,psql2-psql-1);
		psql = psql2;
		if (sLine.length() == 0)
		{
			return 0;
		}

		if (sLine.Find("/*") >= 0)
		{
			bComment = true;
		}
		if (bComment == true)
		{
			if (sLine.Find("*/") >= 0)
				bComment = false;
			continue;
		}
		if (sLine.Find("--[BEGIN]") >= 0)
		{
			bSector = true;
			sql = "";
			continue;
		}
		if (bSector == true && sLine.Find("--[END]") >= 0)
		{
			bSector = false;
			//tfile.writeString("\r\nsql="+sql);
			if (sql.left(5).toLower().find("drop ")!=0 && !MdbExecute(sql))
			{
				LOGERROR("SQL[%s]错误!", sql.data());
				bOk = false;
				//break;
			}
			lines++;
			sql = "";
			bEnd = false;
			continue;
		}
		if (bSector == true)
		{
			sql += sLine + "\r\n";
			continue;
		}
		if (sLine.Find("--") >= 0)
		{
			sLine = sLine.Left(sLine.Find("--"));
		}
		pos = -1;
		while (1)
		{
			if ((pos = sLine.Find("'", pos + 1)) < 0)
				break;
			bSepClosed = !bSepClosed;
		}

		if (bSepClosed && sLine.Find(";") >= 0)
		{
			sLine = sLine.Left(sLine.Find(";"));
			bEnd = true;
		}
		sLine.trim();
		if (bEnd == false && (sLine == "" || sLine.Left(2) == "--" || sLine.Left(1) == "*"))
			continue;
		sql += sLine + " ";
		if (bEnd)
		{
			//tfile.writeString("\r\nsql="+sql);
			if (sql.left(5).toLower().find("drop ")!=0 && !MdbExecute(sql))
			{
				LOGERROR("SQL[%s]错误!", sql.data());
				bOk = false;
				//break;
			}
			lines++;
			sql = "";
			bEnd = false;
		}
	}
// 	if (bOk)
// 	{
// 		LOGERROR("SQL文件执行完毕!共%d个语句被成功执行!\n", lines);
// 	}
	return bOk;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  内存库执行SQL
// 作    者:  邵凯田
// 创建时间:  2015-11-24 13:09
// 参数说明:  @sql
// 返 回 值:  bool
//////////////////////////////////////////////////////////////////////////
bool CMdbPluginApplication::MdbExecute(SString sql)
{
	LOGDEBUG("Execute : %s ",sql.data());
	return m_pRunSqlFun(sql.data());
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  内存库插入函数
// 作    者:  邵凯田
// 创建时间:  2015-11-24 13:18
// 参数说明:  
// 返 回 值:  int
//////////////////////////////////////////////////////////////////////////
int CMdbPluginApplication::MdbInsert(char* sTableName, void *pRowData, int iRowSize, int iRows)
{
	LOGDEBUG("Insert into %s ...",sTableName);
	return m_pMdbInsertFun(sTableName,pRowData,iRowSize,iRows);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  
// 作    者:  邵凯田
// 创建时间:  2016-3-23 21:52
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
int CMdbPluginApplication::MdbRetrieve(char* sql, unsigned char** ppRecordset, int *piRowSize, int *piRows, char** ppsRowResult)
{
	return m_pMdbRetrieve(sql,ppRecordset,piRowSize,piRows,ppsRowResult);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取表字段信息
// 作    者:  邵凯田
// 创建时间:  2016-3-23 23:19
// 参数说明:  @plFields
// 返 回 值:  字段数量
//////////////////////////////////////////////////////////////////////////
int CMdbPluginApplication::MdbGetTableFields(char* tablename,SPtrList<stuTableField> *plFields)
{
	return m_pMdbGetFields(tablename,plFields);
}