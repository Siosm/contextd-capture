#include <QFileDialog>
#include <QMessageBox>
#include "configurationstack.h"
#include "editorconfig.h"
#include "messagearea.h"
#include "ui_messagearea.h"

MessageArea::MessageArea(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::MessageArea)
{
	ui->setupUi(this);
	resize(sizeHint());

	connect(&ConfigurationStack::instance(), SIGNAL(warningsChanged()), this, SLOT(onWarningsChanged()));
	onWarningsChanged();

//	QList<int> intList;
//	intList << 4;	intList << 8;
//	QMap<QString, QList<int> > mrules;
//	mrules["MRule Alpha"]=intList;
//	mrules["MRUle Beta :D"]=intList;

//	addRuleWarning(BypassRuleWarning(2, Domain("impots", "Impots", "", "", 0, 20), mrules));
//	addRuleWarning(UnreachableRuleWarning(11));
//	addRuleWarning(UnknownProgramRuleWarning(6));
//	addRuleWarning(InvalidMRuleRuleWarning(11));
//	addRuleWarning(UndefinedMRuleRuleWarning(22));
//	addRuleWarning(TargetLessConfidentialRuleWarning(5));
//	addRuleWarning(TargetLessReliableRuleWarning(18));
}

MessageArea::~MessageArea()
{
	delete ui;
}

void MessageArea::clear()
{
	QLayoutItem *item;
	while((item=ui->formLayout->itemAt(0)) != 0)
	{
		ui->formLayout->removeItem(item);
		delete item->widget();
		delete item;
	}

	_text_labels.clear();
}

void MessageArea::addWarning(const Warning &warn)
{
	QLabel *icon=new QLabel();
	icon->setPixmap(QPixmap(warn.icon()));

	QLabel *text=new QLabel(warn.text());
	connect(text, SIGNAL(linkActivated(QString)), this, SLOT(onLinkActivated(QString)));
	text->setTextInteractionFlags(Qt::TextBrowserInteraction);
	text->setWordWrap(true);

	ui->formLayout->insertRow(0, icon, text);
	_text_labels.prepend(text);
}

void MessageArea::changeEvent(QEvent *e)
{
    QScrollArea::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QSize MessageArea::sizeHint() const
{
	//Take the real size hint
	QSize size=QScrollArea::sizeHint();

	//Calculate the size of an item in the area
	int itemSize=QLabel().margin()*2+32+ui->formLayout->verticalSpacing();

	//Set the height necessary for displaying 3 items
	size.setHeight(3*itemSize+ui->formLayout->margin()*2-ui->formLayout->verticalSpacing());

	return size;
}

void MessageArea::onLinkActivated(QString uri)
{
	if(uri.startsWith("rule://"))
	{
		bool ok;
		int id=uri.remove(0, 7).toInt(&ok);
		if(ok)
			emit ruleActivated(id);
	}
	else if(uri.startsWith("dom://"))
		emit domainActivated(uri.remove(0, 6));
	else if(uri.startsWith("prog://"))
		emit programActivated(uri.remove(0, 7));
}

void MessageArea::onWarningsChanged()
{
	clear();

	QVector<WarningPtr> warnings=ConfigurationStack::instance().allWarnings();

	for(int i=0; i<warnings.size(); ++i)
	{
		addWarning(*warnings[i]);
	}
}

void MessageArea::onExportToText()
{
	//Ask where to save the graph
	QString path=QFileDialog::getSaveFileName(this, "Export messages to text...", EditorConfig::lastSavedPath(),
											  "Text files (*.txt);;All files (*)");

	//Abort if the user leaves the file dialog
	if(path==QString())
		return;
	else
		EditorConfig::setLastSavedPath(path);

	//Check that writing is possible before going further
	if(!QFileInfo(QFileInfo(path).dir().path()).isWritable())
	{
		QMessageBox::critical(this, "Error", "<b>Could not export messages to " + path +
						   ".</b><br /><br />You do not have the sufficient rights to write to this location.",
						   QMessageBox::Ok);
		return;
	}

	//Write the file header
	QString text="## Context Editor graph messages\n";

	ConfigurationStack &stack=ConfigurationStack::instance();
	if(stack.transitionsFilePath()!=QString())
		text+="## Generated for transitions file '"+stack.transitionsFilePath()+"'\n \n";
	else
		text+="## Unknown transitions file\n\n";

	QRegExp exp("<.*>");
	exp.setMinimal(true);

	//Write messages from last to first
	for(int i=0; i<_text_labels.size(); ++i)
	{
		QLabel *label=_text_labels.at(i);
		if(label)
		{
			QString message=label->text();

			message.replace("<br />", "\n");
			message.replace(exp, "");

			text+=message+"\n \n";
		}
	}

	QFile file(path);
	file.open(QFile::WriteOnly);
	file.write(qPrintable(text));
}
