/**
 *
 * 文 件 名 : SEkho.cpp
 * 创建日期 : 2016-2-26 18:43
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : EKHO使用封装
 *				http://www.eguidedog.net/doc/doc_programme_with_ekho.php
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-26	邵凯田　创建文件
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
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-02-27 13:51
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
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
	//启动服务线程
	SKT_CREATE_THREAD(ThreadSpeak,this);
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-02-27 13:51
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
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
// 描    述:  添加文本到朗读器
// 作    者:  邵凯田
// 创建时间:  2016-2-27 13:41
// 参数说明:  @pStr为文本内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SEkho::Add_Speak(char* pStr)
{
	if(m_pSpVoice == NULL)
		return ;
	if(m_slSpeakText.count() >= 5)
	{
		LOGWARN("超过5个朗读文本在排队，忽略当前文本:%s",pStr);
		return;
	}
	SString str = pStr;
	m_slSpeakText.append(str);
}

void SEkho::SpeakText(char *pStr)
{
#ifdef WIN32
	WCHAR *strSrc;
	//获得临时变量的大小
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
// 描    述:  朗读器主线程
// 作    者:  邵凯田
// 创建时间:  2016-2-27 13:43
// 参数说明:  
// 返 回 值:  
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
