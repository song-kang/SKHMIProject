/**
 *
 * �� �� �� : UnitMgrApplication.h
 * �������� : 2014-4-26 21:45
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ��Ԫ����Ӧ�ó�����
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-26	�ۿ�������ļ�
 *
 **/

#ifndef __UNIT_MGR_APPLICATION_H__
#define __UNIT_MGR_APPLICATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SApplication.h"
#include "AgentEx.h"
#include "mcast_founder.h"
#include "sk_unitconfig.h"

class CUnitMgrApplication : public SApplication  
{
public:
	struct stuPuttingFileParam
	{
		SString sFileName;//�ļ���
		SFile file;//�ļ�
		int begin_soc;//��ʼʱ��
	};
	CUnitMgrApplication();
	virtual ~CUnitMgrApplication();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014:4:26 21:46
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ֹͣ���񣬹ر�����Ӧ�õķ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014:4:26 21:46
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	#ifndef _WITHOUT_AGENT_CHANNEL_
	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 15:20
	// ����˵��:  @wMsgType��ʾ��Ϣ����
	//         :  @pMsgHeadΪ��Ϣͷ
	//         :  @sHeadStr��Ϣͷ�ַ���
	//         :  @pBufferΪ��ϢЯ���Ķ������������ݣ�NULL��ʾ�޶���������
	//         :  @iLengthΪ���������ݳ���
	// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ı�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-13 15:12
	// ����˵��:  @sCmdΪ�������ݣ����ַ�����ʾȡ�������б�
	//         :  @sResultΪ���ؽ�������ı�
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessTxtCmd(SString &sCmd,SString &sResult);

	#endif

	CSKUnitconfig m_UnitConfig;
	CSpUnitAgent m_Agent;
	CMCastFounder m_MCastFounder;
	SPtrList<stuPuttingFileParam> m_PuttingParam;

private:
	inline stuPuttingFileParam* SearchPuttingParam(SString &sFileName)
	{
		register unsigned long pos = 0;
		stuPuttingFileParam *p = m_PuttingParam.FetchFirst(pos);
		while(p)
		{
			if(p->sFileName == sFileName)
				return p;
			p = m_PuttingParam.FetchNext(pos);
		}
		return NULL;
	}

private:
	static void* ThreadLic(void* lp);

public:
	void ExitByLic() {
		exit(-1);
	}

};

#endif //__UNIT_MGR_APPLICATION_H__
