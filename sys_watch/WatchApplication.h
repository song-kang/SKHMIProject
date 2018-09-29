/**
 *
 * �� �� �� : WatchApplication.h
 * �������� : 2015-8-1 13:33
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ���Ź�����APP��
 * �޸ļ�¼ : 
 *
 **/

#ifndef __WATCH_APP_H__
#define __WATCH_APP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SApplication.h"
#include "Watch.h"

class CWatchApplication : public SApplication  
{
public:
	CWatchApplication();
	virtual ~CWatchApplication();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
	// ��    ��:  SK
	// ����ʱ��:  2015-8-1 13:34
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ֹͣ���񣬹ر�����Ӧ�õķ���
	// ��    ��:  SK
	// ����ʱ��:  2015-8-1 13:34
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
	// ��    ��:  SK
	// ����ʱ��:  2016-2-3 15:20
	// ����˵��:  @wMsgType��ʾ��Ϣ����
	//         :  @pMsgHeadΪ��Ϣͷ
	//         :  @sHeadStr��Ϣͷ�ַ���
	//         :  @pBufferΪ��ϢЯ���Ķ������������ݣ�NULL��ʾ�޶���������
	//         :  @iLengthΪ���������ݳ���
	// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ�ܻ�δ����
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);
	
private:
	CWatch m_Watch;

};

#endif //__WATCH_APP_H__
