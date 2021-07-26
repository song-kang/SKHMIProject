/**
 *
 * 文 件 名 : SEkho.h
 * 创建日期 : 2016-2-26 18:42
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : EKHO使用封装
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-26	邵凯田　创建文件
 *
 **/

#ifndef __SSP_EKHO_H__
#define __SSP_EKHO_H__

//#define SSP_ECHO_USED

#ifdef SSP_ECHO_USED

#include "SApi.h"
#include "SService.h"
#ifdef WIN32

//#include <iostream>
#include <sapi.h>
#include <sphelper.h>
#include <atlcom.h>
using namespace std;

#endif

class SEkho : public SService
{
public:
	SEkho();
	~SEkho();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-02-27 13:51
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-02-27 13:51
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加文本到朗读器
	// 作    者:  邵凯田
	// 创建时间:  2016-2-27 13:41
	// 参数说明:  @pStr为文本内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Add_Speak(char* pStr);

	private:

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  朗读器主线程
	// 作    者:  邵凯田
	// 创建时间:  2016-2-27 13:43
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadSpeak(void* lp);

	void SpeakText(char *pStr);
#ifdef WIN32
	ISpVoice * m_pSpVoice;
#else
#endif

	SStringList m_slSpeakText;
};
#endif

#endif//__SSP_EKHO_H__
