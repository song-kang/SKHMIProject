#include "sk_log.h"
#include "SXmlConfig.h"

static CSKLog* g_pLog = NULL;

CSKLog::CSKLog()
{

}
CSKLog::~CSKLog()
{
	
}

CSKLog* CSKLog::GetPtr()
{
	if(g_pLog == NULL)
		g_pLog = new CSKLog();

	return g_pLog;
}

bool CSKLog::Load(SString sPathFile)
{
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
	{
		printf("����־�����ļ�ʧ��!���޷�ʹ����־��¼����! file=%s",sPathFile.data());
		return false;
	}
	SString sModule = SLog::getModuleName();
	SBaseConfig *pPrecommand = xml.SearchChild("log<module='"+sModule+"'>");
	if(pPrecommand == NULL)
	{
		//ȥ��.exe����һ��
		if(sModule.right(4).toLower() == ".exe")
			sModule = sModule.left(sModule.length()-4);
		pPrecommand = xml.SearchChild("log<module='"+sModule+"'>");
	}
	if(pPrecommand == NULL)//û��ר�õģ���ȱʡ��
		pPrecommand = xml.SearchChild("log<module=''>");
	if(pPrecommand == NULL)//û��ר�õģ���ȱʡ��
		pPrecommand = xml.SearchChild("log<module='*'>");
	if(pPrecommand == NULL)
	{
		printf("û���ҵ����ʵ���־������Ϣ!��ȡ��־����ʧ��!\n");
		return false;
	}

	SString level = pPrecommand->GetAttribute("level").toLower();
	if(level == "basedebug")
		SLog::setLogLevel(SLog::LOG_BASEDEBUG);
	else if(level == "debug")
		SLog::setLogLevel(SLog::LOG_DEBUG);
	else if(level == "warn")
		SLog::setLogLevel(SLog::LOG_WARN);
	else if(level == "error")
		SLog::setLogLevel(SLog::LOG_ERROR);
	else if(level == "fault")
		SLog::setLogLevel(SLog::LOG_FAULT);
	else if(level == "none" || level == "" || level == "null" || level == "no")
		SLog::setLogLevel(SLog::LOG_NONE);
	else
		SLog::setLogLevel(SLog::LOG_DEBUG);
	if(pPrecommand->SearchNodeAttribute("concole","open").toLower() == "true")
		SLog::startLogToConsole();
	else
		SLog::stopLogToConsole();
	if(pPrecommand->SearchNodeAttribute("tcp","open").toLower() == "true")
	{
		SString ip = pPrecommand->SearchNodeAttribute("tcp","ip");
		int port = pPrecommand->SearchNodeAttributeI("tcp","port");
		SLog::startLogToTCPSvr(ip.data(),port);
	}
	if(pPrecommand->SearchNodeAttribute("file","open").toLower() == "true")
	{
		SString file = pPrecommand->SearchNodeAttribute("file","file");
		file = file.replace("[@modulename]",sModule);
		int size = pPrecommand->SearchNodeAttributeI("file","size");
		int files = pPrecommand->SearchNodeAttributeI("file","files");
		if(files < 1)
		{
			printf("��Ч����־�ļ�������:%d����ǿ����Ϊ1��\n",files);
			files = 1;
		}
		if(size < 1024)
		{
			printf("��С����־�ļ���С����:%d,��ǿ����Ϊ1024!\n",size);
			size = 1024;
		}
		if(size > 104857600)//100M
		{
			printf("�������־�ļ���С����:%d,��ǿ����Ϊ104857600!",size);
			size = 104857600;
		}
		SLog::startLogToFile(file.data());
		SLog::setLogFileMaxSize(size);
		SLog::setLogFileMaxBackFiles(files);
	}

	return true;
}
