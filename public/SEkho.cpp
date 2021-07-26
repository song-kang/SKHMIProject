/**
 *
 * �� �� �� : SEkho.cpp
 * �������� : 2016-2-26 18:43
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : EKHOʹ�÷�װ
 *				http://www.eguidedog.net/doc/doc_programme_with_ekho.php
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-26	�ۿ�������ļ�
 *
 **/

#include "SEkho.h"

#ifdef SSP_ECHO_USED

#include <sphelper.h>
#include <atlcom.h>

SEkho::SEkho()
{
#ifdef WIN32
	m_pSpVoice = NULL;
#endif
	m_slSpeakText.setShared(true);
}
SEkho::~SEkho()
{

}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ��������,�麯�������������б����ȵ��ô˺���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-02-27 13:51
// ����˵��:  void
// �� �� ֵ:  true��ʾ���������ɹ�,false��ʾ��������ʧ��
//////////////////////////////////////////////////////////////////////////
bool SEkho::Start()
{
	if(!SService::Start())
		return false;
	if(m_pSpVoice != NULL)
		return false;
	HRESULT hr = S_OK;
	CComPtr<IEnumSpObjectTokens> cpEnum;
	ULONG ulCount = 0;
	CComPtr<ISpObjectToken> cpToken;

	// Initialize COM
	CoInitialize(NULL);

	// Create the voice interface object
	if (FAILED(CoCreateInstance(CLSID_SpVoice, NULL, 
		CLSCTX_INPROC_SERVER, IID_ISpVoice, (void **)&m_pSpVoice))) 
	{
		LOGERROR("Fail to create instance of ISpVoice!");
		m_pSpVoice = NULL;
		return false;
	}

	if (FAILED(SpEnumTokens(SPCAT_VOICES, L"Name=Ekho Tibetan", NULL, &cpEnum)))
	{
		LOGERROR("Fail to get voices!");
		m_pSpVoice->Release();
		m_pSpVoice = NULL;
		return false;
	}

	//Get the closest token
	if (FAILED(cpEnum->Next(1, &cpToken, NULL)))
	{
		LOGERROR("Fail to get voice EkhoVoice!");
		m_pSpVoice->Release();
		m_pSpVoice = NULL;
		return false;
	}

	//set the voice 
	if (FAILED(m_pSpVoice->SetVoice( cpToken)))
	{
		LOGERROR("Fail to set voice EkhoVoice!");
		m_pSpVoice->Release();
		m_pSpVoice = NULL;
		return false;
	}
	long rate=1;
	m_pSpVoice->SetRate(rate);
	cpToken.Release();
	cpEnum.Release();
	SpeakText("");
	//���������߳�
	SKT_CREATE_THREAD(ThreadSpeak,this);
	return true;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ֹͣ�������������б����ȵ��ô˺���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-02-27 13:51
// ����˵��:  void
// �� �� ֵ:  true��ʾ����ֹͣ�ɹ�,false��ʾ����ֹͣʧ��
//////////////////////////////////////////////////////////////////////////
bool SEkho::Stop()
{
	if(!SService::Stop())
		return false;
	if(m_pSpVoice == NULL)
		return false;
	while(GetThreadCount() != 0)
		SApi::UsSleep(10000);
	//m_pSpVoice->Release();
	m_pSpVoice = NULL;
	// Shutdown COM
	CoUninitialize ();
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ����ı����ʶ���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-27 13:41
// ����˵��:  @pStrΪ�ı�����
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void SEkho::Add_Speak(char* pStr)
{
	if(m_pSpVoice == NULL)
		return ;
	if(m_slSpeakText.count() >= 5)
	{
		LOGWARN("����5���ʶ��ı����Ŷӣ����Ե�ǰ�ı�:%s",pStr);
		return;
	}
	SString str = pStr;
	m_slSpeakText.append(str);
}

void SEkho::SpeakText(char *pStr)
{
#ifdef WIN32
	WCHAR *strSrc;
	//�����ʱ�����Ĵ�С
	int i = MultiByteToWideChar(CP_ACP, 0, pStr, -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_ACP, 0, pStr, -1, strSrc, i);

	m_pSpVoice->Speak((LPCWSTR)strSrc, SPF_DEFAULT, NULL);
	SApi::UsSleep(500000);
	delete []strSrc;
#else
#endif
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �ʶ������߳�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-27 13:43
// ����˵��:  
// �� �� ֵ:  
//////////////////////////////////////////////////////////////////////////
void* SEkho::ThreadSpeak(void* lp)
{
	SEkho *pThis = (SEkho*)lp;
	SString *pStr;
	pThis->BeginThread();
	while(!pThis->IsQuit())
	{
		if(pThis->m_slSpeakText.count() == 0)
		{
			SApi::UsSleep(100000);
			continue;
		}
		pStr = pThis->m_slSpeakText.at(0);
		pThis->SpeakText(pStr->data());
		pThis->m_slSpeakText.remove(0);
	}
	pThis->EndThread();
	return NULL;
}

#endif//SSP_ECHO_USED
