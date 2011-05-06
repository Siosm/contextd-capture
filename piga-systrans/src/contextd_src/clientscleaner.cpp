#include <QtCore/QDir>
#include <QtCore/QDebug>

#include "clientscleaner.h"


ClientsCleaner::ClientsCleaner(QMap<pid_t, ContextClient > * clients,QObject* parent): QThread(parent), _clients(clients)
{
}


void ClientsCleaner::run()
{
	while(true){
		wait(600);

		QDir proc("/proc/");
		QStringList pidList = proc.entryList();
		QMap<pid_t, ContextClient>::const_iterator mapItr = _clients->constBegin();
		while(mapItr != _clients->constEnd()){
			if(pidList.contains(QString(mapItr.key()))){
				_clients->remove(mapItr.key());
				qDebug() << "Removing pid: " << mapItr.key() << ".";
			}
		}
	}
}
