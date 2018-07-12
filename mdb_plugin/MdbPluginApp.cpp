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

// extern unsigned char g_mdb_na_sql[];
// extern unsigned char g_mdb_oe_sql[];

CMdbPluginApplication::CMdbPluginApplication()
{
	m_pDatabase = new CSsp_Database();			//数据库配置文件
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
	if(!m_pDatabase->Load(GetConfPath()+"sys_database.xml"))
	{
		LOGFAULT("数据库配置文件打开失败!file:%s", m_pDatabase->GetPathFile().data());
		return false;
	}

	SString sql;
	SDatabaseOper *pDb = DB;

	int iRowsCopyed = 0;
	SXmlConfig xml;
	SString sFileName = GetConfPath()+"mdb.xml";
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
				//SRecordset rs;
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
					//pDb->Retrieve(sql,rs);
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
					// 				int totals=100000;
					// 				for(int ii=0;ii<100;ii++)
					// 				{
					// 					for(int jj=0;jj<100000;jj++)
					// 						(*(int*)(pRowBuff+iRowSize*jj)) = ++totals;
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
					//				}
					break;
					if(rows < len)
						break;
					offset += len;
				}

//#endif
			}

			if(m_pDatabase->GetMasterType() == DB_ORACLE)
			{
	//#ifdef OMS_ORACLE_DB
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
				//SRecordset rs;
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

				///////////////////////////////////////////////////////////////////////////
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
				///////////////////////////////////////////////////////////////////////////
	//#endif
			}
		}
		
		pTabNode = xml.GetChildPtr()->FetchNext(pos1);
	}
	delete[] pRowBuff;
	LOGDEBUG("累计同步%d条记录!",iRowsCopyed);

#if 0

// 	m_pMdb = (SMdb*)MDB->GetDatabasePool()->m_DatabasePool.at(0);
// 	m_pMdbClient = m_pMdb->GetMdbClient();

	//创建内存数据库结构
// 	SFile f(this->GetBinPath()+"temp.sql");
// 	if(f.open(IO_Truncate))
// 	{
// 		f.write(g_mdb_na_sql,strlen((char*)g_mdb_na_sql));
// 		f.close();
// 		RunSqlFile(this->GetBinPath()+"temp.sql");
// 	}
// 	else
		RunSqlStrings((char*)g_mdb_na_sql);
		RunSqlStrings((char*)g_mdb_oe_sql);

	//加载历史库到内存库
	SRecordset rs;
	int i,cnt,ret;
	//t_na_unit
	sql = "select na_no,name,unit_type,na_ip,comm_state from t_na_unit";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_unit *p = new t_na_unit[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].na_no = (BYTE)rs.GetValue(i,0).toInt();
			SString::strncpy(p[i].name,sizeof(p[i].name),rs.GetValue(i,1).data());
			p[i].unit_type = (BYTE)rs.GetValue(i,2).toInt();
			SString::strncpy(p[i].na_ip,sizeof(p[i].na_ip),rs.GetValue(i,3).data());
			p[i].comm_state = (BYTE)rs.GetValue(i,4).toInt();
		}
		if((ret=MdbInsert("t_na_unit",p,sizeof(t_na_unit),cnt)) != cnt)
		{
			LOGERROR("插入t_na_unit数据到内存库失败!");
		}
		delete[] p;
	}
	//t_na_capture_port
	sql = "select sub_no,na_no,port_no,port_type,name,bit_rate from t_na_capture_port order by sub_no,na_no,port_no";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_capture_port *p = new t_na_capture_port[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].sub_no = (BYTE)rs.GetValue(i,0).toInt();
			p[i].na_no = (BYTE)rs.GetValue(i,1).toInt();
			p[i].port_no = (BYTE)rs.GetValue(i,2).toInt();
			p[i].port_type = (BYTE)rs.GetValue(i,3).toInt();
			SString::strncpy(p[i].name,sizeof(p[i].name),rs.GetValue(i,4).data());
			p[i].bit_rate = (short)rs.GetValue(i,5).toInt();
		}
		if((ret=MdbInsert("t_na_capture_port",p,sizeof(t_na_capture_port),cnt)) != cnt)
		{
			LOGERROR("插入t_na_capture_port数据到内存库失败!");
		}
		delete[] p;
	}
	//t_na_protocol_type
	sql = "SELECT prot_type,prot_name from t_na_protocol_type order by prot_type";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_protocol_type *p = new t_na_protocol_type[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].prot_type = (BYTE)rs.GetValue(i,0).toInt();
			SString::strncpy(p[i].prot_name,sizeof(p[i].prot_name),rs.GetValue(i,1).data());
		}
		if((ret=MdbInsert("t_na_protocol_type",p,sizeof(t_na_protocol_type),cnt)) != cnt)
		{
			LOGERROR("插入t_na_protocol_type数据到内存库失败!");
		}
		delete[] p;
	}
	//t_na_event_code
	sql = "select prot_type,event_id,name,event_type,event_level from t_na_event_code order by prot_type,event_id";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_event_code *p = new t_na_event_code[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].prot_type = (BYTE)rs.GetValue(i,0).toInt();
			p[i].event_id = (BYTE)rs.GetValue(i,1).toInt();
			SString::strncpy(p[i].name,sizeof(p[i].name),rs.GetValue(i,2).data());
			p[i].event_type = (BYTE)rs.GetValue(i,3).toInt();
			p[i].event_level = (BYTE)rs.GetValue(i,4).toInt();
		}
		if((ret=MdbInsert("t_na_event_code",p,sizeof(t_na_event_code),cnt)) != cnt)
		{
			LOGERROR("插入t_na_event_code数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_sub_network
	sql = "select net_id,net_name,net_type,net_desc,net_bitrate,uiwnd_sn from t_na_sub_network";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_sub_network *p = new t_na_sub_network[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].net_id = (BYTE)rs.GetValue(i,0).toInt();
			SString::strncpy(p[i].net_name,sizeof(p[i].net_name),rs.GetValue(i,1).data());
			p[i].net_type = (BYTE)rs.GetValue(i,2).toInt();
			SString::strncpy(p[i].net_desc,sizeof(p[i].net_desc),rs.GetValue(i,3).data());
			p[i].net_bitrate = (int)rs.GetValue(i,4).toInt();
			p[i].uiwnd_sn = (int)rs.GetValue(i,5).toInt();
		}
		if((ret=MdbInsert("t_na_sub_network",p,sizeof(t_na_sub_network),cnt)) != cnt)
		{
			LOGERROR("插入t_na_sub_network数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_comm_device
	sql = "select com_id,net_id,dev_type,com_name,mgr_ip,mgr_port,mgr_state,state_confirm,dev_factory,dev_model,dev_version,dev_crc,up_time,ext_attr from t_na_comm_device";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_comm_device *p = new t_na_comm_device[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].com_id = (BYTE)rs.GetValue(i,0).toInt();
			p[i].net_id = (BYTE)rs.GetValue(i,1).toInt();
			p[i].dev_type = (BYTE)rs.GetValue(i,2).toInt();
			SString::strncpy(p[i].com_name,sizeof(p[i].com_name),rs.GetValue(i,3).data());
			SString::strncpy(p[i].mgr_ip,sizeof(p[i].mgr_ip),rs.GetValue(i,4).data());
			p[i].mgr_port = (int)rs.GetValue(i,5).toInt();
			p[i].mgr_state = (BYTE)rs.GetValue(i,6).toInt();
			p[i].state_confirm = (BYTE)rs.GetValue(i,7).toInt();
			SString::strncpy(p[i].dev_factory,sizeof(p[i].dev_factory),rs.GetValue(i,8).data());
			SString::strncpy(p[i].dev_model,sizeof(p[i].dev_model),rs.GetValue(i,9).data());
			SString::strncpy(p[i].dev_version,sizeof(p[i].dev_version),rs.GetValue(i,10).data());
			SString::strncpy(p[i].dev_crc,sizeof(p[i].dev_crc),rs.GetValue(i,11).data());
			SString::strncpy(p[i].up_time,sizeof(p[i].up_time),rs.GetValue(i,12).data());
			SString::strncpy(p[i].ext_attr,sizeof(p[i].ext_attr),rs.GetValue(i,13).data());
		}
		if((ret=MdbInsert("t_na_comm_device",p,sizeof(t_na_comm_device),cnt)) != cnt)
		{
			LOGERROR("插入t_na_comm_device数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_comm_device_port
	sql = "select port_id,dev_cls,dev_id,net_id,port_name,port_type,if_type,ip_addr,mac_addr,mcast_mac_addr,port_state,port_flow "
		"state_confirm,port_flow,port_inflow,port_outflow,in_bytes,in_pkgs,in_bcast_pkgs,in_mcast_pkgs,"
		"sum_in_pkts_64,sum_in_pkts_65_127,sum_in_pkts_128_255,sum_in_pkts_256_511,sum_in_pkts_512_1023,sum_in_pkts_1024_1518 "
		" from t_na_comm_device_port";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_comm_device_port *p = new t_na_comm_device_port[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].port_id = (int)rs.GetValue(i,0).toInt();
			p[i].dev_cls = (BYTE)rs.GetValue(i,1).toInt();
			p[i].dev_id = (int)rs.GetValue(i,2).toInt();
			p[i].net_id = (BYTE)rs.GetValue(i,3).toInt();
			SString::strncpy(p[i].port_name,sizeof(p[i].port_name),rs.GetValue(i,4).data());
			p[i].port_type = (BYTE)rs.GetValue(i,5).toInt();
			p[i].if_type = (BYTE)rs.GetValue(i,6).toInt();
			SString::strncpy(p[i].ip_addr,sizeof(p[i].ip_addr),rs.GetValue(i,7).data());
			SString::strncpy(p[i].mac_addr,sizeof(p[i].ip_addr),rs.GetValue(i,8).data());
			SString::strncpy(p[i].mcast_mac_addr,sizeof(p[i].ip_addr),rs.GetValue(i,9).data());
			p[i].port_state = (BYTE)rs.GetValue(i,10).toInt();
			p[i].port_flow = (int)rs.GetValue(i,11).toInt();
			p[i].state_confirm = (BYTE)rs.GetValue(i,12).toInt();
			p[i].port_flow = (int)rs.GetValue(i,13).toInt();
			p[i].port_inflow = (int)rs.GetValue(i,14).toInt();
			p[i].port_outflow = (int)rs.GetValue(i,15).toInt();
			p[i].in_bytes = (int)rs.GetValue(i,16).toInt();
			p[i].in_pkgs = (int)rs.GetValue(i,17).toInt();
			p[i].in_bcast_pkgs = (int)rs.GetValue(i,18).toInt();
			p[i].in_mcast_pkgs = (int)rs.GetValue(i,19).toInt();
			p[i].sum_in_pkts_64 = (int)rs.GetValue(i,20).toInt();
			p[i].sum_in_pkts_65_127 = (int)rs.GetValue(i,21).toInt();
			p[i].sum_in_pkts_128_255 = (int)rs.GetValue(i,22).toInt();
			p[i].sum_in_pkts_256_511 = (int)rs.GetValue(i,23).toInt();
			p[i].sum_in_pkts_512_1023 = (int)rs.GetValue(i,24).toInt();
			p[i].sum_in_pkts_1024_1518 = (int)rs.GetValue(i,25).toInt();
		}
		if((ret=MdbInsert("t_na_comm_device_port",p,sizeof(t_na_comm_device_port),cnt)) != cnt)
		{
			LOGERROR("插入t_na_comm_device_port数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_comm_connect_line
	sql = "select net_id_a,dev_cls_a,dev_id_a,port_id_a,net_id_b,dev_cls_b,dev_id_b,port_id_b,line_type,line_stat,state_confirm,soc from t_na_comm_connect_line";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_comm_connect_line *p = new t_na_comm_connect_line[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].net_id_a		= (BYTE)rs.GetValue(i,0).toInt();
			p[i].dev_cls_a		= (BYTE)rs.GetValue(i,1).toInt();
			p[i].dev_id_a		= (int)rs.GetValue(i,2).toInt();
			p[i].port_id_a		= (int)rs.GetValue(i,3).toInt();
			p[i].net_id_b		= (BYTE)rs.GetValue(i,4).toInt();
			p[i].dev_cls_b		= (BYTE)rs.GetValue(i,5).toInt();
			p[i].dev_id_b		= (int)rs.GetValue(i,6).toInt();
			p[i].port_id_b		= (int)rs.GetValue(i,7).toInt();
			p[i].line_type		= (BYTE)rs.GetValue(i,8).toInt();
			p[i].line_stat		= (BYTE)rs.GetValue(i,9).toInt();
			p[i].state_confirm	= (BYTE)rs.GetValue(i,10).toInt();
			p[i].soc			= (BYTE)rs.GetValue(i,11).toInt();
		}
		if((ret=MdbInsert("t_na_comm_connect_line",p,sizeof(t_na_comm_connect_line),cnt)) != cnt)
		{
			LOGERROR("插入t_na_comm_connect_line数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_comm_mac_addr
	sql = "select ip_addr,mac_addr,na_no,port_no,soc from t_na_comm_mac_addr";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_comm_mac_addr *p = new t_na_comm_mac_addr[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].ip_addr = (int)rs.GetValue(i,0).toInt();
			SString::strncpy(p[i].mac_addr,sizeof(p[i].mac_addr),rs.GetValue(i,1).data());
			p[i].na_no = (BYTE)rs.GetValue(i,2).toInt();
			p[i].port_no = (BYTE)rs.GetValue(i,3).toInt();
			p[i].soc = (int)rs.GetValue(i,4).toInt();
		}
		if((ret=MdbInsert("t_na_comm_mac_addr",p,sizeof(t_na_comm_mac_addr),cnt)) != cnt)
		{
			LOGERROR("插入t_na_comm_mac_addr数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_switch_port_mac
	sql = "select net_id,com_id,port_id,mac_addr,ip_addr,soc from t_na_switch_port_mac";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_switch_port_mac *p = new t_na_switch_port_mac[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].net_id = (int)rs.GetValue(i,0).toInt();
			p[i].com_id = (int)rs.GetValue(i,1).toInt();
			p[i].port_id = (int)rs.GetValue(i,2).toInt();
			SString::strncpy(p[i].mac_addr,sizeof(p[i].mac_addr),rs.GetValue(i,3).data());
			p[i].ip_addr = (BYTE)rs.GetValue(i,4).toInt();
			p[i].soc = (BYTE)rs.GetValue(i,5).toInt();
		}
		if((ret=MdbInsert("t_na_switch_port_mac",p,sizeof(t_na_switch_port_mac),cnt)) != cnt)
		{
			LOGERROR("插入t_na_switch_port_mac数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_mms_session
	sql = "select sub_no,client_ip,server_ip,client_port,server_port,port_no,soc,real_pkgs,real_bytes,state,connect_time from t_na_mms_session";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_mms_session *p = new t_na_mms_session[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].sub_no			= (BYTE)rs.GetValue(i,0).toInt();
			p[i].client_ip		= (int)rs.GetValue(i,1).toInt();
			p[i].server_ip		= (int)rs.GetValue(i,2).toInt();
			p[i].client_port	= (int)rs.GetValue(i,3).toInt();
			p[i].server_port	= (int)rs.GetValue(i,4).toInt();
			p[i].port_no		= (BYTE)rs.GetValue(i,5).toInt();
			p[i].soc			= (int)rs.GetValue(i,6).toInt();
			p[i].real_pkgs		= (int)rs.GetValue(i,7).toInt();
			p[i].real_bytes		= (int)rs.GetValue(i,8).toInt();
			p[i].state			= (BYTE)rs.GetValue(i,9).toInt();
			p[i].connect_time	= (int)rs.GetValue(i,10).toInt();
		}
		if((ret=MdbInsert("t_na_mms_session",p,sizeof(t_na_mms_session),cnt)) != cnt)
		{
			LOGERROR("插入t_na_mms_session数据到内存库失败!");
		}
		delete[] p;
	}

	//t_na_iec104_session
	sql = "select sub_no,client_ip,server_ip,client_port,server_port,port_no,soc,real_pkgs,real_bytes,state,connect_time from t_na_iec104_session";
	cnt = pDb->Retrieve(sql,rs);
	if(cnt > 0)
	{
		t_na_iec104_session *p = new t_na_iec104_session[cnt];
		memset(p,0,sizeof(*p)*cnt);
		for(i=0;i<cnt;i++)
		{
			p[i].sub_no			= (BYTE)rs.GetValue(i,0).toInt();
			p[i].client_ip		= (int)rs.GetValue(i,1).toInt();
			p[i].server_ip		= (int)rs.GetValue(i,2).toInt();
			p[i].client_port	= (int)rs.GetValue(i,3).toInt();
			p[i].server_port	= (int)rs.GetValue(i,4).toInt();
			p[i].port_no		= (BYTE)rs.GetValue(i,5).toInt();
			p[i].soc			= (int)rs.GetValue(i,6).toInt();
			p[i].real_pkgs		= (int)rs.GetValue(i,7).toInt();
			p[i].real_bytes		= (int)rs.GetValue(i,8).toInt();
			p[i].state			= (BYTE)rs.GetValue(i,9).toInt();
			p[i].connect_time	= (int)rs.GetValue(i,10).toInt();
		}
		if((ret=MdbInsert("t_na_iec104_session",p,sizeof(t_na_iec104_session),cnt)) != cnt)
		{
			LOGERROR("插入t_na_iec104_session数据到内存库失败!");
		}
		delete[] p;
	}


	cnt = 100;
	t_ssp_tagged_text *p_tagged_test = new t_ssp_tagged_text[cnt];
	memset(p_tagged_test,0,sizeof(t_ssp_tagged_text)*cnt);
	for(i=0;i<100;i++)
	{
		p_tagged_test[i].tag_no = i+1;
	}
	if((ret=MdbInsert("t_ssp_tagged_text",p_tagged_test,sizeof(t_ssp_tagged_text),cnt)) != cnt)
	{
		LOGERROR("插入t_ssp_tagged_text数据到内存库失败!");
	}
	delete[] p_tagged_test;

	//t_oe_element_general
	//根据IED分批导入，避免内存占用过多
	SRecordset rsIed;
	pDb->Retrieve("select ied_no from t_oe_ied",rsIed);
	int ied,ieds = rsIed.GetRows();
	if(ieds > 0)
	{
		t_oe_element_general *p = NULL;
		int max_cnt = 0;
		for(ied=0;ied<ieds;/*ied++*/)
		{
			sql = "select ied_no,cpu_no,group_no,entry,valtype,itemtype,current_val,reference_val,mms_path from t_oe_element_general where ied_no in (";
			for(int j=0;j<20&&ied<ieds;j++,ied++)
			{
				if(j > 0)
					sql += ",";
				sql += SString::toFormat("%d",rsIed.GetValue(ied/*+j*/,0).toInt());
			}
			sql += ")";
			//sql.sprintf("select ied_no,cpu_no,group_no,entry,valtype,itemtype,current_val,reference_val,mms_path from t_oe_element_general where ied_no=%d",rsIed.GetValue(ied,0).toInt());
			cnt = pDb->Retrieve(sql,rs);
			if(cnt > 0)
			{
				if(cnt > max_cnt)
				{
					if(p != NULL)
						delete[] p;
					max_cnt = cnt;
					p = new t_oe_element_general[cnt];
				}
				memset(p,0,sizeof(*p)*cnt);
				int j;
				for(i=0,j=1;i<cnt;i++,j++)
				{
					p[i].ied_no		= (int)rs.GetValue(i,0).toInt();
					p[i].cpu_no		= (int)rs.GetValue(i,1).toInt();
					p[i].group_no	= (int)rs.GetValue(i,2).toInt();
					p[i].entry		= (int)rs.GetValue(i,3).toInt();
					p[i].valtype	= (BYTE)rs.GetValue(i,4).toInt();
					p[i].itemtype	= (int)rs.GetValue(i,5).toInt();
					SString::strncpy(p[i].current_val,sizeof(p[i].current_val),rs.GetValue(i,6).data());
					SString::strncpy(p[i].reference_val,sizeof(p[i].reference_val),rs.GetValue(i,7).data());
					SString::strncpy(p[i].mms_path,sizeof(p[i].mms_path),rs.GetValue(i,8).data());
				}
				if((ret=MdbInsert("t_oe_element_general",p,sizeof(t_oe_element_general),cnt)) != cnt)
				{
					LOGERROR("插入t_oe_element_general[ied_no=%d]数据到内存库失败!",rsIed.GetValue(ied,0).toInt());
				}				
			}
		}
		if(p != NULL)
			delete[] p;
	}

	//t_oe_element_state
	if(ieds > 0)
	{
		t_oe_element_state *p = NULL;
		int max_cnt = 0;
		for(ied=0;ied<ieds;/*ied++*/)
		{
			sql = "select ied_no,cpu_no,group_no,entry,fun,inf,name,type,evt_cls,val_type,level,on_dsc,off_dsc,unknown_desc,inver,"
				  "current_val,measure_val,soc,usec,mms_path,details from t_oe_element_state where ied_no in (";
			for(int j=0;j<20&&ied<ieds;j++,ied++)
			{
				if(j > 0)
					sql += ",";
				sql += SString::toFormat("%d",rsIed.GetValue(ied/*+j*/,0).toInt());
			}
			sql += ")";
// 			sql.sprintf("select ied_no,cpu_no,group_no,entry,fun,inf,type,evt_cls,val_type,level,"
// 				"current_val,measure_val,soc,usec,mms_path,details from t_oe_element_state where ied_no=%d",rsIed.GetValue(ied,0).toInt());
			cnt = pDb->Retrieve(sql,rs);
			if(cnt > 0)
			{
				if(cnt > max_cnt)
				{
					if(p != NULL)
						delete[] p;
					max_cnt = cnt;
					p = new t_oe_element_state[cnt];
				}
				memset(p,0,sizeof(*p)*cnt);
				int j;
				for(i=0,j=1;i<cnt;i++,j++)
				{
					p[i].ied_no		= (int)rs.GetValue(i,0).toInt();
					p[i].cpu_no		= (int)rs.GetValue(i,1).toInt();
					p[i].group_no	= (int)rs.GetValue(i,2).toInt();
					p[i].entry		= (int)rs.GetValue(i,3).toInt();
					p[i].fun		= (int)rs.GetValue(i,4).toInt();
					p[i].inf		= (int)rs.GetValue(i,5).toInt();
					SString::strncpy(p[i].name,sizeof(p[i].name),rs.GetValue(i,6).data());
					p[i].type		= (int)rs.GetValue(i,7).toInt();
					p[i].evt_cls	= (int)rs.GetValue(i,8).toInt();
					p[i].val_type	= (BYTE)rs.GetValue(i,9).toInt();
					p[i].level      = (BYTE)rs.GetValue(i,10).toInt();
					SString::strncpy(p[i].on_dsc,sizeof(p[i].name),rs.GetValue(i,11).data());
					SString::strncpy(p[i].off_dsc,sizeof(p[i].name),rs.GetValue(i,12).data());
					SString::strncpy(p[i].unknown_desc,sizeof(p[i].name),rs.GetValue(i,13).data());
					p[i].inver		= (int)rs.GetValue(i,14).toInt();
					p[i].current_val= (BYTE)rs.GetValue(i,15).toInt();
					SString::strncpy(p[i].measure_val,sizeof(p[i].measure_val),rs.GetValue(i,16).data());
					p[i].soc		= (int)rs.GetValue(i,17).toInt();
					p[i].usec       = (int)rs.GetValue(i,18).toInt();
					SString::strncpy(p[i].mms_path,sizeof(p[i].mms_path),rs.GetValue(i,19).data());
					SString::strncpy(p[i].details,sizeof(p[i].details),rs.GetValue(i,20).data());
				}
				if((ret=MdbInsert("t_oe_element_state",p,sizeof(t_oe_element_state),cnt)) != cnt)
				{
					LOGERROR("插入t_oe_element_state[ied_no=%d]数据到内存库失败!",rsIed.GetValue(ied,0).toInt());
				}
			}
		}
		if(p != NULL)
			delete[] p;
	}
#endif

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