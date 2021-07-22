
#include <QtCore/QCoreApplication>
#include "..\SKLic\clicense.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	CLicense lic;
	lic.CreateHostIdFile();

	return 0;
}
