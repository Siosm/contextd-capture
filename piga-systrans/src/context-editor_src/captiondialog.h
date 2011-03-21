#ifndef CAPTIONDIALOG_H
#define CAPTIONDIALOG_H

#include <QDialog>
#include <QMap>
#include <QString>
#include "graphicsarrow.h"
#include "graphicsdomain.h"

namespace Ui {
	class CaptionDialog;
}

/// A dialog that contains captions for all the items in the GraphicsView
class CaptionDialog : public QDialog
{
	Q_OBJECT
public:
	/*!
	 * \brief Construct a Caption Dialog
	 * \param parent The parent widget of this dialog
	 */
	explicit CaptionDialog(QWidget *parent = 0);
	~CaptionDialog();

private:
	Ui::CaptionDialog *ui;

	/// A map containing each domain and its associated GraphicsDomain
	QMap<QString, GraphicsDomain*> _domains;

	/// The GraphicsArrow in the arrow QGraphicsView
	GraphicsArrow *_arrow;

public slots:
	/// Clear and redraw the items in the dialog's QGraphicsViews
	void updateViews();

private slots:
	/// Handler for emulating selection of domains in the domain view
	void onDomainActivated(QString name);
};

#endif // CAPTIONDIALOG_H
