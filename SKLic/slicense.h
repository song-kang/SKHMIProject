#ifndef __S_LICENSE_H__
#define __S_LICENSE_H__

#include "SApi.h"

class SLicense
{
public:
	SLicense();
	virtual ~SLicense();

	bool CheckLicense();

private:
	SString CpuId();
	SString HostId();

};

#endif	//__S_LICENSE_H__
