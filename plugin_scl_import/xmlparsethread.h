#ifndef XMLPARSETHREAD_H
#define XMLPARSETHREAD_H

#include <QThread>
#include <QtGui>

class view_plugin_scl_import;

class XmlParseThread : public QThread
{
	Q_OBJECT

public:
	XmlParseThread(QObject *parent);
	~XmlParseThread();

protected:
	virtual void run();

private:
	view_plugin_scl_import *sclImport;
	
};

#endif // XMLPARSETHREAD_H
