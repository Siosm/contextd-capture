#ifndef DOMAINDIALOG_H
#define DOMAINDIALOG_H

#include <QDialog>
#include "domain.h"

namespace Ui {
    class DomainDialog;
}

class DomainDialog : public QDialog
{
    Q_OBJECT

public:
	explicit DomainDialog(QWidget *parent = 0);
	explicit DomainDialog(Domain subject, QWidget *parent = 0);
    ~DomainDialog();

	//Used to tell which action was performed once the dialog is closed
	enum DialogAction {
		Created=QDialog::Accepted+1,
		Edited
	};

	//Override of the result/setResult methods of QDialog to allow custom values
	int result() const;
	void setResult (int result);

	Domain retrieveDomain() const;
	bool defaultDomainChecked() const;

	static void editDomain(QString name);

protected:
    void changeEvent(QEvent *e);

private:
	//Custom QDialog result
	int _result;
    Ui::DomainDialog *ui;
	Domain _domain;

	void initDialog();

private slots:
	void onIconClicked();
	void onOkClicked();
	void onMandatoryFieldsChanged();
};

#endif // DOMAINDIALOG_H
