#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "abstractruleinfodock.h"
#include "appinfodock.h"
#include "browserdock.h"
#include "domaininfodock.h"
#include "messagearea.h"
#include "transitioninfodock.h"
#include "captiondialog.h"
#include "settingsdialog.h"

namespace Ui {
	class MainWindow;
}

/// The main window of the application
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);

private:
	Ui::MainWindow *ui;

	static const QString MainWindowName;
	enum { MaxRecentFiles=5 };

	void updateRecentFilesMenu();
	void addRecentFileEntry(QString path);

	QAction *_recent_file_acts[MaxRecentFiles];
	QAction *_recent_file_separator;

	AbstractRuleInfoDock _absrule_dock;
	AppInfoDock _app_dock;
	BrowserDock _browser_dock;
	DomainInfoDock _domain_dock;
	MessageArea _message_area;
	TransitionInfoDock _transition_dock;

	SettingsDialog _settings;
	CaptionDialog _caption;

private slots:
	void onApplicationActivated(QString name);
	void onDomainActivated(QString name);
	void onRuleActivated(int id);
	void onTransitionArrowActivated(QString appName, QString source, QString dest);
	void onDockClosed();
	void onNewConfiguration();
	void onConfigurationStackChanged();
	void onConfigurationChanged();
	void onConfigurationSaved();
	void onImportApplicationsClicked();
	void onExportAllClicked();
	void onExportApplicationsClicked();
	void onRecentFileClicked();
	void onSettings();
	void onCaption();
};

#endif // MAINWINDOW_H
