#include <QPen>
#include <QLinearGradient>
#include <QStyleOptionGraphicsItem>
#include "configurationstack.h"
#include "graphicsarrow.h"
#include "colors.h"

GraphicsArrow::GraphicsArrow(GraphicsDomain *src, GraphicsDomain *dest, QGraphicsScene *parent, int force_mode) :
	QGraphicsPathItem(0, parent),
	_parent(parent),
	_src(src),
	_dest(dest),
	_apps(),
	_arrow_head(new QGraphicsPathItem(QPainterPath(), this, _parent)),
	_highlighted(false),
	_hidden(false),
	_force_mode(force_mode),
	_warning_type(InvalidType)
{
	//Setup the popup rectangle, only if the mode is Popup because a mode switch would imply a redraw of the whole scene
	if(arrowProgramMode()==EditorConfig::Popup)
	{
		_popup=new GraphicsPopup(0, 0, 16, GraphicsArrowProgram::IconSize+16, this, scene());
		if(EditorConfig::hideMode()!=EditorConfig::LowerOpacityAndForbidSelection)
			setAcceptHoverEvents(true);

		_prog_bg_item=0;
	}
	else
	{
		_popup=0;

		_prog_bg_item=new QGraphicsPathItem(this, scene());
		_prog_bg_item->setOpacity(0.2);
		_prog_bg_item->setZValue(-1);
		_prog_bg_item->setFlag(ItemNegativeZStacksBehindParent);
	}

	//Set the arrow attributes
	setZValue(1);
	setPenAndBrush();
	connect(_src, SIGNAL(hiddenByUser(bool)), this, SLOT(onDomainHiddenByUser()));
	connect(_dest, SIGNAL(hiddenByUser(bool)), this, SLOT(onDomainHiddenByUser()));
}

GraphicsArrow::~GraphicsArrow()
{
	qDeleteAll(_apps);
}

bool GraphicsArrow::addProgram(const QString& name, bool show_label)
{
	return addProgram(name, show_label, true);
}

bool GraphicsArrow::addProgram(const QString& name, bool show_label, bool redraw)
{
	if(!_apps.contains(name))
	{
		GraphicsArrowProgram *circle=new GraphicsArrowProgram(name, this, scene(), show_label);
		connect(circle, SIGNAL(activated(QString)), this, SLOT(onCircleActivated(QString)));
		connect(circle, SIGNAL(hiddenByUser(bool)), this, SLOT(onProgramHiddenByUser()));

		_apps[name]=circle;

		if(redraw)
		{
			//A hack to make sure we check if the child is hidden
			setHiddenByUser(_hidden, true);
			positionPrograms();
		}

		return true;
	}
	else
		return false;
}

void GraphicsArrow::addRule(AbstractRulePtr abstract)
{
	//Update the arrow's warning type by taking the source domain into account
	_warning_type|=ConfigurationStack::instance().warningTypesForRuleAndSourceDomain(abstract->id(), _src->domain().name());

	//Redraw the arrow
	setPenAndBrush(); //FIXME performance issue, should be done after all rules were added if possible

	//Add the rule's program if it's not already added
	bool createdPixmap=addProgram(abstract->appName(), false, false);

	//Insert the rule in the GraphicsArrowProgram for display
	_apps[abstract->appName()]->addRule(abstract);

	//Redraw the programs now if we added one
	if(createdPixmap)
	{
		//A hack to make sure we check if the child is hidden
		setHiddenByUser(_hidden, true);
		positionPrograms();
	}
}

void GraphicsArrow::drawArrow()
{
	if(!path().isEmpty())
	{
		//Draw an arrow-head shaped path at the end of the line's path
		QPainterPath arrowHead;
		QPointF pos=path().currentPosition();
		arrowHead.moveTo(pos+QPointF(-10,-4));
		arrowHead.lineTo(pos);
		arrowHead.lineTo(pos+QPointF(-10,4));
		arrowHead.lineTo(pos+QPointF(-10,-4));

		//Create a graphics path item based on this path
		_arrow_head->setPath(arrowHead);
		_arrow_head->setTransformOriginPoint(path().currentPosition());
		_arrow_head->setRotation(-path().angleAtPercent(1));
	}
}

int GraphicsArrow::programCount() const
{
	return _apps.count();
}

void GraphicsArrow::setPath(const QPainterPath &path, QPoint prog_position)
{
	QGraphicsPathItem::setPath(path);
	drawArrow();

	_prog_position=prog_position;
	_last_path=path;
	positionPrograms();
}

void GraphicsArrow::makeLinePath()
{
	//Check for null pointers before trying to draw the arrow
	if(_src!=0 && _dest!=0)
	{
		//Draw a basic path for the arrow, if the source and destination domains are not in the same position
		if(_src->centerPosF()!=_dest->centerPosF())
		{
			QPainterPath bezierPath;

			QLineF line(_src->centerPosF(), _dest->centerPosF());
			QPointF origBound=line.pointAt(GraphicsDomain::EllipseRadius/line.length());
			QPointF dstBound=line.pointAt((line.length()-GraphicsDomain::EllipseRadius)/line.length());

			bezierPath.moveTo(origBound);
			bezierPath.lineTo(dstBound);

			_prog_position.setX(bezierPath.pointAtPercent(0.5).x());
			_prog_position.setY(bezierPath.pointAtPercent(0.5).y());

			setPath(bezierPath);
		}
	}
}

void GraphicsArrow::positionPrograms()
{
	QList<GraphicsArrowProgram*> pixmaps=_apps.values();
	qSort(pixmaps.begin(), pixmaps.end(), GraphicsArrowProgram::sortByVisibility);

	if(arrowProgramMode()==EditorConfig::HorizontalIcons)
	{
		for(int i=0; i<pixmaps.size(); ++i)
			pixmaps.at(i)->setPos(_prog_position + QPoint((GraphicsArrowProgram::IconSize+6)*i, 0));

		//Bounds of the rectangle in which the icons are placed
		qreal leftBound=scene()->sceneRect().x();
		qreal rightBound=_prog_position.x()+(GraphicsArrowProgram::IconSize+6)*pixmaps.size()-3;
		qreal topBound=_prog_position.y()-5;
		qreal bottomBound=_prog_position.y()+5+GraphicsArrowProgram::IconSize;

		//Create a path out of the bounds
		QPainterPath rectPath;
		rectPath.addRoundedRect(QRectF(QPointF(leftBound, topBound), QPointF(rightBound, bottomBound)), 5, 5);

		//A copy of the arrow's path, used for calculating an intersection
		QPainterPath arrowPath(_last_path);
		arrowPath.lineTo(rightBound, arrowPath.pointAtPercent(1).y());
		arrowPath.lineTo(rightBound, arrowPath.pointAtPercent(0).y());

		//Use the intersection of both paths as the background item for the icons
		_prog_bg_item->setPath(rectPath.intersected(arrowPath));
	}
	else if(arrowProgramMode()==EditorConfig::Popup)
	{
		QRectF rect=_popup->rect();
		rect.setWidth((GraphicsArrowProgram::IconSize+6)*pixmaps.size()+10);
		_popup->setRect(rect);
		_popup->setPos(path().pointAtPercent(0.5) - QPointF(_popup->boundingRect().width()/2, _popup->boundingRect().height()/2));

		for(int i=0; i<pixmaps.size(); ++i)
		{
			QGraphicsPixmapItem *item=pixmaps.at(i);
			item->setParentItem(_popup);
			item->setPos(i*(GraphicsArrowProgram::IconSize+6)+8, 8);
			item->show();
		}
	}
	else
	{
		for(int i=0; i<pixmaps.size(); ++i)
			pixmaps.at(i)->hide();
	}
}

void GraphicsArrow::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
	setCursor(QCursor(Qt::ArrowCursor));
	_popup->show();
}

void GraphicsArrow::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	if(!_popup->contains(QGraphicsItem::mapToItem(_popup, event->pos())))
	{
		setCursor(QCursor(Qt::OpenHandCursor));
		_popup->hide();
	}
}

void GraphicsArrow::setHighlighted(bool highlighted)
{
	if(highlighted!=_highlighted)
	{
		_highlighted=highlighted;
		setPenAndBrush();
		drawArrow();
	}
	else
		_highlighted=highlighted;
}

bool GraphicsArrow::isHighlighted() const
{
	return _highlighted;
}

bool GraphicsArrow::allProgramsHidden() const
{
	QList<GraphicsArrowProgram *> apps=_apps.values();

	for(int i=0; i<apps.size(); ++i)
	{
		if(!apps.at(i)->isHiddenByUser())
			return false;
	}

	return true;
}

void GraphicsArrow::setHiddenByUser(bool hidden)
{
	setHiddenByUser(hidden, false);
}

void GraphicsArrow::setHiddenByUser(bool hidden, bool quiet)
{
	//If the arrow is hidden, or if one if its extremities is
	if(hidden || _src->isHiddenByUser() || _dest->isHiddenByUser() || allProgramsHidden())
	{
		setOpacity(0.4);
		if(EditorConfig::hideMode()==EditorConfig::LowerOpacityAndForbidSelection &&
		   arrowProgramMode()==EditorConfig::Popup)
		{
			setAcceptHoverEvents(false);
		}
	}
	else
	{
		setOpacity(1.0);
		if(EditorConfig::hideMode()==EditorConfig::LowerOpacityAndForbidSelection &&
		   arrowProgramMode()==EditorConfig::Popup)
		{
			setAcceptHoverEvents(true);
		}
	}

	_hidden=hidden;

	//Use the actual criteria for hiding the arrow so that children behave properly
	//This only interests ArrowPrograms so if they're all hidden it's pointless to notify the Arrow is hidden too
	if(!quiet)
		emit hiddenByUser(hidden || _src->isHiddenByUser() || _dest->isHiddenByUser());
}

bool GraphicsArrow::isHiddenByUser() const
{
	return _hidden || _src->isHiddenByUser() || _dest->isHiddenByUser();
}

int GraphicsArrow::arrowProgramMode() const
{
	if(_force_mode!=EditorConfig::InvalidArrowMode)
		return _force_mode;
	else
		return EditorConfig::arrowProgramMode();
}

int GraphicsArrow::warningType() const
{
	return _warning_type;
}

GraphicsDomain *GraphicsArrow::source() const
{
	return _src;
}

GraphicsDomain *GraphicsArrow::target() const
{
	return _dest;
}

void GraphicsArrow::setPenAndBrush()
{
	QColor brushColor;
	QColor penColor;
	QColor propBgBrushColor;
	QColor propBgPenColor;

	if(!_highlighted)
	{
		if(EditorConfig::arrowColorScheme()==EditorConfig::WarningColor)
		{
			ArrowColorAnimation &anim=ArrowColorAnimation::instance();

			brushColor=anim.getColorFromRuleWarningType(_warning_type);
			penColor=brushColor;
		}
		else
		{
			brushColor.setNamedColor(Colors::ArrowNormalHead);
			penColor.setNamedColor(Colors::ArrowNormalPen);
		}

		propBgBrushColor.setNamedColor(Colors::ArrowNormalProgItemBackground);
		propBgPenColor.setNamedColor(Colors::ArrowNormalProgItemForeground);
	}
	else
	{
		brushColor.setNamedColor(Colors::ArrowSelectedHead);
		penColor.setNamedColor(Colors::ArrowSelectedPen);
		propBgBrushColor.setNamedColor(Colors::ArrowSelectedProgItemBackground);
		propBgPenColor.setNamedColor(Colors::ArrowSelectedProgItemForeground);
	}

	//Set the pen and brush
	setPen(QPen(penColor, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
	_arrow_head->setPen(QPen(penColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	_arrow_head->setBrush(QBrush(brushColor, Qt::SolidPattern));

	if(_prog_bg_item)
	{
		_prog_bg_item->setPen(QPen(propBgPenColor, 0.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		_prog_bg_item->setBrush(QBrush(propBgBrushColor));
	}

	//Set the popup's pen and brush
	if(_popup)
		_popup->setPenAndBrush(penColor);
}

QPainterPath GraphicsArrow::shape() const
{
	QPainterPathStroker stroker;
	stroker.setWidth(6);

	return stroker.createStroke(path());
}

void GraphicsArrow::onCircleActivated(QString app_name)
{
	if(_src && _dest)
		emit activated(app_name, _src->domain().name(), _dest->domain().name());
}

void GraphicsArrow::onDomainHiddenByUser()
{
	setHiddenByUser(_hidden);
}

void GraphicsArrow::onProgramHiddenByUser()
{
	setHiddenByUser(_hidden, true);
	positionPrograms();
}
