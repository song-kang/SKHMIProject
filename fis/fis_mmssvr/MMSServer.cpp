#include "MMSServer.h"
#include "public/UK9010App.h"
#include "MMSSVRApp.h"
#include <algorithm>
#include <regex>


MMSServer::MMSServer( MMSSVRApp *app, int subNo, SString name, SString iedName)
{
	m_mmsApp = app;
	m_subNo = subNo;
	m_subName = name;
	m_iedName = iedName;
	m_enableAnalogReport = false;

	gOperatorSn = 0;
	m_commandStartTime = 0;
	m_isCommandRespFinished = false;
	m_reqCommandNo = 0;

	// 测试是否支持C++11 regex
	if (isSupportCPlus11()) {
		LOGDEBUG("支持C++11正则表达式");
	} else {
		LOGDEBUG("不支持C++11正则表达式");
	}
}

MMSServer::~MMSServer( void )
{
	Stop();
}

bool MMSServer::Start()
{
	m_bQuit = false;
	InitRunParam();

	// load scl file first, then call SMmsServer::Start
	int ret = this->AddIedByScd(m_sclFile);
	if (ret == 0)
	{
		LOGERROR("SCD解析成功但其中没有有效的MMS访问点");
		return false;
	}
	else if (ret < 0)
	{
		LOGERROR("SCD解析失败(%s)", m_sclFile.data());
		return false;
	}
	else
	{
		LOGDEBUG("SCD解析成功");
	}

	// 启动服务，触发读取startup.cfg配置
	if (!SMmsServer::Start())
	{
		LOGERROR("SMmsServer::Start() failed");
		return false;
	}
	
	// file service
	if (m_fileServicePath.isEmpty())
	{
		std::string str = SDir::currentDirPath().data();
		while (str.at(str.length() - 1) == '\\' || str.at(str.length() - 1) == '/')
			str = str.substr(0, str.length() - 1);

		int pos1 = str.find_last_of("\\");
		int pos2 = str.find_last_of("/");
		if (pos2 > pos1 && pos2 != std::string::npos)
			pos1 = pos2;

		if (pos1 != std::string::npos)
			str = str.substr(0, pos1);
		
		m_fileServicePath = (str + "/shared").c_str();
	}
	
	this->SetRootPath(m_fileServicePath);

	InitLeafValue();

	return true;
}

bool MMSServer::Stop()
{
	bool ret = SMmsServer::Stop();

	m_bQuit = true;
// 	while (m_iThreads > 0)
// 		SApi::UsSleep(50000);

	return ret;
}

void MMSServer::setStationInfo(int stationNo, SString stationName)
{
	m_subNo = stationNo;
	m_subName = stationName;
}

void MMSServer::setSclFile(SString file)
{
	m_sclFile = file;
	if (m_sclFile.right(3).CompareNoCase("scd") == 0)
		m_isScdMode = true;
	else
		m_isScdMode = false;
}

bool MMSServer::dbSubStateSyslog( SString text,int state )
{
	SString		sql;
	SRecordset	rs;

	int soc,usec;
	SDateTime::getSystemTime(soc,usec);
	SString log;
	if (state)
		log = m_subName+text+"-连接";
	else
		log = m_subName+text+"-中断";

	sql.sprintf("insert into t_ssp_syslog (soc,usec,usr_sn,log_type,log_level,id1,id2,id3,id4,log_text) values "
		"(%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
		soc,usec,
		0,
		state == 1 ? UK9010_LT_COMM_CONN : UK9010_LT_COMM_DISC,
		0,
		m_subNo,0,0,0,
		log.data());
	if (!DB->Execute(sql))
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}

	return true;
}

bool MMSServer::OnMapLeafToUser( char* sIedLdName, char* sLeafPath, stuLeafMap **ppLeafMap )
{
	bool ret = false;
	if (isSupportCPlus11())
	{
		std::cmatch result;
		std::regex regexString("(\\$stVal$|\\$ctlVal$|\\$setVal$|\\$general$|\\$t$||\\$T$|\\$q$|\\$setMag\\$f$|\\$mag\\$f$|\\$setMag\\$i$|\\$mag\\$i$)");
		if (std::regex_search(sLeafPath, result, regexString))
		{
			ret = true;
		}
	}
	else
	{
		SString s = SString(sLeafPath).right(9);
		if (s.find("$stVal") >= 0 || s.find("$ctlVal") >= 0 || s.find("$setVal") >= 0 || s.find("$general") >= 0 ||
			s.find("$t") >= 0 || s.find("$T") >= 0 || s.find("$q") >= 0 || s.find("$setMag$f") >= 0 || s.find("$mag$f") >= 0 ||
			s.find("$setMag$i") >= 0 || s.find("$mag$i") >= 0)
			ret = true;
	}

	if (false == ret)
		return true;

	std::map<std::string, int>::iterator iter = m_mapIedLDName.find(sIedLdName);
	if (iter == m_mapIedLDName.end())
		return true;

	int iedNo = iter->second;

	// 快速找到IED
	static Ied *ied = NULL;
	if (ied && ied->iedNo() == iedNo)
	{
		// 同一个IED继续使用上次获取对象 
	}
	else
	{
		ied = NULL;
		for (std::vector<Ied *>::iterator iterIed = m_iedList.begin(); iterIed != m_iedList.end(); ++iterIed)
		{
			if ((*iterIed)->iedNo() == iedNo)
			{
				ied = (*iterIed);
				break;
			}
		}
	}

	if (!ied)
		return true;

	SString mmspath = SString::toFormat("%s/%s", sIedLdName, sLeafPath);
	DaNode *daNode = ied->findDa(mmspath.data());
	if (daNode)
	{
		*ppLeafMap = new stuLeafMap();
		stuLeafMap *pLeafMap = *ppLeafMap;
		daNode->leaf = pLeafMap;
		m_mapMmspathLeafMap.insert(std::make_pair(mmspath.data(), pLeafMap));
	}

	return true;
}

void MMSServer::OnStartupCfgRead(SString key, SString value)
{
	if (key.CompareNoCase("SCLFileName") == 0)
	{
		if (m_sclFile.isEmpty())
			m_sclFile = value;
	}
	else if (key.CompareNoCase("StationName") == 0)
	{
		m_subName = value;
	}
	else if (key.CompareNoCase("FileServicePath") == 0)
	{
		m_fileServicePath = value;
	}
}

void MMSServer::OnMapDaiToUser(const char *iedName, const char *inst, const char *varName, const char *flattened, const char *val, const char *sAddr)
{
	if (sAddr && strlen(sAddr) > 0)
	{
		SString str = SString::toFormat("%s%s/%s$%s", iedName, inst, varName, flattened);
		m_mapAddr[str.data()] = SString(sAddr);
	}
}

bool MMSServer::GetUnitAgentProcessId(int sub_no,int ied_no,stuSpUnitAgentProcessId *id)
{
	SString		sql;
	SRecordset	rs;

	// 通信异常时不发送
	std::map<int, int>::iterator iter = m_iedComState.find(ied_no);
	if (iter == m_iedComState.end())
	{
		LOGERROR("数据库运行装置表中未发现节点号存在且装置号为[%d]的装置", ied_no);
		return false;
	}

	if (iter->second == false)
	{
		LOGWARN("站点(%d)装置(%d)通信状态断开，取消命令下发", sub_no, ied_no);
		return false;
	}

	// 查找节点下设备
	for (std::vector<RunNode *>::iterator iterNode = m_nodeList.begin(); iterNode != m_nodeList.end(); ++iterNode)
	{
		if ((*iterNode)->findDevice(ied_no))
		{
			id->SetApplicationId((*iterNode)->getNodeId(), (*iterNode)->getNodeNo());
			return true;
		}
	}

	return false;
}

void MMSServer::startCommand()
{
	m_commandStartTime = (unsigned int)::time(NULL);
	m_isCommandRespFinished = false;
	m_reqCmdList.clear();
	m_respCmdList.clear();
	m_currentReqCommand = gOperatorSn;
}

void MMSServer::stopCommand()
{
	gOperatorSn = 0;
	m_commandStartTime = 0;
	m_isCommandRespFinished = true;
}

bool MMSServer::isProcessingCommand()
{
	if (gOperatorSn != 0 || (m_commandStartTime > 0 && !m_isCommandRespFinished))
		return true;
	return false;
}

void MMSServer::checkCommandStatus()
{
	const int kCmdTimeout = 60;  // 60s

	if (m_commandStartTime > 0 && !m_isCommandRespFinished)
	{
		int curTime = (int)::time(NULL);
		int sub = curTime - m_commandStartTime;
		if (sub > kCmdTimeout)
		{
			LOGWARN("当前命令(%d)执行超时(%d)", m_currentReqCommand, sub);
			stopCommand();
		}
	}
}

bool MMSServer::ReadCurrentArea(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_SETTING_AREA req_setting_area;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号

	req_setting_area.iSubNo = m_subNo;
	req_setting_area.iDevNo = command->ied_no;
	req_setting_area.iCpuNo = -1;
	req_setting_area.iSectorNo = -1;

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,UK9010_MSG_REQ_SETTING_AREA,gOperatorSn,0,
		(unsigned char*)&req_setting_area,sizeof(req_setting_area));
	if (!ret)
	{
		stopCommand();
		LOGERROR("发送读当前定值区命令失败。");
	}

	return ret;
}

void MMSServer::ReadCurrentArea(int iedNo, int cpuNo, int groupNo, int entryNo)
{
	oper_command_t oper_command;

	oper_command.command = OPER_READ_CURRENT_AREA;
	oper_command.sub_no = m_subNo;
	oper_command.ied_no = iedNo;
	oper_command.cpu_no = cpuNo;
	oper_command.group_no = groupNo;
	oper_command.entry = entryNo;
	oper_command.value = "";

	m_reqCmdList.push_back(oper_command);
}

void MMSServer::ReadEditArea(int iedNo, int cpuNo, int groupNo, int entryNo)
{
	oper_command_t oper_command;

	oper_command.command = OPER_READ_EDIT_AREA;
	oper_command.sub_no = m_subNo;
	oper_command.ied_no = iedNo;
	oper_command.cpu_no = cpuNo;
	oper_command.group_no = groupNo;
	oper_command.entry = entryNo;
	oper_command.value = "";

	m_reqCmdList.push_back(oper_command);
}

bool MMSServer::ReadEditArea(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_SETTING_AREA req_setting_area;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号

	req_setting_area.iSubNo = m_subNo;
	req_setting_area.iDevNo = command->ied_no;
	req_setting_area.iCpuNo = -1;
	req_setting_area.iSectorNo = -2;

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,UK9010_MSG_REQ_SETTING_AREA,gOperatorSn,0,
		(unsigned char*)&req_setting_area,sizeof(req_setting_area));
	if (!ret)
	{
		stopCommand();
		LOGERROR("发送读编辑定值区命令失败。");
	}

	return ret;
}

bool MMSServer::WriteCurrentArea(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_EDIT_SETTING_AREA edit_setting_area;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号

	edit_setting_area.iSubNo = m_subNo;
	edit_setting_area.iDevNo = command->ied_no;
	edit_setting_area.iCpuNo = -1;
	edit_setting_area.iSectorNo = -1;
	edit_setting_area.iArea = atoi(command->value.c_str());

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,UK9010_MSG_REQ_EDIT_SETTING_AREA,gOperatorSn,0,
		(unsigned char*)&edit_setting_area,sizeof(edit_setting_area));
	if (!ret)
	{
		stopCommand();
		LOGERROR("发送切换当前定值区命令失败。");
		return ret;
	}

	SString log = SString::toFormat("切换当前定值区，切换至[%d]区。", atoi(command->value.c_str()));
	if (!ssp_base::GetPtr()->NewSysLog(UK9010_LT_EDIT_SECTOR,0,log.data(), m_subNo,command->ied_no,0,0))
		LOGWARN("写系统日志失败，内容[%s]。",log.data());

	return ret;
}

bool MMSServer::WriteEditArea(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_EDIT_SETTING_AREA edit_setting_area;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号

	edit_setting_area.iSubNo = m_subNo;
	edit_setting_area.iDevNo = command->ied_no;
	edit_setting_area.iCpuNo = -1;
	edit_setting_area.iSectorNo = -2;
	edit_setting_area.iArea = atoi(command->value.c_str());

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,UK9010_MSG_REQ_EDIT_SETTING_AREA,gOperatorSn,0,
		(unsigned char*)&edit_setting_area,sizeof(edit_setting_area));
	if (!ret)
	{
		stopCommand();
		LOGERROR("发送切换编辑定值区命令失败。");
		return ret;
	}

	SString log = SString::toFormat("切换编辑定值区，切换至[%d]区。", atoi(command->value.c_str()));
	if (!ssp_base::GetPtr()->NewSysLog(UK9010_LT_EDIT_SECTOR,0,log.data(),m_subNo,command->ied_no,0,0))
		LOGWARN("写系统日志失败，内容[%s]。",log.data());

	return ret;
}

bool MMSServer::ReadCurrentAreaSetting(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_SETTING req_setting;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号

	req_setting.iSubNo = m_subNo;
	req_setting.iDevNo = command->ied_no;
	req_setting.iCpuNo = command->cpu_no;
	req_setting.iGroupNo = command->group_no;
	req_setting.iEntryNo = -1;
	req_setting.iSectorNo = -1;

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id, command->command, gOperatorSn,0,
		(unsigned char*)&req_setting,sizeof(req_setting));
	if (!ret)
	{
		stopCommand();
		LOGWARN("发送读当前区定值命令失败。");
	}

	return ret;
}

bool MMSServer::ReadEditAreaSetting(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_SETTING req_setting;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号

	req_setting.iSubNo = m_subNo;
	req_setting.iDevNo = command->ied_no;
	req_setting.iCpuNo = command->cpu_no;
	req_setting.iGroupNo = command->group_no;
	req_setting.iEntryNo = -1;
	req_setting.iSectorNo = -2;

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,UK9010_MSG_REQ_SETTING,gOperatorSn,0,
		(unsigned char*)&req_setting,sizeof(req_setting));
	if (!ret)
	{
		stopCommand();
		LOGWARN("发送读编辑区定值命令失败。");
	}

	return ret;
}

bool MMSServer::WriteEditAreaSetting(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_EDIT_SETTING edit_setting;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号
	command->command_no = gOperatorSn;
	edit_setting.iSubNo = m_subNo;
	edit_setting.iDevNo = command->ied_no;
	edit_setting.iCpuNo = command->cpu_no;
	edit_setting.iGroupNo = command->group_no;
	edit_setting.iEntryNo = command->entry;
	edit_setting.iSectorNo = command->sector;
	memset(edit_setting.visable_value,0,sizeof(edit_setting.visable_value));
	memcpy(edit_setting.visable_value,command->value.c_str(),strlen(command->value.c_str()));

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,UK9010_MSG_REQ_EDIT_SETTING,gOperatorSn,0,
		(unsigned char*)&edit_setting,sizeof(edit_setting));
	if (!ret)
	{
		stopCommand();
		LOGERROR("发送写编辑区定值命令失败。");
		return ret;
	}

	SString		sql;
	SRecordset	rs;
	SString		name;
	sql.sprintf("select name from t_oe_element_general where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		command->ied_no,command->cpu_no,command->group_no,command->entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
		name = rs.GetValue(0,0);
	else
	{
		sql.sprintf("select name from t_oe_element_state where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
			command->ied_no,command->cpu_no,command->group_no,command->entry);
		cnt = DB->Retrieve(sql,rs);
		if (cnt > 0)
			name = rs.GetValue(0,0);
	}
	if (name.length() == 0)
		name.sprintf("未获取到定值名称[%d,%d,%d,%d]",command->ied_no,command->cpu_no,command->group_no,command->entry);		
	SString log = SString::toFormat("预设定值，定值名称[%s]，值[%s]。",name.data(),edit_setting.visable_value);
	if (!ssp_base::GetPtr()->NewSysLog(UK9010_LT_EDIT_SECTOR,0,log.data(), m_subNo,command->ied_no,0,0))
		LOGWARN("写系统日志失败，内容[%s]。",log.data());

	return ret;
}

bool MMSServer::CuteSetting(oper_command_t *command)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_CURE_SETTING edit_setting;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(m_subNo, command->ied_no, &dst_id);
	if (!ret)
	{
		stopCommand();
		return ret;
	}

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号

	edit_setting.iSubNo = m_subNo;
	edit_setting.iDevNo = command->ied_no;
	edit_setting.iCpuNo = command->cpu_no;
	edit_setting.iGroupNo = command->group_no;

	startCommand();
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,UK9010_MSG_REQ_CURE_SETTING,gOperatorSn,0,
		(unsigned char*)&edit_setting,sizeof(edit_setting));
	if (!ret)
	{
		stopCommand();
		LOGERROR("发送固化定值命令失败。");
		return ret;
	}

	SString log = SString::toFormat("固化定值。");
	if (!ssp_base::GetPtr()->NewSysLog(UK9010_LT_EDIT_SECTOR,0,log.data(), m_subNo,command->ied_no,0,0))
		LOGWARN("写系统日志失败，内容[%s]。",log.data());

	return ret;
}

bool MMSServer::waitForCommand(int cmdType, int commandNo, oper_command_t &resp, int timeout, bool peek)
{
	int times = timeout / 10;
	int currentTime = (int)::time(NULL);

	std::list<oper_command_t>::iterator iter;

	// 清理超时回复
	m_respCmdListLock.lock();
	for (iter = m_respCmdList.begin(); iter != m_respCmdList.end();)
	{
		if (abs(currentTime - (int)iter->time) > 120)
		{
			LOGDEBUG("清除超时回复命令%d,序号%d", iter->command, iter->command_no);
			iter = m_respCmdList.erase(iter);
		}
		else
			++iter;
	}
	m_respCmdListLock.unlock();

	for (int count = 0; count < times; ++count)
	{
		m_respCmdListLock.lock();
		for (iter = m_respCmdList.begin(); iter != m_respCmdList.end(); ++iter)
		{
			if (iter->command == cmdType/* && iter->command_no == commandNo*/)
			{
				resp = *iter;

				if (!peek)
					m_respCmdList.erase(iter);

				m_respCmdListLock.unlock();
				return true;
			}
		}
		m_respCmdListLock.unlock();

		SApi::UsSleep(10000);
	}

	return false;
}

bool MMSServer::waitForCommand(int cmdType, int timeout)
{
	int times = timeout / 10;

	std::list<oper_command_t>::iterator iter;
	for (int count = 0; count < times; ++count)
	{
		m_respCmdListLock.lock();
		for (iter = m_respCmdList.begin(); iter != m_respCmdList.end(); ++iter)
		{
			if (iter->command == cmdType)
			{
				m_respCmdList.erase(iter);
				m_respCmdListLock.unlock();
				return true;
			}
		}
		m_respCmdListLock.unlock();

		SApi::UsSleep(10000);
	}

	return false;
}

stuLeafMap * MMSServer::findValueLeafMapByRef(std::string ident)
{
	std::map<std::string, std::string>::iterator iter = m_mapIdentReference.find(ident);
	if (iter != m_mapIdentReference.end())
	{
		std::map<std::string, stuLeafMap *>::iterator iterRef = m_mapMmspathLeafMap.find(iter->second);
		if (iterRef != m_mapMmspathLeafMap.end())
			return iterRef->second;
	}

	return NULL;
}

stuLeafMap * MMSServer::findTimeLeafMapByIdent(std::string ident)
{
	std::map<std::string, std::string>::iterator iter = m_mapIdentReference.find(ident);
	if (iter != m_mapIdentReference.end())
	{
		SString reference = iter->second.c_str();
		std::string referenceT;
		if (reference.right(5).CompareNoCase("mag$f") == 0)
			referenceT = reference.left(reference.length() - 5).data();
		else if (reference.right(5).CompareNoCase("mag$i") == 0)
			referenceT = reference.left(reference.length() - 5).data();
		else if (reference.right(1).CompareNoCase("f") == 0)
			referenceT = reference.left(reference.length() - 1).data();
		else if (reference.right(5).CompareNoCase("stVal") == 0)
			referenceT = reference.left(reference.length() - 5).data();
		else
			return NULL;
		referenceT += "t";

		std::map<std::string, stuLeafMap *>::iterator iterRef = m_mapMmspathLeafMap.find(referenceT);
		if (iterRef != m_mapMmspathLeafMap.end())
			return iterRef->second;

		// 根据$查找
		referenceT = "";
		int count = SString::GetAttributeCount(reference, "$");
		for (int i = 1; i < count; ++i)
		{
			referenceT += SString::GetIdAttribute(i, reference, "$");
		}
		referenceT += "$t";
		iterRef = m_mapMmspathLeafMap.find(referenceT);
		if (iterRef != m_mapMmspathLeafMap.end())
			return iterRef->second;
	}

	return NULL;
}

bool MMSServer::IsDeviceOverhaul(int iedNo)
{
	// 装置是否处在检修状态
	SString sql = SString::toFormat("select overhaul_status from t_oe_ied_overhaul_ass where ied_no=%d", iedNo);
	SRecordset rs;
	DB->Retrieve(sql, rs);
	if (rs.GetRows() > 0)
	{
		int value = rs.GetValueInt(0, 0);
		if (value == 1)
		{
			return true;
		}
	}

	return false;
}

int MMSServer::FindIedNoByIedLd(SString iedLd)
{
	std::map<std::string, int>::iterator iter = m_mapIedLDName.find(iedLd.data());
	if (iter != m_mapIedLDName.end())
		return iter->second;

	return -1;
}

Ied * MMSServer::FindIedByIedNo(int iedNo)
{
	for (std::vector<Ied *>::iterator iter = m_iedList.begin(); iter != m_iedList.end(); ++iter)
	{
		if ((*iter)->iedNo() == iedNo)
		{
			return *iter;
		}
	}

	return NULL;
}

int MMSServer::createReqCommandNo()
{
	if (m_reqCommandNo++ > 99999)
		m_reqCommandNo = 1;
	return m_reqCommandNo;
}

bool MMSServer::isSupportCPlus11()
{
	static bool isFirst = true;
	static bool isSupportcplusplus11 = false;
	if (isFirst)
	{
		isFirst = false;
		std::cmatch result;
		std::string strings1 = "An1GGIO1$MX$AnIn1$mag$f";
		std::string strings2 = "An1GGIO1$MX$AnIn1$mag";
		std::regex regexString("(\\$stVal$|\\$setVal$|\\$t$|\\$q$|\\$setMag\\$f$|\\$mag\\$f$)");
		if (std::regex_search(strings1.c_str(), result, regexString) && !std::regex_search(strings2.c_str(), result, regexString))
			isSupportcplusplus11 = true;
		else
			isSupportcplusplus11 = false;
	}

	return isSupportcplusplus11;
}

bool MMSServer::controlSelect(oper_command_t *command, int controlValue)
{
	// controlValue	 1分，2合，其余无效
	return controlBase(command, UK9010_MSG_REQ_CTRL_SELECT, controlValue);
}

bool MMSServer::controlExecute(oper_command_t *command, int controlValue)
{
	return controlBase(command, UK9010_MSG_REQ_CTRL_EXECUTE, controlValue);
}

bool MMSServer::controlCancel(oper_command_t *command, int controlValue)
{
	return controlBase(command, UK9010_MSG_REQ_CTRL_CANCEL, controlValue);
}

bool MMSServer::controlDirect(oper_command_t *command, int controlValue)
{
	// controlValue	 1分，2合，其余无效
	return controlBase(command, UK9010_MSG_REQ_CTRL_DIRECT, controlValue);
}

bool MMSServer::controlBase(oper_command_t *command, int type, int controlValue)
{
	bool ret = false;
	stuSpUnitAgentProcessId dst_id;
	stuUK9010_MSG_REQ_CTRL req_ctrl;
	dst_id.m_iUnitId = 0;
	ret = GetUnitAgentProcessId(command->sub_no, command->ied_no, &dst_id);
	if (!ret)
	{
		LOGWARN("获取命令ID失败，控制过程被中止。");
		stopCommand();
		return ret;
	}

	req_ctrl.iSubNo = command->sub_no;
	req_ctrl.iDevNo = command->ied_no;
	req_ctrl.iCpuNo = command->cpu_no;
	req_ctrl.iGroupNo = command->group_no;
	req_ctrl.iEntryNo = command->entry;
	memset(req_ctrl.visable_value, 0, sizeof(req_ctrl.visable_value));
	strcpy(req_ctrl.visable_value, SString::toFormat("%d", controlValue).data());

	gOperatorSn = SApplication::NewMsgSn(); //获取序列号
	command->command = type;
	command->command_no = gOperatorSn;
	m_reqCmdListLock.lock();
	m_reqCmdList.push_back(*command);
	m_reqCmdListLock.unlock();
	startCommand();

	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id, type, gOperatorSn, 0,
		(unsigned char*)&req_ctrl, sizeof(req_ctrl));
	if (!ret)
	{
		stopCommand();
		LOGWARN("发送控制[选择]命令失败。");
		return ret;
	}

	return ret;
}

bool MMSServer::updateControlValue(int iedNo, SString doPath, int type, int st)
{
	Ied *ied = FindIedByIedNo(iedNo);
	if (!ied)
	{
		return false;
	}

	DoNode *doNode = ied->findDo(doPath.data());
	if (doNode)
	{
		SString valPath;
		SString timePath;
		int soc, usec;
		SDateTime::getSystemTime(soc, usec);

		switch (type)
		{
		case OPER_CONTROL_SELECT:
			valPath = "$SBOw$ctlVal";
			timePath = "$SBOw$T";
			break;
		case OPER_CONTROL_CANCEL:
			valPath = "$Cancel$ctlVal";
			timePath = "$Cancel$T";
			break;
		}

		if (type == OPER_CONTROL_SELECT || type == OPER_CONTROL_CANCEL)
		{
			DaNode *valNode = doNode->findDa(valPath.data(), false);
			DaNode *tNode = doNode->findDa(timePath.data(), false);
			if (valNode && valNode->leaf && tNode && tNode->leaf)
			{
				SetLeafVarValue(valNode->leaf, st);
				SetLeafVarValue(tNode->leaf, soc, usec);
				return true;
			}

			return false;
		}
		
		bool ret = false;
		if (type == OPER_CONTROL_EXECUTE || type == OPER_CONTROL_DIRECT)
		{
			valPath = "$Oper$ctlVal";
			timePath = "$Oper$T";
			DaNode *valNode = doNode->findDa(valPath.data(), false);
			DaNode *tNode = doNode->findDa(timePath.data(), false);
			if (valNode && valNode->leaf && tNode && tNode->leaf)
			{
				SetLeafVarValue(valNode->leaf, st);
				SetLeafVarValue(tNode->leaf, soc, usec);
				ret = true;
			}

			SString doStPath = doPath.replace("$CO$", "$ST$");
			DoNode *stDoNode = ied->findDo(doStPath.data());
			if (stDoNode)
			{
				valPath = "$stVal";
				timePath = "$t";
				valNode = stDoNode->findDa(valPath.data(), false);
				tNode = stDoNode->findDa(timePath.data(), false);
				if (valNode && valNode->leaf && tNode && tNode->leaf)
				{
					SetLeafVarValue(valNode->leaf, st);
					SetLeafVarValue(tNode->leaf, soc, usec);
					ret = true;
				}
			}
		}

		return ret;
	}
	
	return false;
}

stuLeafMap *MMSServer::FindLeafMap(SString mmspath)
{
	std::map<std::string, stuLeafMap*>::iterator iter;
	iter = m_mapMmspathLeafMap.find(mmspath.data());
	if (iter != m_mapMmspathLeafMap.end())
		return iter->second;
	else
		return NULL;
}

void MMSServer::InitLeafValue()
{
	SDateTime currentTime = SDateTime::currentDateTime();
	int count = 0;

	for (std::vector<Ied *>::iterator iterIed = m_iedList.begin(); iterIed != m_iedList.end(); ++iterIed)
	{
		std::vector<DaNode *> daList;
		(*iterIed)->getAllDa(daList);

		for (std::vector<DaNode *>::iterator iterDa = daList.begin(); iterDa != daList.end(); ++iterDa)
		{
			DaNode *daNode = *iterDa;
			if (daNode->value.length() > 0 && daNode->leaf)
			{
				++count;
				switch (daNode->valType)
				{
				case 7:
					SetLeafVarValue(daNode->leaf, atof(daNode->value.c_str()));
					break;

				case 3:
					SetLeafVarValue(daNode->leaf, atoi(daNode->value.c_str()));
					break;

				default:
					SetLeafVarValue(daNode->leaf, (char*)daNode->value.c_str());
					break;
				}
			}
		}
	}
	
 	LOGDEBUG("初始化节点值完成，共计%d个节点，历时%d毫秒", count, currentTime.msecsTo(SDateTime::currentDateTime()));
}

bool MMSServer::InitIedCommState()
{
	if (!m_iedComState.empty())
		m_iedComState.clear();

	SString sql = SString::toFormat("select ied_no, comstate from t_oe_ied where sub_no=%d", m_subNo);
	SRecordset rs;
	int ret = DB->RetrieveRecordset(sql, rs);
	if (ret <= 0 || rs.GetRows() == 0)
	{
		LOGERROR("数据集不存在sql=%s", sql.data());
		return false;
	}

	for (int row = 0; row < rs.GetRows(); ++row)
	{
		int iedNo = rs.GetValue(row, 0).toInt();
		int state = rs.GetValue(row, 1).toInt();
		m_iedComState[iedNo] = state;
	}

	return true;
}

int MMSServer::OnServerDirectory(SString sPath,SPtrList<stuSMmsFileInfo> &slFiles,bool bNeedToSort/*=false*/)
{
	if (sPath.length() == 0)
	{
		LOGWARN("Read directory %s, refused request", sPath.data());
		if (!slFiles.isEmpty())
			slFiles.clear();
		return 0;
	}

	LOGDEBUG("Read directory %s", sPath.data());
	return SMmsServer::OnServerDirectory(sPath, slFiles, bNeedToSort);
}

SMmsServer::CMmsMemFile* MMSServer::OnReadServerFile(SString sFileName)
{
	SMmsServer::CMmsMemFile *memFile = SMmsServer::OnReadServerFile(sFileName);
	if (memFile)
	{
		LOGDEBUG("Read file %s success", (m_sRootPath + sFileName).data());
	}
	else
	{
		LOGERROR("Read file %s failed", (m_sRootPath + sFileName).data());
	}

	return memFile;
}

int MMSServer::OnWriteServerFile(SString sFileName)
{
	LOGDEBUG("OnWriteServerFile %s", sFileName.data());
	return 1;
}

bool MMSServer::OnWriteLeafValue(const char* sIedLdName, const char* sLeafPath, stuLeafMap *pLeafMap, void *mvluWrVaCtrl)
{
	SString mmspath = SString::toFormat("%s/%s", sIedLdName, sLeafPath);
	if (mmspath.find("$SP$"))
	{}
	if (mmspath.find("$SG$"))
	{}
	else if (mmspath.find("$SE$"))
	{}
	else
	{
		LOGDEBUG("修改类型非SP、SG、SE，忽略修改操作");
		return false;
	}

	int iedNo = -1;
	std::map<std::string, int>::iterator iterIedLd = m_mapIedLDName.find(sIedLdName);
	if (iterIedLd != m_mapIedLDName.end())
		iedNo = iterIedLd->second;

	if (iedNo == -1)
	{
		LOGERROR("%s/%s 隶属设备不存在,忽略请求", sIedLdName, sLeafPath);
		return false;
	}

	SString leafpath = SString::toFormat("%s/%s", sIedLdName, sLeafPath).data();
	Ied *ied = FindIedByIedNo(iedNo);
	DoNode *doNode = NULL;
	DaNode *daNode = NULL;
	if (ied)
	{
		daNode = ied->findDa(mmspath.data());
		if (daNode)
			doNode = daNode->parent;
	}

	if (!doNode)
	{
		LOGERROR("信息点未找到 %s", mmspath.data());
		return false;
	}

	if (doNode->groupType != 17)
	{
		LOGERROR("信息类型非参数类型，忽略修改 %s", mmspath.data());
		return false;
	}

	// 查找节点指针
	std::map<std::string, stuLeafMap *>::iterator iterLeaf;
	iterLeaf = m_mapMmspathLeafMap.find(leafpath.data());
	if (iterLeaf == m_mapMmspathLeafMap.end())
	{
		LOGERROR("节点(%s)不存在", leafpath.data());
		return false;
	}
	stuLeafMap *leafMap = iterLeaf->second;

	// 通信状态
	std::map<int, int>::iterator iter = m_iedComState.find(iedNo);
	if (iter == m_iedComState.end())
	{
		LOGERROR("数据库运行装置表中未发现节点号存在且装置号为[%d]的装置", iedNo);
		return false;
	}

	if (iter->second == false)
	{
		LOGWARN("站点(%d)装置(%d)通信状态断开，取消修改节点值操作(%s/%s)", m_subNo, iedNo, sIedLdName, sLeafPath);
		return false;
	}

	// 命令加入发送队列
	oper_command_t operCommandReq;
	operCommandReq.command = UK9010_MSG_REQ_EDIT_SETTING;
	operCommandReq.sub_no = m_subNo;
	operCommandReq.ied_no = iedNo;
	operCommandReq.domName = sIedLdName;
	operCommandReq.varName = sLeafPath;
	operCommandReq.cpu_no = doNode->cpuno;
	operCommandReq.group_no = doNode->group;
	operCommandReq.entry = doNode->entry;
	operCommandReq.value = GetLeafVarValue(pLeafMap).data();
	m_reqCmdListLock.lock();
	m_reqCmdList.push_back(operCommandReq);
	m_reqCmdListLock.unlock();

	// 通过代理发送命令
	WriteEditAreaSetting(&operCommandReq);
	
// 	bool ret = SApplication::GetPtr()->SendAgentMsg(&dst_id, UK9010_MSG_REQ_LEAF_VALUE, gOperatorSn, 0,
// 		(BYTE*)&req_setting, sizeof(req_setting));
// 	if (!ret)
// 	{
// 		LOGWARN("发送读当前区定值命令失败。");
// 	}

	// wait for response
	oper_command_t resp;
	if (!waitForCommand(operCommandReq.command + 1, operCommandReq.command_no, resp, kCtrlTimeout))
	{
		stopCommand();
		LOGWARN("等待修改回复命令超时, 设备(%d)序号(%d) mmspath=%s", iedNo, operCommandReq.command_no, leafpath.data());
		return false;
	}
	stopCommand();
	
	LOGERROR("修改节点 %s，值 %s 成功", mmspath.data(), operCommandReq.value.c_str());

	return true;
}

bool MMSServer::OnReturnSelect(char *ref, char *st, bool ret)
{
	// 根据路径找到装置编号 
	// ref = 0x08c7e630 "PACS5776DGA4LD0/LLN0$CO$FunEna2$SBOw"
	// ref = 0x08c7e630 "PACS5776DGA4CTRL/CSWI3$CO$Pos$SBOw"
	if (ret == false)
	{
		LOGDEBUG("%s 不可控制", ref);
		return false;
	}
	
	int controlValue = 0;
	bool result = OnReturnBase(ref, st, ret, OPER_CONTROL_SELECT, controlValue);
	LOGDEBUG("请求遥控选择 %s", result ? "成功" : "失败");
	m_lastControlType = OPER_CONTROL_SELECT;

	return result;
}

bool MMSServer::OnReturnOper(char *ref, char *st, bool ret)
{
	// ref = 0x08c7e648 "PACS5776DGA4LD0/LLN0$CO$FunEna2$Oper"
	// ref = 0x08c7e648 "PACS5776DGA4CTRL/CSWI3$CO$Pos$Oper"
	if (ret == false)
	{
		LOGDEBUG("%s 不可控制", ref);
		return false;
	}

	int controlValue = 0;
	int currOperType;
	if (m_lastControlType == OPER_CONTROL_SELECT)
		currOperType = OPER_CONTROL_EXECUTE;
	else
		currOperType = OPER_CONTROL_DIRECT;

	bool result = OnReturnBase(ref, st, ret, currOperType, controlValue);
	LOGDEBUG("请求%s,值%d,%s", currOperType == OPER_CONTROL_DIRECT ? "直控" : "遥控",
		controlValue, result ? "成功" : "失败");
	if (m_lastControlType == OPER_CONTROL_SELECT)
		m_lastControlType = OPER_CONTROL_EXECUTE;
	else
		m_lastControlType = OPER_CONTROL_DIRECT;

	return result;
}

bool MMSServer::OnReturnCancel(char *ref, char *st, bool ret)
{
	// ref = 0x08c7e648 "PACS5776DGA4CTRL/CSWI3$CO$Pos$Cancel"
	if (ret == false)
	{
		LOGDEBUG("%s 不可控制", ref);
		return false;
	}

	int controlValue = 0;
	bool result = OnReturnBase(ref, st, ret, OPER_CONTROL_CANCEL, controlValue);
	LOGDEBUG("请求遥控取消,值%d,%s", controlValue, result ? "成功" : "失败");
	m_lastControlType = OPER_CONTROL_CANCEL;
	return result;
}

bool MMSServer::OnReturnBase(char *ref, char *st, bool ret, int type, int &controlValue)
{
	SString mmspath = ref;
	SString iedLd = SString::GetIdAttribute(1, mmspath, "/");
	int iedNo = FindIedNoByIedLd(iedLd);
	Ied *ied = FindIedByIedNo(iedNo);
	if (!ied)
	{
		return false;
	}

	SString doPath;
	SString attr = ref;
	int count = SString::GetAttributeCount(attr, "$");
	for (int i = 1; i < count; ++i)
	{
		doPath += SString::GetIdAttribute(i, attr, "$");

		if (i + 1 < count)
			doPath += "$";
	}

	DoNode *doNode = ied->findDo(doPath.data());
	if (!doNode)
		return false;

	controlValue = 0;
	if (doNode->controlType == 3 || doNode->controlType == 4)
		controlValue = *(int *)st;
	else
		controlValue = *(unsigned char *)st;

	oper_command_t oper_command;
	oper_command.command = type;
	oper_command.sub_no = m_subNo;
	oper_command.ied_no = iedNo;
	oper_command.cpu_no = doNode->cpuno;
	oper_command.group_no = doNode->group;
	oper_command.entry = doNode->entry;
	oper_command.value = SString::toFormat("%d", st);

	bool result = false;
	switch (type)
	{
	case OPER_CONTROL_DIRECT:
		result = controlDirect(&oper_command, controlValue); break;
	case OPER_CONTROL_SELECT:
		result = controlSelect(&oper_command, controlValue); break;
	case OPER_CONTROL_EXECUTE:
		result = controlExecute(&oper_command, controlValue); break;
	case OPER_CONTROL_CANCEL:
		result = controlCancel(&oper_command, controlValue); break;
	}

	oper_command_t resp;
	if (!waitForCommand(oper_command.command + 1, oper_command.command_no, resp, kCtrlTimeout))
	{
		stopCommand();
		LOGWARN("等待回复命令超时, 设备(%d)序号(%d) mmspath=%s", iedNo, oper_command.command_no, mmspath.data());
		return false;
	}
	stopCommand();
	
	if (resp.result)
	{
		updateControlValue(iedNo, doPath, type, controlValue);
	}

	return resp.result;
}

bool MMSServer::ProcessAgentMsg(unsigned short wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,unsigned char* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	MMSServer::oper_command_t cmd;
	cmd.command = wMsgType;
	cmd.time = ::time(NULL);
	
	switch (wMsgType)
	{
	case UK9010_MSG_RES_ENTRY_VALUE:
		if (iLength == sizeof(stuUK9010_MSG_RES_READ_ENTRY))
		{
			stuUK9010_MSG_RES_READ_ENTRY resp_setting;
			memcpy(&resp_setting, pBuffer, iLength);

			getReqCommand(UK9010_MSG_REQ_ENTRY_VALUE, cmd);
			cmd.command = wMsgType;
			cmd.value = resp_setting.value;
			cmd.result = resp_setting.ret;
		}
		else
		{
			LOGWARN("获取代理申请定值区请求回复，请求数据长度[%d]与结构长度[%d,%d]不一致。", iLength, sizeof(stuUK9010_MSG_RES_READ_LEAF), sizeof(stuUK9010_MSG_RES_READ_LEAF_EX));
			cmd.result = false;
		}
		addRespCommand(cmd);
		break;

	case UK9010_MSG_RES_EDIT_SETTING:
		if (iLength == sizeof(stuUK9010_MSG_RES_EDIT_SETTING))
		{
			stuUK9010_MSG_RES_EDIT_SETTING resp_setting;
			memcpy(&resp_setting, pBuffer, iLength);

			getReqCommand(UK9010_MSG_REQ_EDIT_SETTING, cmd);
			cmd.command = wMsgType;
			cmd.result = resp_setting.iRet == 1 ? true : false;
		}
		else
		{
			LOGWARN("获取代理申请修改参数请求回复，请求数据长度[%d]与结构长度[%d,%d]不一致。", iLength, sizeof(stuUK9010_MSG_RES_READ_LEAF), sizeof(stuUK9010_MSG_RES_READ_LEAF_EX));
			cmd.result = false;
		}
		addRespCommand(cmd);
		break;

	case UK9010_MSG_RES_CTRL_SELECT:
	case UK9010_MSG_RES_CTRL_EXECUTE:
	case UK9010_MSG_RES_CTRL_CANCEL:
	case UK9010_MSG_RES_CTRL_DIRECT:
		if (iLength == sizeof(stuUK9010_MSG_RES_CTRL))
		{
			stuUK9010_MSG_RES_CTRL resp_ctrl;
			memcpy(&resp_ctrl, pBuffer, iLength);

			getReqCommand(wMsgType, cmd);
			cmd.command = wMsgType;
			cmd.result = resp_ctrl.result == 1 ? true : false;
			addRespCommand(cmd);
 		}
		break;

	default:
		return false;
	}

	return true;
}

bool MMSServer::OnReadLeafValue(const char* sIedLdName, const char* sLeafPath, stuLeafMap *pLeafMap, void *mvluRdVaCtrl, int reason)
{
	if (reason != 2)
		return false;

	// 所有数据均从内存库和历史库中获取，不主动向装置下发读取命令
	return true;

	// 获取设备号
	int iedNo = -1;
	std::map<std::string, int>::iterator iterIedLd = m_mapIedLDName.find(sIedLdName);
	if (iterIedLd != m_mapIedLDName.end())
		iedNo = iterIedLd->second;
	
	if (iedNo == -1)
	{
		LOGERROR("%s/%s 隶属设备不存在,忽略请求", sIedLdName, sLeafPath);
		return false;
	}

	// 获取组号等信息
	SString leafpath = SString::toFormat("%s/%s", sIedLdName, sLeafPath).data();
	Ied *ied = FindIedByIedNo(iedNo);
	DoNode *doNode = NULL;
	DaNode *daNode = NULL;
	if (ied)
	{
		daNode = ied->findDa(leafpath.data());
		if (daNode)
			doNode = daNode->parent;
	}

	if (!doNode)
		return false;

	if (doNode->groupType != 17)
	{
		// 从数据库中获取最新值，通过内存库被动更新
		return true;
	}
	
	// 从装置召唤最新值
	// 读取失败次数太多则忽略处理
	const int kFaildCount = 2;
	const int kTimeBlock = 5;
	if (m_readIedLeafStatus[iedNo] >= kFaildCount)
	{
		static bool isAlarmed = false;
		if (::time(NULL) - m_readIedLeafTime[iedNo] < kTimeBlock)
		{
			if (isAlarmed == false)
			{
				isAlarmed = true;
				LOGDEBUG("装置(%d)定值读取失败次数超过%d次,%d秒内忽略该装置读取操作", iedNo, kFaildCount, kTimeBlock);
			}
			return true;
		}
		
		m_readIedLeafStatus[iedNo] = 0;
		isAlarmed = false;
	}

	// 查找节点指针
	std::map<std::string, stuLeafMap *>::iterator iterLeaf;
	iterLeaf = m_mapMmspathLeafMap.find(leafpath.data());
	if (iterLeaf == m_mapMmspathLeafMap.end())
	{
		LOGERROR("节点(%s)不存在", leafpath.data());
		return false;
	}
	stuLeafMap *leafMap = iterLeaf->second;

	// 通信状态
	std::map<int, int>::iterator iter = m_iedComState.find(iedNo);
	if (iter == m_iedComState.end())
	{
		LOGERROR("数据库运行装置表中未发现节点号存在且装置号为[%d]的装置", iedNo);
		return false;
	}

	if (iter->second == false)
	{
		LOGWARN("站点(%d)装置(%d)通信状态断开，取消节点读取(%s/%s)", m_subNo, iedNo, sIedLdName, sLeafPath);
		return false;
	}

	// 发送消息
	stuSpUnitAgentProcessId dst_id;
	dst_id.m_iUnitId = 0;
	stuUK9010_MSG_REQ_READ_ENTRY req_setting;
	req_setting.iSubNo = m_subNo;
	req_setting.iDevNo = iedNo;
	req_setting.iCpuNo = doNode->cpuno;
	req_setting.iGroupNo = doNode->group;
	req_setting.iEntry = doNode->entry;
	bool ret = GetUnitAgentProcessId(m_subNo, iedNo, &dst_id);
	if (!ret)
	{
		LOGERROR("GetUnitAgentProcessId failed");
		return ret;
	}

	// 命令加入发送队列
	gOperatorSn = SApplication::NewMsgSn(); //获取序列号
	oper_command_t operCommandReq;
	operCommandReq.command = UK9010_MSG_REQ_LEAF_VALUE;
	operCommandReq.sub_no = m_subNo;
	operCommandReq.ied_no = iedNo;
	operCommandReq.domName = sIedLdName;
	operCommandReq.varName = sLeafPath;
	operCommandReq.cpu_no = doNode->cpuno;
	operCommandReq.group_no = doNode->group;
	operCommandReq.entry = doNode->entry;
	operCommandReq.command_no = gOperatorSn;
	m_reqCmdListLock.lock();
	m_reqCmdList.push_back(operCommandReq);
	m_reqCmdListLock.unlock();
	
	// 通过代理发送命令
	ret = SApplication::GetPtr()->SendAgentMsg(&dst_id, UK9010_MSG_REQ_ENTRY_VALUE, gOperatorSn, 0,
		(unsigned char*)&req_setting, sizeof(req_setting));
	if (!ret)
	{
		LOGWARN("发送读当前区定值命令失败。");
	}

	// wait for response
	oper_command_t resp;
	if (!waitForCommand(UK9010_MSG_RES_ENTRY_VALUE, operCommandReq.command_no, resp, 5000))
	{
		stopCommand();
		LOGWARN("等待回复命令超时, 设备(%d)序号(%d) mmspath=%s", iedNo, operCommandReq.command_no, leafpath.data());
		m_readIedLeafStatus[iedNo]++;
		m_readIedLeafTime[iedNo] = (long)::time(NULL);
		return false;
	}
	stopCommand();

	if (resp.result)
	{
		switch (resp.valType)
		{
		case 2: // PL2221ALD0/GGIO1$SV$Ind1$subVal success, valtype=2, value=0
		case 5: // PCS931PROT/LLN0$SG$LinNo$setVal
			{
				SetLeafVarValue(leafMap, atoi(resp.value.c_str()));
			}
			break;

		case 11:
			SetLeafVarValue(leafMap, atof(resp.value.c_str()));
			break;

		case 13:
			{
				// PL2221ALD0/GGIO1$ST$Beh$q success, valtype=13, value=
				SetLeafVarValue(leafMap, atoi(resp.value.c_str()));
			}
			break;

		case 14:
			// valtype=14, value=UTC TIME seconds=0, fraction=0, qflags=0
			{
				SString attr = resp.value.c_str();
				SString seconds = SString::GetIdAttribute(1, attr, ",");
				SString fraction = SString::GetIdAttribute(2, attr, ",");
				seconds = SString::GetIdAttribute(2, seconds, "=");
				fraction = SString::GetIdAttribute(2, fraction, "=");
				SetLeafVarValue(leafMap, seconds.toInt(), fraction.toInt());
			}
			break;

		case 15:
			{
				// PL2221ALD0/GGIO1$DC$Ind6$dU success, valtype=15, value=栱??
				SetLeafVarValue(leafMap, (char*)resp.value.c_str());
			}
			break;

		case 16:
			{
				// PL2221APROT/LLN0$EX$Set243$dataNs success, valtype=16, value=SGCC MODEL:2009
				SetLeafVarValue(leafMap, (char*)resp.value.c_str());
			}
			break;

		default:
			LOGWARN("未知的数据类型(%d),当字符数据处理", resp.valType);
			SetLeafVarValue(leafMap, (char*)resp.value.c_str());
			break;
		}
		m_readIedLeafStatus[iedNo] = 0;
		LOGDEBUG("读取定值 %s/%s 成功,值:%s,类型:%d", sIedLdName, sLeafPath, resp.value.c_str(), resp.valType);
	}
	else
	{
		LOGDEBUG("读取定值 %s/%s 失败", sIedLdName, sLeafPath);
	}
	return true;
}

bool MMSServer::getReqCommand(int type, oper_command_t &cmd, int command_no)
{
	m_reqCmdListLock.lock();
	for (std::list<oper_command_t>::iterator iter = m_reqCmdList.begin(); iter != m_reqCmdList.end(); ++iter)
	{
		if ((iter->command == type && command_no == -1) 
			|| iter->command == type && iter->command_no == command_no)
		{
			cmd = *iter;
			m_reqCmdList.erase(iter);
			m_reqCmdListLock.unlock();
			return true;
		}
	}
	m_reqCmdListLock.unlock();

	return false;
}

void MMSServer::addRespCommand(oper_command_t cmd)
{
	m_respCmdListLock.lock();
	m_respCmdList.push_back(cmd);
	m_respCmdListLock.unlock();
}

void MMSServer::updateIedComState(int iedNo, int comState)
{
	m_iedComState[iedNo] = comState;
}

bool MMSServer::ProcessStateChanged(t_oe_element_state *element_state, int soc, int usec)
{
	int value = 0;
	SString svalue;
	SString valueDesc;

	switch (element_state->val_type)
	{
	case 1:	 // 双点状态（1-复归, 2-动作）
		value = element_state->current_val == 1 ? 0 : 1;
		valueDesc = SString::toFormat("%d", value);
		break;

	case 2:  // 单点
		value = element_state->current_val;
		valueDesc = SString::toFormat("%d", value);
		break;

	case 3:	 // 枚举状态
	case 4:  // 测量状态 val_type=4时是遥调信息，内容取自measure_val
		svalue = element_state->measure_val;
		valueDesc = svalue;
		break;

	default:
		value = element_state->current_val;
		valueDesc = SString::toFormat("%d", value);
		break;
	}

	bool isProcessed = false;
	Ied *ied = FindIedByIedNo(element_state->ied_no);
	if (ied)
	{
		DaNode *valNode = ied->findDa(std::string(element_state->mms_path) + "$stVal");
		DaNode *tNode = ied->findDa(std::string(element_state->mms_path) + "$t");

		if (valNode && valNode->leaf)
		{
			if (svalue.length() > 0)
				SetLeafVarValue(valNode->leaf, svalue.data());
			else
				SetLeafVarValue(valNode->leaf, value);
		}
		else
		{
			LOGDEBUG("未找到节点(%s)的值节点", (SString(element_state->mms_path) + "$stVal").data());
		}

		if (tNode && tNode->leaf)
			SetLeafVarValue(tNode->leaf, soc, usec);
		else
		{
			LOGDEBUG("未找到节点(%s)的时间节点", (SString(element_state->mms_path) + "$t").data());
		}

		if ((valNode && valNode->leaf) && (tNode && tNode->leaf))
		{
			isProcessed = true;
			LOGDEBUG("突发变位:%s, %s,值%s", element_state->name, element_state->mms_path, valueDesc.data());
			this->NewReportProc();
		}
	}

	if (!isProcessed)
	{
		LOGDEBUG("未知的遥信变位，装置(%d)CPU(%d)组(%d)条目(%d)路径(%s),值(%s)", element_state->ied_no,
			element_state->cpu_no, element_state->group_no, element_state->entry, 
			element_state->mms_path, valueDesc.data());
	}
	
	return true;
}

bool MMSServer::updateElementGeneralValue(t_oe_element_general *element_general, int soc, int usec)
{
	bool isProcessed = false;
	Ied *ied = FindIedByIedNo(element_general->ied_no);
	if (ied)
	{
		SString mmspath;
		if (strlen(element_general->da_name) > 0)
			mmspath = SString(element_general->mms_path) + "$" + element_general->da_name;
		else
			mmspath = SString(element_general->mms_path);

		DaNode *valNode = ied->findDa(mmspath.data());
		DaNode *tNode = NULL;
		SString timePath;
		
		if (valNode)
		{
			DoNode *doNode = valNode->parent;
			timePath = doNode->path() + "$t";
			tNode = ied->findDa(timePath.data());
		}

		SString svalue = element_general->current_val;
		if (valNode && valNode->leaf)
			SetLeafVarValue(valNode->leaf, svalue.data());
		else
		{
			LOGDEBUG("未找到节点(%s)的值节点", mmspath.data());
		}

		if (tNode && tNode->leaf)
			SetLeafVarValue(tNode->leaf, soc, usec);

		if (valNode && valNode->leaf)
		{
			isProcessed = true;
			LOGDEBUG("遥测变化:%s,%s,值%s", element_general->name, mmspath.data(), svalue.data());
			this->NewReportProc();
		}
	}

	if (!isProcessed)
	{
		LOGDEBUG("未知的遥测变化，装置(%d)CPU(%d)组(%d)条目(%d)路径(%s),值(%s)", element_general->ied_no,
			element_general->cpu_no, element_general->group_no, element_general->entry, 
			element_general->mms_path, element_general->current_val);
	}

	return true;
}

void MMSServer::OnTimerWork()
{
	checkCommandStatus();
}

void MMSServer::InitRunParam()
{
	SString sql;
	SRecordset rs;
	SString mmspath;

	// 运行节点
	std::map<int, std::string> mapNode;
	sql = "SELECT node_no FROM `t_oe_run_node` where module_name not like 'mst_mmsclt' order by node_no";
	DB->RetrieveRecordset(sql, rs);
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		int nodeNo = rs.GetValueInt(row, 0);
		RunNode *node = new RunNode();
		if (false == node->init(nodeNo))
		{
			LOGERROR("初始化运行节点 %d 失败", nodeNo);
			delete node;
			continue;
		}
		m_nodeList.push_back(node);
	}

	for (std::vector<RunNode *>::iterator iter = m_nodeList.begin(); iter != m_nodeList.end(); ++iter)
	{
		std::vector<int> runDevices = (*iter)->getRunDevices();

		for (std::vector<int>::iterator iterDev = runDevices.begin(); iterDev != runDevices.end(); ++iterDev)
		{
			int iedNo = *iterDev;

			Ied *ied = new Ied;
			ied->init(iedNo);
			m_iedList.push_back(ied);
		}
	}

	// 初始化m_iedLd用于快速查找
	sql = SString::toFormat("select ied_no, mms_path, comstate from t_oe_ied");
	DB->Retrieve(sql, rs);
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		int iedNo = rs.GetValueInt(row, 0);
		SString iedpath = rs.GetValueStr(row, 1);
		int comstate = rs.GetValueInt(row, 2);
		m_iedComState.insert(std::make_pair(iedNo, comstate));

		sql = SString::toFormat("select mms_path from t_oe_cpu where ied_no=%d", iedNo);
		SRecordset rsCpu;
		DB->Retrieve(sql, rsCpu);
		for (int group = 0; group < rsCpu.GetRows(); ++group)
		{
			SString ldpath = rsCpu.GetValueStr(group, 0);
// 			if (ldpath.length() > 0)
// 				ldpath = SString::GetIdAttribute(1, ldpath, "$");

			SString iedld;
			if (m_isScdMode)
				iedld = iedpath + ldpath;
			else
				iedld = m_iedName + iedpath + ldpath;

			std::map<std::string, int>::iterator iter = m_mapIedLDName.find(iedld.data());
			if (iter == m_mapIedLDName.end())
				m_mapIedLDName.insert(std::make_pair(iedld.data(), iedNo));
		}	
	}
}

//////////////////////////////////////////////////////////////////////////
bool RunNode::init(int nodeNo)
{
	SString sql;
	SRecordset rs;

	sql = SString::toFormat("SELECT node_no, name, appid FROM t_oe_run_node WHERE node_no=%d", nodeNo);
	DB->Retrieve(sql, rs);
	if (rs.GetRows() == 0)
		return false;

	m_nodeNo = rs.GetValueInt(0, 0);
	m_name = rs.GetValueStr(0, 1);
	m_nodeId = rs.GetValueInt(0, 2);

	sql = SString::toFormat("SELECT ied_no FROM t_oe_run_device WHERE node_no=%d", nodeNo);
	DB->Retrieve(sql, rs);
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		m_runDevices.push_back(rs.GetValueInt(row, 0));
	}
	
	return true;
}

bool RunNode::findDevice(int iedNo)
{
	for (std::vector<int>::iterator iter = m_runDevices.begin(); iter != m_runDevices.end(); ++iter)
	{
		if ((*iter) == iedNo)
			return true;
	}
	
	return false;
}

std::vector<int> & RunNode::getRunDevices()
{
	return m_runDevices;
}

int RunNode::getNodeNo()
{
	return m_nodeNo;
}

int RunNode::getNodeId()
{
	return m_nodeId;
}

int Ied::iedNo()
{
	return m_iedNo;
}

bool Ied::init(int ied_no)
{
	m_iedNo = ied_no;
	
	initGeneral(m_iedNo);
	initState(m_iedNo);
	initControl(m_iedNo);

	return true;
}

DoNode * Ied::findDo(std::string ref)
{
	std::map<std::string, DoNode *>::iterator iter = m_mapDo.find(ref);
	if (iter != m_mapDo.end())
		return iter->second;
	
// 	for (std::map<std::string, DoNode *>::iterator iter = m_mapDo.begin(); iter != m_mapDo.end(); ++iter)
// 	{
// 		DaNode *daNode = iter->second->findDa(ref);
// 		if (daNode)
// 			return iter->second;
// 	}

	return NULL;
}

DaNode * Ied::findDa(std::string ref)
{
	std::map<std::string, DaNode *>::iterator iterDa = m_mapDa.find(ref);
	if (iterDa != m_mapDa.end())
		return iterDa->second;

	return NULL;
}

void Ied::getAllDa(std::vector<DaNode *> &daList)
{
	for (std::map<std::string, DaNode *>::iterator iter = m_mapDa.begin(); iter != m_mapDa.end(); ++iter)
		daList.push_back(iter->second);
}

bool Ied::initGeneral(int iedNo)
{
	SString sql;
	SRecordset rs, rsGroup;

	sql = SString::toFormat("select cpu_no, group_no, type from t_oe_group where ied_no=%d", iedNo);
	DB->RetrieveRecordset(sql, rsGroup);
	for (int groupId = 0; groupId < rsGroup.GetRows(); ++groupId)
	{
		int cpuNo = rsGroup.GetValueInt(groupId, 0);
		int groupNo = rsGroup.GetValueInt(groupId, 1);
		int type = rsGroup.GetValueInt(groupId, 2);

		sql = SString::toFormat("select ied_no, cpu_no, group_no, entry, name, mms_path, da_name, valtype, current_val from t_oe_element_general where ied_no=%d and cpu_no=%d and group_no=%d", 
			iedNo, cpuNo, groupNo);
		DB->RetrieveRecordset(sql, rs);
		for (int row = 0; row < rs.GetRows(); ++row)
		{
			int cpuno = rs.GetValueInt(row, 1);
			int group = rs.GetValueInt(row, 2);
			int entry = rs.GetValueInt(row, 3);
			SString name = rs.GetValueStr(row, 4);
			SString mms_path = rs.GetValueStr(row, 5);
			SString da_name = rs.GetValueStr(row, 6);
			int valtype = rs.GetValueInt(row, 7);
			SString currentVal = rs.GetValue(row, 8);

			if (mms_path.length() > 0 && da_name.length() > 0)
			{
				mms_path += "$";
				mms_path += da_name;
			}
			if (mms_path.length() > 0)
			{
				SString doPath = mms_path;
				if (mms_path.right(7) == "$setVal")
					doPath = mms_path.left(mms_path.length() - 7);
				else if (mms_path.right(6) == "$stVal")
					doPath = mms_path.left(mms_path.length() - 6);
				else if (mms_path.right(6) == "$mag$f" || mms_path.right(6) == "$mag$i")
					doPath = mms_path.left(mms_path.length() - 6);
				else if (mms_path.right(9) == "$setMag$f" || mms_path.right(9) == "$setMag$i")
					doPath = mms_path.left(mms_path.length() - 9);
				else
				{
					doPath = SString::GetIdAttribute(1, mms_path, "$");
					doPath += "$";
					doPath += SString::GetIdAttribute(2, mms_path, "$");
					doPath += "$";
					doPath += SString::GetIdAttribute(3, mms_path, "$");
				}

				DoNode *node = new DoNode(doPath.data());
				node->cpuno = cpuno;
				node->group = group;
				node->entry = entry;
				node->groupType = type;
				node->name = name;

				DaNode *daNode = new DaNode(node);
				daNode->path = mms_path.data();
				daNode->valType = valtype;
				daNode->value = currentVal.data();
				node->addDa(daNode);
				m_mapDa.insert(std::make_pair(daNode->path, daNode));

				daNode = new DaNode(node);
				daNode->path = (doPath + "$t").data();
				daNode->valType = 99;
				node->addDa(daNode);
				m_mapDa.insert(std::make_pair(daNode->path, daNode));

				m_mapDo.insert(std::make_pair(doPath.data(), node));
			}
		}
	}

	return true;
}

bool Ied::initState(int iedNo)
{
	SString sql;
	SRecordset rs;

	sql = SString::toFormat("select ied_no, cpu_no, group_no, entry, name, mms_path, current_val from t_oe_element_state where ied_no=%d", iedNo);
	DB->RetrieveRecordset(sql, rs);
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		int cpuno = rs.GetValueInt(row, 1);
		int group = rs.GetValueInt(row, 2);
		int entry = rs.GetValueInt(row, 3);
		SString name = rs.GetValueStr(row, 4);
		SString mms_path = rs.GetValueStr(row, 5);
		SString currentVal = rs.GetValueStr(row, 6);
		if (mms_path.length() > 0)
		{
			SString doPath = mms_path;

			DoNode *node = new DoNode(doPath.data());
			node->cpuno = cpuno;
			node->group = group;
			node->entry = entry;
			node->name = name;

			DaNode *daNode = new DaNode(node);
			daNode->path = (doPath + "$stVal").data();
			daNode->value = currentVal.data();
			node->addDa(daNode);
			m_mapDa.insert(std::make_pair(daNode->path, daNode));

			daNode = new DaNode(node);
			daNode->path = (doPath + "$q").data();
			node->addDa(daNode);
			m_mapDa.insert(std::make_pair(daNode->path, daNode));

			daNode = new DaNode(node);
			daNode->path = (doPath + "$t").data();
			node->addDa(daNode);
			m_mapDa.insert(std::make_pair(daNode->path, daNode));

			m_mapDo.insert(std::make_pair(doPath.data(), node));
		}
	}

	return true;
}

bool Ied::initControl(int iedNo)
{
	SString sql;
	SRecordset rs;

	sql = SString::toFormat("select ied_no, cpu_no, group_no, entry, name, mms_path, model, type from t_oe_element_control where ied_no=%d", iedNo);
	DB->RetrieveRecordset(sql, rs);
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		int cpuno = rs.GetValueInt(row, 1);
		int group = rs.GetValueInt(row, 2);
		int entry = rs.GetValueInt(row, 3);
		SString name = rs.GetValueStr(row, 4);
		SString mms_path = rs.GetValueStr(row, 5);
		int model = rs.GetValueInt(row, 6);		// 1-直接控制; 2-操作前选择控制; 3-增强型直接控制; 4-增强型操作前选择控制
		int type = rs.GetValueInt(row, 7);		// 1-单点遥控;2-双点遥控;3-遥调;4-档位调节

		if (mms_path.length() > 0)
		{
			// CO
			SString doPath = mms_path;
			DoNode *doNode = new DoNode(doPath.data());
			doNode->cpuno = cpuno;
			doNode->group = group;
			doNode->entry = entry;
			doNode->name = name;
			doNode->controlType = type;
			m_mapDo.insert(std::make_pair(doPath.data(), doNode));

			// ST
// 			SString stDoPath = doPath.replace("$CO$", "$ST$");
// 			DoNode *stDoNode = new DoNode(doPath.data());
// 			stDoNode->cpuno = cpuno;
// 			stDoNode->group = group;
// 			stDoNode->entry = entry;
// 			stDoNode->name = name;
// 			m_mapDo.insert(std::make_pair(stDoPath.data(), stDoNode));

// 			doNode = new DoNode(doPath.data());
// 			doNode->cpuno = cpuno;
// 			doNode->group = group;
// 			doNode->entry = entry;
// 			doNode->name = name;
// 			m_mapDo.insert(std::make_pair(doPath.data(), doNode));

			// direct-with-normal-security
			DaNode *daNode = new DaNode(doNode);
			daNode->path = (doPath + "$Oper$ctlVal").data();
			doNode->addDa(daNode);
			m_mapDa.insert(std::make_pair(daNode->path, daNode));

			daNode = new DaNode(doNode);
			daNode->path = (doPath + "$Oper$T").data();
			doNode->addDa(daNode);
			m_mapDa.insert(std::make_pair(daNode->path, daNode));

			// ST
// 			daNode = new DaNode(stDoNode);
// 			daNode->path = (stDoPath + "$stVal").data();
// 			stDoNode->addDa(daNode);
// 			m_mapDa.insert(std::make_pair(daNode->path, daNode));
// 
// 			daNode = new DaNode(stDoNode);
// 			daNode->path = (stDoPath + "$q").data();
// 			stDoNode->addDa(daNode);
// 			m_mapDa.insert(std::make_pair(daNode->path, daNode));
// 
// 			daNode = new DaNode(stDoNode);
// 			daNode->path = (stDoPath + "$t").data();
// 			stDoNode->addDa(daNode);
// 			m_mapDa.insert(std::make_pair(daNode->path, daNode));

			if (model == 2 || model == 4)
			{
				// sbo-with-enhanced-security
				// SBOw
				daNode = new DaNode(doNode);
				daNode->path = (doPath + "$SBOw$ctlVal").data();
				doNode->addDa(daNode);
				m_mapDa.insert(std::make_pair(daNode->path, daNode));

				daNode = new DaNode(doNode);
				daNode->path = (doPath + "$SBOw$T").data();
				doNode->addDa(daNode);
				m_mapDa.insert(std::make_pair(daNode->path, daNode));

				// Cancel
				daNode = new DaNode(doNode);
				daNode->path = (doPath + "$Cancel$ctlVal").data();
				doNode->addDa(daNode);
				m_mapDa.insert(std::make_pair(daNode->path, daNode));

				daNode = new DaNode(doNode);
				daNode->path = (doPath + "$Cancel$T").data();
				doNode->addDa(daNode);
				m_mapDa.insert(std::make_pair(daNode->path, daNode));
			}
		}
	}

	return true;
}

DoNode::DoNode(std::string path)
{
	m_path = path;
	cpuno = 0;
	group = 0;
	entry = 0;
	groupType = 0;
	controlType = 0;
}

DoNode::~DoNode()
{
	cpuno = 0;
	group = 0;
	entry = 0;
	groupType = 0;
	controlType = 0;
}

void DoNode::clear()
{
	for (std::map<std::string, DaNode *>::iterator iter = m_daList.begin(); iter != m_daList.end(); ++iter)
		delete iter->second;
}

DaNode * DoNode::findDa(std::string name, bool fullpath)
{
	std::string path = name;
	if (fullpath == false)
	{
		if (name.at(0) != '$')
			path = m_path + "$" + name;
		else
			path = m_path + name;
	}
	std::map<std::string, DaNode *>::iterator iter = m_daList.find(path);
	if (iter != m_daList.end())
		return iter->second;
	else
		return NULL;
}

void DoNode::addDa(DaNode *da)
{
	m_daList.insert(std::make_pair(da->path, da));
}

void DoNode::getDaList(std::map<std::string, DaNode *> &daList)
{
	daList = m_daList;
}
