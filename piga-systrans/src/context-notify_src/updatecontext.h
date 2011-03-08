#ifndef UPDATECONTEXT_H
#define UPDATECONTEXT_H

#include <QString>
#include <QObject>

#include "../libcontext_src/libcontext.h"

class UpdateContext : public QObject
{
Q_OBJECT
	QString app_name;
	int event_new_context;

	static void callbackSuccess(unsigned int id, context_result result);
	static void callbackError(unsigned int id, const char* error, const char* message);
public:
	UpdateContext(QString app_name);

public slots:
	void manualContextChange(QString context);

private:
	static void context_changed_callback(const char* previousContext, const char* nextContext, void* user_data);

signals:
	void globalContextChanged(QString previousContext, QString newContext);
};

#endif // UPDATECONTEXT_H
