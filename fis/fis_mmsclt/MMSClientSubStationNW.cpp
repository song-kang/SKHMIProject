#include "MMSClientSubStationNW.h"

//  1	¼���ļ�	װ����1_�߼��豸��_�������2_����ʱ��3_¼������4	/COMTRADE/IEDNAME/
//	2	SCD5	SCD��_�汾��_����ʱ��	/SCD/
//	3	̨���ļ�	deviceinfo.xml	/csg/deviceinfo.xml
//	4	�澯�ļ�	almInfo.xml	/csg/almInfo.xml
//	5	����ӳ���ļ�		/csg/IEDDispName.xml
//	6	����װ��̨���ļ�		/csg/IEDNAME/deviceinfo.xml
//	7	����װ�ø澯�ļ�		/csg/IEDNAME/almInfo.xml
//	8	��־�ļ�	�ļ�����ʶ���_ʱ����ַ���_��ʶ��.��׺6	/LOG/
//	9	������ά�ļ�	STAT_������_�澯��_ʱ����Ϣ	/STAT/7
//	10	Ѳ�ӱ����ļ�		/SecondDevCheck/
//	11	���챨���ļ�		/INSPECTION/
//	12	ͬԴ�ȶ��쳣�����ļ�		/SOURCE/

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

	// 6.2.3.1  �����ļ��ܿ� h)	����װ��������Ӧ�ܴӱ���װ�õ�ȡICD��CID�ļ���������վ����ģ���ļ����±��棻
	if (m_isNeedCheckCidModel)
	{
		checkModelUpdateStatus();

		m_isNeedCheckCidModel = false;
	}

	// C.2.3.3��״̬������Ϣģ��		ComStatus1	SPS	IED��status��of��communication	M	���ӦIED��ͨ��״̬
	int evt_seq;
	createIedCommStateReport(1, evt_seq);
}

void MMSClientSubStationNW::OnDisConnected()
{
	MMSClientSubStation::OnDisConnected();

	m_isNeedCheckCidModel = true;

	// C.2.3.3��״̬������Ϣģ��		ComStatus1	SPS	IED��status��of��communication	M	���ӦIED��ͨ��״̬
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
		// ���澯��Ϣ���,��֪ͨ��վ����
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

	// ����CID�ļ�
	bool ret = this->GetLatestModelFile(getIed()->iedNo(), fmt, tempFile);
	if (ret)
	{
		LOGDEBUG("��ȡװ��(%d:%s)ģ���ļ������سɹ�,%s", getIed()->iedNo(), getIed()->iedName(), tempFile.data());

		SString orgFile = SString::toFormat("%sdata/Model/%s.%s", SBASE_SAPP->GetHomePath().data(), getIed()->iedName(), SString(getModelFileFormatName(fmt)).toLower().data());
		if (SFile::exists(orgFile) && isModelFileSame(orgFile, tempFile) == false)		// �Ա��ļ��Ƿ��и���
		{
			isDiff = true;
		}
		else
		{
			LOGDEBUG("����װ��(%d:%s)ģ���ļ�", getIed()->iedNo(), getIed()->iedName());
			SFile::copy(tempFile, orgFile);
		}
	}
	else
	{
		LOGERROR("��ȡװ��(%d:%s)ģ���ļ�������ʧ��,%s", getIed()->iedNo(), getIed()->iedName(), tempFile.data());
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
		
		// ��ǰʱ��α�������
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
		LOGWARN("�ļ�1(%s)��С(%d)���ļ�2(%s)��С(%d)��ƥ��", file1.data(), fileSize1, file2.data(), fileSize2);
		return false;
	}

	bool isFileSame = true;
	// ���ζ�ȡ�ض���С�ֽ������Ա��Ƿ���ͬ
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
			LOGWARN("�ļ�1(%s)��ȡ��С(%d)���ļ�2(%s)��ȡ��С(%d)��ƥ��", file1.data(), size1, file2.data(), size2);
			break;
		}

		if (size1 > 0)
		{
			if (memcmp(buffer1, buffer2, size1) != 0)
			{
				LOGWARN("�ļ�1(%s)���ļ�2(%s)���ݲ�һ��", file1.data(), file2.data());
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
		LOGERROR("û���ҵ�DevModelSend�ڵ�");
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

	// ����t_oe_element_state��
	sql.sprintf("update t_oe_element_state set current_val=%d,soc=%d,usec=%d,details='%s',is_confirm=0 "
		"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		value, soc, usec, details.data(),
		ied_no, cpu_no, group_no, entry);
	DB->ExecuteSQL(sql);
	MDB->ExecuteSQL(sql);

	// д�뵽��ʷ����
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
		LOGFAULT("���ݿ�������ʧ�ܣ�SQL��%s",sql.data());
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

			// ������ȥ���������ز���
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

		// ������ȥ���������ز���
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

// 6.1.8  ¼���ٻ�Ҫ��
// b)	����Ԫ�ٻ�¼���ļ������ȼ�Ϊ������װ�ù���¼������F�������ɼ���Ԫ����¼������F����������װ�ü���¼������M�������ɼ���Ԫ�ֶ�¼������H����������װ������¼������S�������ɼ���Ԫ����¼������S������
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

// 6.2.3.6.8  �ڱ���װ��֧�ֵ�ǰ���£���վ��ͨ������Ԫֱ���ٻ�����װ���е���ʷ��Ϣ�����������¼����澯��Ϣ���͹���¼����
// ��ִ��ͬ6.2.3.6.2��¼���ٻ����ԡ��籣��װ�ò�֧�ְ�ʱ����ٻ�¼���б��֧�����ֹ���¼��������¼�����򱣻�װ�õ�����¼�����豣���ڹ���Ԫ�� 
bool MMSClientSubStationNW::downloadHisFaultWave()
{
	SDateTime currentTime = SDateTime::currentDateTime();
	SString comtradePath = getComtradeFileListPath(m_lastDownloadWaveListTime, currentTime);
	
	// ��ȡ�����б�
	SPtrList<stuSMmsFileInfo> slFiles;
	slFiles.setAutoDelete(true);
	if (this->GetMvlDirectory(comtradePath, slFiles) < 0)
	{
		LOGERROR("�豸(%d:%s)��ȡ�����б�ʧ��,·��(%s)", getIed()->iedNo(), getIed()->iedName(), comtradePath.data());
		return false;
	}

	// �ļ�ת��Ϊ�����б�
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
		LOGDEBUG("�豸(%d:%s)��ʷ���μ�����,û���²�����Ҫ����", getIed()->iedNo(), getIed()->iedName());
		return true;
	}

	std::map<std::string, bool> mapWaveList;
	for (std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterWave = waveFileList.begin(); iterWave != waveFileList.end();)
	{
		SString attr = iterWave->first;
		SString lastVal = SString::GetIdAttribute(SString::GetAttributeCount(attr, "_"), attr, "_");
	
		if (iterWave->second.size() < 2)
		{
			LOGERROR("�ļ��б����ļ���¼(%s)�����ļ���������ȷ,���Դ���", iterWave->first.c_str());
			std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterDel = iterWave;
			++iterWave;
			waveFileList.erase(iterDel);
		}

		// �ɼ���Ԫ����¼������F����������װ�ü���¼������M����������װ������¼������S������
		// c) ����Ԫ�Զ��ٻ�����¼����Ӧ�ڹ���Ԫ���ش洢��
		// d) ����Ԫ�����������ʵ�¼��һ�㲻�����Զ��ٻ���
		else if (lastVal.length() == 1 && (lastVal.at(0) == 'F' || lastVal.at(0) == 'M' || lastVal.at(0) == 'S'))
		{
			mapWaveList.insert(std::make_pair(iterWave->first, false));
			++iterWave;
		}
		else
		{
			LOGERROR("�����ļ���(%s)������Ҫ��,��������('F','M','S')����,������������", iterWave->first.c_str());
			std::map<std::string, std::vector<stuSMmsFileInfo *> >::iterator iterDel = iterWave;
			++iterWave;
			waveFileList.erase(iterDel);
		}

		// ʣ�������������
// 		else
// 		{
// 			mapWaveList.insert(std::make_pair(iterWave->first, false));
// 			++iterWave;
// 		}
	}

	isWaveFileInWaveList(mapWaveList);

	std::vector<std::string> waveListWithoutSort;
	std::vector<std::string> waveListSorted;

	// ������ȥ���������ļ���Ϣ
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
		LOGDEBUG("�豸(%d:%s)��ʷ���μ�����,û���²�����Ҫ����", getIed()->iedNo(), getIed()->iedName());
		return true;
	}

	sortWaveFileList(waveListWithoutSort, waveListSorted);
	waveListWithoutSort.clear();

	// ���ز����ڵ��ļ�
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
			LOGERROR("�б���δ�ҵ������ļ��� %s", iterSeq->c_str());
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
			// C.3������ԪICD�ļ�Ҫ��
			// �ļ�����	�ļ����Ƹ�ʽ����	ӳ��Ŀ¼
			// ¼���ļ�	װ����1_�߼��豸��_�������2_����ʱ��3_¼������4	/COMTRADE/IEDNAME/
			SString remoteFile = (*iterFile)->filename;
			SString localFile = SString::toFormat("%s%s", saveDir.data(), (*iterFile)->filename.data());
			if (DownMvlFile(remoteFile, localFile))
			{
				LOGDEBUG("�豸(%d:%s)���ز����ļ�(%s)������(%s)�ɹ�", getIed()->iedNo(), getIed()->iedName(), remoteFile.data(), localFile.data());
			}
			else
			{
				LOGERROR("�豸(%d:%s)���ز����ļ�(%s)������(%s)ʧ��", getIed()->iedNo(), getIed()->iedName(), remoteFile.data(), localFile.data());
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
			// ��¼��������
			SString sql = SString::toFormat("insert into t_oe_his_wave(sub_no, device_no, cpuno, act_soc, act_usec, filename, files, cfg_bytes, dat_bytes, hdr_bytes, ext_bytes)"
				"values(%d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d)", 
				m_subNo, getIed()->iedNo(), cpuNo, act_soc, act_usec,
				waveName.data(), iterWave->second.size(), cfgBytes, datBytes, hdrBytes, extBytes);
			if (DB->Execute(sql))
			{
				LOGDEBUG("�豸(%d:%s)����(%s)���ɹ�", getIed()->iedNo(), getIed()->iedName(), iterWave->first.c_str());
			}
			else
			{
				LOGERROR("�豸(%d:%s)����(%s)���ʧ��", getIed()->iedNo(), getIed()->iedName(), iterWave->first.c_str());
			}
		}
	}
	LOGDEBUG("�豸(%d:%s)��ʷ���μ�����,�����ز���%d��,�ɹ�%d��", getIed()->iedNo(), getIed()->iedName(), waveFileList.size(), downloadSuccessCount);

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
	LOGDEBUG("�豸(%d:%s)����ģ�Ͳ�һ�±��� %s, reference=%s", getIed()->iedNo(), getIed()->iedName(), ret ? "�ɹ�" : "ʧ��", reference.data());
} 

// 6.2.3.6.5  Ӧ�߱���ֵ�Զ��ٻ����ٻ����ڿ����û��趨������ֵ�˶Թ��ܣ������ֵ�ǰ��������ֵ���׼��ֵ����Ӧʱ��Ӧ����վ���Ͷ�ֵ����Ӧ�¼���
void MMSClientSubStationNW::onParamTimerChecked(bool isManual, bool isDiff)
{
	if (isManual)
		return;

	int evt_seq = 0;
	bool ret = createParamDiffReport(evt_seq);
	LOGDEBUG("�豸(%d:%s)������ֵ��һ�±��� %s", getIed()->iedNo(), getIed()->iedName(), ret ? "�ɹ�" : "ʧ��");
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
					LOGDEBUG("BTIME ����(%d)����(%d)ת��Ϊʱ����ʾ(%s),�뱾�����(%d)", days, ms, dt.toString("yyyy-MM-dd hh:mm:ss.zzz").data(), diff);

					// ����ģ����Ϣ
					int evt_seq;
					SString doName = SString::toFormat("%s%%TimeOffset", getIed()->getName().data());
					SString details = SString::toFormat("value=%d", diff);
					createModelReport(doName, 0, evt_seq, details);
				}
			}
		}
	}

	MMSClientSubStation::OnMmsReportProcess(pMmsRpt);		// ����ִ�����pMmsRpt�ڴ��Ѿ���ɾ��
}

// C.2.3.3��״̬������Ϣģ��	ActTime3	INS	IED active time of communication	M	���ӦIED�����һ��ͨ��ʱ��
// ע3������Ԫ���ӦIED�����һ��ͨ��ʱ��Ϊ����Ԫ�����յ���ӦIEDͨ�ű��ĵ�ʱ�䣬ʱ����±���ʱ����Ϊ1���ӣ�
void MMSClientSubStationNW::onRecvNewPacket(SDateTime dt)
{
	++m_lastCommunicationCount;
	checkLastCommunicationAlarmBlocked();
	if (!m_isLastCommunicationAlarmBlocked)
	{
		// ���һ��ͨ��ʱ��
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