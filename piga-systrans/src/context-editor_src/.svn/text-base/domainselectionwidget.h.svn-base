#ifndef DOMAINSELECTIONWIDGET_H
#define DOMAINSELECTIONWIDGET_H

#include <QWidget>
#include <QList>
#include <QTreeWidgetItem>
#include "trule.h"
#include "typeddelegate.h"

namespace Ui {
	class DomainSelectionWidget;
}

/// A class for selecting existing domains and creating regular expressions of domains
class DomainSelectionWidget : public QWidget
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Domain Selection Widget
	 * \param parent The parent widget of this widget
	 */
	explicit DomainSelectionWidget(QWidget *parent = 0);
	~DomainSelectionWidget();

	/// Whether at least one in the tree widget is checked
	bool hasCheckedDomains() const;

	/*!
	 * \brief Change the check state of an existing domain to checked
	 * \param name The name of the domain
	 * \param notify Check state for the notify parameter of the domain
	 * \param prompt Check state for the prompt parameter of the domain
	 * \return whether the domain was found and checked
	 */
	bool checkDomain(QString name, Qt::CheckState notify, Qt::CheckState prompt);

	/// Add a regular expression item to the widget, either empty or already filled
	void addExpression(QString expr=QString(), Qt::CheckState notify=Qt::PartiallyChecked, Qt::CheckState prompt=Qt::PartiallyChecked);

	/// Return the list of all TRules for this widget (ie. the domain names/expressions and their notify/prompt parameters)
	QList<TRule> domains() const;

protected:
    void changeEvent(QEvent *e);

private:
	Ui::DomainSelectionWidget *ui;

	/// A delegate used for modifying regular expression items
	TypedDelegate *_delegate;

	/// Available types of tree widget items (existing domain name, or regular expression)
	enum {
		DomainNameItem=0,
		DomainRegExpItem
	};

	/// The QTreeWidgetItem data role for the domain type
	static const int ItemTypeRole=Qt::UserRole+1;

signals:
	void domainChanged();

private slots:
	void onTreeSelectionChanged();
	void onRegexpAddClicked();
	void onRegexpRemoveClicked();
	void onClearCheckboxClicked();

	/// Check the validity of an item when it changes, and emit domainChanged
	void onItemChanged(QTreeWidgetItem *item, int column);
};

#endif // DOMAINSELECTIONWIDGET_H
