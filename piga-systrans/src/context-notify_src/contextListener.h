#ifndef CONTEXTLISTENER_H
#define CONTEXTLISTENER_H

#include <QtCore/QObject>
#include <QDBusContext>

#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMap>

#include "updatecontext.h"
#include "contextlist.h"

#include "changecontextdialog.h"

class ContextListener: public QObject, protected QDBusContext
{
Q_OBJECT
public:
	ContextListener();

private:
	QSystemTrayIcon trayicon;
	QMenu cmenu;
	QAction *Aquit, *AchangeContext;
	QIcon icon;

	ChangeContextDialog ccd;

	UpdateContext upcontext;
	ContextList clist;
public slots:
	Q_SCRIPTABLE QString accept_changing_state(const QString& scope, const QString& current_state, const QString& new_state, const QString& app_name);
	Q_SCRIPTABLE QString context_changed(const QString& scope, const QString& app_name, const QString& context_name);

private slots:
	void openChangeContextDialog();
	void trayIconActivated(QSystemTrayIcon::ActivationReason);
	void changeUserspaceContext();

	void systemContextChanged(QString context_name, QString display_context_name);

signals:
	void systemContextUpdated(QString context_name, QString display_context_name);
};

#endif
