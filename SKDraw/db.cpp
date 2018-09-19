#include "db.h"
#include "sk_database.h"

CWnd::CWnd(CWnd *parent)
	: QObject(parent)
{
	m_pParent = parent;

	m_pSvgBuffer = NULL;
	m_iSvgLen = 0;
}

CWnd::~CWnd()
{
	foreach (CWnd *p, m_lstChilds)
	{
		if (p)
			delete p;
	}
}

void CWnd::RemoveDB()
{
	RemoveChildsDB(m_lstChilds);
	RemoveChilds(m_lstChilds);

	SString sql = SString::toFormat("delete from t_ssp_uicfg_wnd where wnd_sn=%d",m_iWndSn);
	DB->Execute(sql);
}

void CWnd::RemoveChildsDB(QList<CWnd*> list)
{
	foreach (CWnd *wnd, list)
	{
		SString sql = SString::toFormat("delete from t_ssp_uicfg_wnd where wnd_sn=%d",wnd->m_iWndSn);
		DB->Execute(sql);

		RemoveChildsDB(wnd->m_lstChilds);
	}
}

void CWnd::RemoveChilds(QList<CWnd*> &list)
{
	foreach (CWnd *wnd, list)
	{
		RemoveChilds(wnd->m_lstChilds);
		delete wnd;
	}
	list.clear();
}
