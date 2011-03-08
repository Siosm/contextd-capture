#ifndef CHANGECONTEXTDIALOG_H
#define CHANGECONTEXTDIALOG_H

#include "ui_changecontextdialog.h"
#include "contextlist.h"

class ChangeContextDialog : public QDialog, private Ui::ChangeContextDialog {
	Q_OBJECT
	Q_DISABLE_COPY(ChangeContextDialog)

	QString wantedContext;
	ContextList clist;

	unsigned int indexNotAvailable;
public:
	explicit ChangeContextDialog(QWidget *parent = 0);

	QString selectedContext();
	void setSelectedContext(QString context);

protected:
	virtual void changeEvent(QEvent *e);

public slots:
	void setVisible(bool visible);
	void systemContextUpdated();

private slots:
	void comboChanged(QString);
	void accept();
	void updateContextList();

signals:
	void contextChanged(QString context);
};

#endif // CHANGECONTEXTDIALOG_H
