#include "contextListener.h"
#include "../context-common.h"
#include "misc.h"
#include "notifyprompt.h"

#include "../libcontext_src/libcontext.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusMessage>
#include <QCoreApplication>
#include <QMessageBox>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

ContextListener::ContextListener() : QDBusContext(), trayicon(this), \
		upcontext("context-notify")
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
	AchangeContext=cmenu.addAction(tr("Change the current system context"));
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
	connect(&ccd, SIGNAL(contextChanged(QString)), &upcontext, SLOT(manualContextChange(QString)));

	//Connect for updating context related content
	connect(this, SIGNAL(systemContextUpdated(QString, QString)), &ccd, SLOT(systemContextUpdated()));
	connect(this, SIGNAL(systemContextUpdated(QString, QString)), this, SLOT(systemContextChanged(QString, QString)));

	//Relay the signal from upcontext
	connect(&upcontext, SIGNAL(globalContextChanged(QString, QString)), this, SIGNAL(systemContextUpdated(QString,QString)));

	//Emit the current state
	char* cContextBuffer=new char[101];
	if(context_current_global_context(cContextBuffer, 101)==CONTEXT_TRUE)
		emit systemContextUpdated(cContextBuffer, cContextBuffer);	//Fix the display name
	delete cContextBuffer;
}

QString ContextListener::accept_changing_state(const QString& scope, const QString& current_state, const QString& new_state, const QString& app_name)
{
	NotifyPrompt np;

	//Convert to UTF8
	QString current_state_utf8=QString::fromUtf8(current_state.toAscii().constData());
	QString new_state_utf8=QString::fromUtf8(new_state.toAscii().constData());
	QString app_name_utf8=QString::fromUtf8(app_name.toAscii().constData());

	//Get the display names (if they exist)
	QString current_display_state=clist.getContextByName(current_state_utf8, current_state_utf8).display_name;
	QString new_display_state=clist.getContextByName(current_state_utf8, new_state_utf8).display_name;

	QString question;
	if(scope==SCOPE_GLOBAL)
		question=QString::fromUtf8("Your computer will switch from the context '%1' to the context '%2' because of the application '%3'.\n\nDo you agree ?").arg(current_display_state).arg(new_display_state).arg(app_name_utf8);
	else
		question=QString::fromUtf8("The application %1  will switch from the context '%2' to the context '%3'."
							  "\n\nDo you agree ?").arg(app_name_utf8).arg(current_display_state).arg(new_display_state);
	
	np.prompt(question);
	np.exec();
	
	if(np.result()==QDialog::Accepted)
		return CONTEXT_ACCEPT;
	else if(np.result()==QDialog::Rejected)
		return CONTEXT_REFUSE;
	else
		return CONTEXT_REFUSE;
}

QString ContextListener::context_changed(const QString& scope, const QString& app_name, const QString& context_name)
{
	//Change to UTF8
	QString context_name_utf8=QString::fromUtf8(context_name.toAscii().constData());
	QString app_name_utf8=QString::fromUtf8(app_name.toAscii().constData());

	//Get the display name (if it exists)
	QString display_context_name=clist.getContextByName(context_name_utf8).display_name;

	//Create the notification message
	QString msg;
	if(scope==SCOPE_GLOBAL)
		msg = QString::fromUtf8("Your computer has changed its context to '%1' because of the application %2.").arg(display_context_name).arg(app_name_utf8);
	else
		msg = QString::fromUtf8("The application %1 has switched to the context '%2'.").arg(app_name_utf8).arg(display_context_name);


	//Update everyone else
	emit systemContextUpdated(context_name, display_context_name);
	systemContextChanged(context_name, display_context_name);

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

void ContextListener::changeUserspaceContext()
{
	ccd.hide();
}

void ContextListener::systemContextChanged(QString /*context_name*/, QString display_context_name)
{
	trayicon.setToolTip(tr("PIGA System Context Changer\n\nCurrent Context : %1").arg(display_context_name));
}
