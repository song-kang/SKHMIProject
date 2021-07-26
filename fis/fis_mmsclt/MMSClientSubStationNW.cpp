#include "MMSClientSubStationNW.h"

//  1	录波文件	装置名1_逻辑设备名_故障序号2_故障时间3_录波性质4	/COMTRADE/IEDNAME/
//	2	SCD5	SCD名_版本号_更新时间	/SCD/
//	3	台账文件	deviceinfo.xml	/csg/deviceinfo.xml
//	4	告警文件	almInfo.xml	/csg/almInfo.xml
//	5	命名映射文件		/csg/IEDDispName.xml
//	6	保护装置台账文件		/csg/IEDNAME/deviceinfo.xml
//	7	保护装置告警文件		/csg/IEDNAME/almInfo.xml
//	8	日志文件	文件类型识别符_时间戳字符串_标识符.后缀6	/LOG/
//	9	智能运维文件	STAT_功能码_告警码_时间信息	/STAT/7
//	10	巡视报告文件		/SecondDevCheck/
//	11	定检报告文件		/INSPECTION/
//	12	同源比对异常报告文件		/SOURCE/

#define DEVICE_INFO_FILE "/csg/deviceinfo.xml"
#define ALM_INFO_FILE	 "/csg/almInfo.xml"


MMSClientSubStationNW::MMSClientSubStationNW(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName, int iedNo, SString iedDesc, SString iedName)
	: MMSClientSubStation(mmscltApp, nodeNo, substationNo, substationName, iedNo, iedDesc, iedName)
{
	m_isNeedCheckCidModel = false;
	m_lastDownloadWaveListTime = SDateTime::currentDateTime().addDays(-365);
	m_lastCommunicationCount = 0;
	m_isLastCommunicationAlarmBlocked = false;
}

MMSClientSubStationNW::~MMSClientSubStationNW()
{

}

void MMSClientSubStationNW::OnConnected()
{
	MMSClientSubStation::OnConnected();

	// 6.2.3.1  配置文件管控 h)	保护装置重启后，应能从保护装置调取ICD和CID文件，并向主站上送模型文件更新报告；
	if (m_isNeedCheckCidModel)
	{
		checkModelUpdateStatus();

		m_isNeedCheckCidModel = false;
	}

	// C.2.3.3　状态监视信息模型		ComStatus1	SPS	IED　status　of　communication	M	与对应IED的通信状态
	int evt_seq;
	createIedCommStateReport(1, evt_seq);
}

void MMSClientSubStationNW::OnDisConnected()
{
	MMSClientSubStation::OnDisConnected();

	m_isNeedCheckCidModel = true;

	// C.2.3.3　状态监视信息模型		ComStatus1	SPS	IED　status　of　communication	M	与对应IED的通信状态
	int evt_seq;
	createIedCommStateReport(0, evt_seq);
}

bool MMSClientSubStationNW::checkModelUpdateStatus()
{
	bool ret = false;
	bool isDiff = false;
	bool isHaveDiff = false;
	if (checkModelFile(CID, isDiff))
	{
		ret = true;
		isHaveDiff = isDiff ? true : isHaveDiff;
	}

	if (checkModelFile(ICD, isDiff))
	{
		ret = true;
		isHaveDiff = isDiff ? true : isHaveDiff;
	}

	if (checkModelFile(CCD, isDiff))
	{
		ret = true;
		isHaveDiff = isDiff ? true : isHaveDiff;
	}

	if (isHaveDiff)
	{
		// 将告警信息入库,并通知主站发送
		int evt_seq = 0;
		createModelUpdateReport(evt_seq);
	}

	return ret;
}

bool MMSClientSubStationNW::checkModelFile(ModelFileFormat fmt, bool &isDiff)
{
	isDiff = false;
	SString tempDir = SBASE_SAPP->GetHomePath();
	tempDir += "temp";
	if (!SDir::dirExists(tempDir))
		SDir::createDir(tempDir);

	SString tempFile = tempDir + SString::toFormat("/%s.%s", this->getIed()->iedName(), SString(getModelFileFormatName(fmt)).toLower().data());

	// 请求CID文件
	bool ret = this->GetLatestModelFile(getIed()->iedNo(), fmt, tempFile);
	if (ret)
	{
		LOGDEBUG("读取装置(%d:%s)模型文件到本地成功,%s", getIed()->iedNo(), getIed()->iedName(), tempFile.data());

		SString orgFile = SString::toFormat("%sdata/Model/%s.%s", SBASE_SAPP->GetHomePath().data(), getIed()->iedName(), SString(getModelFileFormatName(fmt)).toLower().data());
		if (SFile::exists(orgFile) && isModelFileSame(orgFile, tempFile) == false)		// 对比文件是否有更新
		{
			isDiff = true;
		}
		else
		{
			LOGDEBUG("更新装置(%d:%s)模型文件", getIed()->iedNo(), getIed()->iedName());
			SFile::copy(tempFile, orgFile);
		}
	}
	else
	{
		LOGERROR("读取装置(%d:%s)模型文件到本地失败,%s", getIed()->iedNo(), getIed()->iedName(), tempFile.data());
	}

	SFile::remove(tempFile);

	return ret;
}

void MMSClientSubStationNW::checkLastCommunicationAlarmBlocked()
{
	SDateTime currentTime = SDateTime::currentDateTime();
	int checkInteval = getIed()->getIedParam()->lastCommunicationCheckInteval;
	if (checkInteval <= 5)
		checkInteval = 60;
	const int kBlockCount = 1;

	if (m_lastDownloadWaveListTime.secsTo(currentTime) > checkInteval)
	{
		m_lastDownloadWaveListTime = currentTime;
		
		// 当前时间段闭锁次数
		if (m_lastCommunicationCount > kBlockCount)
			m_isLastCommunicationAlarmBlocked = true;
		else
		{
			m_isLastCommunicationAlarmBlocked = false;
			m_lastCommunicationCount = 0;
		}
	}
	else
	{
		if (m_lastCommunicationCount > kBlockCount)
			m_isLastCommunicationAlarmBlocked = true;
	}
}

bool MMSClientSubStationNW::isModelFileSame(SString file1, SString file2)
{
	long fileSize1 = SFile::fileSize(file1);
	long fileSize2 = SFile::fileSize(file2);

	if (fileSize1 != fileSize2)
	{
		LOGWARN("文件1(%s)大小(%d)与文件2(%s)大小(%d)不匹配", file1.data(), fileSize1, file2.data(), fileSize2);
		return false;
	}

	bool isFileSame = true;
	// 依次读取特定大小字节数，对比是否相同
	const int kBufSize = 10240;
	BYTE *buffer1 = new BYTE[kBufSize];
	BYTE *buffer2 = new BYTE[kBufSize];
	memset(buffer1, 0, kBufSize);
	memset(buffer2, 0, kBufSize);

	SFile fileHandle1(file1);
	fileHandle1.open(IO_ReadOnly);
	SFile fileHandle2(file2);
	fileHandle2.open(IO_ReadOnly);

	while (1)
	{
		long size1 = fileHandle1.readBlock(buffer1, kBufSize);
		long size2 = fileHandle2.readBlock(buffer2, kBufSize);

		if (size1 != size2)
		{
			LOGWARN("文件1(%s)读取大小(%d)与文件2(%s)读取大小(%d)不匹配", file1.data(), size1, file2.data(), size2);
			break;
		}

		if (size1 > 0)
		{
			if (memcmp(buffer1, buffer2, size1) != 0)
			{
				LOGWARN("文件1(%s)与文件2(%s)内容不一致", file1.data(), file2.data());
				break;
			}
		}

		if (size1 < kBufSize)
			break;
	}
	delete buffer1;
	delete buffer2;
	fileHandle1.close();
	fileHandle2.close();

	return isFileSame;
}

bool MMSClientSubStationNW::createModelReport(SString doName, int value, int &evt_seq, SString details)
{
	SString sql = SString::toFormat("select st_sn, ied_no, group_no, cpu_no, entry, mms_path from t_oe_element_state where mms_path like '%%s%'", doName.data());
	SRecordset rs;
	DB->Retrieve(sql, rs);
	if (rs.GetRows() == 0)
	{
		LOGERROR("没有找到DevModelSend节点");
		return false;
	}

	int col = 0;
	int st_sn = rs.GetValueInt(0, col++);
	int ied_no = rs.GetValueInt(0, col++);
	int group_no = rs.GetValueInt(0, col++);
	int cpu_no = rs.GetValueInt(0, col++);
	int entry = rs.GetValueInt(0, col++);
	SString mms_path = rs.GetValue(0, col++);

	int soc = 0;
	int usec = 0;
	SDateTime::getSystemTime(soc, usec);

	if(cpu_no != 255)
	{
		sql.sprintf("update t_oe_ied set is_confirm=0 where ied_no=%d",ied_no);
		DB->Execute(sql);			
		MDB->Execute(sql);
	}

	// 更新t_oe_element_state表
	sql.sprintf("update t_oe_element_state set current_val=%d,soc=%d,usec=%d,details='%s',is_confirm=0 "
		"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		value, soc, usec, details.data(),
		ied_no, cpu_no, group_no, entry);
	DB->ExecuteSQL(sql);
	MDB->ExecuteSQL(sql);

	// 写入到历史表中
	details = SString::toFormat("mmspath=%s;ied_no=%d;iedname=%s;", mms_path.data(), getIed()->iedNo(), getIed()->iedName());
	sql.sprintf("insert into t_oe_his_event (sub_no,ied_no,cpu_no,group_no,entry,"
		"act_soc,act_usec,fun,inf,type,"
		"severity_level,rettime,dpi,nof,fan,"
		"act_q,time_q,reason,st_sn,details) values "
		"(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,'%s')",
		getSubNo(), ied_no, cpu_no, group_no, entry,
		soc, usec, 0, 0, 0,
		0, 0, 2, 0, 0,
		0, 0, "", st_sn, details.data());
	if (!DB->Execute(sql))
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}

	return true;
}

bool MMSClientSubStationNW::createModelUpdateReport(int &evt_seq)
{
	return createModelReport("DevModelSend", 2, evt_seq);
}

bool MMSClientSubStationNW::createModelAbnormalReport(int &evt_seq)
{
	return createModelReport("DevModelDiffer", 2, evt_seq);
}

bool MMSClientSubStationNW::createParamDiffReport(int &evt_seq)
{
	return createModelReport("SetIncon", 2, evt_seq);
}

bool MMSClientSubStationNW::createIedCommStateReport(int commState, int &evt_seq)
{
	SString doName = SString::toFormat("%s%%ComStatus", getIed()->iedName());
	return createModelReport(doName, 1, evt_seq);
}

SString MMSClientSubStationNW::getComtradeFileListPath(SDateTime dtFrom, SDateTime dtTo)
{
	SString comtradePath;
	if (getIed()->getIedParam()->m_comtradePath.isEmpty())
	{
		if (getIed()->getIedParam()->m_isSupportReqWaveByTimePeriod)
		{
			comtradePath = SString::toFormat("%s_%s/COMTRADE,", 
				dtFrom.toString("yyyyMMddhhmmss").data(),
				dtTo.toString("yyyyMMddhhmmss").data());
		}
		else
			comtradePath = "/COMTRADE";
	}
	else
		comtradePath = getIed()->getIedParam()->m_comtradePath;

	return comtradePath;
}

void MMSClientSubStationNW::isWaveFileInWaveList(std::map<std::string, bool> &mapWaveList)
{
	SString condition;	// 'PCS931N2_DREC_000_20170309_103716_000', 'UK5508_RCD_28157_20160303_162539_629_F'
	int conditionCnt = 0;
	for (std::map<std::string, bool>::iterator iterWave = mapWaveList.begin(); iterWave != mapWaveList.end(); ++iterWave)
	{
		if (condition.length() > 0)
			condition += ",";
		condition += SString::toFormat("'%s'", iterWave->first.c_str());

		if (++conditionCnt >= 50)
		{
			SString sql = SString::toFormat("select filename from t_oe_his_wave where device_no=%d and filename in (%s)", getIed()->iedNo(), condition.data());
			SRecordset rs;
			DB->Retrieve(sql, rs);

			// 队列中去除掉已下载部分
			for (int idx = 0; idx < rs.GetRows(); ++idx)
			{
				std::string waveName = rs.GetValue(idx, 0).data();
				std::map<std::string, bool>::iterator iter = mapWaveList.find(waveName);
				if (iter != mapWaveList.end())
				{
					iter->second = true;
				}
			}

			conditionCnt = 0;
			condition.clear();
		}
	}

	if (conditionCnt > 0 && condition.length() > 0)
	{
		SString sql = SString::toFormat("select filename from t_oe_his_wave where device_no=%d and filename in (%s)", getIed()->iedNo(), condition.data());
		SRecordset rs;
		DB->Retrieve(sql, rs);

		// 队列中去除掉已下载部分
		for (int idx = 0; idx < rs.GetRows(); ++idx)
		{
			std::string waveName = rs.GetValue(idx, 0).data();
			std::map<std::string, bool>::iterator iter = mapWaveList.find(waveName);
			if (iter != mapWaveList.end())
			{
				iter->second = true;
			}
		}
	}
}

// 6.1.8  录波召唤要求
// b)	管理单元召唤录波文件的优先级为：保护装置故障录波（“F”）＞采集单元故障录波（“F”）＞保护装置检修录波（“M”）＞采集单元手动录波（“H”）＞保护装置启动录波（“S”）＞采集单元启动录波（“S”）。
void MMSClientSubStationNW::sortWaveFileList(std::vector<std::string> waveList, std::vector<std::string> &waveListSorted)
{
	if (!waveListSorted.empty())
		waveListSorted.clear();

	std::map<std::string, int> mapWaveF;
	std::map<std::string, int> mapWaveM;
	std::map<std::string, int> mapWaveH;
	std::map<std::string, int> mapWaveS;
	std::map<std::string, int> mapWaveOther;

	for (std::vector<std::string>::iterator iter = waveList.begin(); iter != waveList.end(); ++iter)
	{
		SString waveName = *iter;
		if (waveName.isEmpty())
			continue;
		char c = waveName.at(waveName.length() - 1);

		if (c == 'F' || c == 'f')
		{
			mapWaveF[*iter] = 1;
		}
		else if (c == 'M' || c == 'm')
		{
			mapWaveM[*iter] = 1;
		}
		else if (c == 'H' || c == 'h')
		{
			mapWaveH[*iter] = 1;
		}
		else if (c == 'S' || c == 's')
		{
			mapWaveS[*iter] = 1;
		}
		else
		{
			mapWaveOther[*iter] = 1;
		}
	}

	for (std::map<std::string, int>::iterator iter = mapWaveF.begin(); iter != mapWaveF.end(); ++iter)
		waveListSorted.push_back(iter->first);

	for (std::map<std::string, int>::iterator iter = mapWaveM.begin(); iter != mapWaveM.end(); ++iter)
		waveListSorted.push_back(iter->first);

	for (std::map<std::string, int>::iterator iter = mapWaveH.begin(); iter != mapWaveH.end(); ++iter)
		waveListSorted.push_back(iter->first);

	for (std::map<std::string, int>::iterator iter = mapWaveS.begin(); iter != mapWaveS.end(); ++iter)
		waveListSorted.push_back(iter->first);

	for (std::map<std::string, int>::iterator iter = mapWaveOther.begin(); iter != mapWaveOther.end(); ++iter)
		waveListSorted.push_back(iter->first);
}

void MMSClientSubStationNW::checkAndDownloadHisFaultWave()
{
	if (!IsConnected())
		return;

	SDateTime currentTime = SDateTime::currentDateTime();
	if (m_lastDownloadWaveListTime.secsTo(currentTime) > 60 * 5)
	{
		if (getIed()->isAutoDownloadWave())
		{
			downloadHisFaultWave();
			m_lastDownloadWaveListTime = currentTime;
		}
	}
}

// 6.2.3.6.8  在保护装置支持的前提下，主站可通过管理单元直接召唤保护装置中的历史信息（包括动作事件、告警信息）和故障录波，
// 并执行同6.2.3.6.2的录波召唤策略。如保护装置不支持按时间段召唤录波列表或不支持区分故障录波和启动录波，则保护装置的所有录波均需保存在管理单元。 
bool MMSClientSubStationNW::downloadHisFaultWave()
{
	SDateTime currentTime = SDateTime::currentDateTime();
	SString comtradePath = getComtradeFileListPath(m_lastDownloadWaveListTime, currentTime);
	
	// 获取波形列表
	SPtrList<stuSMmsFileInfo> slFiles;
	slFiles.setAutoDelete(true);
	if (this->GetMvlDirectory(comtradePath, slFiles) < 0)
	{
		LOGERROR("设备(%d:%s)获取波形列表失败,路径(%s)", getIed()->iedNo(), getIed()->iedName(), comtradePath.data());
		return false;
	}

	// 文件转换为波形列表
	std::map<std::string, std::vector<stuSMmsFileInfo *> > waveFileList;
	for (int idx = 0; idx < slFiles.count(); ++idx)
	{
		stuSMmsFileInfo *file = slFiles.at(idx);
		std::string waveName;
		std::string filename = file->filename.data();
		int pos = filename.find_last_of('.');
		if (pos != std::string::npos)
		{
			waveName = filename.substr(0, pos);
			waveFileList[waveName].push_back(file);
		}
	}

	if (waveFileList.size() == 0)
	{
		LOGDEBUG("设备(%d:%s)历史波形检查完成,没有新波形需要下载", getIed()->iedNo(), getIed()->iedName());
		return true;
	}

	std::map<std::string, bool> mapWaveList;
	for (std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterWave = waveFileList.begin(); iterWave != waveFileList.end();)
	{
		SString attr = iterWave->first;
		SString lastVal = SString::GetIdAttribute(SString::GetAttributeCount(attr, "_"), attr, "_");
	
		if (iterWave->second.size() < 2)
		{
			LOGERROR("文件列表中文件记录(%s)持有文件数量不正确,忽略处理", iterWave->first.c_str());
			std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterDel = iterWave;
			++iterWave;
			waveFileList.erase(iterDel);
		}

		// 采集单元故障录波（“F”）＞保护装置检修录波（“M”）＞保护装置启动录波（“S”）；
		// c) 管理单元自动召唤上述录波后，应在管理单元本地存储；
		// d) 管理单元对于其他性质的录波一般不采用自动召唤。
		else if (lastVal.length() == 1 && (lastVal.at(0) == 'F' || lastVal.at(0) == 'M' || lastVal.at(0) == 'S'))
		{
			mapWaveList.insert(std::make_pair(iterWave->first, false));
			++iterWave;
		}
		else
		{
			LOGERROR("波形文件名(%s)不符合要求,优先下载('F','M','S')波形,忽略其他处理", iterWave->first.c_str());
			std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterDel = iterWave;
			++iterWave;
			waveFileList.erase(iterDel);
		}

		// 剩余情况都可下载
// 		else
// 		{
// 			mapWaveList.insert(std::make_pair(iterWave->first, false));
// 			++iterWave;
// 		}
	}

	isWaveFileInWaveList(mapWaveList);

	std::vector<std::string> waveListWithoutSort;
	std::vector<std::string> waveListSorted;

	// 队列中去掉已下载文件信息
	for (std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterWave = waveFileList.begin(); iterWave != waveFileList.end();)
	{
		std::map<std::string, bool>::iterator iter = mapWaveList.find(iterWave->first);
		if (iter == mapWaveList.end() || iter->second == true)
		{
			std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterDel = iterWave;
			++iterWave;
			waveFileList.erase(iterDel);
		}
		else	
		{
			waveListWithoutSort.push_back(iterWave->first);
			++iterWave;
		}
	}

	if (waveListWithoutSort.size() == 0)
	{
		LOGDEBUG("设备(%d:%s)历史波形检查完成,没有新波形需要下载", getIed()->iedNo(), getIed()->iedName());
		return true;
	}

	sortWaveFileList(waveListWithoutSort, waveListSorted);
	waveListWithoutSort.clear();

	// 下载不存在的文件
	int downloadSuccessCount = 0;
	int cpuNo = 1;
	if (getIed()->getIedParam()->m_RCDCpuNo != 0)
		cpuNo = getIed()->getIedParam()->m_RCDCpuNo;
	SString saveDir = getWaveFileLocalSaveDir();
	if (SDir::dirExists(saveDir) == false)
		SDir::createDir(saveDir);

	for (std::vector<std::string>::iterator iterSeq = waveListSorted.begin(); iterSeq != waveListSorted.end(); ++iterSeq)
	{
		std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterWave = waveFileList.find(*iterSeq);
		if (iterWave == waveFileList.end())
		{
			LOGERROR("列表中未找到波形文件名 %s", iterSeq->c_str());
			continue;
		}
	
		bool isWaveDownloadSuccess = true;
		SString waveName = iterWave->first.c_str();
		int cfgBytes = 0;
		int datBytes = 0;
		int hdrBytes = 0;
		int extBytes = 0;
		int act_soc = 0;
		int act_usec = 0;

		for (std::vector<stuSMmsFileInfo *>::iterator iterFile = iterWave->second.begin(); iterFile != iterWave->second.end(); ++iterFile)
		{
			// C.3　管理单元ICD文件要求
			// 文件类型	文件名称格式定义	映射目录
			// 录波文件	装置名1_逻辑设备名_故障序号2_故障时间3_录波性质4	/COMTRADE/IEDNAME/
			SString remoteFile = (*iterFile)->filename;
			SString localFile = SString::toFormat("%s%s", saveDir.data(), (*iterFile)->filename.data());
			if (DownMvlFile(remoteFile, localFile))
			{
				LOGDEBUG("设备(%d:%s)下载波形文件(%s)到本地(%s)成功", getIed()->iedNo(), getIed()->iedName(), remoteFile.data(), localFile.data());
			}
			else
			{
				LOGERROR("设备(%d:%s)下载波形文件(%s)到本地(%s)失败", getIed()->iedNo(), getIed()->iedName(), remoteFile.data(), localFile.data());
				isWaveDownloadSuccess = false;
			}

			if (iterFile == iterWave->second.begin())
			{
				act_soc = (int)(*iterFile)->mtime;
			}

			SString ext = (*iterFile)->filename.right(3);
			if (ext.CompareNoCase("CFG") == 0)
				cfgBytes = (*iterFile)->fsize;
			else if (ext.CompareNoCase("DAT") == 0)
				datBytes = (*iterFile)->fsize;
			else if (ext.CompareNoCase("HDR") == 0)
				hdrBytes = (*iterFile)->fsize;
			else
				extBytes += (*iterFile)->fsize;
		}

		if (isWaveDownloadSuccess)
		{
			++downloadSuccessCount;
			// 记录存入数据
			SString sql = SString::toFormat("insert into t_oe_his_wave(sub_no, device_no, cpuno, act_soc, act_usec, filename, files, cfg_bytes, dat_bytes, hdr_bytes, ext_bytes)"
				"values(%d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d)", 
				m_subNo, getIed()->iedNo(), cpuNo, act_soc, act_usec,
				waveName.data(), iterWave->second.size(), cfgBytes, datBytes, hdrBytes, extBytes);
			if (DB->Execute(sql))
			{
				LOGDEBUG("设备(%d:%s)波形(%s)入库成功", getIed()->iedNo(), getIed()->iedName(), iterWave->first.c_str());
			}
			else
			{
				LOGERROR("设备(%d:%s)波形(%s)入库失败", getIed()->iedNo(), getIed()->iedName(), iterWave->first.c_str());
			}
		}
	}
	LOGDEBUG("设备(%d:%s)历史波形检查完成,共下载波形%d个,成功%d个", getIed()->iedNo(), getIed()->iedName(), waveFileList.size(), downloadSuccessCount);

	return true;
}

SString MMSClientSubStationNW::getWaveFileLocalSaveDir()
{
	SString dir = SString::toFormat("%s/shared/COMTRADE/%s/", 
		SBASE_SAPP->GetHomePath().data(),
		getIed()->iedName());

	return dir;
}

void MMSClientSubStationNW::OnMmsUnknownReport(stuSMmsReportItem *pMmsRptItem, SString reference)
{
	int evt_seq = 0;
	bool ret = createModelAbnormalReport(evt_seq);
	LOGDEBUG("设备(%d:%s)创建模型不一致报告 %s, reference=%s", getIed()->iedNo(), getIed()->iedName(), ret ? "成功" : "失败", reference.data());
} 

// 6.2.3.6.5  应具备定值自动召唤（召唤周期可由用户设定）及定值核对功能，当发现当前区保护定值与基准定值不对应时，应向主站发送定值不对应事件。
void MMSClientSubStationNW::onParamTimerChecked(bool isManual, bool isDiff)
{
	if (isManual)
		return;

	int evt_seq = 0;
	bool ret = createParamDiffReport(evt_seq);
	LOGDEBUG("设备(%d:%s)创建定值不一致报告 %s", getIed()->iedNo(), getIed()->iedName(), ret ? "成功" : "失败");
}

void MMSClientSubStationNW::OnTimerWork()
{
	checkAndDownloadHisFaultWave();
	checkLastCommunicationAlarmBlocked();
}

void MMSClientSubStationNW::OnMmsReportProcess(stuSMmsReport * pMmsRpt)
{
	bool isGeneralInterrogation = false;
	for (int idx = 0; idx < pMmsRpt->Items.count(); ++idx)
	{
		stuSMmsReportItem *pMmsRptItem = pMmsRpt->Items.at(idx);
		SString reason = GetMvlVarText(pMmsRptItem->Reason);
		if (reason.length() == 6 && reason.right(1) == "1")
		{
			isGeneralInterrogation = true;
			break;
		}
	}

	if (isGeneralInterrogation)	
	{
		SDateTime currentTime = SDateTime::currentDateTime();
		static SDateTime lastCheckTime;
		if (lastCheckTime.secsTo(currentTime) > 10)
		{
			lastCheckTime = currentTime;

			SPtrList<MMSValue> valueList;
			valueList.setAutoDelete(true);
			GetMvlVarValueList(pMmsRpt->TimeOfEntry, valueList);
			if (valueList.count() > 0)
			{
				int days = 0;
				int ms = 0;
				if (getTimeFromString(valueList.at(0)->getStrValue(), days, ms))
				{
					SDateTime dt = SDateTime(SDate(1984, 1, 1));
					dt = dt.addDays(days);
					dt = dt.addMSecs(ms);
					int diff = dt.msecsTo(currentTime);
					LOGDEBUG("BTIME 天数(%d)毫秒(%d)转换为时间显示(%s),与本机相差(%d)", days, ms, dt.toString("yyyy-MM-dd hh:mm:ss.zzz").data(), diff);

					// 更新模型信息
					int evt_seq;
					SString doName = SString::toFormat("%s%%TimeOffset", getIed()->getName().data());
					SString details = SString::toFormat("value=%d", diff);
					createModelReport(doName, 0, evt_seq, details);
				}
			}
		}
	}

	MMSClientSubStation::OnMmsReportProcess(pMmsRpt);		// 函数执行完后pMmsRpt内存已经被删除
}

// C.2.3.3　状态监视信息模型	ActTime3	INS	IED active time of communication	M	与对应IED的最后一次通信时间
// 注3：管理单元与对应IED的最后一次通信时间为管理单元最后接收到对应IED通信报文的时间，时间更新闭锁时间宜为1分钟；
void MMSClientSubStationNW::onRecvNewPacket(SDateTime dt)
{
	++m_lastCommunicationCount;
	checkLastCommunicationAlarmBlocked();
	if (!m_isLastCommunicationAlarmBlocked)
	{
		// 最后一次通信时间
		int evt_seq;
		SString doName = SString::toFormat("%s%%ActTime", getIed()->getName().data());
		SString details = SString::toFormat("value=%d", dt.soc());
		createModelReport(doName, 0, evt_seq, details);
	}
}

void MMSClientSubStationNW::OnRequestDenial()
{
	int evt_seq;
	SString doName = SString::toFormat("%s%%SvcAlm", getIed()->getName().data());
	SString details = SString::toFormat("value=%d", SDateTime::currentDateTime().soc());
	createModelReport(doName, 0, evt_seq, details);
}

void MMSClientSubStationNW::OnRequestTimeout()
{
	int evt_seq;
	SString doName = SString::toFormat("%s%%SvcAlm", getIed()->getName().data());
	SString details = SString::toFormat("value=%d", SDateTime::currentDateTime().soc());
	createModelReport(doName, 0, evt_seq, details);
}

bool MMSClientSubStationNW::getTimeFromString(SString str, int &days, int &ms)
{
	bool isOk = true;
	SString s = str.replace(" ", ",");
	SString value = SString::GetAttributeValue(s, "day", "=", ",");
	if (!value.isEmpty())
		days = value.toInt();
	else
		isOk = false;

	value = SString::GetAttributeValue(s, "ms", "=", ",");
	if (!value.isEmpty())
		ms = value.toInt();
	else
		isOk = false;

	return isOk;
}