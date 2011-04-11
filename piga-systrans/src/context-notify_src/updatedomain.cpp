#include "updatedomain.h"
#include <QMessageBox>

void UpdateDomain::domain_changed_callback(const char* previousContext, const char* nextContext, void* user_data)
{
	((UpdateDomain*)user_data)->emit domainChanged(previousContext, nextContext);
}

UpdateDomain::UpdateDomain(QString /*app_name*/)
{
	context_set_domain_changed_callback(domain_changed_callback, this);
}

void UpdateDomain::manualDomainChange(QString context)
{
	//register the application at the context daemon
	if(context_is_registered()==CONTEXT_FALSE)
	{
		if(context_register_application("context-notify")==CONTEXT_FALSE)
		{
			QMessageBox::critical(NULL, QString("Context-notify: Failed to register the application."), QString("Context-notify: Failed to register the application."));
			exit(1);
		}
	}

	context_changed_async(callbackSuccess, callbackError, "domain",
					  qPrintable(context), NULL, NULL);
}

void UpdateDomain::callbackSuccess(unsigned int /*id*/, context_result /*result*/)
{

}

void UpdateDomain::callbackError(unsigned int /*id*/, const char* error, const char* message)
{
	QMessageBox::critical(NULL, tr("Communication Error : %1").arg(error),
					  message);
}
