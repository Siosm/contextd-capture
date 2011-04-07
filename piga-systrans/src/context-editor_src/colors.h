#ifndef COLORS_H
#define COLORS_H

#include <QColor>
#include <QPair>
#include <QString>
#include <QVariantAnimation>
#include "warning.h"

/// Default colors used by domains and arrows
namespace Colors
{
	const QString DomainNormalBackground="whitesmoke";
	const QString DomainNormalForeground="slategrey";

	const QString DomainSelectedBackground="lightcyan";
	const QString DomainSelectedForeground="steelblue";

	const QString ArrowNormalPen="darkgrey";
	const QString ArrowNormalHead="darkgrey";
	const QString ArrowNormalProgItemBackground="lightgrey";
	const QString ArrowNormalProgItemForeground="grey";

	const QString ArrowSelectedPen="steelblue";
	const QString ArrowSelectedHead="mediumaquamarine";
	const QString ArrowSelectedProgItemBackground="mediumaquamarine";
	const QString ArrowSelectedProgItemForeground="midnightblue";


};

/// A QVariantAnimation for picking colors on the confidentiality/reliability color scale
class DomainGradientColorAnimation : public QVariantAnimation
{
protected:
	explicit DomainGradientColorAnimation();

public:
	void updateCurrentValue(QVariant const& value);

	/// Get the instance of this color animation
	static DomainGradientColorAnimation &instance();

	/// Get the color for a given ratio included between 0 (best) and 1 (worse)
	QPair<QColor, QColor> getColorsFromRatio(const qreal ratio);
};

/// A QVariantAnimation for picking colors depending on a domain warning type
class DomainWarningColorAnimation : public QVariantAnimation
{
protected:
	explicit DomainWarningColorAnimation();

public:
	void updateCurrentValue(QVariant const& value);

	/// Get the instance of this color animation
	static DomainWarningColorAnimation &instance();

	/// Get the color that matches the most critical warning type set in type
	QPair<QColor, QColor> getColorsFromDomainWarningType(const int type);
};

/// A QVariantAnimation for picking colors depending on a rule warning type
class ArrowColorAnimation : public QVariantAnimation
{
protected:
	explicit ArrowColorAnimation();

public:
	void updateCurrentValue(QVariant const& value);

	/// Get the instance of this color animation
	static ArrowColorAnimation &instance();

	/// Get the color that matches the most critical warning type set in type
	QColor getColorFromRuleWarningType(const int type);
};

#endif // COLORS_H
