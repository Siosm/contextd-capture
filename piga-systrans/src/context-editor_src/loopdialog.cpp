#include "configurationstack.h"
#include "loopdialog.h"
#include "ui_abstractruledialog.h"

LoopDialog::LoopDialog(QWidget *parent) :
		AbstractRuleDialog(parent)
{
	initDialog();
	this->setWindowTitle("New Loop");
}

LoopDialog::LoopDialog(const Loop &subject, QWidget *parent) :
		AbstractRuleDialog(subject, parent)
{
	initDialog();
	this->setWindowTitle("Edit Loop");
}

void LoopDialog::initDialog()
{
	//Adapt labels to this dialog
	ui->infoGroupBox->setTitle("Loop Information");
	ui->notifyCheckBox->setText("Notify Loop (default)");
}


void LoopDialog::editLoop(int id)
{
	//Create a dialog for this loop
	Loop loop = ConfigurationStack::instance().current().findLoop(id);
	LoopDialog dialog(loop);

	dialog.show();
	dialog.exec();

	//Edited the Loop, update our copy of it
	if(dialog.result()==Edited)
	{
		loop.setDisplayName(dialog.displayName());
		loop.setProgram(dialog.program());
		loop.setNotify(dialog.notify());
		loop.setPrompt(dialog.prompt());

		loop.clearTRules();
		QList<TRule> trules = dialog.domains();
		for (int i=0; i<trules.size(); ++i)
			loop.addTRule(trules.at(i));

		loop.clearMRules();
		QList<MRule> mrules = dialog.matchingRules();
		for (int i=0; i<mrules.size(); ++i)
			loop.addMRule(mrules.at(i));

		//Update the Loop object in the configuration
		ConfigurationStack::instance().currentForModifying().updateLoop(loop);
	}
	//Deleted the Loop
	else if(dialog.result()==Deleted)
		ConfigurationStack::instance().currentForModifying().deleteLoop(loop);
}
