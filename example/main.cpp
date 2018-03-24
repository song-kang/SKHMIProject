#include <QtGui/QApplication>
#include "skbasewidget.h"
#include "example.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
	QTextCodec *codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);
#endif

	example *exp = new example;
	SKBaseWidget w(NULL,exp);
	//SKBaseWidget w;
	w.SetWindowSize(800,600);
	//w.SetWindowFixSize(1000,700);
	w.SetWindowTitle("测试");
	w.SetWindowIcon(QIcon(":/example/Resources/test.png"));
	w.SetWindowFlags(SKBASEWIDGET_MAXIMIZE | SKBASEWIDGET_MINIMIZE|SKBASEWIDGET_MAINMENU|SKBASEWIDGET_FULLSCREEN);
	w.SetWindowBackgroundImage(QPixmap(":/example/Resources/skin6.png"));
	//w.HideTopFrame();
	
	QMenu *menu = new QMenu("菜单");
	QAction *act1 = new QAction("菜单项1",NULL);
	QAction *act2 = new QAction("菜单项2",NULL);
	QAction *act3 = new QAction("菜单项3",NULL);
	menu->addAction(act1);
	menu->addAction(act2);
	menu->addAction(act3);
	w.SetWindowMainMenu(menu);
	w.Show();
	//w.ShowFullScreen();
	w.ShowMaximized();
	return a.exec();
}
