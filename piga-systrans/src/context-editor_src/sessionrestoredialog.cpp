#include <QAbstractButton>
#include <QFileDialog>
#include "configurationstack.h"
#include "editorconfig.h"
#include "sessionrestoredialog.h"

SessionRestoreDialog::SessionRestoreDialog(QString tmp_file_path, QWidget *parent) :
	QMessageBox(Question, "Session Restore", "Context Editor previously crashed. "
				"A backup file of your last session is available. What do you want to do?",
				Discard | Save | Yes, parent),
	_tmp_file_path(tmp_file_path)
{
	//Setup buttons (these three are used because they have different roles)
	setEscapeButton(Discard);
	setDefaultButton(Yes);
	setButtonText(Discard, "&Ignore");
	setButtonText(Save, "&Move File To");
	setButtonText(Yes, "&Restore Session");

	//Connect each button to its own handler
	connect(button(Discard), SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
	connect(button(Save), SIGNAL(clicked()), this, SLOT(onMoveClicked()));
	connect(button(Yes), SIGNAL(clicked()), this, SLOT(onRestoreClicked()));
}

void SessionRestoreDialog::onDeleteClicked()
{
	//Remove the file and reject the dialog (so that the session is not restored)
	QDir().remove(_tmp_file_path);
	reject();
}

void SessionRestoreDialog::onMoveClicked()
{
	QString fileLocation=QFileDialog::getSaveFileName(this, "Move file to...", EditorConfig::lastSavedPath());

	//If a path has been selected, try to move the file, and then reject the dialog, otherwise do nothing
	if(fileLocation!=QString())
	{
		EditorConfig::setLastSavedPath(fileLocation);
		//If the selected path is the same than the existing file's one, there is nothing to do
		if(_tmp_file_path.compare(fileLocation) == 0)
			reject();

		bool failed=false;

		//Try to rename with QDir, might fail if partitions are different
		if(!QDir().rename(_tmp_file_path, fileLocation))
		{
			//If it failed, copy the file manually and then delete the old file
			QFile oldFile(_tmp_file_path);
			QFile newFile(fileLocation);

			if(!oldFile.open(QFile::ReadOnly) || !newFile.open(QFile::WriteOnly))
			{
				failed=true;
			}
			else
			{
				//Do the actual copy
				char buffer[16000];
				int len;

				while((len=oldFile.read(buffer, 16000)) > 0)
				{
					newFile.write(buffer, len);
				}

				//If it failed, notify it, else remove the old file
				if(!oldFile.atEnd())
					failed=true;
				else
					QDir().remove(_tmp_file_path);
			}
		}

		if(failed)
			QMessageBox::critical(this, "Could not move file", QString("<b>An error occurred while moving the backup file.</b><br /><br />The file is still available on your filesystem at '%1'.").arg(_tmp_file_path), QMessageBox::Ok);

		reject();
	}
}

void SessionRestoreDialog::onRestoreClicked()
{
	ConfigurationStack::instance().openTmpTransitions(_tmp_file_path);
	QDir().remove(_tmp_file_path);
	accept();
}
