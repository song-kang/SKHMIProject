#ifndef CFUNPOINT_H
#define CFUNPOINT_H

#include <QObject>

class CFunPoint : public QObject
{
	Q_OBJECT

public:
	CFunPoint(CFunPoint *parent);
	~CFunPoint();

	void SetKey(QString key) { m_sKey = key; }
	void SetDesc(QString desc) { m_sDesc = desc; }
	void SetAuth(bool b) { m_bAuth = b; }
	QString GetKey() { return m_sKey; }
	QString GetDesc() { return m_sDesc; }
	bool GetAuth() { return m_bAuth; }

	bool IsExist(QString key);

public:
	CFunPoint *m_pParent;
	QList<CFunPoint*> m_lstChilds;
	unsigned char* m_pImageBuffer;
	int m_iImageLen;

private:
	QString m_sKey;
	QString m_sDesc;
	bool m_bAuth;
	
};

#endif // CFUNPOINT_H
