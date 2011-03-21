#include <QStyleOptionGraphicsItem>
#include <QTextOption>
#include <QTextDocument>
#include <QVariantAnimation>
#include "colors.h"
#include "editorconfig.h"
#include "graphicsdomain.h"
#include "configurationstack.h"

const int GraphicsDomain::EllipseRadius=25;
const int GraphicsDomain::EllipseDiameter=GraphicsDomain::EllipseRadius*2;
const int GraphicsDomain::IconSize=32;

GraphicsDomain::GraphicsDomain(Domain dom, int x, int y, QGraphicsScene *scene, QObject *parent) :
		QObject(parent),
		QGraphicsEllipseItem(x, y, EllipseDiameter, EllipseDiameter, 0, scene),
		_domain(dom),
		_x(x), _y(y),
		_label_bg(new QGraphicsPathItem(this, scene)),
		_label(new QGraphicsTextItem(this, scene)),
		_highlighted(false),
		_hidden(false)
{
	//Set item flags
	setZValue(2);
	if(EditorConfig::hideMode()!=EditorConfig::LowerOpacityAndForbidSelection || !ConfigurationStack::instance().isDomainHidden(dom.name()))
	{
		setFlag(ItemIsSelectable, true);
		setAcceptHoverEvents(true);
	}

	//Add a second outline if the domain is default
	drawDefaultDomainAttributes(ConfigurationStack::instance().current().defaultDomain()==_domain);

	//Set tooltip
	const Configuration &inst=ConfigurationStack::instance().current();
	QString tooltip="<big><b>"+_domain.displayName()+"</b></big>";
	QMap<QString, QStringList> tooltips;

	QVector<Loop> loops=inst.loopsForDomain(dom);
	for(int i=0; i<loops.size(); ++i)
		tooltips[loops.at(i).appName()] << " - "+loops.at(i).displayName();

	QVector<Rule> rules=inst.rulesForDomain(dom);
	for(int i=0; i<rules.size(); ++i)
		if(rules.at(i).tRulesContain(dom) && rules.at(i).transitTo()==dom)
			tooltips[rules.at(i).appName()] << " - "+rules.at(i).displayName();

	QList<QString> appNames=tooltips.keys();
	qSort(appNames);

	for(int i=0; i<appNames.size(); ++i)
	{
		const Program &p=inst.getProgramByName(appNames.at(i));
		if(p.isValid())
			tooltip += "<br /><b>"+p.displayName()+"</b><br />";
		else
			tooltip += "<br /><b>Unknown ("+appNames.at(i)+")</b><br />";

		tooltip += tooltips[appNames.at(i)].join("<br />");
	}
	setToolTip(tooltip);

	//Setup the pen and brush
	setPenAndBrush();

	//Create and position the icon
	QString icon=QFile::exists(_domain.iconPath()) ? _domain.iconPath() : ":/icons/default-domain";
	if(!EditorConfig::fastDraw() && (icon.endsWith(".svg")||icon==":/icons/default-domain"))
	{
		_pixSvg=new GraphicsSvgItem(icon, this);
		_pixSvg->setPos(_x+(EllipseDiameter-IconSize)/2, _y+(EllipseDiameter-IconSize)/2);
		_pixPng=0;
	}
	else
	{
		QPixmap pixmap=QPixmap(icon).scaledToHeight(IconSize, EditorConfig::fastDraw()? Qt::FastTransformation : Qt::SmoothTransformation);
		_pixPng=new QGraphicsPixmapItem(pixmap, this, scene);
		_pixPng->setPos(_x+(EllipseDiameter-IconSize)/2, _y+(EllipseDiameter-IconSize)/2);
		_pixSvg=0;
	}

	//Position the text item below the icon
	QString label=dom.displayName();
	if(EditorConfig::showConfRelInDomLabel())
	{
		if(EditorConfig::domainColorScheme()==EditorConfig::ConfidentialityBased)
			label+=QString(" (%1)").arg(_domain.confidentiality());
		else if(EditorConfig::domainColorScheme()==EditorConfig::ReliabilityBased)
			label+=QString(" (%1)").arg(_domain.reliability());
	}
	_label->setPlainText(label);

	QFontMetrics fm(_label->font());
	qreal textWidth=fm.width(_label->document()->toPlainText())+10;

	QPainterPath bgPath;
	bgPath.addRoundedRect(0, 0, textWidth, fm.height()+3, 5, 3);
	_label_bg->setPath(bgPath);
	_label_bg->setTransformOriginPoint(_label->boundingRect().center());
	_label_bg->setPos(_x+(EllipseDiameter-textWidth)/2, _y+EllipseDiameter+2+3);

	_label->setTextWidth(textWidth);
	_label->setTransformOriginPoint(_label->boundingRect().center());
	_label->setPos(_x+(EllipseDiameter-textWidth)/2, _y+EllipseDiameter+2);

	//Setup text background options
	_label_bg->setPen(QPen(Qt::NoPen));
	_label_bg->setBrush(QBrush("white"));
	_label_bg->setZValue(50);
	if(!EditorConfig::fastDraw())
		_label_bg->setOpacity(0.5);

	//Setup text label options
	_label->document()->setDefaultTextOption(QTextOption(Qt::AlignHCenter));
	_label->setZValue(100);

	//Hide the domain if needed and set the handler for hidden domains signal
	setHiddenByUser(ConfigurationStack::instance().isDomainHidden(dom.name()));
	connect(&ConfigurationStack::instance(), SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onDomainHidden(QString,bool)));
}

void GraphicsDomain::setPos(QPointF point)
{
	setPos(point.x(), point.y());
}

void GraphicsDomain::setPos(qreal x, qreal y)
{
	_x=x;
	_y=y;
	QGraphicsEllipseItem::setPos(x,y);
}

QPoint GraphicsDomain::centerPos() const
{
	return QPoint(_x+EllipseRadius, _y+EllipseRadius);
}

QPointF GraphicsDomain::centerPosF() const
{
	return QPointF(_x+EllipseRadius, _y+EllipseRadius);
}

void GraphicsDomain::setCenterPos(QPointF point)
{
	setCenterPos(point.x(), point.y());
}

void GraphicsDomain::setCenterPos(qreal x, qreal y)
{
	setPos(x, y);
	moveBy(rect().width()/-2, rect().height()/-2);
}

Domain GraphicsDomain::domain() const
{
	return _domain;
}

void GraphicsDomain::setHighlighted(bool highlighted)
{
	if(highlighted!=_highlighted)
	{
		_highlighted=highlighted;
		setPenAndBrush();
	}
	else
		_highlighted=highlighted;
}

bool GraphicsDomain::isHighlighted() const
{
	return _highlighted;
}

void GraphicsDomain::setHiddenByUser(bool hidden)
{
	if(hidden && !_hidden)
	{
		setOpacity(0.4);
		if(EditorConfig::hideMode()==EditorConfig::LowerOpacityAndForbidSelection)
		{
			setFlag(ItemIsSelectable, false);
			setAcceptHoverEvents(false);
		}
	}
	else if (!hidden && _hidden)
	{
		setOpacity(1.0);
		if(EditorConfig::hideMode()==EditorConfig::LowerOpacityAndForbidSelection)
		{
			setFlag(ItemIsSelectable, true);
			setAcceptHoverEvents(true);
		}
	}

	_hidden=hidden;
	emit hiddenByUser(hidden);
}

bool GraphicsDomain::isHiddenByUser() const
{
	return _hidden;
}

void GraphicsDomain::setPenAndBrush()
{
	QColor brushColor;
	QColor penColor;

	if(!_highlighted)
	{
		//Confidentiality- or reliability-based display mode
		if(EditorConfig::domainColorScheme()==EditorConfig::ReliabilityBased ||
		   EditorConfig::domainColorScheme()==EditorConfig::ConfidentialityBased)
		{
			//Use a QVariantAnimation to find the color to use
			DomainGradientColorAnimation &anim=DomainGradientColorAnimation::instance();
			qreal ratio=0;
			if(EditorConfig::domainColorScheme()==EditorConfig::ConfidentialityBased)
				ratio=1 - (qreal)domain().confidentiality()	/ (Domain::ConfidentialityMax-Domain::ConfidentialityMin);
			else if(EditorConfig::domainColorScheme()==EditorConfig::ReliabilityBased)
				ratio=1 - (qreal)domain().reliability() / (Domain::ReliabilityMax-Domain::ReliabilityMin);

			QPair<QColor, QColor> colors=anim.getColorsFromRatio(ratio);
			penColor=colors.first;
			brushColor=colors.second;
		}
		//Non gradient-based display mode
		else if(EditorConfig::domainColorScheme()==EditorConfig::WarningColor)
		{
			ConfigurationStack &stack=ConfigurationStack::instance();
			int warningTypes=stack.warningTypesForDomain(_domain.name());

			if(warningTypes==InvalidType)
			{
				brushColor.setNamedColor(Colors::DomainNormalBackground);
				penColor.setNamedColor(Colors::DomainNormalForeground);
			}
			else
			{
				DomainWarningColorAnimation &anim=DomainWarningColorAnimation::instance();

				QPair<QColor, QColor> colors=anim.getColorsFromDomainWarningType(warningTypes);
				penColor=colors.first;
				brushColor=colors.second;
			}
		}
		//No particular scheme
		else
		{
			brushColor.setNamedColor(Colors::DomainNormalBackground);
			penColor.setNamedColor(Colors::DomainNormalForeground);
		}
	}
	else
	{
		brushColor.setNamedColor(Colors::DomainSelectedBackground);
		penColor.setNamedColor(Colors::DomainSelectedForeground);
	}

	//Set the pen and brush
	setBrush(QBrush(brushColor));
	setPen(QPen(penColor, 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));

	if(_extra_outline)
		_extra_outline->setPen(QPen(penColor, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
}

void GraphicsDomain::drawDefaultDomainAttributes(bool is_default)
{
	if(is_default)
		_extra_outline=new QGraphicsEllipseItem(rect().left()+3, rect().top()+3, EllipseDiameter-6, EllipseDiameter-6, this, scene());
	else
		_extra_outline=0;
}

void GraphicsDomain::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
	setCursor(QCursor(Qt::PointingHandCursor));
}

void GraphicsDomain::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
	setCursor(QCursor(Qt::OpenHandCursor));
}

void GraphicsDomain::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
	emit activated(_domain.name());
}

void GraphicsDomain::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QStyleOptionGraphicsItem *_option = const_cast<QStyleOptionGraphicsItem*>(option);
	_option->state &= ~QStyle::State_Selected;
	QGraphicsEllipseItem::paint(painter, _option, widget);
}

void GraphicsDomain::onDomainHidden(QString name, bool hidden)
{
	if(name==_domain.name() && hidden!=_hidden)
		setHiddenByUser(hidden);
}
