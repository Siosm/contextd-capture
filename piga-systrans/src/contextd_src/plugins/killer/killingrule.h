#ifndef KILLINGRULE_H
#define KILLINGRULE_H

#include <QMap>
#include <QString>
#include "xmlsp.h"

class KillingRule : public XMLSP::Parser
{
private:
	QString app_name;
	QString regexp_context;
	QString msg_kill;
	int sig_num;

	QMap<QString, QString> params;

public:
	KillingRule();
	KillingRule(QString app_name, QString regexp_context, int sig_num, QString msg_kill);

	void addParameter(QString param_name, QString param);

	bool isMatched(QString app_name, QString context, QMap<QString, QString> client_params);

	QString messageKill();

	bool operator==(const KillingRule& b) const;
	bool operator!=(const KillingRule& b) const;
};

#endif // KILLINGRULE_H
