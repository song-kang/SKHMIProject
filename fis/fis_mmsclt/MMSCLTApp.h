#ifndef _MMSCLT_APPLICATION_H_
#define _MMSCLT_APPLICATION_H_


#include "MMSClient.h"
#include "public/UK9010App.h"
#define MODULE_NAME "mst_mmsclt"

class MMSCLTApp : public CUK9010Application
{
public:
	enum RunMode
	{
		RunModeUnknown = 0,
		RunModeMainStation = 1,
		RunModeSubStation = 2,
	};

	// �����淶
	enum RegionalSpecification
	{
		RS_Default = 0,
		RS_NWIR = 1,			// ��������¼����
	};

public:
	MMSCLTApp(void);
	virtual ~MMSCLTApp(void);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2010-7-1 9:15
	// ����˵��:  
	// �� �� ֵ:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ֹͣ���񣬹ر�����Ӧ�õķ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2010-7-1 9:18
	// ����˵��:  
	// �� �� ֵ:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 15:20
	// ����˵��:  @wMsgType��ʾ��Ϣ����
	//         :  @pMsgHeadΪ��Ϣͷ
	//         :  @pBufferΪ��ϢЯ���Ķ������������ݣ�NULL��ʾ�޶���������
	//         :  @iLengthΪ���������ݳ���
	// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ�ܻ�δ����
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	inline void lock(){m_Lock.lock();}
	inline void unlock(){m_Lock.unlock();}

	// Description: ��ȡ����ģʽ��Ŀǰ��Ϊ��վ����ģʽ����վ����ģʽ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      ����RunModeö�ٶ���
	RunMode GetRunMode() { return m_runMode; }

private:
	CSsp_Database *m_pDatabase;	//���ݿ������ļ�
	SPtrList<MMSClient>	m_MMSMgrs;	//װ�ù������
	int	node_no;//�ڵ��
	bool is_period;//���������Ƿ�������Ч
	int	report_id;//ע�ᱨ���
	int substation_id;//��վ��
	RunMode m_runMode;						// ����ģʽ
	RegionalSpecification m_regionalSpec;	// �����淶
	SLock m_Lock;
	stuGlobalConfig m_globalConfig;
	SDateTime m_lastGenerateParamReportTime;

	static void* ThreadReadFile(void* lp); //���ڶ�ȡ�ļ��߳�
	static void* ThreadChannelMonitor(void* lp);	// ��ʱ��ֵѲ���ң��ˢ��
#ifdef TEST_CODE
	static void* ThreadTestReadFile(void* lp);
#endif
private:
	/********************************************  
	* @brief �������豸�б��м����豸���ڴ�
	* @author �ο�
	* @date 2015/12/24
	* @return ���سɹ���ʧ�� -true �ɹ� -false ʧ��
	*********************************************/ 
	bool LoadParam();

	bool LoadSubstation(int sub_no);
		
	bool LoadGlobalConfig();

	MMSClient *GetMmsClientBySubstation(int sub_no);
	MMSClient *GetMmsClientByIedNo(int sub_no, int ied_no);
	MMSClient *GetMmsClient(int sub_no, int ied_no);

	bool SendErrorAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *head,SString strMsg="");

	bool ProcessReqSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING *req_setting);

	bool ProcessReqSettingArea(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING_AREA *req_setting_area);

	bool ProcessReqEditSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_EDIT_SETTING *req_edit_setting);

	bool ProcessReqEditSettingArea(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_EDIT_SETTING_AREA *req_edit_setting_area);

	bool ProcessReqCureSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CURE_SETTING *req_cure_setting);

	bool ProcessReqCtrlSelect(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqCtrlExecute(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqCtrlCancel(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqCtrlDirect(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqFileList(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_FILE_LIST *req_file);

	bool ProcessReqFileRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_FILE *req_file);

	bool ProcessReqJournalVmd(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_VMD_SPEC *req_journal_vmd_spec);

	bool ProcessReqJournalRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_READ *req_journal_read);

	bool ProcessReqJournalStateRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_STATE_READ *req_journal_state_read);

	bool ProcessReqReadLeaf(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_READ_LEAF *req);
	bool ProcessReqReadLeaf(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_READ_LEAF_EX *req);

	bool ProcessReqSettingCheck(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_SETTING_CHECK *req);

	bool ProcessReqLatestCid(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_LATEST_CID *req);

	// ��ȡģ��������������ѹ�壬ʹ��ԭ����ֵ��ʽ
	bool ProcessReqValue(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING *req_setting);

	SPtrList<stuSMmsFileInfo> m_iReadFiles;
	bool ReadFileList();
	bool ReadFile();
	bool IsDownFile(stuUK9010_MSG_RES_FILE_LIST *file,SString localFileName);
	bool CreateDailyParamCheckReport();
};

#endif	//_MMSCLT_APPLICATION_H_