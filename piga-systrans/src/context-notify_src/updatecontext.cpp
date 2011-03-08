#include "updatecontext.h"
#include <QMessageBox>

void UpdateContext::context_changed_callback(const char* previousContext, const char* nextContext, void* user_data)
{
	((UpdateContext*)user_data)->emit globalContextChanged(previousContext, nextContext);
}

UpdateContext::UpdateContext(QString app_name)
{
	//register the application at the context daemon
	context_register_application(app_name.toAscii().data());

	context_set_context_changed_callback(context_changed_callback, this);
}

void UpdateContext::manualContextChange(QString context)
{
	context_changed_async(callbackSuccess, callbackError, "context",
					  qPrintable(context), NULL, NULL);
}

void UpdateContext::callbackSuccess(unsigned int /*id*/, context_result /*result*/)
{
	//qDebug("Callback : Success !!!!");
}

void UpdateContext::callbackError(unsigned int /*id*/, const char* error, const char* message)
{
	QMessageBox::critical(NULL, tr("Communication Error : %1").arg(error),
					  message);
}
