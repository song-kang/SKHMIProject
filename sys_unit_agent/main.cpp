/**
 *
 * �� �� �� : main.cpp
 * �������� : 2014-4-26 21:38
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ��Ԫ����������
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-26	�ۿ�������ļ�
 *
 **/


//////////////////////////////////////////////////////////////////////////
// History
// 2.0.0    2014-04-26 �ۿ����ʼ�汾
//////////////////////////////////////////////////////////////////////////

#define MOD_VERSION "2.0.0"

#ifndef MOD_DATE
#define MOD_DATE "2014-05-20"
#endif
#ifndef MOD_TIME
#define MOD_TIME "12:36:00"
#endif

#include "UnitMgrApplication.h"
#include "..\SKLic\slicense.h"
CUnitMgrApplication g_app;

#ifdef WIN32
#include "vld.h"
#endif

int main(int argc, char* argv[])
{
	SLicense lic;
	if(!lic.CheckLicense()) {
		printf("Licsence error\n");
		return 1;
	}

	g_app.SetModuleDesc("��Ԫ����������");
	g_app.SetVersion(MOD_VERSION);
	g_app.SetApplicationId(SP_UA_APPNO_AGENT);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME);
	return 0;
}
