#include "clicense.h"
#include <QSettings>
#include <QFileInfo>
#include <QStringList>

#define SKEY "shc1218()++"

CLicense::CLicense()
{

}
CLicense::~CLicense()
{
	
}

QString CLicense::CpuId()
{
	QString id;
#ifdef WIN32
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	id= QString::number(systemInfo.dwActiveProcessorMask,16).toUpper()+
		QString::number(systemInfo.dwNumberOfProcessors,16).toUpper()+
		QString::number(systemInfo.dwPageSize,16).toUpper()+
		QString::number(systemInfo.dwProcessorType,16).toUpper()+
		QString::number(systemInfo.wProcessorLevel,16).toUpper()+
		QString::number(systemInfo.wProcessorRevision,16).toUpper();

	unsigned long s1,s2;
	__asm
	{
		mov eax,01h
			xor edx,edx
			cpuid
			mov s1,edx
			mov s2,eax
	}
	QString s;
	s.sprintf("%08X%08X",s1,s2);
	id += s;
#else
	unsigned long s1,s2,s3,s4;
	asm volatile
		(
		"movl $0x01,%%eax;\n\t"  
		"xorl %%edx,%%edx;\n\t"  
		"cpuid;\n\t"   
		"movl %%edx,%0;\n\t"  
		"movl %%eax,%1;\n\t"  
		:"=m"(s1),"=m"(s2)
		);
	asm volatile     
		(
		"movl $0x03,%%eax;\n\t"  
		"xorl %%ecx,%%ecx;\n\t"  
		"xorl %%edx,%%edx;\n\t"  
		"cpuid;\n\t"  
		"movl %%edx,%0;\n\t"  
		"movl %%ecx,%1;\n\t"  
		:"=m"(s3),"=m"(s4)  
		);   
	QString s;
	s.sprintf("%08X%08X%08X%08X",s1,s2,s3,s4);
	id += s;
#endif
	return id;
}

QString CLicense::HostId()
{
	return CpuId();
}

QString CLicense::HostId(const QString& sLicFile)
{
	QFileInfo fi(sLicFile);
	if(fi.fileName()!="license.ini")
		return "no license.ini";

	QSettings lic(sLicFile,QSettings::IniFormat);
	lic.beginGroup("license");
	QString licString = lic.value("lic").toString();
	lic.endGroup();
	if(licString.isEmpty())
		return "no lic value";

	QString s = SApi::Decrypt_String(licString.toStdString().data(), SKEY);
	return s;
}

void CLicense::CreateHostIdFile()
{
	QSettings setting("../conf/hostid.ini",QSettings::IniFormat);
	setting.beginGroup("host");
	setting.setValue("id",HostId());
	setting.endGroup();
}

bool CLicense::CreateLicenseFile(const QString& sHostFile)
{
	QFileInfo fi(sHostFile);
	if(fi.fileName()!="hostid.ini")
		return false;

	QSettings host(sHostFile,QSettings::IniFormat);
	host.beginGroup("host");
	QString hostId = host.value("id").toString();
	host.endGroup();
	if(hostId.isEmpty())
		return false;

	QString licString = SApi::Encrypt_String(hostId.toStdString().data(), SKEY);
	QString licFile = sHostFile;
	licFile.replace("hostid.ini","license.ini");
	QSettings lic(licFile,QSettings::IniFormat);
	lic.beginGroup("license");
	lic.setValue("lic",licString);
	lic.endGroup();
	return true;
}

bool CLicense::CreateLicenseFileWithTime(const QString& sHostFile, const QDate& startTime, const QDate& endTime)
{
	QFileInfo fi(sHostFile);
	if(fi.fileName()!="hostid.ini")
		return false;

	QSettings host(sHostFile,QSettings::IniFormat);
	host.beginGroup("host");
	QString hostId = host.value("id").toString();
	host.endGroup();
	if(hostId.isEmpty())
		return false;

	hostId += "$|$" + startTime.toString("yyyy-MM-dd");
	hostId += "$|$" + endTime.toString("yyyy-MM-dd");
	QString licString = SApi::Encrypt_String(hostId.toStdString().data(), SKEY);
	QString licFile = sHostFile;
	licFile.replace("hostid.ini","license.ini");
	QSettings lic(licFile,QSettings::IniFormat);
	lic.beginGroup("license");
	lic.setValue("lic",licString);
	lic.endGroup();
	return true;
}

bool CLicense::CheckLicense()
{
	QSettings lic("../conf/license.ini",QSettings::IniFormat);
	lic.beginGroup("license");
	QString licString = lic.value("lic").toString();
	lic.endGroup();
	if(licString.isEmpty())
		return false;

	licString = SApi::Decrypt_String(licString.toStdString().data(), SKEY);
	QStringList sList = licString.split("$|$");
	if (sList.at(0) != HostId())
	{
		LOGDEBUG("Licsense mismatching");
		return false;
	}

	if (sList.count() == 3)
	{
		QDate start = QDate::fromString(sList.at(1),"yyyy-MM-dd");
		QDate end = QDate::fromString(sList.at(2),"yyyy-MM-dd");
		QDate today = QDate::currentDate();
		if(today<start || today>end )
		{
			LOGDEBUG("Licsense expired");
			return false;
		}
	}

	return true;
}
