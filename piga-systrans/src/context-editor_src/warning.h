#ifndef RULEWARNING_H
#define RULEWARNING_H

#include <QList>
#include <QMap>
#include <QString>
#include <QSharedPointer>
#include "domain.h"
#include "mrule.h"

/// Types of existing RuleWarnings
typedef enum
{
	/// Invalid type
	InvalidType=0,

	/// For a given source domain, the rule is bypassed by other ones
	Bypassed=1<<1,

	/// The domain is not reachable
	/// The rule has no reachable source domain
	Unreachable=1<<2,

	/// The rule's program is not known by the editor
	UnknownProgram=1<<3,

	/// The rule uses invalid actions for a known program
	InvalidMRuleProgramData=1<<4,

	/// The rule uses undefined actions for a known program
	UndefinedMRuleProgramData=1<<5,

	/// There are cases when another rule may match the same data
	MatchedSomewhereElse=1<<6,

	/// Some of the source domains are more confidential than the rule's target
	ConfidentialityLoss=1<<7,

	/// Some of the source domains are more reliable than the rule's target
	ReliabilityLoss=1<<8
} WarningType;

/// The most abstract kind of warnings
class Warning
{
public:
	/*!
	 * \brief The text that explains the nature of this warning
	 * \param item_pov Whether the text is going to be displayed next to the item's information
	 */
	virtual QString text(bool item_pov=false) const;

	/// The path to an icon that depends on the gravity of the warning
	virtual QString icon() const;

	WarningType warningType() const;

protected:
	/*!
	 * \brief Construct an abstract Warning
	 */
	Warning();

	/// The type of this warning
	WarningType _warning_type;
};

typedef QSharedPointer<Warning> WarningPtr;

/// A class for representing abstract warnings related to a domain
class DomainWarning : public Warning
{
protected:
	/*!
	 * \brief Construct an abstract Rule Warning
	 * \param name The name of the domain concerned by the warning
	 */
	DomainWarning(QString name);

	/// Retrieve the domain involved in the warning
	QString domain() const;

	/// The name of the domain concerned by the warning
	QString _domain;
};

typedef QSharedPointer<DomainWarning> DomainWarningPtr;

struct UnreachableDomainWarning : public DomainWarning
{
	UnreachableDomainWarning(QString name);
	virtual QString text(bool dom_pov=false) const;
	virtual QString icon() const;
};

/// A class for representing abstract warnings related to a rule
class RuleWarning : public Warning
{
protected:
	/*!
	 * \brief Construct an abstract Rule Warning
	 * \param id The ID of the rule concerned by the warning
	 */
	RuleWarning(int id);

	/// Retrieve the rule involved in the warning
	int rule() const;

	/// The ID of the rule concerned by the warning
	int _rule;
};
typedef QSharedPointer<RuleWarning> RuleWarningPtr;

class BypassRuleWarning : public RuleWarning
{
public:
	/*!
	 * \brief Construct a Rule Warning related to rule bypassing
	 * \param id The ID of the bypassed rule
	 * \param source The domain in which the bypassing occurs
	 * \param bypassed_mrules A list of bypassing rules for each bypassed mrule
	 * \param total Whether the whole rule is being bypassed
	 */
	BypassRuleWarning(int id, Domain source, QMap<QString, QList<int> > bypassed_mrules);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;
	virtual Domain source() const;

private:
	Domain _source;
	QMap<QString, QList<int> > _bypassed_mrules;
	bool _total;
};

struct UnreachableRuleWarning : public RuleWarning
{
	UnreachableRuleWarning(int id);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;
};

struct UnknownProgramRuleWarning : public RuleWarning
{
	UnknownProgramRuleWarning(int id);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;
};

struct InvalidMRuleRuleWarning : public RuleWarning
{
	InvalidMRuleRuleWarning(int id);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;
};

struct UndefinedMRuleRuleWarning : public RuleWarning
{
	UndefinedMRuleRuleWarning(int id);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;
};

class MatchedSomewhereElseRuleWarning : public RuleWarning
{
public:
	/*!
	 * \brief Construct a Rule Warning related to duplicate mrules
	 * \param id The ID of the first rule
	 * \param other The ID of the second rule
	 * \param this_mrule The duplicate mrule in the first rule
	 * \param other_mrule The duplicate mrule in the second rule
	 */
	MatchedSomewhereElseRuleWarning(int id, int other, MRule this_mrule, MRule other_mrule);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;

private:
	int _other;
	MRule _this_mrule;
	MRule _other_mrule;
};

struct ConfidentialityLossRuleWarning : public RuleWarning
{
	ConfidentialityLossRuleWarning(int id);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;
};

struct ReliabilityLossRuleWarning : public RuleWarning
{
	ReliabilityLossRuleWarning(int id);
	virtual QString text(bool rule_pov=false) const;
	virtual QString icon() const;
};

#endif // RULEWARNING_H
