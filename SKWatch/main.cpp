#include "skwatch.h"
#include "skhead.h"

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

	SKWatch w;
	w.Start(argc, argv);
	w.show();
	return a.exec();
}
