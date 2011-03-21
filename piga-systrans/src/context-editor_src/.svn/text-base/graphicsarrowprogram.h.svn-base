#ifndef GRAPHICSARROWPROGRAM_H
#define GRAPHICSARROWPROGRAM_H

#include <QGraphicsPixmapItem>
#include "loop.h"
#include "rule.h"

/// Forward declaration of GraphicsArrow
class GraphicsArrow;

/// A class for representing rules for a given program and GraphicsArrow
class GraphicsArrowProgram : public QObject, public QGraphicsPixmapItem
{
	friend class CaptionDialog;
    Q_OBJECT
public:
	/// The size in pixels to use for rendering the program icon of the object
	static const qreal IconSize;

	/*!
	 * \brief Construct a GraphicsArrowProgram
	 * \param app_name The name of the program attached to this item
	 * \param parent The parent arrow of the item
	 * \param scene The scene in which to draw the item (should be parent->scene())
	 * \param show_label Whether to show the app name as a label below the program icon
	 */
	explicit GraphicsArrowProgram(QString app_name, GraphicsArrow *parent=0, QGraphicsScene *scene=0, bool show_label=false);

	/// Add a rule to the item (shown in the tooltip and used for rule warning calculation)
	void addRule(AbstractRulePtr abstract);

	/// Whether the rule is hidden by the user
	void setHiddenByUser(bool hidden);
	bool isHiddenByUser() const;

	/// The arrow of whom this item is a child of
	GraphicsArrow *parentArrow() const;

	/// The name of the program attached to this item
	QString appName() const;

	/// The list of rules attached to this item
	QVector<AbstractRulePtr> rules() const;

	/// Used for sorting programs, first sorts by visibility, then by app name
	static bool sortByVisibility(const GraphicsArrowProgram *first, const GraphicsArrowProgram *other);

private:
	GraphicsArrow *_parent;
	QString _app_name;
	QVector<AbstractRulePtr> _rules;

	/// An item used for drawing an outline around the program icon if the warning type is valid
	QGraphicsPathItem *_border;

	/// Whether the arrow is currently hidden by the user
	bool _hidden;

	/// A flag representing the Rule WarningType applying to the program item depending on the added rules
	int _warning_type;

	/// A version of setHiddenByUser that can avoid signal sending
	void setHiddenByUser(bool hidden, bool quiet);
	bool allRulesHidden() const;

	/// Mouse hover and click events
	void hoverEnterEvent(QGraphicsSceneHoverEvent *);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	/// Redefinition of the QGraphicsItem function for drawing the item
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
	void activated(QString);
	void hiddenByUser(bool);

private slots:
	void onHiddenProgramChanged(QString name, bool hidden);
	void onHiddenRuleChanged(int id, bool hidden);
	void onParentHiddenByUser();

};

#endif // GRAPHICSARROWPROGRAM_H
