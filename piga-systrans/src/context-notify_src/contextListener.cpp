#include "contextListener.h"
#include "../context-common.h"
#include "misc.h"
#include "notifyprompt.h"

#include "../libcontext_src/libcontext.h"
#include "notify.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusMessage>
#include <QCoreApplication>
#include <QMessageBox>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

ContextListener::ContextListener() : QDBusContext(), trayicon(this), \
		updomain("context-notify")
{
	//Create the notify dbus service
	if (!QDBusConnection::systemBus().isConnected())
	{
		printd("Cannot connect to the D-Bus system bus.\n"
			 "To start it, run:\n"
			 "\teval `dbus-launch --auto-syntax`\n");

		QApplication::instance()->exit(-1);
	}

	if (!QDBusConnection::systemBus().registerService(CONTEXTLISTENER_NAME))
	{
		printd("registerService Error : "+QDBusConnection::systemBus().lastError().message());
		QApplication::instance()->exit(1);
	}

	if (!QDBusConnection::systemBus().registerObject(LISTENER_PATH, this, QDBusConnection::ExportAllSlots))
	{
		printd("registerObject Error : "+QDBusConnection::systemBus().lastError().message());
		QApplication::instance()->exit(1);
	}

	//Add a systray Ico
	AchangeContext=cmenu.addAction(tr("Change the current domain"));
	cmenu.addSeparator();
	Aquit=cmenu.addAction(tr("Quit"));
	trayicon.setContextMenu(&cmenu);
	icon.addPixmap(QPixmap(QString::fromUtf8(":/icons/images/systrayicon.png")), QIcon::Normal, QIcon::Off);
	trayicon.setIcon(icon);
	trayicon.show();

	//connect systray menu
	connect(Aquit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
	connect(AchangeContext, SIGNAL(triggered()), this, SLOT(openChangeContextDialog()));
	connect(&trayicon, SIGNAL(activated ( QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
	connect(&ccd, SIGNAL(domainChanged(QString)), &updomain, SLOT(manualDomainChange(QString)));

	//Connect for updating context related content
	connect(this, SIGNAL(domainUpdated(QString, QString)), &ccd, SLOT(domainUpdated(QString, QString)));
	connect(this, SIGNAL(domainUpdated(QString, QString)), this, SLOT(domainChanged(QString, QString)));

	//Relay the signal from upcontext
	connect(&updomain, SIGNAL(domainChanged(QString, QString)), this, SIGNAL(domainUpdated(QString,QString)));

	//Emit the current domain
	DomainList::Domain_p currentDomain=clist.currentDomain();
	emit domainUpdated(currentDomain.domain_name, currentDomain.display_name);

	// Init libnotify
	initNotify("context-notify");
}

ContextListener::~ContextListener()
{
	closeNotify();
}

QString ContextListener::accept_changing_domain(const QString& domainFrom, const QString& domainTo, const QString& app_name)
{
	NotifyPrompt np;

	//Convert to UTF8
	QString domainFrom_utf8=QString::fromUtf8(domainFrom.toAscii().constData());
	QString domainTo_utf8=QString::fromUtf8(domainTo.toAscii().constData());
	QString app_name_utf8=QString::fromUtf8(app_name.toAscii().constData());

	//Get the display names (if they exist)
	DomainList::Domain_p dp_from=clist.getDomainByName(domainFrom_utf8, domainFrom_utf8);
	DomainList::Domain_p dp_to=clist.getDomainByName(domainFrom_utf8, domainTo_utf8);

	np.prompt(dp_from, dp_to, app_name_utf8);

	if(np.result()==QDialog::Accepted)
		return CONTEXT_ACCEPT;
	else if(np.result()==QDialog::Rejected)
		return CONTEXT_REFUSE;
	else
		return CONTEXT_REFUSE;
}

QString ContextListener::domain_changed(const QString& app_name, const QString& domain_name)
{
	//Change to UTF8
	QString app_name_utf8=QString::fromUtf8(app_name.toAscii().constData());

	//Get the domain from the domain name
	DomainList::Domain_p domain=clist.getDomainByName(QString::fromUtf8(domain_name.toAscii().constData()), domain_name);

	//Create the notification message
	QString msg = QObject::tr("Your are now in the domain '%1'.").arg(domain.display_name);

	// Tell the user about the transition
	sendNotification(qPrintable(tr("Domain transition")), qPrintable(msg), 2000, 1, qPrintable(domain.icon));

	//Update everyone else
	emit domainUpdated(domain.domain_name, domain.display_name);

	return DBUS_SUCCESS;
}

Q_SCRIPTABLE QString ContextListener::notify(const QString& msg, int severity)
{
	const char* iconPath=NULL;
	QString title;
	if(severity==0)
	{
		title=tr("Contextd log");
	}
	else if(severity==1)
	{
		title=tr("Contextd notification");
	}
	else if(severity==2)
	{
		title=tr("Contextd error");
	}
	else
		title=tr("Contextd");

	sendNotification(qPrintable(title), qPrintable(msg), 2000, severity, iconPath);

	return DBUS_SUCCESS;
}

void ContextListener::openChangeContextDialog()
{
	ccd.show();
}

void ContextListener::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger)
	{
		ccd.setVisible(!ccd.isVisible());
	}
}

void ContextListener::domainChanged(QString /*domain_name*/, QString display_domain_name)
{
	trayicon.setToolTip(tr("PIGA System Domain Changer\n\nCurrent domain : %1").arg(display_domain_name));
}
