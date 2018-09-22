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
	void SetType(int type) { m_iType = type; }
	void SetRefSn(int sn) { m_iRefSn = sn; }
	QString GetKey() { return m_sKey; }
	QString GetDesc() { return m_sDesc; }
	bool GetAuth() { return m_bAuth; }
	int GetType() { return m_iType; }
	int GetRefSn() { return m_iRefSn; }

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
	int	m_iType;
	int m_iRefSn; //m_iType=0,1时无效;m_iType=2时为T_SSP_UICFG_WND的wnd_sn;
	
};

#endif // CFUNPOINT_H
