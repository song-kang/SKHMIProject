#ifndef SKHEAD_H
#define SKHEAD_H

#include <QAbstractButton>
#include <QApplication>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QCursor>
#include <QDate>
#include <QDateTime>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <qmath.h>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>
#include <QProcess>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSettings>
#include <QSound>
#include <QStyleFactory>
#include <QStyleOption>
#include <QTextCodec>
#include <QTime>
#include <QTimer>
#include <QtGui>
#include <QUrl>
#include <QWidget>

#define COLOR_GW	"#006569"
#define COLOR_NW	"#094896"

#define qSafeDelete(p) do { if(p) delete (p); (p)=0; } while(0)

typedef enum{
	UI_ZH,
	UI_EN
}LANGUAGE;

class Common
{
	Common();
	~Common();

public:
	static QString Common::GetCurrentAppPath()
	{
		return QCoreApplication::applicationDirPath()+"/";
	}

	static QString Common::GetCurrentAppName()
	{
		return QCoreApplication::applicationName();
	}

	static bool Common::FolderExists(QString strFolder)
	{
		QDir tempFolder(strFolder);
		if(tempFolder.exists())
			return true;
		return false;
	}

	static bool Common::CreateFolder(QString strFolder)
	{
		QDir dir;
		return dir.mkdir(strFolder);
	}

	static bool Common::FileExists(QString strFile)
	{
		QFileInfo fi(strFile);
		if (fi.exists())
			return true;
		return false;
	}

	static void ClearLayout(QLayout *layout)
	{
		QLayoutItem *item = NULL;
		while((item = layout->takeAt(0)) != 0)
		{
			if(item->widget())
				delete item->widget();

			QLayout *childLayout = item->layout();
			if(childLayout)
				ClearLayout(childLayout);

			delete item;
		}
	}

	static QWidget* GetWidget(int x, int y)
	{
		return QApplication::widgetAt(x, y);
	}
};

#endif // SKHEAD_H
