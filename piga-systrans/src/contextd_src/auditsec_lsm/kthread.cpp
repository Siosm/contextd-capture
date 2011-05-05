#include "kthread.h"
#include "stdarg.h"

#include <QtCore/QDebug>


KThread::KThread(KernelContext * kc) : KC(kc)
{
	_keep_going = true;
}


void KThread::run()
{
	while(_keep_going && (auditsec_question(KC->usai()) == 0)){
		qDebug() << "KernelContext: " << KC->usai()->execname << " (" << KC->usai()->pid << ")";
		if(KC->is_registered() == KERNEL_ERROR){
			KC->register_application(KC->usai()->execname);
		}

		switch (KC->usai()->type){
			case AUDITSEC_FILE:
				KC->domain_changed(xmlContext(
					"fullpath", KC->usai()->auditsec_struct.file.fullpath,
					//"filename", KC->usai()->auditsec_struct.file.name,
					NULL, NULL));

				#ifdef DEBUG
				read_execpath(KC->usai()->pid, exec_path);
				qDebug() << "file access: " << KC->usai()->auditsec_struct.file.fullpath
				<< "/" << KC->usai()->auditsec_struct.file.name << ", pid: " << KC->usai()->pid
				<< ", execname: " << exec_path
				/*<< ", mask: " << KC->usai()->auditsec_struct.file.mask*/;
				#endif /* DEBUG */
				break;

			case AUDITSEC_DIR:
				KC->domain_changed(xmlContext(
					"fullpath", KC->usai()->auditsec_struct.file.fullpath,
					NULL, NULL));

				#ifdef DEBUG
				read_execpath(KC->usai()->pid, exec_path);
				qDebug(); << "mkdir: " << KC->usai()->auditsec_struct.dir.fullpath
				<< ", pid: " << KC->usai()->pid << ", execname: " << exec_path << KC->usai()->execname
				/*<< ", mode: " << KC->usai()->auditsec_struct.dir.mode*/;
				#endif /* DEBUG */
				break;
			
			case AUDITSEC_SOCKET:
				qDebug() << "Case socket";
				auditsec_answer(true);
				break;

			default:
				qCritical("KernelContext: can't determine struct type !");
				auditsec_answer(false);
				break;
		}
	}
	qDebug("KernelContext: loop end.");
}


QString KThread::toXMLEntities(QString str)
{
	return str.replace("&","&amp;").replace("<", "$lt;").replace(">", "&gt;").replace("\"", "&quot;");
}


QString KThread::xmlContext(char * param_name, ...)
{
	const char* name;
	const char* value;

	QString xmlContext="<xmlContext>";

	va_list vl;
	va_start(vl, param_name);

	name = param_name;
	value = va_arg(vl, char*);

	do {
		if(name == NULL)
			name = "";
		if(value == NULL)
			value = "";

		QString sname = name;
		QString svalue = toXMLEntities(value);

		if(sname!="" && svalue!="")
			xmlContext += QString("<parameter name=\"%1\" value=\"%2\"/>").arg(sname).arg(svalue);

		name = va_arg(vl, char*);
		value = va_arg(vl, char*);
	} while (name != NULL);
	va_end(vl);

	xmlContext+="</xmlContext>";

	return xmlContext;
}
