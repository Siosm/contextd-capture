#ifndef GRAPHICSDOMAIN_H
#define GRAPHICSDOMAIN_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include "graphicssvgitem.h"
#include "domain.h"

/// A QGraphicsEllipseItem representing a Domain in the GraphicsView
class GraphicsDomain : public QObject, public QGraphicsEllipseItem
{
	friend class CaptionDialog;
	Q_OBJECT

public:
	/// The radius of the ellipse item in pixels
	static const int EllipseRadius;

	/// The diameter of the ellipse item (twice the radius)
	static const int EllipseDiameter;

	/// The size in pixels at which the domain's icon should be rendered
	static const int IconSize;

	/*!
	 * \brief Construct a GraphicsDomain
	 * \param dom The domain represented by this item
	 * \param x The x coordinate of the item in the scene
	 * \param y The y coordinate of the item in the scene
	 * \param scene The scene in which the item should be rendered
	 * \param parent The QObject parent of the item
	 */
	explicit GraphicsDomain(Domain dom, int x=0, int y=0, QGraphicsScene *scene=0, QObject *parent=0);

	/// Overrides of the setPos function that store the position internally
	void setPos(QPointF);
	void setPos(qreal x, qreal y);

	/// Retrieve the center position of the item
	QPoint centerPos() const;
	QPointF centerPosF() const;

	/// Set the position of the center of the item rather than it's top-left point
	void setCenterPos(QPointF);
	void setCenterPos(qreal x, qreal y);

	/// Set whether the arrow should be highlighted
	void setHighlighted(bool highlighted);
	bool isHighlighted() const;

	/// Set whether the arrow should be hidden when the view is drawn
	void setHiddenByUser(bool hidden);
	bool isHiddenByUser() const;

	/// The domain represented by this item
	Domain domain() const;

private:
	Domain _domain;
	qreal _x,_y;

	/// Pointers to the domain icon depending on its type (svg or bitmap)
	GraphicsSvgItem *_pixSvg;
	QGraphicsPixmapItem *_pixPng;

	/// An extra outline that is drawn if the domain is the default domain
	QGraphicsEllipseItem *_extra_outline;

	/// The text label of the item and a background used for better readability
	QGraphicsPathItem *_label_bg;
	QGraphicsTextItem *_label;
	bool _highlighted;
	bool _hidden;

	/// Set the circle's color depending on whether the domain is selected, and whether it is default
	void setColor();

	/// Mouse hover and click events
	void hoverEnterEvent(QGraphicsSceneHoverEvent *);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

	/// Set the pen and brush style depending on whether the domain is highlighted
	void setPenAndBrush();

	/// Draw the attributes used to differentiate the default domain from normal ones
	void drawDefaultDomainAttributes(bool is_default);

	/// Redefinition of the QGraphicsItem function for drawing the item
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
	void activated(QString);
	void hiddenByUser(bool);

private slots:
	void onDomainHidden(QString name, bool hidden);
};

#endif // GRAPHICSDOMAIN_H
