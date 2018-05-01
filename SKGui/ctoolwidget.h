#ifndef CTOOLWIDGET_H
#define CTOOLWIDGET_H

#include "ui_ctoolwidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"
#include "skgui.h"

class CHMIWidget;
class CToolWidget : public SKWidget
{
	Q_OBJECT

public:
	struct stuToolButton
	{
		stuToolButton()
		{
			m_pToolButton = NULL;
			m_bLock = false;
		}
		QToolButton *m_pToolButton;
		bool m_bLock;
	};
public:
	CToolWidget(QWidget *parent = 0);
	~CToolWidget();

	void InsertToolButton(QToolButton *btn);
	void SetToolButtonClicked(QString name);
	void SetToolButtonClicked(QToolButton *btn);
	void SetToolButtonUnclicked(QToolButton *btn);
	QPoint GetStartPos() { return ui.btnStart->pos(); }
	void CreateToolButton(QString name, QString desc, QIcon icon);
	void DeleteToolButton(QString name);
	void DeleteAllToolButton();
	void RefreshArrow();

public:
	QList<stuToolButton*> m_lstToolButton;

private:
	Ui::CToolWidget ui;

	QTimer *m_pDateTimer;
	QMap<int,QString> m_weekMap;
	QMenu *m_pFunPointMenu;
	QAction *m_pFunPointCloseAction;
	QAction *m_pFunPointLockAction;
	QString m_sCurrentFunName;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void resizeEvent(QResizeEvent *);
	virtual void mousePressEvent(QMouseEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigStart();

private slots:
	void SlotLeft();
	void SlotRight();
	void SlotDesktop();
	void SlotToolButtonClick();
	void SlotDateTime();
	void SlotTriggerMenu(QAction *action);

public slots:
	void SlotStart();

private:
	CHMIWidget *m_pHmi;

};

#endif // CTOOLWIDGET_H
