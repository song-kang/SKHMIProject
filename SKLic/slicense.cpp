#include "slicense.h"
#include "SIniFile.h"

#define SKEY "shc1218()++"

SLicense::SLicense()
{

}
SLicense::~SLicense()
{
	
}

SString SLicense::CpuId()
{
	SString id;
#ifdef WIN32
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	id= SString::toString((long)systemInfo.dwActiveProcessorMask,16).toUpper()+
		SString::toString((long)systemInfo.dwNumberOfProcessors,16).toUpper()+
		SString::toString((long)systemInfo.dwPageSize,16).toUpper()+
		SString::toString((long)systemInfo.dwProcessorType,16).toUpper()+
		SString::toString((long)systemInfo.wProcessorLevel,16).toUpper()+
		SString::toString((long)systemInfo.wProcessorRevision,16).toUpper();

	unsigned long s1,s2;
	__asm
	{
		mov eax,01h
			xor edx,edx
			cpuid
			mov s1,edx
			mov s2,eax
	}
	SString s;
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
	SString s;
	s.sprintf("%08X%08X%08X%08X",s1,s2,s3,s4);
	id += s;
#endif
	return id;
}

SString SLicense::HostId()
{
	return CpuId();
}

bool SLicense::CheckLicense()
{
	SIniFile ini;
	if (!ini.ReadIniFile("../conf/license.ini")) {
		LOGWARN("No license.ini found");
		return false;
	}

	SString val = ini.GetKeyStringValue("license", "lic");
	if (val.isEmpty()) {
		LOGWARN("No lic read");
		return false;
	}

	SString lic = SApi::Decrypt_String(val, SKEY);
	SString hostid = SString::GetIdAttribute(1, lic, "$|$");
	if (hostid != HostId())
	{
		LOGWARN("Licsense mismatching");
		return false;
	}

	if (SString::GetAttributeCount(lic, "$|$") == 3)
	{
		SString sStart = SString::GetIdAttribute(2, lic, "$|$");
		SString sEnd = SString::GetIdAttribute(3, lic, "$|$");
		SDateTime start = SDateTime::makeDateTime("yyyy-MM-dd", sStart);
		SDateTime end = SDateTime::makeDateTime("yyyy-MM-dd", sEnd);
		SDateTime today = SDateTime::currentDateTime();
		if(today<start || today>end )
		{
			LOGWARN("Licsense expired");
			return false;
		}
	}

	return true;
}
