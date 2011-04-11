#ifndef NOTIFYPROMPT_H
#define NOTIFYPROMPT_H

#include "ui_notifyprompt.h"

#include "domainlist.h"

class QTimer;

class NotifyPrompt : public QDialog, private Ui::NotifyPrompt {
	Q_OBJECT
	Q_DISABLE_COPY(NotifyPrompt)

	QTimer* timer;

public:
	explicit NotifyPrompt(QWidget *parent = 0);

	void prompt(const DomainList::Domain_p& domain_from, const DomainList::Domain_p& domain_to, const QString& appName);

protected:
	virtual void changeEvent(QEvent *e);
};

#endif // NOTIFYPROMPT_H
