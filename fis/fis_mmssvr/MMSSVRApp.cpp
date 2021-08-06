#include "MMSSVRApp.h"

MMSSVRApp::MMSSVRApp(void)
{
	SetApplicationId(UK9010_APP_MMSSVR);

	m_pDatabase = new CSsp_Database();			//数据库配置文件
	m_pDatabase->SetPoolSize(10);
	node_no = 0;
	is_period = false;
	report_id = 1;
	m_stationNo = 0;
}

MMSSVRApp::~MMSSVRApp(void)
{
	for (std::map<std::string, std::vector<MMSServer*> >::iterator iter = m_mmsServerList.begin(); iter != m_mmsServerList.end(); ++iter)
	{
		for (std::vector<MMSServer*>::iterator iterMms = iter->second.begin(); iterMms != iter->second.end(); ++iterMms)
		{
			delete *iterMms;
		}
	}
	m_mmsServerList.clear();
	delete m_pDatabase;
	CConfigMgr::Quit();
}

bool MMSSVRApp::Start()
{
	if(!CUK9010Application::Start())
	{
		LOGERROR("加载单元配置失败!");
		return false;
	}

	//加载数据库配置
	if (!m_pDatabase->Load(GetConfPath() + "sys_database.xml"))
	{
		LOGFAULT("数据库配置文件打开失败!file:%s", m_pDatabase->GetPathFile().data());
		return false;
	}

	// station info
	if (!initStation())
	{
		LOGFAULT("初始化变电站信息失败");
		return false;
	}

	std::vector<Ied> iedList;
	if (!initScl(iedList))
	{
		LOGFAULT("初始化变电站信息失败");
		return false;
	}

	// 启动所有IED的运行A、B网监听
	for (std::vector<Ied>::iterator iter = iedList.begin(); iter != iedList.end(); ++iter)
	{
		if (iter->ipa.length() > 0)
		{
			LOGDEBUG("绑定本地A网端口 %s", iter->ipa.data());
			MMSServer *mmsServer = new MMSServer(this, m_stationNo, m_stationName, iter->name);
//			mmsServer->SetBindIPAddr(iter->ipa);
			mmsServer->setSclFile(m_sclFile);
			mmsServer->Start();
			m_mmsServerList[iter->name.data()].push_back(mmsServer);
		}
		
		if (iter->ipb.length() > 0)
		{
			LOGDEBUG("绑定本地B网端口 %s", iter->ipb.data());
			MMSServer *mmsServer = new MMSServer(this, m_stationNo, m_stationName, iter->name);
//			mmsServer->SetBindIPAddr(iter->ipb);
			mmsServer->setSclFile(m_sclFile);
			mmsServer->Start();
			m_mmsServerList[iter->name.data()].push_back(mmsServer);
		}
	}

	printf("RegisterTriggerCallback, MDB=%p, pool size=%d\n", MDB->GetDatabasePool(), MDB->GetDatabasePool()->GetPoolSize());
	CMdbClient *m_pMdbTrgClient = ((SMdb*)MDB->GetDatabasePool()->GetDatabaseByIdx(MDB->GetDatabasePool()->GetPoolSize()-1))->GetMdbClient();
	if (m_pMdbTrgClient)
	{
 		m_pMdbTrgClient->RegisterTriggerCallback(OnRealEventCallback, this, "t_oe_element_state", 0x02);
 		m_pMdbTrgClient->RegisterTriggerCallback(OnRealEventCallback, this, "t_oe_element_general", 0x02);
		m_pMdbTrgClient->RegisterTriggerCallback(OnRealEventCallback, this, "t_oe_element_control", 0x02);
		m_pMdbTrgClient->RegisterTriggerCallback(OnRealEventCallback, this, "t_oe_ied", 0x02);
	}
	else
	{
		LOGWARN("GetMdbClient failed, pool size=%d", MDB->GetDatabasePool()->GetPoolSize());
	}

	SKT_CREATE_THREAD(ThreadMainLoop, this);

	return true;
}

bool MMSSVRApp::Stop()
{
	for (std::map<std::string, std::vector<MMSServer*> >::iterator iterIed = m_mmsServerList.begin(); 
		iterIed != m_mmsServerList.end(); ++iterIed)
	{
		for (std::vector<MMSServer*>::iterator iterNet = iterIed->second.begin(); iterNet != iterIed->second.end(); ++iterNet)
		{
			(*iterNet)->Stop();
		}
	}
	
	return true;
}

bool MMSSVRApp::ProcessAgentMsg( unsigned short wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,unsigned char* pBuffer/*=NULL*/,int iLength/*=0*/ )
{
	bool result = false;
	for (std::map<std::string, std::vector<MMSServer*> >::iterator iterIed = m_mmsServerList.begin(); 
		iterIed != m_mmsServerList.end(); ++iterIed)
	{
		for (std::vector<MMSServer*>::iterator iter = iterIed->second.begin(); iter != iterIed->second.end(); ++iter)
		{
			if ((*iter)->ProcessAgentMsg(wMsgType, pMsgHead, sHeadStr, pBuffer, iLength))
				result = true;
			break;
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  处理文本命令
// 作    者:  邵凯田
// 创建时间:  2016-2-13 15:12
// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
//         :  @sResult为返回结果，纯文本
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool MMSSVRApp::ProcessTxtCmd(SString &sCmd,SString &sResult)
{
	if(sCmd == "")
	{
		sResult = "units   -get unit info\n";
	}
	else if(sCmd == "units")
	{
		SPtrList<stuSpUnitAgentUnitInfo> units;
		int ret = GetUnitInfo(units);
		sResult = SString::toFormat("units=%d\n",ret);
		for(int i=0;i<units.count();i++)
		{
			sResult += SString::toFormat("%d  %s  %s\n",units[i]->m_iUnitId,units[i]->m_sUnitName,units[i]->m_sUnitDesc);
		}
	}
	return true;
}

unsigned char* MMSSVRApp::OnRealEventCallback( void* cbParam, SString &sTable, eMdbTriggerType eType, int iTrgRows,int iRowSize,unsigned char *pTrgData )
{
	S_UNUSED(eType);
	if (sTable == "t_oe_element_state" && sizeof(t_oe_element_state) != iRowSize)
		return 0;
	else if (sTable == "t_oe_element_general" && sizeof(t_oe_element_general) != iRowSize)
		return 0;
	else if (sTable == "t_na_comm_device_port" && sizeof(t_na_comm_device_port) != iRowSize)
		return 0;
	else if (sTable == "t_oe_ied" && sizeof(t_oe_ied) != iRowSize)
		return 0;

	MMSSVRApp *pThis = (MMSSVRApp*)cbParam;

	if (sTable == "t_oe_element_state")
		pThis->ProcessStateChanged(pTrgData, iTrgRows);
	else if (sTable == "t_oe_element_general")
		pThis->ProcessGeneralChanged(pTrgData, iTrgRows);
	else if (sTable == "t_oe_element_control")
		pThis->ProcessControlChanged(pTrgData, iTrgRows);
	else if (sTable == "t_oe_ied")
		pThis->ProcessIedChanged(pTrgData, iTrgRows);

	return 0;
}

bool MMSSVRApp::ProcessStateChanged( unsigned char *pTrgData, int iTrgRows)
{
	t_oe_element_state element_state;
	int soc, usec;
	SDateTime::getSystemTime(soc, usec);

	for (int i = 0; i < iTrgRows; i++)
	{
		memcpy(&element_state, pTrgData + i*sizeof(t_oe_element_state*), sizeof(t_oe_element_state));

		if (m_isScdMode == false)
		{
			for (std::vector<MMSServer*>::iterator iter = m_mmsServerList.begin()->second.begin();
				iter != m_mmsServerList.begin()->second.end(); ++iter)
				(*iter)->ProcessStateChanged(&element_state, soc, usec);
		}
	}

	return true;
}

bool MMSSVRApp::ProcessGeneralChanged( unsigned char *pTrgData, int iTrgRows )
{
	t_oe_element_general element_general;
	int soc, usec;
	unsigned long long recv_tm = 0;

	for (int i = 0; i < iTrgRows; i++)
	{
		memcpy(&element_general, pTrgData + i*sizeof(t_oe_element_general*), sizeof(t_oe_element_general));

		SDateTime::getSystemTime(soc, usec);
		recv_tm = (unsigned long long)soc * 1000 + (unsigned long long)usec/1000;

		if (m_isScdMode == false)
		{
			for (std::vector<MMSServer*>::iterator iter = m_mmsServerList.begin()->second.begin();
				iter != m_mmsServerList.begin()->second.end(); ++iter)
				(*iter)->updateElementGeneralValue(&element_general, soc, usec);
		}
	}

	return true;
}

bool MMSSVRApp::ProcessControlChanged(unsigned char *pTrgData, int iTrgRows)
{
	return true;
}

bool MMSSVRApp::ProcessIedChanged(unsigned char *pTrgData, int iTrgRows)
{
	static std::map<int, int> lastIedState;
	t_oe_ied oe_ied;
	int soc, usec;
	unsigned long long recv_tm = 0;

	SDateTime::getSystemTime(soc, usec);
	recv_tm = (unsigned long long)soc * 1000 + (unsigned long long)usec/1000;

	for (int i = 0; i < iTrgRows; i++)
	{
		memcpy(&oe_ied, pTrgData + i * sizeof(t_oe_ied*), sizeof(t_oe_ied));

		std::map<int, int>::iterator iter;
		iter = lastIedState.find(oe_ied.ied_no);
		if (iter != lastIedState.end())
		{
			if (iter->second != oe_ied.comstate)
			{
				LOGDEBUG("装置(%s)(%d) 通信状态变化为(%d)", oe_ied.name, oe_ied.ied_no, oe_ied.comstate);
				if (m_isScdMode == false)
				{
					for (std::vector<MMSServer*>::iterator iter = m_mmsServerList.begin()->second.begin();
						iter != m_mmsServerList.begin()->second.end(); ++iter)
						(*iter)->updateIedComState(oe_ied.ied_no, oe_ied.comstate);
				}
			}
		}
		else
		{
			LOGDEBUG("装置(%s)(%d) 通信状态变化为(%d)", oe_ied.name, oe_ied.ied_no, oe_ied.comstate);
			if (m_isScdMode == false)
			{
				for (std::vector<MMSServer*>::iterator iter = m_mmsServerList.begin()->second.begin();
					iter != m_mmsServerList.begin()->second.end(); ++iter)
					(*iter)->updateIedComState(oe_ied.ied_no, oe_ied.comstate);
			}
		}

		lastIedState[oe_ied.ied_no] = oe_ied.comstate;
	}

	return true;
}

void* MMSSVRApp::ThreadMainLoop( void* lp )
{
	MMSSVRApp *pthis = (MMSSVRApp *)lp;
	if (pthis)
	{
		while (!pthis->IsQuit())
		{
			for (std::vector<MMSServer*>::iterator iter = pthis->m_mmsServerList.begin()->second.begin();
				iter != pthis->m_mmsServerList.begin()->second.end(); ++iter)
				(*iter)->OnTimerWork();
			SApi::UsSleep(1000000);
		}
	}
	return NULL;
}

bool MMSSVRApp::initStation()
{
	SString sql = "select sub_no, name from t_cim_substation";
	SRecordset rs;
	DB->Retrieve(sql, rs);
	if (rs.GetRows() > 0)
	{
		m_stationNo = rs.GetValue(0, 0).toInt();
		m_stationName = rs.GetValue(0, 1);

		return true;
	}

	return false;
}

bool MMSSVRApp::initScl(std::vector<Ied> &iedList)
{
	SString cidFile = "uk.cid";
	SString scdFile = "uk.scd";
	SString iedName;

	if (SFile::exists(cidFile))
	{
		m_sclFile = cidFile;
		m_isScdMode = false;
		LOGDEBUG("CID文件%s存在，将加载该文件运行", cidFile.data());
	}
	else if (SFile::exists(scdFile))
	{
		m_sclFile = scdFile;
		m_isScdMode = true;
		LOGDEBUG("SCD文件%s存在，将加载该文件运行", scdFile.data());
	}
	else
	{
		LOGERROR("CID文件%s和SCD文件%s不存在，将退出运行", cidFile.data(), scdFile.data());
		return false;
	}

	SXmlConfig xml;
	if (xml.ReadConfig(m_sclFile))
	{
		int iedCount = xml.GetChildCount("IED");
		for (int iedId = 0; iedId < iedCount; ++iedId)
		{
			SBaseConfig *iedNode = xml.GetChildNode(iedId, "IED");
			iedName = iedNode->GetAttribute("name");

			if (iedName.length() > 0)
			{
				Ied ied;
				ied.name = iedName;
				iedList.push_back(ied);
			}
		}
	}

	if (iedName.isEmpty())
	{
		LOGERROR("获取CID文件IEDName失败");
		return false;
	}

	// 根据IedName在Subnetwork中查看有几个子网
	SBaseConfig *commNode = xml.GetChildNode("Communication");
	if (commNode)
	{
		int netCount = commNode->GetChildCount("SubNetwork");
		for (int netId = 0; netId < netCount; ++netId)
		{
			SBaseConfig *netNode = commNode->GetChildNode(netId, "SubNetwork");
			SString type = netNode->GetAttribute("type");
			SString desc = netNode->GetAttribute("desc");
			SString name = netNode->GetAttribute("name");
			if (name.toUpper().find("MMS") >= 0 || desc.toUpper().find("MMS") >= 0 || type.toUpper().find("MMS") >= 0)
			{
				bool isNetworkA = false;
				if (desc.find("A") >= 0 || name.find("A") >= 0)
				{
					isNetworkA = true;
				}
				else if (desc.find("B") >= 0 || name.find("B") >= 0)
				{
					isNetworkA = false;
				}
				else
					continue;

				for (std::vector<Ied>::iterator iter = iedList.begin(); iter != iedList.end(); ++iter)
				{
					SBaseConfig *netIedNode = netNode->GetChildNode("ConnectedAP", "iedName", iter->name);
					SBaseConfig *addrNode = netIedNode ? netIedNode->GetChildNode("Address") : NULL;
					if (addrNode)
					{
						SBaseConfig *node = addrNode->GetChildNode("P", "type", "IP");
						if (node)
						{
							SString ipaddr = node->GetNodeValue();
							if (isNetworkA)
								iter->ipa = ipaddr;
							else
								iter->ipb = ipaddr;
						}
					}
				}
			}
		}
	}

	return true;
}
