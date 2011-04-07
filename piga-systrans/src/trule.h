#ifndef TRULE_H
#define TRULE_H

/**
 * \file trule.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include <QString>

class AbstractRule;

class TRule
{
	QString _from_domain;
	bool _notify;
	bool _has_notify;
	bool _prompt;
	bool _has_prompt;

public:
	TRule();
	TRule(const QString from_domain);

	QString fromDomain() const;
	void setFromDomain(QString from_domain);

	bool notify() const;
	bool hasNotify() const;
	void setNotify(bool value);

	bool prompt() const;
	bool hasPrompt() const;
	void setPrompt(bool value);

	bool isValid() const;

	bool matches(QString domain) const;
};

#endif // TRULE_H
