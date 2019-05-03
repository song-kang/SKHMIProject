#ifndef _INPUTS_THREAD_H_
#define _INPUTS_THREAD_H_

#include <QThread>
#include "xmlobject.h"

class view_plugin_scl_import;

class InputsThread : public QThread
{
	Q_OBJECT

public:
	InputsThread(QObject *parent);
	~InputsThread();

protected:
	virtual void run();

private:
	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	int					sub_no;
	QString				error;
	QString				warnText;

private:
	bool findElementByState(int extrefIedNo,int extrefIntNo,QString iedName,QString mms,
		QString & error,QString &warnText);
	bool findElementByGeneral(int extrefIedNo,int extrefIntNo,int iedNo,QString mms,
		QString & error,QString &warnText);
	bool updateElementInputExtref(int extrefIedNo,int extrefIntNo,
		int iedNo,int cpuNo,int grpNo,int entry,QString & error,QString &warnText);
	int _findIedNo(QString iedName,QString & error);
	int _get_type(int extrefIedNo,int extrefIntNo,int iedNo,int cpuNo,int grpNo,QString & error,QString &warnText);

};

#endif // _INPUTS_THREAD_H_
