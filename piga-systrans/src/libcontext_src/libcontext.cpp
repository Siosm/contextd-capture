#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "libcontext.h"
#include "contextd_communicator.h"

Contextd_Communicator* ccom()
{
	static Contextd_Communicator ccom;
	return &ccom;
}

extern "C" context_bool context_is_registered()
{
	return ccom()->isRegistered()?CONTEXT_TRUE:CONTEXT_FALSE;
}

extern "C" const char* context_getLastError()
{
	return qPrintable(ccom()->getLastError());
}

extern "C" context_bool context_register_application(const char* app_name)
{
	return ccom()->registerApplication(app_name)?CONTEXT_TRUE:CONTEXT_FALSE;;
}

extern "C" context_result context_changed(const char* param_name, ...)
{
	const char* name;
	const char* value;

	QString xmlContext="<xmlContext>";

	va_list vl;
	va_start(vl, param_name);

	name=param_name;
	value=va_arg(vl,char*);

	do {
		if(name==NULL)
			name="";
		if(value==NULL)
			value="";

		QString sname=name;
		QString svalue=ccom()->toXMLEntities(value);

		if(sname!="" && svalue!="")
			xmlContext+=QString("<parameter name=\"%1\" value=\"%2\"/>").arg(sname).arg(svalue);

		name=va_arg(vl,char*);
		value=va_arg(vl,char*);
	} while (name!=NULL);
	va_end(vl);

	xmlContext+="</xmlContext>";

	return ccom()->contextChanged(xmlContext);
}

extern "C" unsigned int context_changed_async(contextChangedCallbackSuccess success_cb,
									 contextChangedCallbackError error_cb,
									 const char* param_name, ...)
{
	const char* name;
	const char* value;

	QString xmlContext="<xmlContext>";

	va_list vl;
	va_start(vl, param_name);

	name=param_name;
	value=va_arg(vl,char*);

	do {
		if(name==NULL)
			name="";
		if(value==NULL)
			value="";

		QString sname=name;
		QString svalue=ccom()->toXMLEntities(value);

		if(sname!="" && svalue!="")
			xmlContext+=QString("<parameter name=\"%1\" value=\"%2\"/>").arg(sname).arg(svalue);

		name=va_arg(vl,char*);
		value=va_arg(vl,char*);
	} while (name!=NULL);
	va_end(vl);

	xmlContext+="</xmlContext>";

	return ccom()->contextChangedAsync(success_cb, error_cb, xmlContext);
}

extern "C" context_bool context_required_context(char* required_context, size_t maxlen, const char* param_name, ...)
{
	const char* name;
	const char* value;

	QString xmlContext="<xmlContext>";

	va_list vl;
	va_start(vl, param_name);

	name=param_name;
	value=va_arg(vl,char*);

	do {
		if(name==NULL)
			name="";
		if(value==NULL)
			value="";

		QString sname=name;
		QString svalue=ccom()->toXMLEntities(value);

		if(sname!="" && svalue!="")
			xmlContext+=QString("<parameter name=\"%1\" value=\"%2\"/>").arg(sname).arg(svalue);

		name=va_arg(vl,char*);
		value=va_arg(vl,char*);
	} while (name!=NULL);
	va_end(vl);

	xmlContext+="</xmlContext>";

	QString req_context;
	bool res=ccom()->requiredContext(xmlContext, req_context);
	strncpy(required_context, qPrintable(req_context), maxlen-1);
	required_context[maxlen-1]='\0';

	return res?CONTEXT_TRUE:CONTEXT_FALSE;
}

void context_set_context_changed_callback(contextChangedCallback ccc, void* user_data)
{
	ccom()->setContextChangedCallback(ccc, user_data);
}

extern "C" context_bool context_current_local_context(char* state_buf, unsigned int max_size)
{
	QString res=ccom()->currentLocalContext();
	strncpy(state_buf, qPrintable(res), max_size-1);

	if(res!="<none>")
		return CONTEXT_TRUE;
	else
		return CONTEXT_FALSE;
}

extern "C" context_bool context_current_global_context(char* state_buf, unsigned int max_size)
{
	QString res=ccom()->currentGlobalContext();
	strncpy(state_buf, qPrintable(res), max_size-1);

	if(res!="<none>")
		return CONTEXT_TRUE;
	else
		return CONTEXT_FALSE;
}
