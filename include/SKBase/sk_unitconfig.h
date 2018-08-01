#ifndef __SK_UNITCONFIG_H__
#define __SK_UNITCONFIG_H__

#include "sk_base_inc.h"

#define SK_UNITCONFIG CSKUnitconfig::GetPtr()

class SK_BASE_EXPORT CSpUnit
{
public:
	CSpUnit() {};
	~CSpUnit() {};

	int m_iUnitId;
	SString m_sUnitCommIp;
	SString m_sUnitName;
	SString m_sUnitDesc;
};

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
	SString GetUnitIp() { return m_sThisIp; }

public:
	int m_iUnitId;
	SString m_sUnitName;
	SString m_sUnitDesc;
	SString m_sSystemType;
	SString m_sThisIp;
	SPtrList<CSpUnit> m_Units;

};

#endif	//__SK_UNITCONFIG_H__