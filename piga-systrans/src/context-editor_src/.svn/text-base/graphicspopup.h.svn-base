#ifndef GRAPHICSPOPUP_H
#define GRAPHICSPOPUP_H

#include <QColor>
#include <QGraphicsRectItem>

/// Forward declaration of GraphicsArrow
class GraphicsArrow;

/// A rectangle containing GraphicsArrowProgram objects for a given GraphicsArrow
class GraphicsPopup : public QObject, public QGraphicsRectItem
{
	friend class CaptionDialog;
    Q_OBJECT
public:
	/*!
	 * \brief Construct a GraphicsPopup
	 * \param x The x coordinate of the popup in the parent's coordinate system
	 * \param y The y coordinate of the popup in the parent's coordinate system
	 * \param w The starting width of the item
	 * \param h The starting height of the item
	 * \param parent The parent GraphicsArrow of the item
	 * \param scene The parent scene of the item (should be parent->scene())
	 */
	explicit GraphicsPopup(qreal x=0, qreal y=0, qreal w=50, qreal h=50, GraphicsArrow *parent=0, QGraphicsScene *scene=0);

	/// Reset the pen and brush used by the item depending on the passed pen color
	void setPenAndBrush(QColor penColor);

private:
	GraphicsArrow* _parent;

	/// Bypass the mouse hover events
	void hoverEnterEvent(QGraphicsSceneHoverEvent *);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

signals:

public slots:

};

#endif // GRAPHICSPOPUP_H
