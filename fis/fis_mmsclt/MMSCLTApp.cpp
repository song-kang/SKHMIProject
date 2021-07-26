#include "MMSCLTApp.h"
#include "MMSClientSubStation.h"
#include "MMSClientSubStationNW.h"
#include "MMSClientMainStation.h"
#include "NaModel.h"

MMSCLTApp::MMSCLTApp(void)
{
	SetApplicationId(UK9010_APP_MMSCLT);

	m_pDatabase = new CSsp_Database();			//数据库配置文件
	
	node_no = 0;
	is_period = false;
	report_id = 1;
	substation_id = 0;
	m_runMode = RunModeUnknown;
	m_regionalSpec = RS_Default;
	m_globalConfig.paramchk_enabled = false;
	m_globalConfig.syncNetworkTopologyFileEnabled = false;
}

MMSCLTApp::~MMSCLTApp(void)
{
	while(m_MMSMgrs.count() > 0)
		SApi::UsSleep(500000);	//等待client全部析构
	
// 	map<string,t_reference*>::iterator iter;
// 	for (iter = map_reference.begin( ); iter != map_reference.end( ); iter++ )
// 		delete iter->second;
// 	map_reference.clear();

	delete m_pDatabase;
	CConfigMgr::Quit();
}

bool MMSCLTApp::Start()
{
	if(!CUK9010Application::Start())
	{
		LOGERROR("加载单元配置失败!");
		return false;
	}

	//加载数据库配置
	if (!m_pDatabase->Load(GetConfPath()+"sys_database.xml"))
	{
		LOGFAULT("数据库配置文件打开失败!file:%s", m_pDatabase->GetPathFile().data());
		return false;
	}

	if (!LoadGlobalConfig())
		return false;

	if (!LoadParam())
		return false;

	if (!LoadSubstation(substation_id))
		return false;
	
	//if (!LoadIed())
	//{
	//	LOGFAULT("从运行设备列表中加载设备至内存失败。");
	//	return false;
	//}

	return true;
}

bool MMSCLTApp::Stop()
{
	for (int i = 0; i < m_MMSMgrs.count(); i++)
		delete m_MMSMgrs.at(i);

	m_MMSMgrs.clear();

	SMmsClient::ExitGlobal();

	return true;
}

bool MMSCLTApp::LoadParam()
{
	SString			sql;
	SRecordset		rs;
	int				iRet;

	node_no = GetParamI("n");
	SetApplicationId(UK9010_APP_MMSCLT,node_no);

	sql = SString::toFormat("select module_name,description from t_oe_run_node where node_no=%d",node_no);
	iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_run_node表中未找到节点号 = %d 的数据。",node_no);
		return false;
	}
	else if (iRet > 0)
	{
		SString module_name = rs.GetValue(0,0);
		if (module_name != MODULE_NAME)
		{	
			LOGFAULT("数据库t_oe_run_node表中节点号 = %d 的运行模块名不是[%s]。",node_no,MODULE_NAME);
			return false;
		}

		LOGDEBUG("运行程序节点[%d]，描述[%s]。",node_no,rs.GetValue(0,1).data());
	}

	sql = SString::toFormat("select param_name,current_val from t_oe_module_param "
		"where node_no=%d and module_name='%s'",node_no,MODULE_NAME);
	iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_module_param表中未找到节点号=%d，程序名=%s的数据。",node_no,MODULE_NAME);
		return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			SString param = rs.GetValue(i,0);
			if (param == "is_period")
			{
				if (rs.GetValue(i,1).toInt() == 1 || rs.GetValue(i,1).CompareNoCase("true") == 0)
					is_period = true;
				else
					is_period = false;
			}
			else if (param == "report_id")
				report_id = rs.GetValue(i,1).toInt();
			else if (param == "substation_id")
				substation_id = rs.GetValue(i,1).toInt();
			else if (param == "run_mode")
				m_runMode = (RunMode)rs.GetValue(i,1).toInt();
			else if (param == "region_specification")
				m_regionalSpec = (RegionalSpecification)rs.GetValue(i,1).toInt();
			else if (param.CompareNoCase("SyncNetworkTopologyFileEnabled") == 0)
			{
				if (rs.GetValue(i,1).toInt() == 1 || rs.GetValue(i,1).CompareNoCase("true") == 0)
					m_globalConfig.syncNetworkTopologyFileEnabled = true;
				else
					m_globalConfig.syncNetworkTopologyFileEnabled = false;
			}
		}

		LOGDEBUG("运行程序参数：周期上送是否生效[%d]，注册报告号[%d]，运维子站号[%d]。",is_period,report_id,substation_id);
	}

	return true;
}

bool MMSCLTApp::LoadSubstation(int sub_no)
{
	SString			sql;
	SRecordset		rs;
	int				iRet;
	MMSClient		*cli;
	SString			sub_name;

	sql.sprintf("select name from t_cim_substation where sub_no=%d",sub_no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("数据库t_cim_substation表中未找到子站[%d]的数据。",sub_no);
		return false;
	}
	else if (cnt > 0)
	{
		sub_name = rs.GetValue(0,0);
	}

	sql = SString::toFormat("select ied_no from t_oe_run_device where node_no=%d", node_no);//and ied_no=3
	iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_ied表中未找到节点号=%d,子站号 = %d 的数据。", node_no, sub_no);
		return false;
	}

	SString ieds;
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		ieds += rs.GetValue(row, 0);

		if (row + 1 != rs.GetRows())
			ieds += ",";
	}

	sql = SString::toFormat("select ied_no, name, mms_path from t_oe_ied where sub_no=%d and ied_no in (%s)",sub_no, ieds.data());//and ied_no=3
	iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_ied表中未找到子站号 = %d 的数据。",sub_no);
		return false;
	}

	bool ret = false;
	if (m_runMode == RunModeSubStation)
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			int iedNo = rs.GetValue(i, 0).toInt();
			SString iedDesc = rs.GetValue(i, 1);
			SString iedName = rs.GetValue(i, 2);

			if (m_regionalSpec == RS_NWIR)
				cli = new MMSClientSubStationNW(this, node_no, sub_no, sub_name, iedNo, iedDesc, iedName);
			else
				cli = new MMSClientSubStation(this, node_no, sub_no, sub_name, iedNo, iedDesc, iedName);
			cli->setReportID(this->report_id);
			cli->setIsPeriod(this->is_period);
			cli->setGlobalConfig(&m_globalConfig);

			if (!cli->Init())
			{
				LOGFAULT("数据库t_oe_ied表中装置号 = %d 的装置初始化失败。", iedNo);
				delete cli;
				continue;
			}

			if (cli->Start())
				m_MMSMgrs.append(cli);
		}

		ret = m_MMSMgrs.count() > 0 ? true : false;
	}
	else if (m_runMode == RunModeMainStation)
	{
		cli = new MMSClientMainStation(this, node_no, sub_no, sub_name);
		cli->setReportID(this->report_id);
		cli->setIsPeriod(this->is_period);
		cli->setGlobalConfig(&m_globalConfig);
		
		if (!cli->Init())
		{
			delete cli;
			return false;
		}

		if (cli->Start())
		{
			m_MMSMgrs.append(cli);

			LOGDEBUG("%s 同步网络拓扑文件功能", m_globalConfig.syncNetworkTopologyFileEnabled ? "启用" : "禁用");
			if (m_globalConfig.syncNetworkTopologyFileEnabled)
			{
				S_CREATE_THREAD(ThreadReadFile,this);
			}
		}

		ret = m_MMSMgrs.count() > 0 ? true : false;
	}
	else
	{
		LOGERROR("未知的运行模式(%d)", m_runMode);
		return false;
	}

	LOGDEBUG("%s 定值巡检功能", m_globalConfig.paramchk_enabled ? "启用" : "禁用");
	LOGDEBUG("%s 模拟量通道监视功能", m_globalConfig.analogChannelMonitorEnabled ? "启用" : "禁用");
	if (m_globalConfig.paramchk_enabled || m_globalConfig.analogChannelMonitorEnabled)
	{
		S_CREATE_THREAD(ThreadChannelMonitor, this);
	}
	
#ifdef TEST_CODE
	S_CREATE_THREAD(ThreadTestReadFile,this);
#endif
	return ret;
}

bool MMSCLTApp::LoadGlobalConfig()
{
	SRecordset rs;

	m_globalConfig.paramchk_enabled = true;
	m_globalConfig.analogChannelMonitorEnabled = false;
	SString sql = "select set_value from t_ssp_setup where set_key='paramchk.enabled'";
	DB->Retrieve(sql ,rs);
	if (rs.GetRows() > 0)
	{
		SString val = rs.GetValue(0, 0);
		if (val.toUpper().compare("TRUE") == 0 || val.toInt() == 1)
			m_globalConfig.paramchk_enabled = true;
		else
			m_globalConfig.paramchk_enabled = false;
	}

	sql = "select set_value from t_ssp_setup where set_key='analog.channel.monitor.enabled'";
	DB->Retrieve(sql ,rs);
	if (rs.GetRows() > 0)
	{
		SString val = rs.GetValue(0, 0);
		if (val.toUpper().compare("TRUE") == 0 || val.toInt() == 1)
			m_globalConfig.analogChannelMonitorEnabled = true;
		else
			m_globalConfig.analogChannelMonitorEnabled = false;
	}

	sql = "select set_value from t_ssp_setup where set_key='paramchk.type'";
	m_globalConfig.paramchk_type = DB->SelectIntoI(sql);
	
	sql = "select set_value from t_ssp_setup where set_key='paramchk.time'";
	m_globalConfig.paramchk_time = DB->SelectInto(sql);

	return true;
}

bool MMSCLTApp::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	switch (wMsgType)
	{
	case UK9010_MSG_REQ_SETTING:
		if (iLength != sizeof(stuUK9010_MSG_REQ_SETTING))
		{
			LOGWARN("获取代理申请定值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_SETTING));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_SETTING req_setting;
			memcpy(&req_setting,pBuffer,iLength);
			ProcessReqSetting(UK9010_MSG_RES_SETTING,pMsgHead,&req_setting);
		}
		break;
	case UK9010_MSG_REQ_SETTING_AREA:
		if (iLength != sizeof(stuUK9010_MSG_REQ_SETTING_AREA))
		{
			LOGWARN("获取代理申请定值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_SETTING_AREA));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING_AREA,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_SETTING_AREA req_setting_area;
			memcpy(&req_setting_area,pBuffer,iLength);
			ProcessReqSettingArea(UK9010_MSG_RES_SETTING_AREA,pMsgHead,&req_setting_area);
		}
		break;
	case UK9010_MSG_REQ_EDIT_SETTING:
		if (iLength != sizeof(stuUK9010_MSG_REQ_EDIT_SETTING))
		{
			LOGWARN("获取代理申请定值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_EDIT_SETTING));
			SendErrorAgentMsg(UK9010_MSG_RES_EDIT_SETTING,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_EDIT_SETTING req_edit_setting;
			memcpy(&req_edit_setting,pBuffer,iLength);
			ProcessReqEditSetting(UK9010_MSG_RES_EDIT_SETTING,pMsgHead,&req_edit_setting);
		}
		break;
	case UK9010_MSG_REQ_EDIT_SETTING_AREA:
		if (iLength != sizeof(stuUK9010_MSG_REQ_EDIT_SETTING_AREA))
		{
			LOGWARN("获取代理申请定值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_EDIT_SETTING_AREA));
			SendErrorAgentMsg(UK9010_MSG_RES_EDIT_SETTING_AREA,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_EDIT_SETTING_AREA req_edit_setting_area;
			memcpy(&req_edit_setting_area,pBuffer,iLength);
			ProcessReqEditSettingArea(UK9010_MSG_RES_EDIT_SETTING_AREA,pMsgHead,&req_edit_setting_area);
		}
		break;
	case UK9010_MSG_REQ_CURE_SETTING:
		if (iLength != sizeof(stuUK9010_MSG_REQ_CURE_SETTING))
		{
			LOGWARN("获取代理申请定值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_CURE_SETTING));
			SendErrorAgentMsg(UK9010_MSG_RES_CURE_SETTING,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_CURE_SETTING req_cure_setting;
			memcpy(&req_cure_setting,pBuffer,iLength);
			ProcessReqCureSetting(UK9010_MSG_RES_CURE_SETTING,pMsgHead,&req_cure_setting);
		}
		break;
	case UK9010_MSG_REQ_CTRL_SELECT:
		if (iLength != sizeof(stuUK9010_MSG_REQ_CTRL))
		{
			LOGWARN("获取代理申请控制选择，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_CTRL));
			SendErrorAgentMsg(UK9010_MSG_RES_CTRL_SELECT,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_CTRL req_ctrl;
			memcpy(&req_ctrl,pBuffer,iLength);
			ProcessReqCtrlSelect(UK9010_MSG_RES_CTRL_SELECT,pMsgHead,&req_ctrl);
		}
		break;
	case UK9010_MSG_REQ_CTRL_EXECUTE:
		if (iLength != sizeof(stuUK9010_MSG_REQ_CTRL))
		{
			LOGWARN("获取代理申请控制执行，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_CTRL));
			SendErrorAgentMsg(UK9010_MSG_RES_CTRL_EXECUTE,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_CTRL req_ctrl;
			memcpy(&req_ctrl,pBuffer,iLength);
			ProcessReqCtrlExecute(UK9010_MSG_RES_CTRL_EXECUTE,pMsgHead,&req_ctrl);
		}
		break;
	case UK9010_MSG_REQ_CTRL_CANCEL:
		if (iLength != sizeof(stuUK9010_MSG_REQ_CTRL))
		{
			LOGWARN("获取代理申请控制撤消，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_CTRL));
			SendErrorAgentMsg(UK9010_MSG_RES_CTRL_CANCEL,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_CTRL req_ctrl;
			memcpy(&req_ctrl,pBuffer,iLength);
			ProcessReqCtrlCancel(UK9010_MSG_RES_CTRL_CANCEL,pMsgHead,&req_ctrl);
		}
		break;
	case UK9010_MSG_REQ_CTRL_DIRECT:
		if (iLength != sizeof(stuUK9010_MSG_REQ_CTRL))
		{
			LOGWARN("获取代理申请直接控制，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_CTRL));
			SendErrorAgentMsg(UK9010_MSG_RES_CTRL_DIRECT,pMsgHead);
			return false;
		}
		else
		{
			stuUK9010_MSG_REQ_CTRL req_ctrl;
			memcpy(&req_ctrl,pBuffer,iLength);
			ProcessReqCtrlDirect(UK9010_MSG_RES_CTRL_DIRECT,pMsgHead,&req_ctrl);
		}
		break;
	case UK9010_MSG_REQ_FILE_LIST:
		if (iLength != sizeof(stuUK9010_MSG_REQ_FILE_LIST))
		{
			LOGWARN("获取代理申请文件列表请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_FILE_LIST));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_FILE_LIST req_file;
			memcpy(&req_file,pBuffer,iLength);
			ProcessReqFileList(UK9010_MSG_RES_FILE_LIST,pMsgHead,&req_file);
		}
		break;
	case UK9010_MSG_REQ_FILE_READ:
		if (iLength != sizeof(stuUK9010_MSG_REQ_FILE))
		{
			LOGWARN("获取代理申请文件列表请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_FILE));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_FILE req_file;
 			memcpy(&req_file,pBuffer,iLength);
			ProcessReqFileRead(UK9010_MSG_RES_FILE_READ,pMsgHead,&req_file);
		}
		break;
	case UK9010_MSG_REQ_JOURNAL_VMD_SPEC:
		if (iLength != sizeof(stuUK9010_MSG_REQ_JOURNAL_VMD_SPEC))
		{
			LOGWARN("获取代理申请文件列表请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_JOURNAL_VMD_SPEC));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_JOURNAL_VMD_SPEC req_journal_vmd_spec;
			memcpy(&req_journal_vmd_spec,pBuffer,iLength);
			ProcessReqJournalVmd(UK9010_MSG_RES_JOURNAL_VMD_SPEC,pMsgHead,&req_journal_vmd_spec);
		}
		break;
	case UK9010_MSG_REQ_JOURNAL_READ:
		if (iLength != sizeof(stuUK9010_MSG_REQ_JOURNAL_READ))
		{
			LOGWARN("获取代理申请文件列表请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_JOURNAL_READ));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_JOURNAL_READ req_journal_read;
			memcpy(&req_journal_read,pBuffer,iLength);
			ProcessReqJournalRead(UK9010_MSG_RES_JOURNAL_READ,pMsgHead,&req_journal_read);
		}
		break;
	case UK9010_MSG_REQ_JOURNAL_STATE_READ:
		if (iLength != sizeof(stuUK9010_MSG_REQ_JOURNAL_STATE_READ))
		{
			LOGWARN("获取代理申请文件列表请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_JOURNAL_STATE_READ));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_JOURNAL_STATE_READ req_journal_state_read;
			memcpy(&req_journal_state_read,pBuffer,iLength);
			ProcessReqJournalStateRead(UK9010_MSG_RES_JOURNAL_STATE_READ,pMsgHead,&req_journal_state_read);
		}
		break;

	case UK9010_MSG_REQ_LEAF_VALUE:
		if (iLength == sizeof(stuUK9010_MSG_REQ_READ_LEAF))
		{
			stuUK9010_MSG_REQ_READ_LEAF req_journal_state_read;
			memcpy(&req_journal_state_read,pBuffer,iLength);
			ProcessReqReadLeaf(UK9010_MSG_RES_LEAF_VALUE,pMsgHead,&req_journal_state_read);
		}
		else if (iLength == sizeof(stuUK9010_MSG_REQ_READ_LEAF_EX))
		{
			stuUK9010_MSG_REQ_READ_LEAF_EX req_journal_state_read;
			memcpy(&req_journal_state_read,pBuffer,iLength);
			ProcessReqReadLeaf(UK9010_MSG_RES_LEAF_VALUE,pMsgHead,&req_journal_state_read);
		}
		else
		{
			LOGWARN("获取代理申请节点数据请求，请求数据长度[%d]与结构长度[%d,%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_READ_LEAF),sizeof(stuUK9010_MSG_REQ_READ_LEAF_EX));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING,pMsgHead);
		}
		break;

	case UK9010_MSG_REQ_SETTING_CHECK:
		if (iLength != sizeof(stuUK9010_MSG_REQ_SETTING_CHECK))
		{
			LOGWARN("获取代理申请定值校核，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_SETTING_CHECK));
			SendErrorAgentMsg(UK9010_MSG_RES_SETTING_CHECK,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_SETTING_CHECK req_setting_check;
			memcpy(&req_setting_check,pBuffer,iLength);
			ProcessReqSettingCheck(UK9010_MSG_RES_SETTING_CHECK,pMsgHead,&req_setting_check);
		}
		break;

	case UK9010_MSG_REQ_LATEST_CID:
		if (iLength != sizeof(stuUK9010_MSG_REQ_LATEST_CID))
		{
			LOGWARN("获取代理申请定值校核，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_LATEST_CID));
			SendErrorAgentMsg(UK9010_MSG_RES_LATEST_CID,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_LATEST_CID req;
			memcpy(&req,pBuffer,iLength);
			LOGDEBUG("收到请求最新CID文件命令,站号(%d)设备号(%d)类型(%s)", req.iSubNo, req.iDevNo, req.iType == 0 ? "CID" : (req.iType == 1 ? "ICD" : "SCD"));
			ProcessReqLatestCid(UK9010_MSG_RES_LATEST_CID, pMsgHead,&req);
		}
		break;

	case UK9010_MSG_REQ_ANALOG:
		if (iLength != sizeof(stuUK9010_MSG_REQ_SETTING))
		{
			LOGWARN("获取代理申请遥测值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_SETTING));
			SendErrorAgentMsg(UK9010_MSG_RES_ANALOG,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_SETTING req_setting;
			memcpy(&req_setting,pBuffer,iLength);
			ProcessReqValue(UK9010_MSG_RES_ANALOG,pMsgHead,&req_setting);
		}
		break;

	case UK9010_MSG_REQ_SWITCH:
		if (iLength != sizeof(stuUK9010_MSG_REQ_SETTING))
		{
			LOGWARN("获取代理申请开关量值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_SETTING));
			SendErrorAgentMsg(UK9010_MSG_RES_SWITCH,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_SETTING req_setting;
			memcpy(&req_setting,pBuffer,iLength);
			ProcessReqValue(UK9010_MSG_RES_SWITCH,pMsgHead,&req_setting);
		}
		break;

	case UK9010_MSG_REQ_RYABAN:
		if (iLength != sizeof(stuUK9010_MSG_REQ_SETTING))
		{
			LOGWARN("获取代理申请压板值请求，请求数据长度[%d]与结构长度[%d]不一致。",iLength,sizeof(stuUK9010_MSG_REQ_SETTING));
			SendErrorAgentMsg(UK9010_MSG_RES_RYABAN,pMsgHead);
		}
		else
		{
			stuUK9010_MSG_REQ_SETTING req_setting;
			memcpy(&req_setting,pBuffer,iLength);
			ProcessReqValue(UK9010_MSG_RES_RYABAN,pMsgHead,&req_setting);
		}
		break;

	default:
		break;
	}

	return true;
}

bool MMSCLTApp::SendErrorAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *head,SString strMsg)
{
	stuSpUnitAgentProcessId dst_id = head->m_SrcId;

	bool ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_FAIL,0,0,&strMsg);
	if (!ret)
		LOGWARN("发送错误回复命令失败。");

	return ret;
}

MMSClient* MMSCLTApp::GetMmsClientBySubstation(int sub_no)
{
	MMSClient *cli = 0;

	for (int i = 0; i < m_MMSMgrs.count(); i++)
	{
		cli = m_MMSMgrs.at(i);
		if (cli->getSubNo() == sub_no)
			return cli;
		}

	return 0;
}

MMSClient * MMSCLTApp::GetMmsClientByIedNo(int sub_no, int ied_no)
{
	MMSClient *cli = NULL;

	for (int i = 0; i < m_MMSMgrs.count(); i++)
	{
		cli = m_MMSMgrs.at(i);
		if (cli->getSubNo() == sub_no && cli->getIed(ied_no))
		{
			return cli;
		}
	}

	return NULL;
}

MMSClient * MMSCLTApp::GetMmsClient(int sub_no, int ied_no)
{
	MMSClient *cli = NULL;
	if (m_runMode == RunModeMainStation)
		cli = GetMmsClientBySubstation(sub_no);
	else if (m_runMode == RunModeSubStation)
		cli = GetMmsClientByIedNo(sub_no, ied_no);

	return cli;
}

bool MMSCLTApp::ProcessReqSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING *req_setting)
{
	MMSClient *cli = GetMmsClient(req_setting->iSubNo, req_setting->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请定值数据中的子站号[%d]。",req_setting->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	time_t t1 = ::time(NULL);
	if (req_setting->iSectorNo == -1) //当前区
		ret = cli->ReadCurrentAreaSetting(req_setting->iDevNo,req_setting->iCpuNo,req_setting->iGroupNo,MMSCLT_TIMEOUT);
	else if (req_setting->iSectorNo == -2) //编辑区
		ret = cli->ReadEditAreaSetting(req_setting->iDevNo,req_setting->iCpuNo,req_setting->iGroupNo,MMSCLT_TIMEOUT);
	else if (req_setting->iSectorNo >= 0) //指定区
		ret = cli->ReadAppointAreaSetting(req_setting->iDevNo,req_setting->iCpuNo,req_setting->iGroupNo,MMSCLT_TIMEOUT);
	time_t t2 = ::time(NULL);

	LOGDEBUG("获取%s定值%s,耗时%d秒"
		, req_setting->iSectorNo == -1 ? "当前区" : (req_setting->iSectorNo == -2 ? "编辑区" : "指定区")
		, ret ? "成功" : "失败"
		, (int)t2 - (int)t1);

	if (ret)
	{
		stuUK9010_MSG_RES_SETTING res_setting;
		res_setting.iSubNo = req_setting->iSubNo;
		res_setting.iDevNo = req_setting->iDevNo;
		res_setting.iCpuNo = req_setting->iCpuNo;
		res_setting.iGroupNo = req_setting->iGroupNo;
		res_setting.iEntryNo = req_setting->iEntryNo;
		res_setting.iSectorNo = req_setting->iSectorNo;
		memset(res_setting.visable_value,0,sizeof(res_setting.visable_value));
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,
			SP_UA_MSG_RESULT_SUCCESS,(BYTE*)&res_setting,sizeof(stuUK9010_MSG_RES_SETTING));
		if (!ret)
		{
			LOGWARN("发送申请定值回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqSettingArea(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING_AREA *req_setting_area)
{
	MMSClient *cli = GetMmsClient(req_setting_area->iSubNo, req_setting_area->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请定值区数据中的子站号[%d]。",req_setting_area->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	int  area = 0;
	if (req_setting_area->iSectorNo == -1) //当前区
		ret = cli->GetCurrentArea(req_setting_area->iDevNo,area,MMSCLT_TIMEOUT);
	else if (req_setting_area->iSectorNo == -2) //编辑区
		ret = cli->GetEditArea(req_setting_area->iDevNo,area,MMSCLT_TIMEOUT);

	LOGDEBUG("获取%s定值区%s"
		, req_setting_area->iSectorNo == -1 ? "当前区" : "编辑区"
		, ret ? "成功" : "失败");

	if (ret)
	{
		stuUK9010_MSG_RES_SETTING_AREA res_setting_area;
		res_setting_area.iSubNo = req_setting_area->iSubNo;
		res_setting_area.iDevNo = req_setting_area->iDevNo;
		res_setting_area.iCpuNo = req_setting_area->iCpuNo;
		res_setting_area.iSectorNo = req_setting_area->iSectorNo;
		memset(res_setting_area.visable_value,0,sizeof(res_setting_area.visable_value));
		memcpy(res_setting_area.visable_value,&area,sizeof(int));
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,
			SP_UA_MSG_RESULT_SUCCESS,(BYTE*)&res_setting_area,sizeof(stuUK9010_MSG_RES_SETTING_AREA));
		if (!ret)
		{
			LOGWARN("发送申请定值区回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqEditSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_EDIT_SETTING *req_edit_setting)
{
	MMSClient *cli = GetMmsClient(req_edit_setting->iSubNo, req_edit_setting->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请修改定值数据中的子站号[%d]。",req_edit_setting->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	SString value = req_edit_setting->visable_value;
	ret = cli->WriteSettingValue(req_edit_setting->iDevNo,req_edit_setting->iCpuNo,
		req_edit_setting->iGroupNo,req_edit_setting->iEntryNo,
		req_edit_setting->iSectorNo,value,MMSCLT_TIMEOUT);

	if (ret)
	{
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,0,0);
		if (!ret)
		{
			LOGWARN("发送申请修改定值回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqEditSettingArea(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_EDIT_SETTING_AREA *req_edit_setting_area)
{
	MMSClient *cli = GetMmsClient(req_edit_setting_area->iSubNo, req_edit_setting_area->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请修改定值区数据中的子站号[%d]。",req_edit_setting_area->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	if (req_edit_setting_area->iSectorNo == -1) //当前区
		ret = cli->SetCurrentArea(req_edit_setting_area->iDevNo,req_edit_setting_area->iArea,MMSCLT_TIMEOUT);
	else if (req_edit_setting_area->iSectorNo == -2) //编辑区
		ret = cli->SetEditArea(req_edit_setting_area->iDevNo,req_edit_setting_area->iArea,MMSCLT_TIMEOUT);

	if (ret)
	{
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,0,0);
		if (!ret)
		{
			LOGWARN("发送申请修改定值区回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqCureSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CURE_SETTING *req_cure_setting)
{
	MMSClient *cli = GetMmsClient(req_cure_setting->iSubNo, req_cure_setting->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请定值固化数据中的子站号[%d]。",req_cure_setting->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	ret = cli->CureSettingValue(req_cure_setting->iDevNo,req_cure_setting->iCpuNo,req_cure_setting->iGroupNo,MMSCLT_TIMEOUT);

	if (ret)
	{
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,0,0);
		if (!ret)
		{
			LOGWARN("发送申请修改定值回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqCtrlSelect(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl)
{
	MMSClient *cli = GetMmsClient(req_ctrl->iSubNo, req_ctrl->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请控制选择数据中的子站号[%d]。",req_ctrl->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	if (req_ctrl->visable_value[0] != 1 && req_ctrl->visable_value[0] != 2)
	{
		LOGWARN("申请控制选择命令中，控制值异常[%d]。",req_ctrl->visable_value[0]);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}
	int controlValue = req_ctrl->visable_value[0];
	ret = cli->ControlSelect(req_ctrl->iDevNo,req_ctrl->iCpuNo,req_ctrl->iGroupNo,req_ctrl->iEntryNo,controlValue,MMSCLT_TIMEOUT);
	if (ret)
	{
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,0,0);
		if (!ret)
		{
			LOGWARN("发送控制选择回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqCtrlExecute(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl)
{
	MMSClient *cli = GetMmsClient(req_ctrl->iSubNo, req_ctrl->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请控制执行数据中的子站号[%d]。",req_ctrl->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	if (req_ctrl->visable_value[0] != 1 && req_ctrl->visable_value[0] != 2)
	{
		LOGWARN("申请控制执行命令中，控制值异常[%d]。",req_ctrl->visable_value[0]);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}
	int controlValue = req_ctrl->visable_value[0];
	ret = cli->ControlExecute(req_ctrl->iDevNo,req_ctrl->iCpuNo,req_ctrl->iGroupNo,req_ctrl->iEntryNo,controlValue,MMSCLT_TIMEOUT);
	if (ret)
	{
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,0,0);
		if (!ret)
		{
			LOGWARN("发送控制执行回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqCtrlCancel(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl)
{
	MMSClient *cli = GetMmsClient(req_ctrl->iSubNo, req_ctrl->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请控制撤消数据中的子站号[%d]。",req_ctrl->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	if (req_ctrl->visable_value[0] != 1 && req_ctrl->visable_value[0] != 2)
	{
		LOGWARN("申请控制撤消命令中，控制值异常[%d]。",req_ctrl->visable_value[0]);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}
	int controlValue = req_ctrl->visable_value[0];
	ret = cli->ControlCancel(req_ctrl->iDevNo,req_ctrl->iCpuNo,req_ctrl->iGroupNo,req_ctrl->iEntryNo,controlValue,MMSCLT_TIMEOUT);
	if (ret)
	{
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,0,0);
		if (!ret)
		{
			LOGWARN("发送控制撤消回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqCtrlDirect(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl)
{
	MMSClient *cli = GetMmsClient(req_ctrl->iSubNo, req_ctrl->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请直接控制数据中的子站号[%d]。",req_ctrl->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	if (req_ctrl->visable_value[0] == -1)
		ret = cli->ResetLed(req_ctrl->iDevNo);
	else if (req_ctrl->visable_value[0] == -2)
		ret = cli->ResetIed(req_ctrl->iDevNo);
	else
	{
		// 空调遥控含多种模式下发，数据类型为Int，忽略判断 
// 		if (req_ctrl->visable_value[0] != 0 && req_ctrl->visable_value[0] != 1)
// 		{
// 			LOGWARN("申请直控命令中，控制值异常[%d],应为0或1。", req_ctrl->visable_value[0]);
// 			SendErrorAgentMsg(wMsgType, head);
// 			return false;
// 		}
		int controlValue = req_ctrl->visable_value[0];
		ret = cli->ControlDirect(req_ctrl->iDevNo, req_ctrl->iCpuNo, req_ctrl->iGroupNo, req_ctrl->iEntryNo, controlValue, MMSCLT_TIMEOUT);
	}
	if (ret)
	{
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,0,0);
		if (!ret)
		{
			LOGWARN("发送直接控制回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqFileList(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_FILE_LIST *req_file)
{
	MMSClient *cli = GetMmsClient(req_file->iSubNo, req_file->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请文件列表数据中的子站号[%d]。",req_file->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	SPtrList<stuSMmsFileInfo> files;
	files.setAutoDelete(true);
	SString m_path = SString::toFormat("%s",req_file->visable_value);
	int ret = cli->GetMvlDirectory(m_path,files,false);
	if (ret >= 0)
	{
		std::vector<stuUK9010_MSG_RES_FILE_LIST> fileList;

		for (int i = 0; i < files.count(); i++)
		{
			stuUK9010_MSG_RES_FILE_LIST file;
			memset(file.fileName,0,sizeof(file.fileName));
			memcpy(file.fileName,files.at(i)->filename.data(),files.at(i)->filename.size());
			file.fileSize = files.at(i)->fsize;
			file.fileTime = files.at(i)->mtime;
			file.follow = false;

			if (file.fileTime < req_file->socFrom || file.fileTime > req_file->socTo)
			{
				continue;
			}

			fileList.push_back(file);
		}

		if (fileList.empty())
		{
			ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS, NULL, 0);
			if (!ret)
			{
				LOGWARN("发送申请文件列表回复命令失败。");
				return false;
			}
			else
			{
				LOGDEBUG("请求列表%s成功,数量为 0", req_file->visable_value);
			}
		}

		for (size_t i = 0; i < fileList.size(); ++i)
		{
			stuUK9010_MSG_RES_FILE_LIST file = fileList.at(i);
			if (i == fileList.size() - 1)
				file.follow = false;
			else
				file.follow = true;
			ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
				(BYTE*)&file,sizeof(stuUK9010_MSG_RES_FILE_LIST));
			if (!ret)
			{
				LOGWARN("发送申请文件列表回复命令失败。");
				return false;
			}
			else
			{
				LOGDEBUG("发送文件列表，%s(%dB)。",file.fileName,file.fileSize);
			}
		}
	}
	else
	{
		if (ret == -1)
		{
			LOGERROR("通信未连接");
		}
		else if (ret == -1)
		{
			LOGERROR("召唤失败");
		}
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqFileRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_FILE *req_file)
{
	MMSClient *cli = GetMmsClient(req_file->iSubNo, req_file->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请读取文件数据中的子站号[%d]。",req_file->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	SString m_file = SString::toFormat("%s",req_file->visable_value);
	SString local_file = m_file;
	int count = SString::GetAttributeCount(local_file,"/");
	if (count)
		local_file = SString::GetIdAttribute(count,local_file,"/");
	//local_file = local_file.replace("/","-");
	//local_file = local_file.replace("\\","-");
	//local_file = local_file.replace("*","-");
	//local_file = local_file.replace(":","-");
	//local_file = local_file.replace("?","-");
	//local_file = local_file.replace("<","-");
	//local_file = local_file.replace(">","-");
	//local_file = local_file.replace("|","-");
	//local_file = local_file.replace("\"","-");
	SString gb_name = local_file.toGb2312();

	SString path = SString::toFormat("%s/data", SBASE_SAPP->GetHomePath().data());
	if (!SDir::dirExists(path))
	{
		SDir::createDir(path);
	}

	path += SString::toFormat("/%d", req_file->iSubNo);
	if (!SDir::dirExists(path))
	{
		SDir::createDir(path);
	}

	path += SString::toFormat("/%d", req_file->iDevNo);
	if (!SDir::dirExists(path))
	{
		SDir::createDir(path);
	}

	bool ret = cli->DownMvlFile(m_file,SString::toFormat("%s/%s",path.data(), gb_name.data()));
	LOGDEBUG("读取文件(%s)到本地(%s/%s)%s", m_file.data(), path.data(), gb_name.data(), ret ? "成功" : "失败");
	if (ret)
	{
		stuUK9010_MSG_RES_FILE msg_res_file = {0};
		strcpy(msg_res_file.local_path, SString::toFormat("%s/%s",path.data(), gb_name.data()).data());
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS, 
			(BYTE*)&msg_res_file, sizeof(stuUK9010_MSG_RES_FILE));
		if (!ret)
		{
			LOGWARN("发送申请读取文件回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

bool MMSCLTApp::ProcessReqJournalVmd(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_VMD_SPEC *req_journal_vmd_spec)
{
	MMSClient *cli = GetMmsClient(req_journal_vmd_spec->iSubNo, req_journal_vmd_spec->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请日志值名数据中的子站号[%d]。",req_journal_vmd_spec->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	SString		sql;
	SRecordset	rs;
	SRecordset	rs1;

	sql = SString::toFormat("select reportcontrol_ln,reportcontrol_name,cpu_no from t_oe_group "
		"where ied_no=%d and logcontrol_logEna='true'",req_journal_vmd_spec->iDevNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}
	else if (iRet > 0)
	{
		bool ret;
		SPtrList<MMSValue> value;
		for (int i = 0; i < iRet; i++)
		{
			value.clear();
			SString dom_name;
			sql.sprintf("select t_oe_ied.mms_path,t_oe_cpu.mms_path from t_oe_ied,t_oe_cpu "
				"where t_oe_ied.ied_no=%d and t_oe_cpu.cpu_no=%d and t_oe_ied.ied_no=t_oe_cpu.ied_no",
				req_journal_vmd_spec->iDevNo,rs.GetValue(i,2).toInt());
			int iRet1 = DB->Retrieve(sql,rs1);
			if (iRet1 > 0)
				dom_name = rs1.GetValue(0,0)+rs1.GetValue(0,1);
			else
				continue;

			//SString var = rs.GetValue(i,0)+"$LG$"+rs.GetValue(i,1)+"$LogRef";
			SString var = rs.GetValue(i,0)+"$LG$"+rs.GetValue(i,1);
			ret = cli->ReadValue(dom_name.data(),var.data(),value);
			if (ret)
			{
				MMSValue *m_logRef = 0;
				for (int j = 0; j < value.count(); j++)
				{
					if (value.at(j)->getName() == "LogRef")
						m_logRef = value.at(j);
				}
				if (!m_logRef)
					continue;

				stuUK9010_MSG_RES_JOURNAL_VMD_SPEC res_journal_vmd_spec;
				memset(res_journal_vmd_spec.domName,0,sizeof(res_journal_vmd_spec.domName));
				memcpy(res_journal_vmd_spec.domName,dom_name,strlen(dom_name));
				//SString var = SString::GetIdAttribute(2,value.at(0)->getStrValue(),"/");
				SString strValue = m_logRef->getStrValue();
				SString var = SString::GetIdAttribute(2, strValue, "/");
				memset(res_journal_vmd_spec.varName,0,sizeof(res_journal_vmd_spec.varName));
				memcpy(res_journal_vmd_spec.varName,var.data(),var.size());
				if (i == iRet-1)
					res_journal_vmd_spec.follow = false;
				else
					res_journal_vmd_spec.follow = true;
				
				ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
					(BYTE*)&res_journal_vmd_spec,sizeof(stuUK9010_MSG_RES_JOURNAL_VMD_SPEC));
				if (!ret)
				{
					LOGWARN("发送申请日志值名回复命令失败。");
				}
			}
		}
	}

	return true;
}

bool MMSCLTApp::ProcessReqJournalRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_READ *req_journal_read)
{
	MMSClient *cli = GetMmsClient(req_journal_read->iSubNo, req_journal_read->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请日志值名数据中的子站号[%d]。",req_journal_read->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	int type = 0;
	bool ret = false;
	SDateTime startDt(req_journal_read->socStart);
	SDateTime endDt(req_journal_read->socEnd);
	SPtrList<stuJournalData> lstJData;
	bool follow = true;
	bool ret_follow = false;
	int ret_cnt = 0;
	int i,j;
	while(follow)
	{
		lstJData.clear();
		ret = cli->ReadJournal(req_journal_read->domName,req_journal_read->varName,startDt,endDt,req_journal_read->entry_id,lstJData,ret_follow);
		if (ret)
		{
			stuJournalData *jd;
			stuUK9010_MSG_RES_JOURNAL_READ res_journal_read;
			for (i = 0; i < lstJData.count(); i++)
			{
				jd = lstJData.at(i);
				
				SString		sql;
				SRecordset	rs;
				int			iRet;
				type = 0;
				sql.sprintf("select name from t_oe_element_state where ied_no=%d and mms_path='%s'",
					req_journal_read->iDevNo,jd->ref.data());
				iRet = DB->Retrieve(sql,rs);
				if (iRet <= 0)
				{
					sql.sprintf("select name from t_oe_element_general where ied_no=%d and mms_path='%s'",
						req_journal_read->iDevNo,jd->ref.data());
					iRet = DB->Retrieve(sql,rs);
					if (iRet <= 0)
						continue;
					else
						type = 1;
				}
				SString name = rs.GetValue(0,0);
				memset(res_journal_read.desc,0,sizeof(res_journal_read.desc));
				memcpy(res_journal_read.desc,name.data(),name.size());

				memset(res_journal_read.entry_id,0,sizeof(res_journal_read.entry_id));
				memcpy(res_journal_read.entry_id,jd->entry_id,sizeof(jd->entry_id));

				if (type == 0)	//遥信值
				{
					for (j = 0; j < jd->value.count(); j++)
					{
						if (jd->value.at(j)->getType() == TYPE_BOOL)
						{
							memset(res_journal_read.visable_value,0,sizeof(res_journal_read.visable_value));
							memcpy(res_journal_read.visable_value,jd->value.at(j)->getStrValue().data(),jd->value.at(j)->getStrValue().size());
						}
						if (jd->value.at(j)->getType() == TYPE_UTC_TIME)
						{
							res_journal_read.soc = jd->value.at(j)->getUint32Value();
							res_journal_read.usec = jd->value.at(j)->getInt32Value();
						}
					}
				}
				else if (type == 1)	//测量值
				{
					for (j = 0; j < jd->value.count(); j++)
					{
						switch (jd->value.at(j)->getType())
						{
						case TYPE_INT8:
						case TYPE_INT16:
						case TYPE_INT32:
						case TYPE_UINT8:
						case TYPE_UINT16:
						case TYPE_UINT32:
						case TYPE_FLOAT:
						case TYPE_UTF8_STRING:
							memset(res_journal_read.visable_value,0,sizeof(res_journal_read.visable_value));
							memcpy(res_journal_read.visable_value,jd->value.at(j)->getStrValue().data(),jd->value.at(j)->getStrValue().size());
							break;
						}
					}
				}

				if (i == lstJData.count()-1 && ret_follow == false)
					res_journal_read.follow = false;
				else
					res_journal_read.follow = true;

				if (++ret_cnt >= 5000)
				{
					ret_follow = false;
					res_journal_read.follow = false;
				}

				ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
					(BYTE*)&res_journal_read,sizeof(stuUK9010_MSG_RES_JOURNAL_READ));
				if (!ret)
				{
					LOGWARN("发送申请日志值名回复命令失败。");
				}
			}
		}

		follow = ret_follow;
	}
	
	return true;
}

bool MMSCLTApp::ProcessReqJournalStateRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_STATE_READ *req_journal_state_read)
{
	MMSClient *cli = GetMmsClient(req_journal_state_read->iSubNo, req_journal_state_read->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请日志值名数据中的子站号[%d]。",req_journal_state_read->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	unsigned long entries = 0;
	bool deletable = false;
	stuUK9010_MSG_RES_JOURNAL_STATE_READ res_journal_state_read;
	res_journal_state_read.entryNum = entries;
	res_journal_state_read.deletable = deletable;
	ret = cli->GetJournalState(req_journal_state_read->domName,req_journal_state_read->varName,entries,deletable);
	if (ret)
	{
		res_journal_state_read.entryNum = entries;
		res_journal_state_read.deletable = deletable;
	}

	//res_journal_state_read.ret = ret;
	res_journal_state_read.ret = true;	//临时改为true

	ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
		(BYTE*)&res_journal_state_read,sizeof(stuUK9010_MSG_RES_JOURNAL_STATE_READ));
	if (!ret)
	{
		LOGWARN("发送申请日志状态回复命令失败。");
	}

	return true;
}

bool MMSCLTApp::ProcessReqReadLeaf(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_READ_LEAF_EX *req)
{
	MMSClient *cli = GetMmsClient(req->iSubNo, req->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现读取节点值数据中的子站号[%d]。",req->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	unsigned long entries = 0;
	stuUK9010_MSG_RES_READ_LEAF_EX resp;
	resp.iCommandNo = req->iCommandNo;
	SString value;
	ret = cli->ReadLeafValue(req->domName, req->varName, resp.type, value, 3);
	if (ret)
	{
		strcpy(resp.value, value.data());
	}
	resp.ret = ret;

	ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
		(BYTE*)&resp, sizeof(stuUK9010_MSG_RES_READ_LEAF_EX));
	if (!ret)
	{
		LOGWARN("发送读取节点值命令失败。");
	}

	return true;
}

bool MMSCLTApp::ProcessReqReadLeaf(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_READ_LEAF *req)
{
	MMSClient *cli = GetMmsClient(req->iSubNo, req->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现读取节点值数据中的子站号[%d]。",req->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	unsigned long entries = 0;
	stuUK9010_MSG_RES_READ_LEAF resp;
	SString value;
	ret = cli->ReadLeafValue(req->domName, req->varName, resp.type, value);
	if (ret)
	{
		strcpy(resp.value, value.data());
	}
	resp.ret = ret;

	ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
		(BYTE*)&resp, sizeof(stuUK9010_MSG_RES_READ_LEAF));
	if (!ret)
	{
		LOGWARN("发送读取节点值命令失败。");
	}

	return true;
}

bool MMSCLTApp::ProcessReqSettingCheck(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_SETTING_CHECK *req)
{
	MMSClient *cli = GetMmsClient(req->iSubNo, req->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现读取节点值数据中的子站号[%d]。",req->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

 	bool ret = false;
 	unsigned long entries = 0;
 	stuUK9010_MSG_RES_SETTING_CHECK resp;

	// 装置读取定值并入库
	ret = cli->ReadParamCheckValue(req->iDevNo, req->iCpuNo, req->iGroupNo, true);
	if (!ret)
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	resp.ret = ret;
	ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
		(BYTE*)&resp, sizeof(stuUK9010_MSG_RES_SETTING_CHECK));
	if (!ret)
	{
		LOGWARN("发送读取节点值命令失败。");
	}

	return true;
}

bool MMSCLTApp::ProcessReqLatestCid(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_LATEST_CID *req)
{
	MMSClient *cli = GetMmsClient(req->iSubNo, req->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现读取节点值数据中的子站号[%d]。",req->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	unsigned long entries = 0;
	stuUK9010_MSG_RES_LATEST_CID resp;
	SString downloadFile;

	// 装置读取定值并入库
	ret = cli->GetLatestModelFile(req->iDevNo, (ModelFileFormat)req->iType, downloadFile);
	if (!ret)
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	resp.ret = ret;
	strcpy(resp.path, downloadFile.data());
	ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,SP_UA_MSG_RESULT_SUCCESS,
		(BYTE*)&resp, sizeof(stuUK9010_MSG_RES_LATEST_CID));
	if (!ret)
	{
		LOGWARN("发送请求最新CID文件回复命令失败。");
	}

	return true;
}

bool MMSCLTApp::ProcessReqValue(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING *req_setting)
{
	MMSClient *cli = GetMmsClient(req_setting->iSubNo, req_setting->iDevNo);
	if (!cli)
	{
		LOGWARN("子站管理队列中，未发现申请值数据中的子站号[%d]。",req_setting->iSubNo);
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	bool ret = false;
	time_t t1 = ::time(NULL);
	ret = cli->ReadValueSetting(req_setting->iDevNo,req_setting->iCpuNo,req_setting->iGroupNo,MMSCLT_TIMEOUT);
	time_t t2 = ::time(NULL);

	LOGDEBUG("获取数据值%s,耗时%d秒"
		, ret ? "成功" : "失败"
		, (int)t2 - (int)t1);

	if (ret)
	{
		stuUK9010_MSG_RES_SETTING res_setting;
		res_setting.iSubNo = req_setting->iSubNo;
		res_setting.iDevNo = req_setting->iDevNo;
		res_setting.iCpuNo = req_setting->iCpuNo;
		res_setting.iGroupNo = req_setting->iGroupNo;
		res_setting.iEntryNo = req_setting->iEntryNo;
		res_setting.iSectorNo = req_setting->iSectorNo;
		memset(res_setting.visable_value,0,sizeof(res_setting.visable_value));
		ret = SApplication::GetPtr()->SendAgentMsg(&head->m_SrcId,wMsgType,head->m_dwMsgSn,
			SP_UA_MSG_RESULT_SUCCESS,(BYTE*)&res_setting,sizeof(stuUK9010_MSG_RES_SETTING));
		if (!ret)
		{
			LOGWARN("发送申请值回复命令失败。");
			return false;
		}
	}
	else
	{
		SendErrorAgentMsg(wMsgType,head);
		return false;
	}

	return true;
}

// bool MMSCLTApp::InitIedParam( int ied_no, MMSClient *cli )
// {
// 	SString		ied_ip_a = "";
// 	SString		ied_ip_b = "";
// 	SString		sql;
// 	SRecordset	rs;
// 
// 	sql = SString::toFormat("select param_name, current_val from t_oe_ied_param "
// 		"where ied_no=%d", ied_no);
// 	int iRet = DB->Retrieve(sql,rs);
// 	if (iRet < 0)
// 	{
// 		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
// 		return false;
// 	}
// 	else if (iRet == 0)
// 	{
// 		LOGWARN("数据库t_oe_ied_param表中未找到节点号=%d，程序名=%s的数据。", ied_no, MODULE_NAME);
// 		return false;
// 	}
// 	else if (iRet > 0)
// 	{
// 		for (int i = 0; i < rs.GetRows(); i++)
// 		{
// 			SString param = rs.GetValue(i, 0);
// 			if (param == "net_a_ip")
// 				ied_ip_a = rs.GetValue(i, 1);
// 			else if (param == "net_b_ip")
// 				ied_ip_b = rs.GetValue(i, 1);
// 		}
// 
// 		if (ied_ip_a.isEmpty() && ied_ip_b.isEmpty())	//A、B网地址均无，不创建此装置
// 		{
// 			LOGWARN("数据库t_oe_ied_param表中未找到节点号=%d，程序名=%s的A或B网地址配置。", ied_no, MODULE_NAME);
// 			return false;
// 		}	
// 	}
// 
// 	cli->setNetAIP(ied_ip_a);
// 	cli->setNetBIP(ied_ip_b);
// 	cli->SetMmsServerAddr(ied_ip_a.data(), ied_ip_b.data());
// 
// 	return true;
// }

#define READ_FILE_SYNC		30
#define READ_FILE_PATH		"/*.*"
#ifdef _WIN32
#define SAVE_FILE_PATH		"D:\\"
#else
#define SAVE_FILE_PATH		"/home/uk9010/shared/"
#endif
void* MMSCLTApp::ThreadReadFile(void* lp)
{
	unsigned int times = 0;
	if (!SDir::dirExists(SAVE_FILE_PATH))
		SDir::createDir(SAVE_FILE_PATH);

	MMSCLTApp *pThis = (MMSCLTApp*)lp;
	while(!pThis->IsQuit())
	{
		if (!(++times % READ_FILE_SYNC))
		{
			pThis->m_iReadFiles.clear();
			if (pThis->ReadFileList())
			{
				pThis->ReadFile();

				CNaModelMgr mgr;
				mgr.m_iSubNo = pThis->substation_id;
				mgr.RestoreToDb(SAVE_FILE_PATH);
			}
		}
		SApi::UsSleep(1000000);
	}

	pThis->EndThread();
	return NULL;
}

void* MMSCLTApp::ThreadChannelMonitor(void* lp)
{
	MMSCLTApp *pThis = (MMSCLTApp*)lp;
	pThis->BeginThread();

	SPtrList<MMSClient> clients = pThis->m_MMSMgrs;
	while(!pThis->IsQuit())
	{
		for (int i = 0; i < clients.count(); ++i)
		{
			if (pThis->m_globalConfig.paramchk_enabled)
				clients.at(i)->checkParam();

			if (pThis->m_globalConfig.analogChannelMonitorEnabled)
				clients.at(i)->checkAnalogChannel();
		}

		SDateTime currentTime = SDateTime::currentDateTime();
		if (currentTime.day() != pThis->m_lastGenerateParamReportTime.day() && currentTime.hour() >= 23)
		{
			pThis->m_lastGenerateParamReportTime = currentTime;
			pThis->CreateDailyParamCheckReport();
		}
		
		SApi::UsSleep(1000000);
	}

	pThis->EndThread();
	return NULL;
}

#ifdef TEST_CODE
void* MMSCLTApp::ThreadTestReadFile(void* lp)
{
	MMSCLTApp *pThis = (MMSCLTApp*)lp;

	pThis->BeginThread();
	SApi::UsSleep(1000000*10);

	stuSpUnitAgentMsgHead msgHead;
	SString sHeadStr;
	stuUK9010_MSG_REQ_FILE req;
	req.iSubNo = 1;
	req.iDevNo = 1;
	strcpy(req.visable_value, "pcs931.cid");

	// UK9010_MSG_REQ_LATEST_CID
	stuUK9010_MSG_REQ_LATEST_CID reqCid;
	reqCid.iSubNo = 1;
	reqCid.iDevNo = 1;
	reqCid.iType = 0;

	while (!pThis->IsQuit())
	{
		if (!pThis->IsConnected())
		{
			SApi::UsSleep(1000000);
				continue;
		}
//		pThis->ProcessAgentMsg(UK9010_MSG_REQ_FILE_READ, &msgHead, sHeadStr, (BYTE*)&req, sizeof(req));
		pThis->ProcessAgentMsg(UK9010_MSG_REQ_LATEST_CID, &msgHead, sHeadStr, (BYTE*)&reqCid, sizeof(reqCid));

		SApi::UsSleep(60*1000000);
	}

	pThis->EndThread();
	return NULL;
}
#endif

bool MMSCLTApp::ReadFileList()
{
	MMSClient *cli = GetMmsClientBySubstation(substation_id);
	if (!cli)
	{
		LOGWARN("ReadFileList时，未发现申请文件列表数据中的子站号[%d]。",substation_id);
		return false;
	}

	int ret = cli->GetMvlDirectory(READ_FILE_PATH,m_iReadFiles,false);
	if (ret == 0)
	{
		LOGWARN("ReadFileList时，获取文件列表为空。");
		return false;
	}
	else if (ret < 0)
	{
		LOGWARN("ReadFileList时，获取文件列表失败，文件路径[%s]。",READ_FILE_PATH);
		return false;
	}

	return true;
}

bool MMSCLTApp::ReadFile()
{
	MMSClient *cli = GetMmsClientBySubstation(substation_id);
	if (!cli)
	{
		LOGWARN("ReadFile时，未发现申请文件列表数据中的子站号[%d]。",substation_id);
		return false;
	}

	stuUK9010_MSG_RES_FILE_LIST file;
	for (int i = 0; i < m_iReadFiles.count(); i++)
	{
		memset(file.fileName,0,sizeof(file.fileName));
		memcpy(file.fileName,m_iReadFiles.at(i)->filename.data(),m_iReadFiles.at(i)->filename.size());
		file.fileSize = m_iReadFiles.at(i)->fsize;
		file.fileTime = m_iReadFiles.at(i)->mtime;
		if (file.fileSize == 0 && file.fileTime == 0)
			continue;
		
		SString fileName = file.fileName;
		int count = SString::GetAttributeCount(fileName,"/");
		if (count)
		{
			SString f = SString::GetIdAttribute(count,fileName,"/");
#ifdef _WIN32
			SString gb_name = f.toGb2312();
#else
			SString gb_name = f;
#endif
			SString localFileName = SString::toFormat("%s%s",SAVE_FILE_PATH,gb_name.data());
			if (IsDownFile(&file,localFileName))
			{
				bool ret = cli->DownMvlFile(fileName,localFileName);
				if (ret == false)
				{
					LOGWARN("ReadFile时，读取文件失败，文件名[%s]。",fileName.data());
					return false;
				}
			}
		}
		else
		{
			LOGWARN("ReadFile时，文件名[%s]有异常。",fileName.data());
			return false;
		}

		SApi::UsSleep(1000000);
	}

	return true;
}

bool MMSCLTApp::IsDownFile(stuUK9010_MSG_RES_FILE_LIST *file,SString localFileName)
{
	if (!SFile::exists(localFileName))
	{
		LOGDEBUG("文件[%s]不存在，下载文件。",localFileName.data());
		return true; //文件不存在直接下载
	}

	SDateTime dt = SFile::filetime(localFileName);
	int soc = dt.soc();
	if (file->fileTime > soc)
	{
		LOGDEBUG("文件[%s]旧，[%d > %d]，下载文件。",localFileName.data(),file->fileTime,soc);
		return true; //列表中文件比本地文件新则下载
	}

	return false;
}

bool MMSCLTApp::CreateDailyParamCheckReport()
{
	// prepare data
	int DeviceCount = 0;
	int MismatchDeviceCount = 0;
	int ParamCount = 0;
	int MismatchParamCount = 0;

	std::map<int, Ied *> mapIed;
	std::map<int, std::vector<ParamCheckItem *> > mapParamCheckItem;
	for (int i = 0; i < m_MMSMgrs.count(); ++i)
	{
		MMSClient *client = m_MMSMgrs.at(i);
		std::vector<Ied *> ieds = client->getIeds();

		for (std::vector<Ied *>::iterator iter = ieds.begin(); iter != ieds.end(); ++iter)
		{
			std::vector<ParamCheckItem *> paramCheckItem;
			(*iter)->getParamCheckList(paramCheckItem);
			mapParamCheckItem[(*iter)->iedNo()] = paramCheckItem;
			mapIed[(*iter)->iedNo()] = *iter;

			bool isHaveMismatchParam = false;
			for (std::vector<ParamCheckItem *>::iterator iterParam = paramCheckItem.begin(); iterParam != paramCheckItem.end(); ++iterParam)
			{
				++ParamCount;
				if ((*iterParam)->current_val.length() > 0 && (*iterParam)->current_val != (*iterParam)->last_val)
				{
						++MismatchParamCount;
						isHaveMismatchParam = true;
				}
			}

			if (isHaveMismatchParam)
				++MismatchDeviceCount;
			++DeviceCount;
		}
	}

	// Save to xml
	SDateTime currentTime = SDateTime::currentDateTime();
	SString filename = SString::toFormat("%s.xml", currentTime.toString("yyyy-MM-dd").data());
	SXmlConfig xml;
	SBaseConfig *node = NULL;
	xml.SetNodeName("ParamCheckReport");

	// Date
	node = xml.AddChildNode("Date");
	node->SetNodeValue(currentTime.toString("yyyy-MM-dd"));

	// Time
	node = xml.AddChildNode("Time");
	node->SetNodeValue(currentTime.toString("hh:mm:ss"));

	// DeviceCount
	node = xml.AddChildNode("DeviceCount");
	node->SetNodeValue(SString::toFormat("%d", DeviceCount));

	// MismatchDeviceCount
	node = xml.AddChildNode("MismatchDeviceCount");
	node->SetNodeValue(SString::toFormat("%d", MismatchDeviceCount));

	// ParamCount
	node = xml.AddChildNode("ParamCount");
	node->SetNodeValue(SString::toFormat("%d", ParamCount));

	// MismatchParamCount
	node = xml.AddChildNode("MismatchParamCount");
	node->SetNodeValue(SString::toFormat("%d", MismatchParamCount));

	// Devices
	SBaseConfig *devsNode = xml.AddChildNode("Devices");
	for (std::map<int, std::vector<ParamCheckItem *> >::iterator iter = mapParamCheckItem.begin(); iter != mapParamCheckItem.end(); ++iter)
	{
		node = devsNode->AddChildNode("Device");
		node->SetAttribute("no", SString::toFormat("%d", iter->first));
		node->SetAttribute("name", mapIed[iter->first]->getName());

		for (std::vector<ParamCheckItem *>::iterator iterParam = iter->second.begin(); iterParam != iter->second.end(); ++iterParam)
		{
			SBaseConfig *paramNode = node->AddChildNode("Param");
			paramNode->SetAttribute("gen_sn", SString::toFormat("%d", (*iterParam)->gen_sn));
			paramNode->SetAttribute("cpuno", SString::toFormat("%d", (*iterParam)->cpu_no));
			paramNode->SetAttribute("group", SString::toFormat("%d", (*iterParam)->group_no));
			paramNode->SetAttribute("entry", SString::toFormat("%d", (*iterParam)->entry));
			paramNode->SetAttribute("ValueType", SString::toFormat("%d", (*iterParam)->valtype));
			paramNode->SetAttribute("CurrentValue",  (*iterParam)->current_val);
			paramNode->SetAttribute("LastValue",  (*iterParam)->last_val);
			paramNode->SetAttribute("IsMatch", (*iterParam)->current_val == (*iterParam)->last_val ? "true" : "false");
		}
	}

	// Save to file
	SString path = SBASE_SAPP->GetBinPath() + "/" + filename;
	xml.SaveConfig(path, "UTF-8");

	// save to database
	SString sql = SString::toFormat("select date from t_rpt_day where date='%s'", currentTime.toString("yyyy-MM-dd").data());
	SRecordset rs;
	DB->Retrieve(sql, rs);
	if (rs.GetRows() == 0)
	{
		sql = SString::toFormat("insert into t_rpt_day(date) values('%s')", currentTime.toString("yyyy-MM-dd").data());
		if (!DB->ExecuteSQL(sql))
		{
			LOGERROR("写入记录到t_rpt_day失败,date=%s", currentTime.toString("yyyy-MM-dd").data());
			SFile::remove(path);
			return false;
		}
		LOGDEBUG("写入记录到t_rpt_day成功,date=%s", currentTime.toString("yyyy-MM-dd").data());
	}

	if (DB->UpdateLobFromFile("t_rpt_day", "paramchk", SString::toFormat("date='%s'", currentTime.toString("yyyy-MM-dd").data()), path))
	{
		LOGDEBUG("生成定值巡检每日报表成功,报告日期%s", currentTime.toString("yyyy-MM-dd").data());
	}
	else
	{
		LOGERROR("生成定值巡检每日报表失败,报告日期%s", currentTime.toString("yyyy-MM-dd").data());
	}
	SFile::remove(path);

	return true;
}
