#include <QtGui/QApplication>
#include "skgui.h"

#ifdef _DEBUG
#include <vld.h>
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

#if _WIN32
	a.setFont(QFont("Microsoft Yahei", 10));
#else
	int fontId = QFontDatabase::addApplicationFont(":/msyh.ttf");
	if (fontId != -1) 
	{
		QStringList lstFont = QFontDatabase::applicationFontFamilies(fontId);
		if (androidFont.size() != 0) 
		{
			QFont font(lstFont.at(0));
			font.setPixelSize(10);
			a.setFont(font);
		}
	}
#endif

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
	QTextCodec *codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);
#endif

	if (SK_GUI->Begin())
		a.exec();

	SK_GUI->End();
	return 0;
}
