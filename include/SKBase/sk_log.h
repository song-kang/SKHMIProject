#ifndef __SK_LOG_H__
#define __SK_LOG_H__

#include "sk_base_inc.h"

#define SK_LOG CSKLog::GetPtr()

class SK_BASE_EXPORT CSKLog
{
public:
	CSKLog();
	virtual ~CSKLog();

	static CSKLog* GetPtr();
	bool Load(SString sPathFile);
};

#endif	//__SK_LOG_H__
