#include <QApplication>
#include <QTranslator>
#include <stdio.h>

#include "contextListener.h"
#include "misc.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QString locale = QLocale::system().name().section('_', 0, 0);

	QTranslator translator;
	translator.load(QString(":/translations/context-notify_")+locale);
	app.installTranslator(&translator);

	ContextListener contextList;

	QIcon icon;
	icon.addPixmap(QPixmap(QString::fromUtf8(":/icons/images/systrayicon.png")), QIcon::Normal, QIcon::Off);

	app.setQuitOnLastWindowClosed(false);
	app.setApplicationName("Context-Notify");
	app.setWindowIcon(icon);
	app.exec();

	return 0;
}
