#ifndef UPDATEDOMAIN_H
#define UPDATEDOMAIN_H

#include <QString>
#include <QObject>

#include "../libcontext_src/libcontext.h"

class UpdateDomain : public QObject
{
Q_OBJECT
	QString app_name;
	int event_new_domain;

	static void callbackSuccess(unsigned int id, context_result result);
	static void callbackError(unsigned int id, const char* error, const char* message);
public:
	UpdateDomain(QString app_name);

public slots:
	void manualDomainChange(QString domain);

private:
	static void domain_changed_callback(const char* previousDomain, const char* nextDomain, void* user_data);

signals:
	void domainChanged(QString previousDomain, QString newDomain);
};

#endif // UPDATEDOMAIN_H
