#ifndef CONTEXTLISTENER_H
#define CONTEXTLISTENER_H

#include <QtCore/QObject>
#include <QDBusContext>

#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMap>

#include "updatedomain.h"
#include "domainlist.h"

#include "changecontextdialog.h"

class ContextListener: public QObject, protected QDBusContext
{
Q_OBJECT
public:
	ContextListener();
	~ContextListener();
private:
	QSystemTrayIcon trayicon;
	QMenu cmenu;
	QAction *Aquit, *AchangeContext;
	QIcon icon;

	ChangeContextDialog ccd;

	UpdateDomain updomain;
	DomainList clist;

public slots:
	Q_SCRIPTABLE QString accept_changing_domain(const QString& current_domain, const QString& new_domain, const QString& app_name);
	Q_SCRIPTABLE QString domain_changed(const QString& app_name, const QString& domain_name);
	Q_SCRIPTABLE QString notify(const QString& msg, int severity);

private slots:
	void openChangeContextDialog();
	void trayIconActivated(QSystemTrayIcon::ActivationReason);

	void domainChanged(QString context_name, QString display_context_name);

signals:
	void domainUpdated(QString context_name, QString display_context_name);
};

#endif
