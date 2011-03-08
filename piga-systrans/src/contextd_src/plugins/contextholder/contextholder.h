#ifndef CONTEXTHOLDER_H
#define CONTEXTHOLDER_H

#include <QObject>
#include "../abstractcontextdplugin.h"
#include "context.h"

#include <QReadWriteLock>

class ContextHolder : public QObject, public AbstractContextdPlugin
{
Q_OBJECT
private:
	//Context
	static Context c_global_state;
	static Context default_global_state;

	//To ensure thread-safe and reentrant code
	static QReadWriteLock readwritelock;

public:
	static ContextHolder& instance();
    explicit ContextHolder(QObject *parent = 0);

	void eventHandler(const ContextdPluginEvent* event);

	static Context currentGlobalState();
	static Context setCurrentGlobalState(Context context);

	static Context setDefaultGlobalContext(Context default_context);
	static Context defaultGlobalContext();

	static Context resetToGlobalDefaultContext();

signals:
	void globalContextChanged(Context previousGlobalContext, Context globalContext);

public slots:

};

#endif // CONTEXTHOLDER_H
