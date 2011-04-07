#ifndef EDITORDOCK_H
#define EDITORDOCK_H

#include <QCloseEvent>
#include <QDockWidget>

/// An abstract QDockWidget class
class EditorDock : public QDockWidget
{
	Q_OBJECT
public:
	/*!
	 * \brief Construct an Editor Dock
	 * \param parent The parent widget of this dock
	 */
	explicit EditorDock(QWidget *parent = 0);
	~EditorDock();

	/// Whether the data in the dock is valid or not
	virtual bool isValid() const;

protected:
	virtual void closeEvent(QCloseEvent *e);

private:
	/// Functions to update and reset the contents of the dock
	virtual void updateDock();
	virtual void resetDock();

signals:
	/// Emitted when the dock is closed
	void dockClosed();

	/// Activation of an item
	void domainActivated(QString);
	void programActivated(QString);
	void ruleActivated(int);
};

#endif // EDITORDOCK_H
