#pragma once

/*
 �Ϸ���������¼����ģ�͵���
 �����ĵ����Ϸ���������¼���������淶������壩��
*/

#include "MMSClientSubStation.h"
#include "public/UK9010App.h"


class MMSClientSubStationNW : public MMSClientSubStation
{
public:
	MMSClientSubStationNW(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName, int iedNo, SString iedDesc, SString iedName);
	virtual ~MMSClientSubStationNW();

public:
	virtual void OnConnected();
	virtual void OnDisConnected();

public:
	// Description: ����δ֪��MMS����,���յ�����˵ı��浫�Ǳ���δ�ҵ�ӳ��ʱ����
	// Author:      qianchenglin
	// Date:        2018-05-16
	// Parameter:   @pMmsRpt[in]�������
	//              @reference[in]
	// Return:      void
	virtual void OnMmsUnknownReport(stuSMmsReportItem *pMmsRptItem, SString reference);

	// Description: ��ֵѲ��󴥷�
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @isManual[in]true�Ƿ����ֶ�Ѳ�죬falseΪ�Զ�Ѳ��
	//              @isDiff[in]�Ƿ��ж�ֵ��һ��
	// Return:      void
	virtual void onParamTimerChecked(bool isManual, bool isDiff);

	// Description: ÿ���Ӵ���һ�Σ�������̳�����Զ�������
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      void
	virtual void OnTimerWork();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  MMSͻ�����洦��Ļص��麯����ʹ���걨��ṹ����Ҫͨ��delete�ͷű���ָ��
	//            ����������������غ�����ͬ������Ҳ���Խ�����뱨����н��л��洦��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-10-9 9:14
	// ����˵��:  @pMmsRpt��ʾ��ǰ���Ӷ�Ӧ��ͻ����������
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnMmsReportProcess(stuSMmsReport * pMmsRpt);
	
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ͨ�Żظ����ĺ󴥷�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-5-26 10:17
	// ����˵��:  void
	// �� �� ֵ:  NULl
	virtual void onRecvNewPacket(SDateTime dt);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ͨ���������Ӧ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-5-26 10:17
	// ����˵��:  void
	// �� �� ֵ:  NULl
	virtual void OnRequestDenial();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ͨ������ʱ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-5-26 10:17
	// ����˵��:  void
	// �� �� ֵ:  NULl
	virtual void OnRequestTimeout();

protected:
	bool checkModelUpdateStatus();
	bool checkModelFile(ModelFileFormat fmt, bool &isDiff);
	void checkLastCommunicationAlarmBlocked();				// ������ͨ��ʱ������澯
	bool isModelFileSame(SString file1, SString file2);
	bool createModelReport(SString doName, int value, int &evt_seq, SString details = "");	// ����ģ�ͱ��沢���
	bool createModelUpdateReport(int &evt_seq);			// ����ģ���ļ����±��沢���
	bool createModelAbnormalReport(int &evt_seq);		// ����ģ���쳣���沢���
	bool createParamDiffReport(int &evt_seq);			// ������ֵ��һ�±��沢���
	bool createIedCommStateReport(int commState, int &evt_seq);			// ����ͨ��״̬���沢���


	// Description: ��ȡװ�ò����ļ��б�·�������ͨ�Ų���������comtrade_path������ʹ�ø�·��,
	//              ������������req_wave_time_period=1���ʾװ��֧��ʱ�䷶Χ�ٻ������б�
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @dtFrom[in]�������ʱ��ȡ�ļ��б���Ҫ����ʱ�䷶Χ
	//              @dtTo[in]ʱ���ֹ��
	// Return:      ����װ���ļ��б�·��
	SString getComtradeFileListPath(SDateTime dtFrom, SDateTime dtTo);

	// Description: �����ļ��Ƿ�����б���
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @mapWaveList[in]key=���ݴ���Ĳ����ļ�����value=�����Ƿ����
	// Return:      void
	void isWaveFileInWaveList(std::map<std::string, bool> &mapWaveList);

	// Description: �������б�Ҫ������
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @waveList[in]��������ļ��б�
	// Return:      void
	void sortWaveFileList(std::vector<std::string> waveList, std::vector<std::string> &waveListSorted);

	// Description: ����δ���ص���ʷ����¼��
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      �ɹ�����true�����򷵻�false
	bool downloadHisFaultWave();

	// Description: �������δ���ص���ʷ����¼��,��������ؼ���ж��Ƿ񴥷���������
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      void
	void checkAndDownloadHisFaultWave();

	// ��ȡ�����ļ����ش洢·��������/��β
	SString getWaveFileLocalSaveDir();

	// ��UTC�ַ����л�ȡʱ��
	// �磺"UTC TIME day=2764467, ms=12556"
	bool getTimeFromString(SString str, int &days, int &ms);

protected:
	bool m_isNeedCheckCidModel;
	SDateTime m_lastDownloadWaveListTime;
	SDateTime m_lastCommunicationTime;
	int m_lastCommunicationCount;
	bool m_isLastCommunicationAlarmBlocked;
};
