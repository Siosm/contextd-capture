#include <QIcon>
#include <QRadioButton>
#include <QPushButton>

#include "configurationstack.h"
#include "selectdefaultdomaindialog.h"
#include "ui_selectdefaultdomaindialog.h"

SelectDefaultDomainDialog::SelectDefaultDomainDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::SelectDefaultDomainDialog)
{
    ui->setupUi(this);

	const Configuration &inst=ConfigurationStack::instance().current();
	Domain defaultDomain=ConfigurationStack::instance().current().defaultDomain();

	//Set the label
	ui->label->setText("Please select a new default domain to replace "+defaultDomain.displayName()+" .");

	//Forbid clicking on Ok till a domain has been selected
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	//Fill the list of available domains
	QList<Domain> domains=inst.domainList();
	for(QList<Domain>::const_iterator it=domains.begin(); it!=domains.end(); ++it)
	{
		const Domain &dom=*it;

		if(dom!=defaultDomain)
		{
			QIcon icon(dom.iconPath());
			if(icon.isNull())
				icon.addFile(":/icons/default-domain");

			QListWidgetItem *item=new QListWidgetItem(icon, QString(), ui->listWidget);
			QRadioButton *radio=new QRadioButton(dom.displayName(), ui->listWidget);

			ui->listWidget->setItemWidget(item, radio);
			item->setData(Qt::UserRole, dom.name());
			connect(radio, SIGNAL(clicked()), this, SLOT(onRadioButtonClicked()));

			_map[radio]=item;
		}
	}

	//Make the selection of the list item instead of the radio button trigger the button's selection
	connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
}

SelectDefaultDomainDialog::~SelectDefaultDomainDialog()
{
    delete ui;
}

QString SelectDefaultDomainDialog::newDefaultDomainName() const
{
	return _selected_name;
}

void SelectDefaultDomainDialog::changeEvent(QEvent *e)
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

void SelectDefaultDomainDialog::onItemSelectionChanged()
{
	QList<QListWidgetItem *> items=ui->listWidget->selectedItems();

	if(!items.isEmpty())
	{
		QRadioButton *radio=dynamic_cast<QRadioButton *>(ui->listWidget->itemWidget(items.at(0)));

		if(radio)
			radio->setChecked(true);
	}
}

void SelectDefaultDomainDialog::onRadioButtonClicked()
{
	QRadioButton *radio=qobject_cast<QRadioButton *>(sender());
	if(radio)
	{
		QListWidgetItem *item=_map[radio];
		_selected_name=item->data(Qt::UserRole).toString();
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
}
