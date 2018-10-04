/**
 *
 * �� �� �� : Watch.h
 * �������� : 2015-8-1 13:35
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ���Ź�������
 * �޸ļ�¼ : 
 *
 **/

#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SApi.h"
#include "SString.h"
//#include "WatchDog.h"
#include "sk_config_mgr.h"

//////////////////////////////////////////////////////////////////////////
// ��    ��:  stuExtModule
// ��    ��:  SK
// ����ʱ��:  2015-8-1 13:39
// ��    ��:  ���ػ����̵���Ϣ
//////////////////////////////////////////////////////////////////////////
struct stuExtModule
{
	int iSn;
	SString sModule;
	SString sCmdLine;
	SString sArg;
	SString sPath;
	int first_delay_sec;//�����������ͺ�ʱ�䣨��λ�룩
	bool is_first;
	int iStartErrTimes;//����ʧ�ܴ�����>100����60������һ��,>1000��3000������һ��
	time_t tLastStartErr;//���һ������ʧ�ܵ�ʱ��
	int iStartTimes;//������������������1�����г����������
	time_t tLastStart;//���һ�������ɹ�ʱ��
	bool is_run;
	bool is_watch;
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  CWatch
// ��    ��:  SK
// ����ʱ��:  2015-8-1 13:39
// ��    ��:  ������
//////////////////////////////////////////////////////////////////////////
class CWatch : public CConfigBase
{
public:
	CWatch();
	virtual ~CWatch();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ִ��һ���ػ�����
	// ��    ��:  SK
	// ����ʱ��:  2015-8-1 15:16
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void Watch();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���������ļ�
	// ��    ��:  SK
	// ����ʱ��:  2015-8-1 13:52
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);

	static void* ThreadWatch(void *lp);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���в��ػ����н���
	// ��    ��:  SK
	// ����ʱ��:  2015-8-1 13:40
	// ����˵��:  
	// �� �� ֵ:  true��ʾ�����ػ����У�false��ʾ��Ҫ��ֹ����
	//////////////////////////////////////////////////////////////////////////
	bool Run(int argc, char* argv[]);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ֹ���б��ػ��Ľ���
	// ��    ��:  SK
	// ����ʱ��:  2015-8-1 13:40
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void HaltAll();
	
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �Ȳ����������ػ����򣬲���ֹ
	// ��    ��:  SK
	// ����ʱ��:  2015-8-1 14:04
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void HaltOtherSelfModule();

	void SetModuleName(SString name) { m_sModuleName = name; }

	bool m_bQuit,m_bQuited;
	bool m_bHardWatch;//�Ƿ�����Ӳ�����Ź�
	SPtrList<stuExtModule> m_ExtModuleList;//��������ģ���б�
	//CWatchDogPPC m_WatchDog;//���Ź�
	SString m_sBinPath;
	bool m_bCmdNoDog;//���в������Ƿ���nodog
	SLock m_Lock;//������
	SString m_sModuleName;
};

#endif //__WATCH_DOG_H__