#ifndef __SK_UNITCONFIG_H__
#define __SK_UNITCONFIG_H__

#include "sk_base_inc.h"

#define SK_UNITCONFIG CSKUnitconfig::GetPtr()

class SK_BASE_EXPORT CSKUnitconfig
{
public:
	CSKUnitconfig(void);
	virtual ~CSKUnitconfig(void);

	static CSKUnitconfig* GetPtr();
	bool Load(SString sPathFile);

	int GetUnitId() { return m_iUnitId; }
	SString GetUnitName() { return m_sUnitName; }
	SString GetUnitDesc() { return m_sUnitDesc; }
	SString GetSystemType() { return m_sSystemType; }

private:
	int m_iUnitId;
	SString m_sUnitName;
	SString m_sUnitDesc;
	SString m_sSystemType;

};

#endif	//__SK_UNITCONFIG_H__