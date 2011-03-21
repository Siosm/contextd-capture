#include <QCursor>
#include "colors.h"
#include "graphicsarrow.h"
#include "graphicspopup.h"

GraphicsPopup::GraphicsPopup(qreal x, qreal y, qreal w, qreal h, GraphicsArrow *parent, QGraphicsScene *scene) :
	QGraphicsRectItem(x, y, w, h, 0, scene),
	_parent(parent)
{
	hide();
	setAcceptHoverEvents(true);
	setZValue(5);
	setPos(parent->path().pointAtPercent(0.5) - QPointF(boundingRect().width()/2, boundingRect().height()/2));

	if(!EditorConfig::fastDraw())
		setOpacity(0.85);
}

void GraphicsPopup::setPenAndBrush(QColor penColor)
{
	QColor brushColor;

	if(_parent && _parent->isHighlighted())
	{
		brushColor.setNamedColor(Colors::DomainSelectedBackground);
	}
	else
	{
		if(penColor==Colors::ArrowNormalPen)
			brushColor.setNamedColor(Colors::DomainNormalBackground);
		else
		{
			brushColor.setRed(255 - (255-penColor.red())/3);
			brushColor.setGreen(255 - (255-penColor.green())/3);
			brushColor.setBlue(255 - (255-penColor.blue())/3);
		}
	}

	//Set the pen and brush
	setBrush(QBrush(brushColor));
	setPen(QPen(penColor, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
}

void GraphicsPopup::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
	setCursor(QCursor(Qt::ArrowCursor));
}

void GraphicsPopup::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	if(!_parent || !_parent->contains(QGraphicsItem::mapToItem(_parent, event->pos())))
	{
		setCursor(QCursor(Qt::OpenHandCursor));
		hide();
	}
}
