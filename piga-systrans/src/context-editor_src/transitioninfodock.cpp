#include <QGraphicsItem>
#include <QTextDocument>
#include <assert.h>
#include "configurationstack.h"
#include "editorconfig.h"
#include "graphicsarrow.h"
#include "graphicsdomain.h"
#include "loopdialog.h"
#include "ruledialog.h"
#include "transitioninfodock.h"
#include "ui_transitioninfodock.h"

TransitionInfoDock::TransitionInfoDock(QWidget *parent) :
	EditorDock(parent),
	ui(new Ui::TransitionInfoDock),
	_scene(new QGraphicsScene)
{
    ui->setupUi(this);

	ConfigurationStack &stack=ConfigurationStack::instance();

	//Connect buttons
	connect(ui->abstractRuleDeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
	connect(ui->abstractRuleEditButton, SIGNAL(clicked()), this, SLOT(onEditClicked()));
	connect(ui->abstractRuleList, SIGNAL(itemSelectionChanged()), this, SLOT(onListSelectionChanged()));
	connect(ui->abstractRuleList, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onItemActivated(QTreeWidgetItem*,int)));

	//Watch changes of the configuration instance
	connect(&stack, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(domainChanged(QString,QString)), this, SLOT(onDomainChanged(QString,QString)));
	connect(&stack, SIGNAL(allRulesChanged()), this, SLOT(onRulesChanged()));
	connect(&stack, SIGNAL(loopsChanged()), this, SLOT(onRulesChanged()));
	connect(&stack, SIGNAL(rulesChanged()), this, SLOT(onRulesChanged()));
	connect(&stack, SIGNAL(warningsChanged()), this, SLOT(onRulesChanged()));
	connect(&stack, SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramChanged(QString,QString)));
	onListSelectionChanged();

	//Resizing and sorting of the tree widget
	ui->abstractRuleList->sortByColumn(0, Qt::AscendingOrder);
	ui->abstractRuleList->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->abstractRuleList->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->abstractRuleList->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui->abstractRuleList->header()->setResizeMode(3, QHeaderView::ResizeToContents);

	//Create the graphics scene in which we'll draw the domains and app involved in the transition
	ui->graphicsView->setScene(_scene);
	ui->graphicsView->setInteractive(false);
}

TransitionInfoDock::~TransitionInfoDock()
{
    delete ui;
	delete _scene;
}

void TransitionInfoDock::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool TransitionInfoDock::setProgram(Program prog)
{
	_program = prog;
	if(isValid())
		updateDock();

	return true;
}

bool TransitionInfoDock::setSourceDomain(Domain dom)
{
	if(dom!=Domain())
	{
		_source=dom;
		if(isValid())
			updateDock();

		return true;
	}
	else
	{
		resetDock();
		return false;
	}
}

bool TransitionInfoDock::setTargetDomain(Domain dom)
{
	if(dom!=Domain())
	{
		_target=dom;
		if(isValid())
			updateDock();

		return true;
	}
	else
	{
		resetDock();
		return false;
	}
}

bool  TransitionInfoDock::setAllProperties(Program prog, Domain src, Domain dest)
{
	//If both domains are valid, then the dock is valid
	if((src!=Domain()) && (dest!=Domain()))
	{
		_program=prog;
		_source=src;
		_target=dest;

		updateDock();
		return true;
	}
	else
	{
		resetDock();
		return false;
	}
}

Program TransitionInfoDock::program() const
{
	return _program;
}

Domain TransitionInfoDock::sourceDomain() const
{
	return _source;
}

Domain TransitionInfoDock::targetDomain() const
{
	return _target;
}

bool TransitionInfoDock::isValid() const
{
	return ((_source!=Domain()) && (_target!=Domain()));
}

void TransitionInfoDock::addAbstractRuleToList(AbstractRulePtr rule, const TRule &trule)
{
	assert(!rule.isNull());

	//Create and fill the new tree item with the rule information;
	QStringList itemStrList(rule->displayName());
	itemStrList.append(rule->ruleType());
	QTreeWidgetItem *item = new QTreeWidgetItem(ui->abstractRuleList, itemStrList);
	item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	item->setData(0, Qt::UserRole, rule->id());

	//If the rule has warnings, display a warning icon
	ConfigurationStack &stack=ConfigurationStack::instance();
	if(stack.ruleHasWarnings(rule->id()) && (stack.warningTypesForRuleAndSourceDomain(rule->id(), _source.name())!=InvalidType))
		item->setIcon(0, QIcon(":/icons/warning"));

	//Setup the notify and prompt checkboxes for the new item
	if(trule.hasNotify())
		item->setCheckState(2, trule.notify() ? Qt::Checked : Qt::Unchecked);
	else
		item->setCheckState(2, rule->notify() ? Qt::Checked : Qt::Unchecked);

	if(trule.hasPrompt())
		item->setCheckState(3, trule.prompt() ? Qt::Checked : Qt::Unchecked);
	else
		item->setCheckState(3, rule->prompt() ? Qt::Checked : Qt::Unchecked);

	//Add the matching rules of the rule as child widgets
	QVector<MRule> mrules = rule->mRules();
	QVector<MRule>::const_iterator it;

	for(it=mrules.begin(); it!=mrules.end(); ++it)
	{
		const MRule &mrule = *it;

		QTreeWidgetItem *mruleItem = new QTreeWidgetItem(item, QStringList(mrule.displayName()));
		mruleItem->setFirstColumnSpanned(true);

		const QMap<QString, QString> &map = mrule.conditions();
		QMap<QString, QString>::const_iterator condit;

		for(condit=map.begin(); condit!=map.end(); ++condit)
		{
			QStringList childStrList(condit.key());
			childStrList.append(condit.value());
			QTreeWidgetItem *condItem = new QTreeWidgetItem(mruleItem, childStrList);
			condItem->setToolTip(0, condit.key());
			condItem->setToolTip(1, condit.value());
		}
	}
}

void TransitionInfoDock::updateDock()
{
	//Update the graphics scene
	ui->graphicsView->setRenderHint(QPainter::Antialiasing, !EditorConfig::fastDraw());
	ui->graphicsView->scene()->clear();
	ui->graphicsView->centerOn(0, 0);

	//Create a source domain circle, a target domain one and an arrow
	GraphicsDomain *sourceGDom=new GraphicsDomain(_source, -100, 0, ui->graphicsView->scene());
	GraphicsDomain *targetGDom=new GraphicsDomain(_target, 100, 0, ui->graphicsView->scene());
	GraphicsArrow *arrow=new GraphicsArrow(sourceGDom, targetGDom, ui->graphicsView->scene(), EditorConfig::HorizontalIcons);
	arrow->makeLinePath();
	arrow->addProgram(_program.name(), true);

	//Clear old loops and rules in the tree widget
	ui->abstractRuleList->clear();

	//Get all the existing abstract rules
	const Configuration &inst = ConfigurationStack::instance().current();
	QVector<AbstractRulePtr> abstracts=inst.abstractRulesForProgram(_program.name());
	QVector<MRule> mrules;

	//Then for each one whose trules interest us, call addAbstractRule
	for(int i=0; i<abstracts.size(); ++i)
	{
		AbstractRulePtr &abstract=abstracts[i];
		RulePtr rule=abstract.dynamicCast<Rule>();

		//Source domain checking is done below so we can accept loops so far, and only check target domain for rules
		if((rule.isNull()) || (rule->transitTo()==_target))
		{
			const QVector<TRule> &trules=abstract->tRules();
			bool found=false;

			for(int i=0; !found && i<trules.size(); ++i)
			{
				if(trules[i].matches(_source.name()))
				{
					mrules << abstract->mRules();
					addAbstractRuleToList(abstract, trules[i]);
					found=true;
				}
			}
		}
	}

	//Hint to optimize the size of each column before they are rendered
	ui->abstractRuleList->header()->resizeSections(QHeaderView::ResizeToContents);
}

void TransitionInfoDock::resetDock()
{
	ui->graphicsView->scene()->clear();
	ui->abstractRuleList->clear();

	//Avoids useless scrollbars
	ui->abstractRuleList->header()->resizeSections(QHeaderView::ResizeToContents);
}

void TransitionInfoDock::onUpdateDock()
{
	updateDock();
}

void TransitionInfoDock::onDeleteClicked()
{
	QTreeWidgetItem *item = ui->abstractRuleList->selectedItems().first();
	assert(item!=NULL);

	while (item->parent())
		item=item->parent();

	int id = item->data(0, Qt::UserRole).toInt();
	QString type = item->text(1);

	if(type == Loop().ruleType())
		ConfigurationStack::instance().currentForModifying().deleteLoop(id);
	else if(type == Rule().ruleType())
		ConfigurationStack::instance().currentForModifying().deleteRule(id);
}

void TransitionInfoDock::onItemActivated(QTreeWidgetItem *item, int /*column*/)
{
	while (item->parent())
		item=item->parent();

	emit ruleActivated(item->data(0, Qt::UserRole).toInt());
}

void TransitionInfoDock::onEditClicked()
{
	QTreeWidgetItem *item = ui->abstractRuleList->selectedItems().first();
	assert(item!=NULL);

	while (item->parent())
		item=item->parent();

	int id=item->data(0, Qt::UserRole).toInt();
	QString type = item->text(1);

	if(type == Loop().ruleType())
		LoopDialog::editLoop(id);
	else if(type == Rule().ruleType())
		RuleDialog::editRule(id);
}

void TransitionInfoDock::onListSelectionChanged()
{
	//Check that an item is selected before allowing deletion
	bool valid=ui->abstractRuleList->selectedItems().size() > 0;
	ui->abstractRuleDeleteButton->setEnabled(valid);
	ui->abstractRuleEditButton->setEnabled(valid);
}

void TransitionInfoDock::onConfigurationChanged()
{
	const Configuration &inst=ConfigurationStack::instance().current();

	if(isValid())
		setAllProperties(inst.getProgramByName(_program.name()),
						 inst.getDomainByName(_source.name()),
						 inst.getDomainByName(_target.name()));
}

void TransitionInfoDock::onDomainChanged(QString old_name, QString new_name)
{
	if(_source.name() == old_name)
		setSourceDomain(ConfigurationStack::instance().current().getDomainByName(new_name));

	if(_target.name() == old_name)
		setTargetDomain(ConfigurationStack::instance().current().getDomainByName(new_name));
}

void TransitionInfoDock::onRulesChanged()
{
	if(isValid())
		updateDock();
	else
		resetDock();
}

void TransitionInfoDock::onProgramChanged(QString old_name, QString new_name)
{
	//The current (unknown) program was added to Editor, fetch it
	if(old_name==QString() && _program.name()==new_name)
		setProgram(ConfigurationStack::instance().current().getProgramByName(new_name));

	//The current (known) program was deleted from Editor, fetch the unknown version
	else if(new_name==QString() && _program.name()==old_name)
		setProgram(ConfigurationStack::instance().current().getProgramByName(old_name, true));

	//The current program has changed, fetch the new version
	else if(_program.name()==old_name)
		setProgram(ConfigurationStack::instance().current().getProgramByName(new_name));
}
