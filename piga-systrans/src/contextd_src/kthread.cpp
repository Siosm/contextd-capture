#include "kthread.h"
#include "stdarg.h"


KThread::KThread(KernelContext * kc) : KC(kc)
{
	connect(this, SIGNAL(register_application(const QString, uint)), kc, SLOT(register_application(const QString, uint)));
	connect(this, SIGNAL(domain_changed(const QString &xml_context)), kc, SLOT(domain_changed(const QString &xml_context)));
	connect(this, SIGNAL(required_domain(const QString &xml_context)), kc, SLOT(required_domain(const QString &xml_context)));
	connect(this, SIGNAL(current_domain()), kc, SLOT(current_domain()));
	connect(this, SIGNAL(is_registered()), kc, SLOT(is_registered()));
	connect(this, SIGNAL(register_for_domain_changes_updates()), kc, SLOT(register_for_domain_changes_updates()));
}


void KThread::run()
{
	while(KC->auditsec_question(KC->usai()) == 0){
		if(KC->is_registered_k(KC->usai()->pid) == false){
			emit(register_application(KC->usai()->execname, 0));
		}
		switch (KC->usai()->type){
			case AUDITSEC_FILE:
				emit(domain_changed(xmlContext(
					//"pid", usai->pid,
					"fullpath", KC->usai()->auditsec_struct.file.fullpath,
					//"filename", usai->auditsec_struct.file.name,
					 NULL, NULL)));
				#ifdef DEBUG
				read_execpath(usai->pid, exec_path);
				std::cout << "AuditSec, file access: " << usai->auditsec_struct.file.fullpath
				<< "/" << usai->auditsec_struct.file.name << ", pid: " << usai->pid << ", execname: "
				<< exec_path /*<< ", mask: " << usai->auditsec_struct.file.mask*/ << std::endl;
				#endif /* DEBUG */
				break;

			case AUDITSEC_DIR:
				emit(domain_changed(xmlContext(
					//"pid", usai->pid,
					"fullpath", KC->usai()->auditsec_struct.file.fullpath,
					NULL, NULL)));
				#ifdef DEBUG
				read_execpath(usai->pid, exec_path);
				std::cout << "AuditSec, mkdir: " << usai->auditsec_struct.dir.fullpath
				<< ", pid: " << usai->pid << ", execname: " << exec_path << usai->execname /*<< ", mode: "
				<< usai->auditsec_struct.dir.mode*/ << std::endl;
				#endif /* DEBUG */
				break;

			default:
				qCritical("AuditSec, can't determine struct type !");
				KC->auditsec_answer(false);
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
