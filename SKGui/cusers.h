#ifndef CUSERS_H
#define CUSERS_H

#include <QObject>

class CUsers;
class CUser : public QObject
{
	Q_OBJECT

public:
	CUser(CUsers *parent);
	~CUser();

	void SetSn(qint32 sn) { m_iSn = sn; }
	void SetCode(QString code) { m_sCode = code; }
	void SetGrpCode(QString code) { m_sGrpCode = code; }
	void SetName(QString name) { m_sName = name; }
	void SetPassword(QString password) { m_sPassword = password; }
	void SetDesc(QString desc) { m_sDesc = desc; }
	void SetEmail(QString email) { m_sEmail = email; }
	void SetMobile(QString mobile) { m_sMobile = mobile; }
	void SetLoginTime(qint32 time) { m_iLoginTime = time; }
	void SetLoginTimeout(qint32 time) { m_iLoginTimeout = time; }

	qint32 GetSn() { return m_iSn; }
	QString GetCode() { return m_sCode; }
	QString GetGrpCode() {return m_sGrpCode; }
	QString GetName() { return m_sName; }
	QString GetPassword() { return m_sPassword; }
	QString GetDesc() { return m_sDesc; }
	QString GetEmail() { return m_sEmail; }
	QString GetMobile() { return m_sMobile; }
	quint32 GetLoginTime() { return m_iLoginTime; }
	quint32 GetLoginTimeout() { return m_iLoginTimeout; }

public:
	QList<QString> m_lstAuth;

private:
	qint32 m_iSn;
	QString m_sCode;
	QString m_sGrpCode;
	QString m_sName;
	QString m_sPassword;
	QString m_sDesc;
	QString m_sEmail;
	QString m_sMobile;
	quint32 m_iLoginTime;
	quint32 m_iLoginTimeout;

};

//=============== CUsers ====================
class CUsers : public QObject
{
	Q_OBJECT

public:
	CUsers(QObject *parent = 0);
	~CUsers();

	void SetCode(QString code) { m_sCode = code; }
	void SetName(QString name) { m_sName = name; }
	void SetDesc(QString desc) { m_sDesc = desc; }

	QString GetCode() { return m_sCode; }
	QString GetName() { return m_sName; }
	QString GetDesc() { return m_sDesc; }

public:
	QList<CUser*> m_lstUser;
	QList<QString> m_lstAuth;

private:
	QString m_sCode;
	QString m_sName;
	QString m_sDesc;
	
};

#endif // CUSERS_H
