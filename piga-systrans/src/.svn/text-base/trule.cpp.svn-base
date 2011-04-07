#include "trule.h"
#include "configuration.h"
#include "abstractrule.h"

#include <QRegExp>

TRule::TRule() :
		_from_domain(QString()),
		_notify(true),
		_has_notify(false),
		_prompt(true),
		_has_prompt(false)
{
}

TRule::TRule(const QString from_domain) :
		_from_domain(from_domain),
		_notify(true),
		_has_notify(false),
		_prompt(true),
		_has_prompt(false)
{
}

QString TRule::fromDomain() const
{
	return _from_domain;
}

void TRule::setFromDomain(QString from_domain)
{
	_from_domain=from_domain;
}

bool TRule::notify() const
{
	return _notify;
}

bool TRule::hasNotify() const
{
	return _has_notify;
}

void TRule::setNotify(bool value)
{
	_notify=value;
	_has_notify=true;
}

bool TRule::prompt() const
{
	return _prompt;
}

bool TRule::hasPrompt() const
{
	return _has_prompt;
}

void TRule::setPrompt(bool value)
{
	_prompt=value;
	_has_prompt=true;
}

bool TRule::isValid() const
{
	return _from_domain != QString();
}

bool TRule::matches(QString domain) const
{
	QRegExp regexp(fromDomain());

	return regexp.exactMatch(domain);
}
