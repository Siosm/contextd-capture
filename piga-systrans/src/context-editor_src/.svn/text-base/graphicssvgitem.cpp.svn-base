#include <QtSvg/QSvgRenderer>
#include <QPointF>
#include "graphicsdomain.h"
#include "graphicssvgitem.h"

GraphicsSvgItem::GraphicsSvgItem(QString &path, QGraphicsItem *parent) :
	QGraphicsSvgItem(path, parent),
	_parent(parent)
{
}

void GraphicsSvgItem::setScale(qreal scale)
{
	qreal ratio=boundingRect().x()/GraphicsDomain::IconSize;
	qDebug("%f: %f/%d: %f", scale, boundingRect().x(), GraphicsDomain::IconSize, ratio);
	QGraphicsSvgItem::setScale(scale);
}

void GraphicsSvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	if(!renderer()->isValid())
		return;

	QRectF rect=boundingRect();
	rect.setSize(QSizeF(GraphicsDomain::IconSize,GraphicsDomain::IconSize));

	if (elementId().isEmpty())
		renderer()->render(painter, rect);
	else
		renderer()->render(painter, elementId(), rect);
}
