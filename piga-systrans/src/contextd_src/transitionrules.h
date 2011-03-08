#ifndef TRANSITIONRULES_H
#define TRANSITIONRULES_H

#include <QReadWriteLock>
#include "../xmlsp.h"
#include "transition.h"
#include "context.h"
#include "program.h"
#include <QMap>
#include <QVector>
#include <QString>
#include <QMutex>

class TransitionRules : /*public QObject,*/ private XMLSP::Parser
{
private:
	//We associate the program name to its related rules (the vector)
	//that finally leads to a map associating a parameter name to its regexp
	static QMap<QString, QVector<Transition> > rules;
	static QMap<QString, Context > contexts;
	static QMap<QString, Program > programs;

	static QReadWriteLock readwritelock;
	static QMutex init;

	//XML Parsing
	bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes);
	bool on_cdata(const std::string& cdata);
	bool on_tag_close(const std::string& tag_name);
	void on_error(int errnr, int line, int col, const std::string& message);

private:
	bool readConf();
public:
	static TransitionRules& instance();

	TransitionRules();

	//Contexts
	static bool updateRules();

	static Context getContextByName(QString name);

	const Transition updatedContext(QString appName, QString current_state, QMap<QString, QString> values);
	
	//Programs
	static bool programAllowed(QString name, QString full_path);
	static Program getProgramByName(QString name);
};

#endif // TRANSITIONRULES_H
