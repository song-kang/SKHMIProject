/**
 *
 * �� �� �� : WatchDogPPC.h
 * �������� : 2011-4-13 9:26
 * �޸����� : $Date: 2011/08/31 08:43:32 $
 * ��ǰ�汾 : $Revision: 1.1 $
 * �������� : PowerPC8315ƽ̨�µ�Ӳ�����Ź�������
 * �޸ļ�¼ : 
 *
 **/

#if !defined(AFX_WATCHDOGPPC_H__056E28A6_8C67_4A4C_9FF7_D1C6CB3B4E77__INCLUDED_)
#define AFX_WATCHDOGPPC_H__056E28A6_8C67_4A4C_9FF7_D1C6CB3B4E77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PPC_WATCHDOG_DEVICE "/dev/watchdog"

#include "SApi.h"
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

class CWatchDogPPC  
{
public:
	CWatchDogPPC();
	virtual ~CWatchDogPPC();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �򿪿��Ź��豸
	// ��    ��:  SK
	// ����ʱ��:  2011-4-13 9:26
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool Open();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ι������δ���豸ʱ�Զ���
	// ��    ��:  SK
	// ����ʱ��:  2011-4-13 9:27
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool FeedDog();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �رտ��Ź���ֹͣ��Ӳ���ػ�����
	// ��    ��:  SK
	// ����ʱ��:  2011-4-13 9:27
	// ����˵��:  void
	// �� �� ֵ:  true
	//////////////////////////////////////////////////////////////////////////
	bool Close();

private:
	int m_hHandle;//���Ź����þ��
};

#endif // !defined(AFX_WATCHDOGPPC_H__056E28A6_8C67_4A4C_9FF7_D1C6CB3B4E77__INCLUDED_)
