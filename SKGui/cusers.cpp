#include "cusers.h"
#include "skgui.h"
#include "cfunpoint.h"

//=============== CUser ====================
CUser::CUser(CUsers *parent)
	: QObject(parent)
{
	m_pUsers = parent;
}

CUser::~CUser()
{
	foreach (stuAuth *auth, m_lstAuth)
		delete auth;
}

bool CUser::IsExistKey(QString key)
{
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == key)
			return true;
	}

	return false;
}

void CUser::SetAuth(CFunPoint *funPoint, bool auth)
{
	if (auth)
		SetAuthTrue(funPoint);
	else
		SetAuthFalse(funPoint);
}

void CUser::SetAuthTrue(CFunPoint *funPoint)
{
	if (!funPoint)
		return;

	SString sql;
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == funPoint->GetKey())
		{
			auth->auth = true;
			sql.sprintf("update t_ssp_user_auth set auth=1 where usr_sn=%d and fun_key='%s'",
				m_iSn,funPoint->GetKey().toStdString().data());
			DB->Execute(sql);
			SetAuthTrue(funPoint->m_pParent);
			break;
		}
	}
}

void CUser::SetAuthFalse(CFunPoint *funPoint)
{
	if (!funPoint)
		return;

	SString sql;
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == funPoint->GetKey())
		{
			if (funPoint->m_lstChilds.count() <= 1)
			{
				auth->auth = false;
				sql.sprintf("update t_ssp_user_auth set auth=0 where usr_sn=%d and fun_key='%s'",
					m_iSn,funPoint->GetKey().toStdString().data());
				DB->Execute(sql);
			}
			else
			{
				bool bOne = false;
				foreach (CFunPoint *p, funPoint->m_lstChilds)
				{
					if (IsAuthTrue(p->GetKey()))
					{
						bOne = true;
						break;
					}
				}
				if (bOne == false)
				{
					auth->auth = false;
					sql.sprintf("update t_ssp_user_auth set auth=0 where usr_sn=%d and fun_key='%s'",
						m_iSn,funPoint->GetKey().toStdString().data());
					DB->Execute(sql);
				}
			}

			SetAuthFalse(funPoint->m_pParent);
			break;
		}
	}
}

bool CUser::IsAuthTrue(QString key)
{
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == key)
			return auth->auth;
	}

	return false;
}

//=============== CUsers ====================
CUsers::CUsers(QObject *parent)
	: QObject(parent)
{

}

CUsers::~CUsers()
{
	foreach (CUser *user, m_lstUser)
		delete user;
	foreach (stuAuth *auth, m_lstAuth)
		delete auth;
}

bool CUsers::IsExistKey(QString key)
{
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == key)
			return true;
	}

	return false;
}

void CUsers::SetAuth(CFunPoint *funPoint, bool auth)
{
	if (auth)
	{
		SetAuthTrue(funPoint);
		foreach (CUser *user, m_lstUser)
			user->SetAuthTrue(funPoint);
	}
	else
	{
		SetAuthFalse(funPoint);
		foreach (CUser *user, m_lstUser)
			user->SetAuthFalse(funPoint);
	}
}

void CUsers::SetAuthTrue(CFunPoint *funPoint)
{
	if (!funPoint)
		return;

	SString sql;
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == funPoint->GetKey())
		{
			auth->auth = true;
			sql.sprintf("update t_ssp_usergroup_auth set auth=1 where grp_code='%s' and fun_key='%s'",
				m_sCode.toStdString().data(),funPoint->GetKey().toStdString().data());
			DB->Execute(sql);
			SetAuthTrue(funPoint->m_pParent);
			break;
		}
	}
}

void CUsers::SetAuthFalse(CFunPoint *funPoint)
{
	if (!funPoint)
		return;

	SString sql;
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == funPoint->GetKey())
		{
			if (funPoint->m_lstChilds.count() <= 1)
			{
				auth->auth = false;
				sql.sprintf("update t_ssp_usergroup_auth set auth=0 where grp_code='%s' and fun_key='%s'",
					m_sCode.toStdString().data(),funPoint->GetKey().toStdString().data());
				DB->Execute(sql);
			}
			else
			{
				bool bOne = false;
				foreach (CFunPoint *p, funPoint->m_lstChilds)
				{
					if (IsAuthTrue(p->GetKey()))
					{
						bOne = true;
						break;
					}
				}
				if (bOne == false)
				{
					auth->auth = false;
					sql.sprintf("update t_ssp_usergroup_auth set auth=0 where grp_code='%s' and fun_key='%s'",
						m_sCode.toStdString().data(),funPoint->GetKey().toStdString().data());
					DB->Execute(sql);
				}
			}
			
			SetAuthFalse(funPoint->m_pParent);
			break;
		}
	}
}

bool CUsers::IsAuthTrue(QString key)
{
	foreach (stuAuth *auth, m_lstAuth)
	{
		if (auth->fun_key == key)
			return auth->auth;
	}

	return false;
}
