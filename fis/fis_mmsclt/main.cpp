#include "MMSCLTApp.h"

/**
 * Ver    Date        Author  Comments
 * -----  ----------  ------  -------------------------------------------
 * 1.0.0  2015-11-16  �ο�    �ӱ��Զ�����ά��վHMI��ʼ�汾
 *
 **/

#define MOD_VERSION "1.0.2"

#ifndef MOD_DATE
#define MOD_DATE "2018-11-13"
#endif
#ifndef MOD_TIME
#define MOD_TIME "13:50:00"
#endif

#ifdef _DEBUG
//#include "vld.h"
#endif

int main(int argc, char* argv[])
{
	MMSCLTApp g_app;
	g_app.SetModuleDesc("IEC61850�ɼ���վ�������");
	g_app.SetVersion(MOD_VERSION);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME,true);
	g_app.Quit();
	return 0;
}

