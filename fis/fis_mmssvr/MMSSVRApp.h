#pragma once

#include "MMSServer.h"
#include "public/UK9010App.h"
#include "public/mdb_oe_def.h"
#include "public/mdb_na_def.h"
#define MODULE_NAME "mst_mmssvr"


class CSMmsDataset;
class MMSSVRApp : public CUK9010Application
{
public:
	struct Ied
	{
		SString name;
		SString ipa;
		SString ipb;
	};

public:
	MMSSVRApp(void);
	virtual ~MMSSVRApp(void);

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
	virtual bool ProcessAgentMsg(unsigned short wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,unsigned char* pBuffer=NULL,int iLength=0);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ı�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-13 15:12
	// ����˵��:  @sCmdΪ�������ݣ����ַ�����ʾȡ�������б�
	//         :  @sResultΪ���ؽ�������ı�
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessTxtCmd(SString &sCmd,SString &sResult);

	inline void lock(){m_Lock.lock();}
	inline void unlock(){m_Lock.unlock();}

	// Description: ����״̬��ͻ��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]��������ָ��
	//              @rows[in]������Ŀ��
	// Return:      ����ɹ�����true�����򷵻�false
	bool ProcessStateChanged(unsigned char *data, int rows);

	// Description: ����ģ����ͻ��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]��������ָ��
	//              @rows[in]������Ŀ��
	// Return:      ����ɹ�����true�����򷵻�false
	bool ProcessGeneralChanged(unsigned char *data, int rows);

	// Description: ����ͨ���豸�������ݱ仯ͻ��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]��������ָ��
	//              @rows[in]������Ŀ��
	// Return:      ����ɹ�����true�����򷵻�false
	bool ProcessControlChanged(unsigned char *data, int rows);

	// Description: ����IED�豸���ݱ仯ͻ��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]��������ָ��
	//              @rows[in]������Ŀ��
	// Return:      ����ɹ�����true�����򷵻�false
	bool ProcessIedChanged(unsigned char *data, int rows);

	// Description: �¼�ͻ���ص�
	static unsigned char* OnRealEventCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,
		int iTrgRows,int iRowSize,unsigned char *pTrgData);

	// Description: ���¼�ѭ���߳�
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @lp[in]����ָ��
	static void* ThreadMainLoop(void* lp);

protected:
	bool initStation();
	bool initScl(std::vector<Ied> &iedList);	// ����ģ���ļ���ʼ��
	bool useModelType(std::string &filename);		// ����true��ʾcid

private:
	std::map<std::string, std::vector<MMSServer*> > m_mmsServerList;
	SString m_sclFile;
	bool m_isScdMode;
	CSsp_Database *m_pDatabase;	//���ݿ������ļ�
	int	node_no;//�ڵ��
	bool is_period;//���������Ƿ�������Ч
	int	report_id;//ע�ᱨ���
	int m_stationNo;//��վ��
	SString m_stationName;
	SLock m_Lock;
};
