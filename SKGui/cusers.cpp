#include "cusers.h"

//=============== CUser ====================
CUser::CUser(CUsers *parent)
	: QObject(parent)
{

}

CUser::~CUser()
{

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
}
