#include <QtGui/QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QSettings>
#include "editorconfig.h"
#include "mainwindow.h"
#include "configurationstack.h"
#include "sessionrestoredialog.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("Context Editor");
	a.setOrganizationName("pigaos");//LIFO SDS

	//Store settings as ini-files because windows's register is not supported by file watchers
	QSettings::setDefaultFormat(QSettings::IniFormat);

	//Get the PIDs of running instances
	QStringList pids=QSettings().value("instancePIDs", QStringList()).toStringList();

	//If other instances are meant to run, check that they exist
	bool spawnedDialog=false;
	bool sessionRestored=false;
	for(int i=0; i<pids.size(); ++i)
	{
		//If the instance can not be found
		if((pids.at(i).toInt()>0) && (!QFile::exists("/proc/"+pids.at(i))))
		{
			//Spawn the restore dialog at least once
			if(!spawnedDialog)
			{
				QString filePath=QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/autosave/"+pids.at(i)+".xml";
				if(QFile::exists(filePath))
				{
					spawnedDialog=true;
					SessionRestoreDialog dialog(filePath);

					dialog.show();
					dialog.exec();

					if(dialog.result()==QMessageBox::Accepted)
						sessionRestored=true;
				}
			}

			//Remove the invalid PID from the list
			pids.removeAt(i);
			--i;
		}
	}

	//Open last file if reopenLastFile is checked in the settings, and if no file was previously restored
	if(EditorConfig::reopenLastFile() && !sessionRestored)
		ConfigurationStack::instance().openTransitionsFile(QSettings().value("recentTransitionFiles").toStringList().at(0));

	//Add our instance PID to the list, and save it to the settings
	pids << QString("%1").arg(getpid());
	QSettings().setValue("instancePIDs", pids);

	//Run the main window
	MainWindow w;
#if defined(Q_WS_S60)
	w.showMaximized();
#else
	w.show();
#endif
	int result=a.exec();

	//Remove our instance PID from the list of running instances
	pids=QSettings().value("instancePIDs", QStringList()).toStringList();
	pids.removeAll(QString("%1").arg(getpid()));
	QSettings().setValue("instancePIDs", pids);

	QDir().remove(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+QString("/autosave/%1.xml").arg(getpid()));

	return result;
}
