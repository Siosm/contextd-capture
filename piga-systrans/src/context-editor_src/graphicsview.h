#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPair>
#include "graphicsarrow.h"
#include "graphicsdomain.h"

/// An area for displaying and manipulating GraphicsItems
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a GraphicsView
	 * \param parent The parent widget for the view
	 */
    explicit GraphicsView(QWidget *parent = 0);

	/// Highlight a domain in the view (will unhighlight any other item)
	void highlightDomain(QString name);

	/// Highlight a transition arrow and it's connected domains
	void highlightTransitionArrow(QString source, QString dest);

	/// Highlight all the transition arrows that contain the passed rule
	void highlightRule(int id);

	/// Unhighlight all highlighted items
	void unhighlight();

protected:
	void wheelEvent(QWheelEvent *event);

private:
	/// A map containing each domain and its associated GraphicsDomain, and a list of highlighted domains
	QMap<QString, GraphicsDomain*> _domains;
	QVector<QString> _hl_domains;

	/// A map containing all the arrows for a given source and destination, and a list of highlighted arrows
	QMap<QPair<QString, QString>, GraphicsArrow*> _arrows;
	QVector<QPair<QString, QString> > _hl_arrows;

	/// The available zoom modes and a function for manipulation the scene's scale
	enum {
		ZoomIn=0,
		ZoomOut,
		ZoomBest,
		ZoomOriginal
	};
	void zoom(int mode, qreal value=0.0f);

	/// The context menu of the graphics view, and the one for the different items
	QMenu *_menu;
	QMenu *_domain_menu;
	QMenu *_program_menu;

	void setupMenus();
	void contextMenuEvent(QContextMenuEvent *event);

signals:
	void domainActivated(QString);
	void programActivated(QString);
	void transitionArrowActivated(QString, QString, QString);

	void newTransition();
	void newDomain();
	void newProgram();

private slots:
	void onConfigurationChanged();
	void onHiddenDomainChanged();
	void onHiddenProgramChanged();
	void onHiddenRuleChanged();

public slots:
	/// Handlers for zoom management
	void onZoomIn();
	void onZoomOut();
	void onZoomFitBest();
	void onZoomOriginalSize();

	/// Show all items that were previously hidden
	void onShowAllItems();

	/// Update the contents of the view
	void updateView();

	/// Focus on a domain (hide all items not related to this domain)
	void onDomainFocused(QString name);

	/// Focus on a program (hide all items not related to this program)
	void onProgramFocused(QString name);

	/// Export the graph to a PNG image
	void onExportGraphAsPNG();
};

#endif // GRAPHICSVIEW_H
