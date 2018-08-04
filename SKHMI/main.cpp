#include <QtGui/QApplication>
#include "skgui.h"

#ifdef _DEBUG
#include <vld.h>
#endif

class GlobalApplication : public QApplication
{
public:
	GlobalApplication(int &argc,char **argv) : QApplication(argc,argv) {};
	~GlobalApplication() {};

	bool notify(QObject *o, QEvent *e)
	{
		if (e->type() == QEvent::KeyPress ||
			e->type() == QEvent::MouseMove ||
			e->type() == QEvent::MouseButtonPress ||
			e->type() == QEvent::MouseButtonDblClick)
		{
			SK_GUI->m_iLoginOutTime = 0;
		}

		return QApplication::notify(o,e);
	}
};

int main(int argc, char *argv[])
{
	GlobalApplication a(argc, argv);

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

	if (SK_GUI->Begin(argv))
		a.exec();

	SK_GUI->End();
	return 0;
}
