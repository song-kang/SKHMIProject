/**
 *
 * �� �� �� : main.cpp
 * �������� : 2015-8-1 13:31
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ���Ź�������(��Ӳ�����Ź�)
 * �޸ļ�¼ : 
 *
 **/

#include "WatchApplication.h"

CWatchApplication g_app;

int main(int argc, char* argv[])
{
	g_app.SetModuleDesc("���Ź�����");
	g_app.SetVersion("1.0.0");
	g_app.Run(argc,argv,__DATE__,__TIME__);

	return 0;
}
