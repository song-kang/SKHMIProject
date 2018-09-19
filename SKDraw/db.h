#ifndef DB_H
#define DB_H

#include <QObject>

#define WND_TYPE_FOLDER		0
#define WND_TYPE_SVG		1

class CWnd : public QObject
{
	Q_OBJECT

public:
	CWnd(CWnd *parent = 0);
	~CWnd();

	void SetWndSn(int sn) { m_iWndSn = sn; }
	void SetPWndSn(int sn) { m_iPWndSn = sn; }
	void SetWndType(int type) { m_iWndType = type; }
	void SetWndName(QString name) { m_sWndName = name; }
	void SetCreateAuth(QString auth) { m_sCreateAuth = auth; }
	void SetModifyAuth(QString auth) { m_sModifyAuth = auth; }
	void SetCreateTime(int soc) { m_iCreateTime = soc; }
	void SetModifyTime(int soc) { m_iModifyTime = soc; }

	int GetWndSn() { return m_iWndSn; }
	int GetPWndSn() { return m_iPWndSn; }
	int GetWndType() { return m_iWndType; }
	int GetCreateTime() { return m_iCreateTime; }
	int GetModifyTime() { return m_iModifyTime; }
	QString GetWndName() { return m_sWndName; }
	QString GetCreateAuth() { return m_sCreateAuth; }
	QString GetModifyAuth() { return m_sModifyAuth; }

public:
	void RemoveDB();
	void RemoveChildsDB(QList<CWnd*> list);
	void RemoveChilds(QList<CWnd*> &list);

public:
	CWnd *m_pParent;
	QList<CWnd*> m_lstChilds;
	unsigned char *m_pSvgBuffer;
	int m_iSvgLen;

private:
	int m_iWndSn;
	int m_iPWndSn;
	int m_iWndType;
	QString m_sWndName;
	QString m_sCreateAuth;
	QString m_sModifyAuth;
	int m_iCreateTime;
	int m_iModifyTime;
	
};

#endif // DB_H
