/**
 *
 * �� �� �� : WatchApplication.cpp
 * �������� : 2015-8-1 13:33
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ���Ź�����APP��
 * �޸ļ�¼ : 
 *
 **/

#include "WatchApplication.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatchApplication::CWatchApplication()
{

}

CWatchApplication::~CWatchApplication()
{

}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
// ��    ��:  SK
// ����ʱ��:  2015-8-1 13:34
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatchApplication::Start()
{
	m_Watch.m_sBinPath = GetBinPath();
	m_Watch.Load(GetConfPath()+"sys_watch.xml");

	if(!IsParam("noreload"))
	{
		LOGDEBUG("���������ļ�֡�����ܣ��������ļ��ı�ʱ���Զ�Ӧ��������!����ر��������������:noreload");
		CConfigMgr::SetReloadSeconds(5);
		CConfigMgr::StartReload();
	}
	m_Watch.m_bCmdNoDog = IsParam("nodog");
	if(!m_Watch.Run(m_iArgc,m_ppArgv))
		Quit();//�˳�����
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ֹͣ���񣬹ر�����Ӧ�õķ���
// ��    ��:  SK
// ����ʱ��:  2015-8-1 13:34
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatchApplication::Stop()
{
	m_Watch.m_bQuit = true;
	while(!m_Watch.m_bQuited)
		SApi::UsSleep(1000);
	return true;
}

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
bool CWatchApplication::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	return false;
}

