#ifndef MESSAGEAREA_H
#define MESSAGEAREA_H

#include <QLabel>
#include <QList>
#include <QScrollArea>
#include <QSize>
#include "warning.h"

namespace Ui {
    class MessageArea;
}

/// An area in which messages can be displayed
class MessageArea : public QScrollArea
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Message Area
	 * \param parent The parent widget where this area will be displayed
	 */
    explicit MessageArea(QWidget *parent = 0);
	~MessageArea();

protected:
    void changeEvent(QEvent *e);

	/// A reimplementation of the sizeHint function that fixes the height to three items' height
	virtual QSize sizeHint() const;

private:
	Ui::MessageArea *ui;
	QList<QLabel *> _text_labels;

	/// Clear the message area
	void clear();

	/// Display a Warning in the top of the message area
	void addWarning(const Warning &warn);

signals:
	/// Signals emitted when a domain/program/rule link is activated
	void domainActivated(QString);
	void programActivated(QString);
	void ruleActivated(int);

private slots:
	/// Handler for clicks on links inside the message area
	void onLinkActivated(QString uri);

public slots:
	/// Handler that fetches current warnings from the stack and displays them
	void onWarningsChanged();

	/// Handler that exports the message area's data to a text file
	void onExportToText();
};

#endif // MESSAGEAREA_H
