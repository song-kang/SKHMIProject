#include "cfunpoint.h"

CFunPoint::CFunPoint(CFunPoint *parent)
	: QObject(parent)
{
	m_pParent = parent;

	m_pImageBuffer = NULL;
	m_iImageLen = 0;
}

CFunPoint::~CFunPoint()
{
	foreach (CFunPoint *p, m_lstChilds)
		delete p;
}

bool CFunPoint::IsExist(QString key)
{
	if (m_sKey == key)
		return true;

	foreach (CFunPoint *p, m_lstChilds)
	{
		if (p->IsExist(key))
			return true;
	}

	return false;
}