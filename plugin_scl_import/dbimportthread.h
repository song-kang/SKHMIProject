#ifndef DBIMPORTTHREAD_H
#define DBIMPORTTHREAD_H

#include <QThread>
#include "xmlobject.h"

class view_plugin_scl_import;

class DbImportThread : public QThread
{
	Q_OBJECT

public:
	DbImportThread(QObject *parent);
	~DbImportThread();

	void setName(QString name) { iedName = name; }
	void setDesc(QString desc) { iedDesc = desc; }
	void setIedNo(int no) { iedNo = no; }
	void setSubstation(int no) { substationNo = no; }

public:
	Qt::CheckState	checkStateS1;
	Qt::CheckState	checkStateG1;
	Qt::CheckState	checkStateM1;

protected:
	virtual void run();

	bool dbImport(QString iedName,QString & error,QString &warnText);

private:
	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	QString				iedName;
	QString				iedDesc;
	int					iedNo;
	QString				error;
	QString				warnText;
	int					substationNo;

};

#endif // DBIMPORTTHREAD_H
