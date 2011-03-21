#include <QColor>
#include "colors.h"

DomainGradientColorAnimation::DomainGradientColorAnimation()
{
	//Setup the colors of the animation
	setKeyValueAt(0, QColor("chartreuse"));
	setKeyValueAt(0.25, QColor("greenyellow"));
	setKeyValueAt(0.5, QColor("gold"));
	setKeyValueAt(0.75, QColor("orangered"));
	setKeyValueAt(1, QColor(200, 34, 0));

	//Setup the duration
	setDuration(100);
}

void DomainGradientColorAnimation::updateCurrentValue(const QVariant &/*value*/)
{}

DomainGradientColorAnimation &DomainGradientColorAnimation::instance()
{
	static DomainGradientColorAnimation anim;

	return anim;
}

QPair<QColor, QColor> DomainGradientColorAnimation::getColorsFromRatio(const qreal ratio)
{
	QPair<QColor, QColor> colors;

	//Use the animation's duration to find a current value
	setCurrentTime(ratio*100);

	//First color comes from the value
	colors.first=currentValue().value<QColor>();

	//Second color is slightly lighter
	colors.second.setRed(255 - (255-colors.first.red())/3);
	colors.second.setGreen(255 - (255-colors.first.green())/3);
	colors.second.setBlue(255 - (255-colors.first.blue())/3);

	return colors;
}




DomainWarningColorAnimation::DomainWarningColorAnimation()
{
	//Setup the colors of the animation
	setKeyValueAt(0, QColor("darkgrey"));
	setKeyValueAt(1, QColor("gold"));

	//Setup the duration
	setDuration(100);
}

void DomainWarningColorAnimation::updateCurrentValue(const QVariant &/*value*/)
{}

DomainWarningColorAnimation &DomainWarningColorAnimation::instance()
{
	static DomainWarningColorAnimation anim;

	return anim;
}

QPair<QColor, QColor> DomainWarningColorAnimation::getColorsFromDomainWarningType(const int type)
{
	QPair<QColor, QColor> colors;

	//Calculate time depending on the priority of each warning
	int time=0;

	if(type & Unreachable)
		time=50;

	setCurrentTime(time);

	//First color comes from the value
	colors.first=currentValue().value<QColor>();

	//Second color is slightly lighter
	colors.second.setRed(255 - (255-colors.first.red())/3);
	colors.second.setGreen(255 - (255-colors.first.green())/3);
	colors.second.setBlue(255 - (255-colors.first.blue())/3);

	return colors;
}



ArrowColorAnimation::ArrowColorAnimation()
{
	//Setup the colors of the animation
	setKeyValueAt(0, QColor("darkgrey"));
	setKeyValueAt(0.02, QColor("gold"));//0,01 is Unreachable
	setKeyValueAt(0.1, QColor("goldenrod"));
	setKeyValueAt(0.2, QColor("gold"));
	setKeyValueAt(0.4, QColor("orange"));
	setKeyValueAt(0.6, QColor("darkorange"));
	setKeyValueAt(0.8, QColor("orangered"));
	setKeyValueAt(0.9, QColor("red"));
	setKeyValueAt(1, QColor("black"));

	//Setup the duration
	setDuration(100);
}

void ArrowColorAnimation::updateCurrentValue(const QVariant &/*value*/)
{}

ArrowColorAnimation &ArrowColorAnimation::instance()
{
	static ArrowColorAnimation anim;

	return anim;
}

QColor ArrowColorAnimation::getColorFromRuleWarningType(const int type)
{
	int time=0;

	if(type & Unreachable)
		time=1;

	if(type & UnknownProgram)
		time=10;

	if(type & MatchedSomewhereElse)
		time=20;

	if(type & UndefinedMRuleProgramData)
		time=36;

	if(type & (ConfidentialityLoss | ReliabilityLoss))
		time=60;

	if(type & InvalidMRuleProgramData)
		time=85;

	if(type & Bypassed)
		time=93;

	setCurrentTime(time);
	return currentValue().value<QColor>();
}
