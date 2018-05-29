#include "skdraw.h"
#include "skhead.h"
#include "skbasewidget.h"

#ifdef _DEBUG
#include <vld.h>
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

#ifdef WIN32
	a.setFont(QFont("Microsoft Yahei", 10));
#else
	QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath()+"/../lib/plugins");
	int fontId = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath()+"/../font/msyh.ttf");
	if (fontId != -1)
	{
		QStringList lstFont = QFontDatabase::applicationFontFamilies(fontId);
		if (lstFont.size() != 0) 
		{
			QFont font(lstFont.at(0));
			font.setPixelSize(13);
			a.setFont(font);
		}
	}
#endif

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
	QTextCodec *codec = QTextCodec::codecForName("GBK");
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);
#endif

	SKDraw *draw = new SKDraw();
	SKBaseWidget w(NULL,draw);
	draw->SetApp(&w);
	draw->InitSlot();
	draw->Start();
	w.SetWindowSize(1000,600);
	w.setMinimumSize(1000,600);
	w.SetWindowTitle("SKDraw");
#ifdef WIN32
	w.SetWindowIcon(QIcon(":/images/logo"));
#else
	w.SetWindowIcon(":/images/logo");
#endif
	w.SetWindowFlags(SKBASEWIDGET_MAXIMIZE | SKBASEWIDGET_MINIMIZE|SKBASEWIDGET_FULLSCREEN);
	w.ShowMaximized();

	return a.exec();
}
