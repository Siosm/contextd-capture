#include "appexportdialog.h"
#include "configurationstack.h"
#include "editorconfig.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loopdialog.h"
#include "ruledialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

const QString MainWindow::MainWindowName="Context Editor";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	//Insert actions for recent files
	_recent_file_separator=ui->menuFile->addSeparator();
	_recent_file_separator->setVisible(false);
	ui->menuFile->insertAction(ui->actionQuit, _recent_file_separator);

	for(int i=0; i<MaxRecentFiles; ++i)
	{
		_recent_file_acts[i] = new QAction(ui->menuFile);
		_recent_file_acts[i]->setVisible(false);
		ui->menuFile->insertAction(_recent_file_separator, _recent_file_acts[i]);

		connect(_recent_file_acts[i], SIGNAL(triggered()), this, SLOT(onRecentFileClicked()));
	}
	updateRecentFilesMenu();

	//QKeySequences for menu entries
	ui->actionNewRules->setShortcut(QKeySequence::New);
	ui->actionOpenRules->setShortcut(QKeySequence::Open);
	ui->actionSave->setShortcut(QKeySequence::Save);
	ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
	ui->actionQuit->setShortcut(QKeySequence::Quit);
	ui->actionUndo->setShortcut(QKeySequence::Undo);
	ui->actionRedo->setShortcut(QKeySequence::Redo);
	ui->actionZoomin->setShortcut(QKeySequence::ZoomIn);
	ui->actionZoomout->setShortcut(QKeySequence::ZoomOut);
	ui->actionNormalsize->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));

	//Load the stack
	ConfigurationStack &stack=ConfigurationStack::instance();
	stack.setParentWidget(this);
	onNewConfiguration();

	//Add the browser dock
	connect(ui->actionBrowser, SIGNAL(triggered(bool)), &_browser_dock, SLOT(setVisible(bool)));
	connect(&_browser_dock, SIGNAL(visibilityChanged(bool)), ui->actionBrowser, SLOT(setChecked(bool)));
	this->addDockWidget(Qt::LeftDockWidgetArea, &_browser_dock, Qt::Vertical);
	if(QSettings().value("ui_displayBrowser", true).toBool())
		_browser_dock.show();
	else
		_browser_dock.hide();

	//Add the message dock
	connect(ui->actionMessages, SIGNAL(triggered(bool)), &_message_area, SLOT(setVisible(bool)));
	ui->splitter->addWidget(&_message_area);
	if(QSettings().value("ui_displayMessageArea", false).toBool())
	{
		ui->actionMessages->setChecked(true);
		_message_area.show();
	}
	else
	{
		ui->actionMessages->setChecked(false);
		_message_area.hide();
	}

	//Try to maximize the window by default
	this->window()->showMaximized();

	//Connect buttons and browser dock events
	connect(ui->actionNewRules, SIGNAL(triggered()), &stack, SLOT(onNew()));
	connect(ui->actionOpenRules, SIGNAL(triggered()), &stack, SLOT(onOpen()));
	connect(ui->actionSave, SIGNAL(triggered()), &stack, SLOT(onSave()));
	connect(ui->actionSaveAs, SIGNAL(triggered()), &stack, SLOT(onSaveAs()));
	connect(ui->actionNewApplication, SIGNAL(triggered()), &_browser_dock, SLOT(onAddProgram()));
	connect(ui->actionOpenApplication, SIGNAL(triggered()), this, SLOT(onImportApplicationsClicked()));
	connect(ui->actionExportAll, SIGNAL(triggered()), this, SLOT(onExportAllClicked()));
	connect(ui->actionExportApplications, SIGNAL(triggered()), this, SLOT(onExportApplicationsClicked()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui->actionUndo, SIGNAL(triggered()), &stack, SLOT(onUndo()));
	connect(ui->actionRedo, SIGNAL(triggered()), &stack, SLOT(onRedo()));
	connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(onSettings()));
	connect(ui->actionCaption, SIGNAL(triggered()), this, SLOT(onCaption()));

	//Configuration stack signals
	connect(&stack, SIGNAL(stackChanged()), this, SLOT(onConfigurationStackChanged()));
	connect(&stack, SIGNAL(configurationSaved()), this, SLOT(onConfigurationSaved()));
	connect(&stack, SIGNAL(newConfiguration()), this, SLOT(onNewConfiguration()));
	onConfigurationStackChanged();

	//Changes in the configuration itself
	connect(&stack, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(domainChanged(QString,QString)), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(allRulesChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(loopsChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(rulesChanged()), this, SLOT(onConfigurationChanged()));

	//Connect the graphics view's events to handlers, and zoom buttons to the view
	ui->graphicsView->updateView();
	connect(ui->graphicsView, SIGNAL(domainActivated(QString)), this, SLOT(onDomainActivated(QString)));
	connect(ui->graphicsView, SIGNAL(programActivated(QString)), this, SLOT(onApplicationActivated(QString)));
	connect(ui->graphicsView, SIGNAL(transitionArrowActivated(QString,QString,QString)), this, SLOT(onTransitionArrowActivated(QString,QString,QString)));
	connect(ui->actionZoomin, SIGNAL(triggered()), ui->graphicsView, SLOT(onZoomIn()));
	connect(ui->actionZoomout, SIGNAL(triggered()), ui->graphicsView, SLOT(onZoomOut()));
	connect(ui->actionBestfit, SIGNAL(triggered()), ui->graphicsView, SLOT(onZoomFitBest()));
	connect(ui->actionNormalsize, SIGNAL(triggered()), ui->graphicsView, SLOT(onZoomOriginalSize()));
	connect(ui->action_Show_all_items, SIGNAL(triggered()), ui->graphicsView, SLOT(onShowAllItems()));
	connect(&_browser_dock, SIGNAL(applicationFocused(QString)), ui->graphicsView, SLOT(onProgramFocused(QString)));
	connect(&_browser_dock, SIGNAL(domainFocused(QString)), ui->graphicsView, SLOT(onDomainFocused(QString)));
	connect(ui->graphicsView, SIGNAL(newDomain()), &_browser_dock, SLOT(onAddDomain()));
	connect(ui->graphicsView, SIGNAL(newProgram()), &_browser_dock, SLOT(onAddProgram()));
	connect(ui->graphicsView, SIGNAL(newTransition()), &_browser_dock, SLOT(onAddRule()));

	//Tools menu
	connect(ui->actionExportgraphasPNG, SIGNAL(triggered()), ui->graphicsView, SLOT(onExportGraphAsPNG()));
	connect(ui->actionExportmessagestotext, SIGNAL(triggered()), &_message_area, SLOT(onExportToText()));

	//Connect xxxActivated dock signals to their handlers
	connect(&_absrule_dock, SIGNAL(domainActivated(QString)), this, SLOT(onDomainActivated(QString)));
	connect(&_absrule_dock, SIGNAL(programActivated(QString)), this, SLOT(onApplicationActivated(QString)));
	connect(&_app_dock, SIGNAL(ruleActivated(int)), this, SLOT(onRuleActivated(int)));
	connect(&_browser_dock, SIGNAL(programActivated(QString)), this, SLOT(onApplicationActivated(QString)));
	connect(&_browser_dock, SIGNAL(domainActivated(QString)), this, SLOT(onDomainActivated(QString)));
	connect(&_browser_dock, SIGNAL(ruleActivated(int)), this, SLOT(onRuleActivated(int)));
	connect(&_domain_dock, SIGNAL(domainActivated(QString)), this, SLOT(onDomainActivated(QString)));
	connect(&_domain_dock, SIGNAL(programActivated(QString)), this, SLOT(onApplicationActivated(QString)));
	connect(&_domain_dock, SIGNAL(ruleActivated(int)), this, SLOT(onRuleActivated(int)));
	connect(&_transition_dock, SIGNAL(ruleActivated(int)), this, SLOT(onRuleActivated(int)));
	connect(&_message_area, SIGNAL(domainActivated(QString)), this, SLOT(onDomainActivated(QString)));
	connect(&_message_area, SIGNAL(programActivated(QString)), this, SLOT(onApplicationActivated(QString)));
	connect(&_message_area, SIGNAL(ruleActivated(int)), this, SLOT(onRuleActivated(int)));

	//Settings dialog handlers
	connect(&_settings, SIGNAL(dockSettingChanged()), &_browser_dock, SLOT(onUpdateDock()));
	connect(&_settings, SIGNAL(dockSettingChanged()), &_domain_dock, SLOT(onUpdateDock()));
	connect(&_settings, SIGNAL(dockSettingChanged()), &_transition_dock, SLOT(onUpdateDock()));
	connect(&_settings, SIGNAL(drawingSettingChanged()), &_transition_dock, SLOT(onUpdateDock()));
	connect(&_settings, SIGNAL(drawingSettingChanged()), ui->graphicsView, SLOT(updateView()));
	connect(&_settings, SIGNAL(viewSettingChanged()), ui->graphicsView, SLOT(updateView()));
	connect(&_settings, SIGNAL(warningSettingsChanged()), &stack, SLOT(onUpdateWarnings()));
	connect(&_settings, SIGNAL(drawingSettingChanged()), &_caption, SLOT(updateViews()));
	connect(&_settings, SIGNAL(viewSettingChanged()), &_caption, SLOT(updateViews()));
}

MainWindow::~MainWindow()
{
    delete ui;

	for (int i=0; i<MaxRecentFiles; ++i)
		delete _recent_file_acts[i];
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	//Save programs
	ConfigurationStack::instance().savePrograms();

	bool decision=false;

	//Ask if the current transitions file should be saved
	if(ConfigurationStack::instance().hasChanges())
	{
		if(ConfigurationStack::instance().proposeSaveTransitions())
			decision=true;
	}
	else
		decision=true;

	//Save graphics settings before closing the window
	if(decision)
	{
		QSettings().setValue("ui_displayBrowser", _browser_dock.isVisible());
		QSettings().setValue("ui_displayMessageArea", _message_area.isVisible());
		event->accept();
	}
	else
		event->ignore();
}

void MainWindow::updateRecentFilesMenu()
{
	QStringList files=QSettings().value("recentTransitionFiles").toStringList();

	//Remove files that don't exist anymore
	int i=0;
	while(i<files.size())
	{
		if (!QFileInfo(files.at(i)).exists())
			files.removeAt(i);
		else
			++i;
	}

	int numRecentFiles=qMin(files.size(), (int)MaxRecentFiles);

	for(int i=0; i<numRecentFiles; ++i)
	{
		QString text=QString("&%1 %2").arg(i+1).arg(QFileInfo(files[i]).fileName());

		_recent_file_acts[i]->setText(text);
		_recent_file_acts[i]->setToolTip(files[i]);
		_recent_file_acts[i]->setData(files[i]);
		_recent_file_acts[i]->setVisible(true);
	}
	for (int j=numRecentFiles; j<MaxRecentFiles; ++j)
		_recent_file_acts[j]->setVisible(false);

	_recent_file_separator->setVisible(numRecentFiles > 0);
}

void MainWindow::addRecentFileEntry(QString path)
{
	QStringList files=QSettings().value("recentTransitionFiles").toStringList();

	files.removeAll(path);
	files.prepend(path);
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	QSettings().setValue("recentTransitionFiles", files);
	updateRecentFilesMenu();
}

void MainWindow::onApplicationActivated(QString name)
{
	_app_dock.setProgram(ConfigurationStack::instance().current().getProgramByName(name));
	removeDockWidget(&_absrule_dock);
	removeDockWidget(&_domain_dock);
	removeDockWidget(&_transition_dock);
	addDockWidget(Qt::RightDockWidgetArea, &_app_dock, Qt::Vertical);
	_app_dock.show();

	ui->graphicsView->unhighlight();
}

void MainWindow::onDomainActivated(QString name)
{
	_domain_dock.setDomain(ConfigurationStack::instance().current().getDomainByName(name));
	removeDockWidget(&_absrule_dock);
	removeDockWidget(&_app_dock);
	removeDockWidget(&_transition_dock);
	addDockWidget(Qt::RightDockWidgetArea, &_domain_dock, Qt::Vertical);
	_domain_dock.show();

	connect(&_domain_dock, SIGNAL(dockClosed()), this, SLOT(onDockClosed()));
	ui->graphicsView->highlightDomain(name);
}

void MainWindow::onRuleActivated(int id)
{
	_absrule_dock.setRule(id);
	removeDockWidget(&_app_dock);
	removeDockWidget(&_domain_dock);
	removeDockWidget(&_transition_dock);
	addDockWidget(Qt::RightDockWidgetArea, &_absrule_dock, Qt::Vertical);
	_absrule_dock.show();

	connect(&_absrule_dock, SIGNAL(dockClosed()), this, SLOT(onDockClosed()));
	ui->graphicsView->highlightRule(id);
}

void MainWindow::onTransitionArrowActivated(QString appName, QString source, QString dest)
{
	const Configuration &inst=ConfigurationStack::instance().current();

	_transition_dock.setAllProperties(inst.getProgramByName(appName, true),
									  inst.getDomainByName(source),
									  inst.getDomainByName(dest));
	removeDockWidget(&_absrule_dock);
	removeDockWidget(&_app_dock);
	removeDockWidget(&_domain_dock);
	addDockWidget(Qt::RightDockWidgetArea, &_transition_dock, Qt::Vertical);
	_transition_dock.show();

	connect(&_transition_dock, SIGNAL(dockClosed()), this, SLOT(onDockClosed()));
	ui->graphicsView->highlightTransitionArrow(source, dest);
}

void MainWindow::onDockClosed()
{
	ui->graphicsView->unhighlight();
	disconnect(this, SLOT(onDockClosed()));
}

void MainWindow::onNewConfiguration()
{
	removeDockWidget(&_absrule_dock);
	removeDockWidget(&_app_dock);
	removeDockWidget(&_domain_dock);
	removeDockWidget(&_transition_dock);
	onConfigurationSaved();
}

void MainWindow::onConfigurationStackChanged()
{
	ui->actionUndo->setEnabled(ConfigurationStack::instance().canUndo());
	ui->actionRedo->setEnabled(ConfigurationStack::instance().canRedo());
	onConfigurationChanged();
}

void MainWindow::onConfigurationChanged()
{
	if(ConfigurationStack::instance().hasChanges())
		setWindowTitle(MainWindowName + " - *" + ConfigurationStack::instance().transitionsFilePath("Untitled"));
	else
		setWindowTitle(MainWindowName + " - " + ConfigurationStack::instance().transitionsFilePath("Untitled"));
}

void MainWindow::onConfigurationSaved()
{
	//If there is a filename, then this is an opened or saved file, add it to the recent files list
	QString path=ConfigurationStack::instance().transitionsFilePath(QString());
	if(path!=QString())
	{
		addRecentFileEntry(path);
		setWindowTitle(MainWindowName + " - " + ConfigurationStack::instance().transitionsFilePath("Untitled"));
	}
	else
		setWindowTitle(MainWindowName + " - Untitled");
}

void MainWindow::onImportApplicationsClicked()
{
	QStringList names=QFileDialog::getOpenFileNames(this, "Import programs...",
													EditorConfig::instance().lastOpenedPath(),
													"All files (*);;XML files (*.xml)");

	if(names.size()>0)
	{
		EditorConfig::setLastOpenedPath(names.at(0));
		ConfigurationStack::instance().addProgramsFromFiles(names);
	}
}

void MainWindow::onExportAllClicked()
{
	QString path=QFileDialog::getExistingDirectory(this, "Export all...", EditorConfig::lastSavedPath());

	//If the dialog was accepted, export everything
	if(path!=QString())
	{
		EditorConfig::setLastSavedPath(path);

		//Check if the user can write in the chosen directory
		if(QFileInfo(path).isWritable())
		{
			const Configuration &inst=ConfigurationStack::instance().current();

			//Export transitions.xml
			QFile file(path+"/transitions.xml");

			//Open the file in write mode
			if(file.open(QIODevice::WriteOnly))
			{
				//Write the data
				if(!file.write(qPrintable(inst.retrieveTransitionsDOM())))
				{
					QMessageBox::critical(this, "Error",
												"<b>Could not export transitions file."
												".</b><br /><br />This may happen if there is no space left on your hard drive.",
												QMessageBox::Ok);
				}
			}

			//Create the applications subdirectory
			if(!QDir(path).mkdir("program.d"))
			{
				QMessageBox::critical(this, "Error",
											"<b>Could not create applications subdirectory."
											".</b><br /><br />This may happen if there is no space left on your hard drive.",
											QMessageBox::Ok);
			}
			else
			{
				//Export every application
				QList<Program> apps=inst.programList();
				for(int i=0; i<apps.size(); ++i)
				{
					//Fetch the program for each name in the list
					const Program &p=apps.at(i);

					if(p.isValid())
					{
						QFile file(path+"/program.d/"+p.name()+".xml");

						//Open the file in write mode
						if(file.open(QIODevice::WriteOnly))
						{
							//Write the data
							if(!file.write(qPrintable(p.retrieveDOM())))
							{
								QMessageBox::critical(this, "Error",
															"<b>Could not export application " + p.displayName() +
															".</b><br /><br />This may happen if there is no space left on your hard drive.",
															QMessageBox::Ok);
							}
						}
					}
				}
			}
		}
		else
			QMessageBox::critical(this, "Error",
										"<b>Could not export to " + path +
										".</b><br /><br />You do not have the sufficient rights to write to this location.",
										QMessageBox::Ok);
	}
}

void MainWindow::onExportApplicationsClicked()
{
	AppExportDialog::doExport(this);
}

void MainWindow::onRecentFileClicked()
{
	QAction *action = qobject_cast<QAction *>(sender());

	if(action)
		ConfigurationStack::instance().openTransitionsFile(action->data().toString());
}

void MainWindow::onSettings()
{
	if(_settings.isHidden())
	{
		_settings.show();
		_settings.exec();
	}
	else
	{
		_settings.activateWindow();
		_settings.raise();
	}
}

void MainWindow::onCaption()
{
	if(_caption.isHidden())
	{
		_caption.show();
		_caption.updateViews();
		_caption.exec();
	}
	else
	{
		_caption.activateWindow();
		_caption.raise();
	}
}
