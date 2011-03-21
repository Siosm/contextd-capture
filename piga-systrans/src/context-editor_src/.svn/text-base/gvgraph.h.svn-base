#ifndef GVGRAPH_H
#define GVGRAPH_H

#include <graphviz/types.h>
#include <graphviz/graph.h>

#include <QPointF>
#include <QFont>
#include <QMap>
#include <QPainterPath>
#include <QPair>
#include <QString>
#include <QStringList>

/// A struct containing the information for a GVGraph's node
struct GVNode
{
	/// The unique identifier of the node in the graph
	QString name;

	/// The position of the center point of the node from the top-left corner
	QPoint centerPos;

	/// The size of the node in pixels
	double height, width;
};

/// A struct containing the information for a GVGraph's edge
struct GVEdge
{
	/// The source and target nodes of the edge
	QString source;
	QString target;

	/// Path of the edge's line
	QPainterPath path;

	/// Top left point of the label area
	QPoint labelPos;
};

/// An object containing a libgraph graph and its associated nodes and edges
class GVGraph
{
public:
	/// Default DPI value used by dot
	static const qreal DotDefaultDPI;

	/// Margin and spacing for edge label items
	static const int EdgeLabelMargin;
	static const int EdgeLabelSpacing;

	/*!
	 * \brief Construct a GraphViz graph object
	 * \param name The name of the graph, must be unique in the application
	 * \param font The font to use for the graph
	 * \param node_size The size in pixels of each node
	 * \param edge_item_size The size in pixels of each item in an edge's label
	 */
	GVGraph(QString name, QFont font=QFont(), qreal node_size=50, qreal edge_item_size=24);
	~GVGraph();

	/// Add and remove nodes
	void addNode(const QString& name);
	void addNodes(const QStringList& names);
	void removeNode(const QString& name);
	void clearNodes();

	/// Add and remove edges. label_item_count indicates the number of items in an edge's label
	void addEdge(const QString& source, const QString& target, int label_item_count=0);
	void removeEdge(const QString& source, const QString& target);
	void removeEdge(const QPair<QString, QString>& key);

	/// Set the root node of the graph
	void setRootNode(const QString& name);

	/// Set the font to use in all the labels
	void setFont(QFont font);

	/// Apply the dot layout to the graph's data
	void applyLayout();

	/// Return a list of objects representing nodes of the graph
	QList<GVNode> nodes() const;

	/// Return a list of objects representing edges of the graph
	QList<GVEdge> edges() const;

	/// Return a rectangle representing the minimal area in which the graph can be drawn
	QRectF boundingRect() const;

private:
	GVC_t *_context;
	Agraph_t *_graph;
	QFont _font;
	QFont _edge_font;
	QMap<QString, Agnode_t*> _nodes;
	QMap<QPair<QString, QString>, QPair<Agedge_t*, int> > _edges;
	qreal _edge_label_item_size;

	/// Returns a string that has the good size in pixels for containing label_item_count label items for an edge
	QString labelForEdge(int label_item_count) const;
};

#endif // GVGRAPH_H
