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
		qDebug() << "Boucle: " << KC->usai()->execname;
		if(KC->is_registered() == KERNEL_SUCCESS)
			KC->register_application(KC->usai()->execname, 0);

		switch (KC->usai()->type){
			case AUDITSEC_FILE:
				KC->domain_changed(xmlContext(
					"fullpath", KC->usai()->auditsec_struct.file.fullpath,
					//"filename", usai->auditsec_struct.file.name,
					NULL, NULL));

				#ifdef DEBUG
				read_execpath(usai->pid, exec_path);
				qDebug() << "file access: " << usai->auditsec_struct.file.fullpath
				<< "/" << usai->auditsec_struct.file.name << ", pid: " << usai->pid
				<< ", execname: " << exec_path
				/*<< ", mask: " << usai->auditsec_struct.file.mask*/;
				#endif /* DEBUG */
				break;

			case AUDITSEC_DIR:
				KC->domain_changed(xmlContext(
					"fullpath", KC->usai()->auditsec_struct.file.fullpath,
					NULL, NULL));

				#ifdef DEBUG
				read_execpath(usai->pid, exec_path);
				qDebug(); << "mkdir: " << usai->auditsec_struct.dir.fullpath
				<< ", pid: " << usai->pid << ", execname: " << exec_path << usai->execname
				/*<< ", mode: " << usai->auditsec_struct.dir.mode*/;
				#endif /* DEBUG */
				break;

			default:
				qCritical("AuditSec, can't determine struct type !");
				auditsec_answer(false);
				break;
		}
	}
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