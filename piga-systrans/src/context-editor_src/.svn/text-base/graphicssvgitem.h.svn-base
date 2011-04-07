#ifndef GRAPHICSSVGITEM_H
#define GRAPHICSSVGITEM_H

#include <QGraphicsSvgItem>

/// An override of QGraphicsSvgItem that scales SVG to the IconSize of GraphicsDomains
class GraphicsSvgItem : public QGraphicsSvgItem
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a GraphicsSvgItem
	 * \param path The path to the SVG image to render
	 * \param parent The parent item for this item (usually a GraphicsDomain)
	 */
	explicit GraphicsSvgItem(QString &path, QGraphicsItem *parent = 0);

	/// An override of the setScale method that takes the IconSize scaling into account
	void setScale(qreal scale);

	/// An override of the paint method that renders the SVG at the wanted scale
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	QGraphicsItem *_parent;

};

#endif // GRAPHICSSVGITEM_H
