#include "example.h"
#include <QWebFrame>

example::example(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);}").arg(objectName()));

	ui.webView_bar->setStyleSheet("background:transparent");
	ui.webView_pie->setStyleSheet("background:transparent");
	ui.webView_line->setStyleSheet("background:transparent");

	ui.webView_bar->setContextMenuPolicy (Qt::NoContextMenu);
	ui.webView_pie->setContextMenuPolicy (Qt::NoContextMenu);
	ui.webView_line->setContextMenuPolicy (Qt::NoContextMenu);

	ui.webView_bar->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	ui.webView_pie->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	ui.webView_line->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	ui.webView_bar->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
	ui.webView_pie->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
	ui.webView_line->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

	QFile file(":/example/Resources/standBar.html");
	file.open(QIODevice::ReadOnly);
	QString html = QString::fromUtf8(file.readAll().constData());
	ui.webView_bar->setHtml(html);

	QFile file1(":/example/Resources/standPie.html");
	file1.open(QIODevice::ReadOnly);
	html = QString::fromUtf8(file1.readAll().constData());
	ui.webView_pie->setHtml(html);

	QFile file2(":/example/Resources/dynaLine.html");
	file2.open(QIODevice::ReadOnly);
	html = QString::fromUtf8(file2.readAll().constData());
	ui.webView_line->setHtml(html);

	m_pTimer = new QTimer(this);
	m_pTimer->setInterval(1000);
	m_pTimer->start();

	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(SlotTimeout()));
}

example::~example()
{

}

void example::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

static bool m_bInit = true;
void example::SlotTimeout()
{
	//LoadBar();
	//LoadPie();
	LoadLine();
	if (m_bInit)
	{
		LoadBar();
		LoadPie();
		m_bInit = false;
	}
}

void example::LoadBar()
{
	int mms1 = rand()%10;
	int tcp1 = rand()%10;
	int goose1 = rand()%10;
	int smv1 = rand()%10;
	int udp1 = rand()%10;

	int mms2 = rand()%10;
	int tcp2 = rand()%10;
	int goose2 = rand()%10;
	int smv2 = rand()%10;
	int udp2 = rand()%10;

	if(m_bInit)
	{
		QString js = tr("init(\"%1px\",\"%2px\")").arg(ui.webView_bar->height()-20).arg(ui.webView_bar->width()-20);
		ui.webView_bar->page()->mainFrame()->evaluateJavaScript(js);
	}

	QString js = tr("setValue([{value:%1},{value:%2},{value:%3},{value:%4},{value:%5}],\
			[{value:%6},{value:%7},{value:%8},{value:%9},{value:%10}])")
			.arg(mms1).arg(tcp1).arg(goose1).arg(smv1).arg(udp1)
			.arg(mms2).arg(tcp2).arg(goose2).arg(smv2).arg(udp2);

	ui.webView_bar->page()->mainFrame()->evaluateJavaScript(js);
}

void example::LoadPie()
{
	int mms = rand()%10;
	int tcp = rand()%10;
	int goose = rand()%10;
	int smv = rand()%10;
	int udp = rand()%10;

	if(m_bInit)
	{
		QString js = tr("init(\"%1px\",\"%2px\")").arg(ui.webView_pie->height()-10).arg(ui.webView_pie->width());
		ui.webView_pie->page()->mainFrame()->evaluateJavaScript(js);
	}

	QString js = tr("setValue(["
		"{value:%1, name:'开关分合状态校验'},"
		"{value:%2, name:'开关分合时间校验'},"
		"{value:%3, name:'保护动作智能告警'},"
		"{value:%4, name:'遥信不一致监视'},"
		"{value:%5, name:'遥测跳变监视'}])").arg(mms).arg(tcp).arg(goose).arg(smv).arg(udp);
	ui.webView_pie->page()->mainFrame()->evaluateJavaScript(js);
}

void example::LoadLine()
{
	if(m_bInit)
	{
		QString js = tr("init(\"%1px\",\"%2px\")").arg(ui.webView_line->height()-20).arg(ui.webView_line->width());
		ui.webView_line->page()->mainFrame()->evaluateJavaScript(js);
	}

	int mms = rand()%30+330;
	int tcp = rand()%20+220;
	int goose = rand()%10+110;
	int smv = rand()%5+35;
	int udp = rand()%2+10;

	QString js = tr("setValue(%1,%2,%3,%4,%5)").arg(mms).arg(tcp).arg(goose).arg(smv).arg(udp);
	ui.webView_line->page()->mainFrame()->evaluateJavaScript(js);
}
