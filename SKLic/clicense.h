#ifndef __C_LICENSE_H__
#define __C_LICENSE_H__

#include "SApi.h"
#include <QString>
#include <QDate>

class CLicense
{
public:
	CLicense();
	virtual ~CLicense();

	QString CpuId();
	QString HostId();
	QString HostId(const QString& sLicFile);
	void CreateHostIdFile();
	bool CreateLicenseFile(const QString& sHostFile);
	bool CreateLicenseFileWithTime(const QString& sHostFile, const QDate& startTime, const QDate& endTime);
	bool CheckLicense();

};

#endif	//__C_LICENSE_H__
