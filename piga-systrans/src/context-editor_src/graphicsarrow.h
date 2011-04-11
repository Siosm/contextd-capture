#ifndef GRAPHICSARROW_H
#define GRAPHICSARROW_H

#include <QPointF>
#include <QGraphicsPathItem>
#include "editorconfig.h"
#include "graphicsarrowprogram.h"
#include "graphicsdomain.h"
#include "graphicspopup.h"
#include "loop.h"
#include "rule.h"

/// A QGraphicsPathItem representing a transition arrow between two GraphicsDomains
class GraphicsArrow : public QObject, public QGraphicsPathItem
{
	friend class CaptionDialog;
	Q_OBJECT

public:
	/*!
	 * \brief Construct a GraphicsArrow
	 * \param src The source GraphicsDomain of the transition
	 * \param dest The target GraphicsDomain of the transition
	 * \param parent The scene in which to draw the item
	 * \param force_mode If set, the arrow mode to use for this item despite the settings
	 */
	explicit GraphicsArrow(GraphicsDomain *src, GraphicsDomain *dest, QGraphicsScene *parent = 0, int force_mode=EditorConfig::InvalidArrowMode);
	~GraphicsArrow();

	/// Add programs or rules to the transition arrow
	bool addProgram(const QString& name, bool show_label=false);
	void addRule(AbstractRulePtr abstract);

	/// The number of programs added to the arrow so far
	int programCount() const;

	/// Set which path should be used for the arrow instead of the one calculated by the constructor
	void setPath(const QPainterPath &path, QPoint prog_position=QPoint());

	/// Calculate a line path from src to dest and use this path for the arrow
	void makeLinePath();

	/// Set whether the arrow should be highlighted
	void setHighlighted(bool highlighted);
	bool isHighlighted() const;

	/// Set whether the arrow should be hidden when the view is drawn
	void setHiddenByUser(bool hidden);
	bool isHiddenByUser() const;

	/// The arrow program mode for this arrow
	int arrowProgramMode() const;

	/// The warning type calculated from the rules that were added
	int warningType() const;

	/// The source and target domains of the arrow
	GraphicsDomain *source() const;
	GraphicsDomain *target() const;

private:
	QGraphicsScene *_parent;

	/// The source and target domains of the arrow
	GraphicsDomain *_src, *_dest;

	/// The start top-left coordinate to use for positiong programs
	QPoint _prog_position;

	/// The last path that was set in the app, used for calculating path intersections
	QPainterPath _last_path;

	/// The map to GraphicsArrowProgram objects representing the applications added so far
	QMap<QString, GraphicsArrowProgram*> _apps;

	/// An item representing the head of the drawn arrow
	QGraphicsPathItem *_arrow_head;

	/// An item used in the HorizontalIcons arrow mode as a background for the program icons
	QGraphicsPathItem *_prog_bg_item;

	/// An item used in the Popup arrow mode as a background for the program icons
	GraphicsPopup *_popup;

	/// Whether the arrow is currently highlighted in the Editor
	bool _highlighted;

	/// Whether the arrow is currently hidden by the user
	bool _hidden;

	/// A flag for holding the arrowMode that was set in the constructor
	int _force_mode;

	/// A flag representing the Rule WarningType applying to the arrow depending on the added rules
	int _warning_type;

	/// Add a program to the arrow
	bool addProgram(const QString& name, bool show_label, bool redraw);

	/// Redraw the arrow head
	void drawArrow();

	/// Recalculate the position of the child programs
	void positionPrograms();

	/// Whether all child GraphicsArrowProgram are hidden by the user
	bool allProgramsHidden() const;

	/// Bypass the mouse hover events
	void hoverEnterEvent(QGraphicsSceneHoverEvent *);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

	/// Set the pen and brush style depending on whether the arrow is highlighted
	void setPenAndBrush();

	/// A version of setHiddenByUser that can avoid signal sending
	void setHiddenByUser(bool hidden, bool quiet);

	/// Override of methods used for calculating the selection area of the arrow
	virtual QPainterPath shape() const;

signals:
	void activated(QString, QString, QString);
	void hiddenByUser(bool);

private slots:
	void onCircleActivated(QString);
	void onDomainHiddenByUser();
	void onProgramHiddenByUser();
};

#endif // GRAPHICSARROW_H
