#include <QStyleOptionGraphicsItem>
#include <QGraphicsBlurEffect>
#include <QPainterPathStroker>
#include "editorconfig.h"
#include "graphicsarrowprogram.h"
#include "graphicsarrow.h"
#include "colors.h"
#include "configurationstack.h"

const qreal GraphicsArrowProgram::IconSize=24.0f;

GraphicsArrowProgram::GraphicsArrowProgram(QString app_name, GraphicsArrow *parent, QGraphicsScene *scene, bool show_label) :
	QGraphicsPixmapItem(parent, scene),
	_parent(parent),
	_app_name(app_name),
	_border(0),
	_hidden(false),
	_warning_type(InvalidType)
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	//Set icon and tooltip data from the program
	Program program=stack.current().getProgramByName(_app_name, true);

	QPixmap icon(program.icon());
	if(icon.isNull())
		icon.load(":/icons/default-program");

	setPixmap(icon.scaledToHeight(IconSize, EditorConfig::fastDraw()? Qt::FastTransformation : Qt::SmoothTransformation));
	setToolTip("<big><b>"+program.displayName()+"</b></big>");

	//Set various flags
	if(EditorConfig::hideMode()!=EditorConfig::LowerOpacityAndForbidSelection)
	{
		setFlag(ItemIsSelectable, true);
		setAcceptHoverEvents(true);
	}
	setFlag(ItemIgnoresParentOpacity, true);
	setZValue(1);

	//Connect signals to their handlers for hiding the program
	connect(&ConfigurationStack::instance(), SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged(QString,bool)));
	connect(&ConfigurationStack::instance(), SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged(int,bool)));
	connect(parent, SIGNAL(hiddenByUser(bool)), this, SLOT(onParentHiddenByUser()));

	//Draw the label if it was required
	if(show_label)
	{
		QGraphicsTextItem *label=new QGraphicsTextItem(program.displayName(), this, scene);

		//Position the text item below the icon
		QFontMetrics fm(label->font());
		qreal textWidth=fm.width(program.displayName())+10;
		label->setTextWidth(textWidth);
		label->setTransformOriginPoint(label->boundingRect().center());
		label->setPos(x()+(IconSize-textWidth)/2, y()+IconSize+2);
	}

	//Hide the program if needed
	setHiddenByUser(ConfigurationStack::instance().isProgramHidden(_app_name));
}

void GraphicsArrowProgram::addRule(AbstractRulePtr abstract)
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	//Append the rule to our list
	_rules << abstract;

	//Reset the tooltip
	Program program=stack.current().getProgramByName(_app_name, true);
	QString tooltip="<big><b>"+program.displayName()+"</b></big>";

	QVectorIterator<AbstractRulePtr> i(_rules);
	while(i.hasNext())
	{
		AbstractRulePtr rule=i.next();
		QVectorIterator<MRule> j(rule->mRules());

		tooltip+="<br />"+rule->displayName();

		while(j.hasNext())
			tooltip+="<br /> &nbsp; - "+j.next().displayName();
	}

	setToolTip(tooltip);

	//If the rule has warnings, calculate the new warning type of the arrow program
	if(stack.ruleHasWarnings(abstract->id()) && EditorConfig::outlineArrowPrograms() && _parent)
	{
		_warning_type|=stack.warningTypesForRuleAndSourceDomain(abstract->id(), _parent->source()->domain().name());

		//If there is a warning to display, change the border's color
		if(_warning_type!=InvalidType)
		{
			//Create the border if it didn't exist
			if(_border==0)
			{
				QPainterPathStroker stroker;
				stroker.setWidth(3);

				_border=new QGraphicsPathItem(stroker.createStroke(shape()), this, scene());
				_border->setPen(QPen(QBrush(), 0, Qt::NoPen));
				_border->setFlag(ItemStacksBehindParent, true);
			}

			ArrowColorAnimation &anim=ArrowColorAnimation::instance();
			_border->setBrush(anim.getColorFromRuleWarningType(_parent->warningType()));
			_border->setGraphicsEffect(new QGraphicsBlurEffect());
		}
	}

	//Hide the rule if necessary
	if(_hidden ^ stack.isRuleHidden(abstract->id()))
		setHiddenByUser(allRulesHidden() || stack.isProgramHidden(_app_name));
}

void GraphicsArrowProgram::setHiddenByUser(bool hidden)
{
	setHiddenByUser(hidden, false);
}

void GraphicsArrowProgram::setHiddenByUser(bool hidden, bool quiet)
{
	if(hidden || _parent->isHiddenByUser())
	{
		setOpacity(0.4);
		if(EditorConfig::hideMode()==EditorConfig::LowerOpacityAndForbidSelection)
		{
			setFlag(ItemIsSelectable, false);
			setAcceptHoverEvents(false);
		}
	}
	else
	{
		setOpacity(1.0);
		if(EditorConfig::hideMode()==EditorConfig::LowerOpacityAndForbidSelection)
		{
			setFlag(ItemIsSelectable, true);
			setAcceptHoverEvents(true);
		}
	}

	_hidden=hidden;
	if(!quiet)
		emit hiddenByUser(hidden);
}

bool GraphicsArrowProgram::allRulesHidden() const
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	for(int i=0; i<_rules.size(); ++i)
	{
		if(!stack.isRuleHidden(_rules[i]->id()))
			return false;
	}

	return true;
}

bool GraphicsArrowProgram::isHiddenByUser() const
{
	return _hidden;
}

GraphicsArrow *GraphicsArrowProgram::parentArrow() const
{
	return _parent;
}

QString GraphicsArrowProgram::appName() const
{
	return _app_name;
}

QVector<AbstractRulePtr> GraphicsArrowProgram::rules() const
{
	return _rules;
}

void GraphicsArrowProgram::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
	setCursor(QCursor(Qt::PointingHandCursor));
}

void GraphicsArrowProgram::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
	setCursor(QCursor(Qt::OpenHandCursor));
}

bool GraphicsArrowProgram::sortByVisibility(const GraphicsArrowProgram *first, const GraphicsArrowProgram *other)
{
	if(first->isHiddenByUser() && !other->isHiddenByUser())
		return false;
	else if(!first->isHiddenByUser() && other->isHiddenByUser())
		return true;
	else
		return first->_app_name<other->_app_name;
}

QVariant GraphicsArrowProgram::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if(change==ItemSelectedHasChanged)
		if(value.toBool())
			emit activated(_app_name);

	return value;
}

void GraphicsArrowProgram::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QStyleOptionGraphicsItem *_option = const_cast<QStyleOptionGraphicsItem*>(option);
	_option->state &= ~QStyle::State_Selected;
	QGraphicsPixmapItem::paint(painter, _option, widget);
}

void GraphicsArrowProgram::onHiddenProgramChanged(QString name, bool hidden)
{
	if(name==_app_name)
		setHiddenByUser(hidden || allRulesHidden());
}

void GraphicsArrowProgram::onHiddenRuleChanged(int id, bool)
{
	for(int i=0; i<_rules.size(); ++i)
		if(_rules[i]->id()==id)
		{
			ConfigurationStack &stack=ConfigurationStack::instance();
			setHiddenByUser(allRulesHidden() || stack.isProgramHidden(_app_name));
			return;
		}
}

void GraphicsArrowProgram::onParentHiddenByUser()
{
	setHiddenByUser(_hidden, true);
}
