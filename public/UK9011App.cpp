/**
 *
 * �� �� �� : UK9010App.cpp
 * �������� : 2016-2-6 9:40
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : UK9010��ά��վӦ�û�����
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-6	�ۿ�������ļ�
 *
 **/
#include "UK9011App.h"
#include "sp_config_mgr.h"

CUK9011Application::CUK9011Application()
{
	m_sSoftwareType = C_SOFTWARE_TYPE;
}

CUK9011Application::~CUK9011Application()
{
	CConfigMgr::Quit();
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
// ��    ��:  �ۿ���
// ����ʱ��:  2015-7-30 16:05
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CUK9011Application::Start()
{
	SString sFile = GetConfPath()+"sys_unitconfig.xml";
	if(!m_UnitConfig.Load(sFile))
	{
		LOGFAULT("���ص�Ԫ�����ļ�(%s)ʱʧ��!",sFile.data());
		return false;
	}
	
	m_iUnitId = m_UnitConfig.m_iUnitId;
	m_sUnitName = m_UnitConfig.m_sUnitName;
	m_sUnitDesc = m_UnitConfig.m_sUnitDesc;
	m_sSoftwareType = m_UnitConfig.m_sSystemType;
	
	return true;
}

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
bool CUK9011Application::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	S_UNUSED(wMsgType);
	S_UNUSED(pMsgHead);
	S_UNUSED(sHeadStr);
	S_UNUSED(pBuffer);
	S_UNUSED(iLength);
	return false;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��SQL�������������תΪ�ַ��������ֶ�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-5-6 16:23
// ����˵��:  @sFieldΪ�ֶ�����
// �� �� ֵ:  ���ݲ�ͬ�����ݿ����Ͷ���ͬ
//////////////////////////////////////////////////////////////////////////
SString CUK9011Application::Date2String(SString sField)
{
#if defined(OMS_ORACLE_DB)
	return "to_char("+sField+",'yyyy-mm-dd hh24:mi:ss')";
#elif defined(OMS_MYSQL_DB)
	return "date_to_str("+sField+",'%Y-%m-%d %k:%i:%s')";
#else
	return sField;
#endif
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��SQL������ַ�����Ϊ��������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-5-6 16:25
// ����˵��:  @sDateTime��ʽΪ:yyyy-MM-dd hh:mm:ss
// �� �� ֵ:  ���ݲ�ͬ�����ݿ����Ͷ���ͬ
//////////////////////////////////////////////////////////////////////////
SString CUK9011Application::String2Date(SString sDateTime)
{
#if defined(OMS_ORACLE_DB)
	return "to_date('"+sDateTime+"','yyyy-mm-dd hh24:mi:ss')";
#elif defined(OMS_MYSQL_DB)
	return "str_to_date('"+sDateTime+"','%Y-%m-%d %k:%i:%s')";
#else
	return sDateTime;
#endif
}


