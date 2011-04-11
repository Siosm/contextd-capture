#include "appexportdialog.h"
#include "configurationstack.h"
#include "ui_appexportdialog.h"
#include "editorconfig.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QFileInfo>

AppExportDialog::AppExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppExportDialog)
{
    ui->setupUi(this);

	//Connect widgets
	connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(onSelectAllClicked()));
	connect(ui->unselectButton, SIGNAL(clicked()), this, SLOT(onUnselectAllClicked()));
	connect(ui->togglebutton, SIGNAL(clicked()), this, SLOT(onToggleClicked()));
	connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
	connect(ui->pathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->appsList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onMandatoryFieldsChanged()));
	onMandatoryFieldsChanged();

	//Setup the applications list
	ui->appsList->clear();

	QList<Program> list = ConfigurationStack::instance().current().programList();
	QList<Program>::const_iterator it;

	for (it=list.begin(); it!=list.end(); ++it) {
		const Program &prog = *it;

		QPixmap icon(prog.icon());
		if(icon.isNull())
			icon.load(":/icons/default-program");

		QListWidgetItem *item = new QListWidgetItem(QIcon(icon), prog.displayName(), ui->appsList);
		item->setData(Qt::UserRole, prog.name());
		item->setCheckState(Qt::Unchecked);
	}
}

AppExportDialog::~AppExportDialog()
{
    delete ui;
}

QStringList AppExportDialog::selectedApplications() const
{
	QStringList names;

	for(int i=0; i<ui->appsList->count(); ++i)
	{
		QListWidgetItem *item=ui->appsList->item(i);

		if(item->checkState()==Qt::Checked)
			names.append(item->data(Qt::UserRole).toString());
	}

	return names;
}

QString AppExportDialog::exportPath() const
{
	return ui->pathLineEdit->text();
}

void AppExportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AppExportDialog::doExport(QWidget *parent)
{
	//Create an application export dialog
	AppExportDialog dialog(parent);

	dialog.show();
	dialog.exec();

	//If it was accepted, fetch the selected applications
	if(dialog.result()==QDialog::Accepted)
	{
		QString path=dialog.exportPath();

		//Check if the user can write in the chosen directory
		if(QFileInfo(path).isWritable())
		{
			QStringList apps=dialog.selectedApplications();
			for(int i=0; i<apps.size(); ++i)
			{
				//Fetch the program for each name in the list
				Program p=ConfigurationStack::instance().current().getProgramByName(apps.at(i));

				if(p.isValid())
				{
					QFile file(path+"/"+p.name()+".xml");

					//Open the file in write mode
					if(file.open(QIODevice::WriteOnly))
					{
						//Write the data
						if(!file.write(qPrintable(p.retrieveDOM())))
						{
							QMessageBox::critical(parent, "Error",
														"<b>Could not export " + p.displayName() +
														".</b><br /><br />This may happen if there is no space left on your hard drive.",
														QMessageBox::Ok);
						}
					}
				}
			}
		}
		else
			QMessageBox::critical(parent, "Error",
										"<b>Could not export applications to " + path +
										".</b><br /><br />You do not have the sufficient rights to write to this location.",
										QMessageBox::Ok);
	}
}

void AppExportDialog::onSelectAllClicked()
{
	for(int i=0; i<ui->appsList->count(); ++i)
		ui->appsList->item(i)->setCheckState(Qt::Checked);
}

void AppExportDialog::onUnselectAllClicked()
{
	for(int i=0; i<ui->appsList->count(); ++i)
		ui->appsList->item(i)->setCheckState(Qt::Unchecked);
}

void AppExportDialog::onToggleClicked()
{
	for(int i=0; i<ui->appsList->count(); ++i)
	{
		QListWidgetItem *item=ui->appsList->item(i);

		if(item->checkState()==Qt::Checked)
			item->setCheckState(Qt::Unchecked);
		else
			item->setCheckState(Qt::Checked);
	}
}

void AppExportDialog::onBrowseClicked()
{
	//Start directory of the browse dialog
	QString dir;
	if(QFileInfo(ui->pathLineEdit->text()).isDir())
		dir=ui->pathLineEdit->text();
	else
		dir=EditorConfig::lastOpenedPath();

	//Exec the dialog and retrieve its answer
	QString path=QFileDialog::getExistingDirectory(this, "Choose directory", dir);
	if(path!=QString())
	{
		ui->pathLineEdit->setText(path);
		EditorConfig::setLastOpenedPath(path);
	}
}

void AppExportDialog::onMandatoryFieldsChanged()
{
	QFileInfo info(ui->pathLineEdit->text());
	bool valid=info.isDir();

	if(valid)
	{
		valid=false;
		for (int i=0; i<ui->appsList->count() && !valid; ++i)
			valid=ui->appsList->item(i)->checkState()==Qt::Checked;
	}

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}
