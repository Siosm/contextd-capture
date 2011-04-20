#include "killingrule.h"

#include <QtCore/QRegExp>
#include <QtCore/QList>

KillingRule::KillingRule()
{
}

KillingRule::KillingRule(QString app_name, QString regexp_context, int sig_num, QString msg_kill) :
					app_name(app_name), regexp_context(regexp_context), msg_kill(msg_kill), sig_num(sig_num)
{

}

void KillingRule::addParameter(QString param_name, QString param)
{
	params[param_name]=param;
}

bool KillingRule::isMatched(QString app_name, QString context, QMap<QString, QString> client_params)
{
	if(this->app_name!=app_name)
		return false;

	if(!QRegExp(regexp_context).exactMatch(context))
		return false;

	//Now, try to match _every_ param of the KillingRule
	QList<QString> keys=params.keys();
	for(int i=0;i<keys.size();i++)
	{
		if(client_params.contains((keys[i])))
		{
			if(!QRegExp(params[keys[i]]).exactMatch(client_params[keys[i]]))
				return false;
		}
		else
			return false;
	}

	return true;
}

int KillingRule::signal()
{
	return sig_num;
}

QString KillingRule::messageKill()
{
	return msg_kill;
}

bool KillingRule::operator==(const KillingRule& b) const
{
	return app_name==b.app_name && regexp_context==b.regexp_context && msg_kill==b.msg_kill && sig_num==b.sig_num && params==b.params;
}

bool KillingRule::operator!=(const KillingRule& b) const
{
	return !operator==(b);
}
