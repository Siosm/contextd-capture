#ifndef SELECTDEFAULTDOMAINDIALOG_H
#define SELECTDEFAULTDOMAINDIALOG_H

#include <QDialog>
#include <QMap>
#include <QRadioButton>
#include <QListWidgetItem>

namespace Ui {
    class SelectDefaultDomainDialog;
}

/// A dialog asking to select a new default domain
class SelectDefaultDomainDialog : public QDialog
{
    Q_OBJECT
public:/*!
	 * \brief Construct a Select Default Domain Dialog
	 * \param parent The parent widget of the dialog
	 */
	explicit SelectDefaultDomainDialog(QWidget *parent = 0);
    ~SelectDefaultDomainDialog();

	/// The name of the new default domain, if one has been selected yet
	QString newDefaultDomainName() const;

protected:
    void changeEvent(QEvent *e);

private:
	Ui::SelectDefaultDomainDialog *ui;

	/// A map between radio buttons and list items
	QMap<QRadioButton *, QListWidgetItem *>_map;

	/// The name of the last selected domain
	QString _selected_name;

private slots:
	/// A slot to forward selection of a list widget item to its radio button
	void onItemSelectionChanged();

	/// The slot that fills the selected domain QString using the radio's mapped item's text
	void onRadioButtonClicked();
};

#endif // SELECTDEFAULTDOMAINDIALOG_H
