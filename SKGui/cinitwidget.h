#ifndef CINITWIDGET_H
#define CINITWIDGET_H

#include "ui_cinitwidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"
#include "cfunpoint.h"
#include "cusers.h"

class CInitWidget;
class LoadThread : public QThread
{
	Q_OBJECT

public:
	LoadThread(QObject *parent);
	~LoadThread();

	QString GetError() { return m_sError; }

protected:
	virtual void run();

signals:
	void SigText(QString text);

private:
	QString m_sError;

private:
	void DownLoadPicture();

private:
	CInitWidget *m_pApp;
};

///////////////////////////// CInitWidget //////////////////////////////////
class CInitWidget : public SKWidget
{
	Q_OBJECT

public:
	CInitWidget(QWidget *parent = 0);
	~CInitWidget();

private:
	Ui::CInitWidget ui;

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	QPropertyAnimation *m_pAnimation;
	LoadThread *m_pLoadThread;
	SKBaseWidget *m_pLogin;
	SKBaseWidget *m_pDBSet;

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigClose();

private slots:
	void SlotClose();
	void SlotCfg();
	void SlotDBSetClose();
	void SlotValueChanged(const QVariant &value);
	void SlotAnimationFinished();
	void SlotLoadThreadFinished();
	void SlotLoadThreadText(QString text);

private:
	SKBaseWidget *m_app;

};

#endif // CINITWIDGET_H
