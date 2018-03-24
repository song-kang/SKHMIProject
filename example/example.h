#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QtGui/QWidget>
#include "ui_example.h"
#include "skhead.h"
#include "skwidget.h"

class example : public SKWidget
{
	Q_OBJECT

public:
	example(QWidget *parent = 0);
	~example();

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	Ui::exampleClass ui;

	QTimer *m_pTimer;

private:
	void LoadBar();
	void LoadPie();
	void LoadLine();

private slots:
	void SlotTimeout();
};

#endif // EXAMPLE_H
