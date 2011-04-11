#include "changecontextdialog.h"
#include "domain.h"
#include <QMessageBox>
#include <QPushButton>

ChangeContextDialog::ChangeContextDialog(QWidget *parent) :
    QDialog(parent)
{
	setupUi(this);
	
	updateContextList();

	connect(this, SIGNAL(accepted()), this, SLOT(accept()));
}

QString ChangeContextDialog::selectedContext()
{
	return wantedContext;
}

void ChangeContextDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
	   retranslateUi(this);
	   break;
    default:
	   break;
    }
}

void ChangeContextDialog::setVisible(bool visible)
{
	if(visible)
	{
		updateContextList();
		domainUpdated(QString(), clist.currentDomain().domain_name);
	}

	QDialog::setVisible(visible);
}

void ChangeContextDialog::setSelectedContext(QString context)
{
	//Disconnect so as we can select the right context
	disconnect(this->contextCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(comboChanged(QString)));

	contextCombo->setCurrentIndex(contextCombo->findData(context));

	//Re-Connect
	connect(this->contextCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(comboChanged(QString)));
}


void ChangeContextDialog::updateContextList()
{
	//Save the current item
	QString previous=contextCombo->currentText();

	//Get the list of the available contexts
    QVector<DomainList::Domain_p> contexts=clist.domainList();

	//Empty the combo
	contextCombo->clear();
	contextCombo->addItem(QIcon(),tr("-- Available transitions --"), "");

	//Fill the combo, first with the available items
	for(int i=0;i<contexts.size();i++)
	{
		if(contexts[i].reachable && contexts[i].domain_name!="")
		{
			contextCombo->addItem(QIcon(contexts[i].icon),
							  contexts[i].display_name,
							  contexts[i].domain_name);
		}
	}

	//Fill the combo, now with the others
	contextCombo->addItem(QIcon(),tr("-- Other transitions --"), "");
	indexNotAvailable=contextCombo->count()-1;

	for(int i=0;i<contexts.size();i++)
	{
		if(!contexts[i].reachable && contexts[i].domain_name!="")
		{
			contextCombo->addItem(QIcon(contexts[i].icon),
							  contexts[i].display_name,
							  contexts[i].domain_name);
		}
	}

	//Select the current context
    setSelectedContext(clist.currentDomain().domain_name);

	//Save the current
	wantedContext=contextCombo->itemData(contextCombo->currentIndex()).toString();
}

void ChangeContextDialog::comboChanged(QString /*new_context*/)
{
	if(contextCombo->count()>0)
	{
		unsigned int c_index=contextCombo->currentIndex();
		QString c_itemData=contextCombo->itemData(c_index).toString();
	
		if(c_index>0 && c_index<indexNotAvailable)
		{
			//Get the current item data and store it
			wantedContext=c_itemData;
	
			buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		}
		else if(c_index>indexNotAvailable)
		{
			QMessageBox::critical(this,
							  tr("Error : The transition to the context '%1' is not allowed").arg(c_itemData),
							  tr("Transiting from your current context to the context '%1' is not allowed by the current set of  rule").arg(c_itemData)
							  );

			//Return to the current context
            setSelectedContext(clist.currentDomain().domain_name);
		}
		else
		{
			buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		}
	}
}

void ChangeContextDialog::domainUpdated(QString /*app*/, QString newDomain)
{
	DomainList::Domain_p currentContext=clist.getDomainByName(newDomain, newDomain);

	//Update the current context
	QString text=tr("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
"p, li { white-space: pre-wrap; }"
"</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Your current domain is '<span style=\" font-size:9pt; font-weight:600;\">%1</span>'</p></body></html>");

	this->lblExecContext->setText(text.arg(currentContext.display_name));
}

void ChangeContextDialog::accept()
{
	emit domainChanged(wantedContext);
	hide();
}


