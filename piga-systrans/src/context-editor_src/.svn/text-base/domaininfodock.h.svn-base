#ifndef DOMAININFODOCK_H
#define DOMAININFODOCK_H

#include "domain.h"
#include "editordock.h"

namespace Ui {
    class DomainInfoDock;
}

/// A QDock displaying information about a domain
class DomainInfoDock : public EditorDock
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Domain Info Dock
	 * \param parent The parent widget of the dock
	 */
	explicit DomainInfoDock(QWidget *parent = 0);
	~DomainInfoDock();

	/// Set and get the domain for the dock
	void setDomain(Domain dom);
	Domain domain() const;

	/// Whether the dock's domain is valid
	bool isValid() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DomainInfoDock *ui;
	Domain _domain;

	/// Functions to update and reset the contents of the dock
	void updateDock();
	void resetDock();

public slots:
	/// A slot for asking the dock to be updated
	void onUpdateDock();

private slots:
	/// Watch the changes over the dock's domain
	void onDomainChanged(QString oldName, QString newName);

	/// Update the dock when a program or a rule changed
	void onProgramsOrRulesChanged();

	/// Edit the domain of the dock
	void onEditClicked();
};

#endif // DOMAININFODOCK_H
