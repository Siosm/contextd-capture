#include <QtCore/QDir>
#include <QtCore/QDebug>

#include "clientscleaner.h"


ClientsCleaner::ClientsCleaner(QMap<pid_t, ContextClient > * clients, QObject* parent): QThread(parent), _clients(clients)
{
}


void ClientsCleaner::run()
{
	qDebug("Cleaning clients");
	QDir proc("/proc/");
	QStringList pidList = proc.entryList();
	QMap<pid_t, ContextClient>::const_iterator mapItr = _clients->constBegin();
	while(mapItr != _clients->constEnd()){
		qDebug() << "Looking for pid: " << mapItr.key() << ".";
		if(pidList.contains(QString("%1").arg(mapItr.key()))){
			qDebug() << "Removing pid: " << mapItr.key() << ".";
			_clients->remove(mapItr.key());
			mapItr = _clients->constBegin();
		}else{
			++mapItr;
		}
	}
}
