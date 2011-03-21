#include <QFileDialog>
#include <QMessageBox>
#include <QWheelEvent>
#include <QMatrix>
#include <QMenu>
#include <math.h>
#include "appdialog.h"
#include "configurationstack.h"
#include "domaindialog.h"
#include "editorconfig.h"
#include "graphicsview.h"
#include "gvgraph.h"
#include "loopdialog.h"
#include "ruledialog.h"
#include "selectdefaultdomaindialog.h"

GraphicsView::GraphicsView(QWidget *parent) :
	QGraphicsView(parent)
{
	setCursor(Qt::OpenHandCursor);
	setAcceptDrops(true);
	setDragMode(QGraphicsView::ScrollHandDrag);
	setScene(new QGraphicsScene());

	ConfigurationStack &stack=ConfigurationStack::instance();
	connect(&stack, SIGNAL(allRulesChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(rulesChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(loopsChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(programChanged(QString,QString)), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(warningsChanged()), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(domainChanged(QString,QString)), this, SLOT(onConfigurationChanged()));
	connect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
	connect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));
	connect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));

	setupMenus();
}

void GraphicsView::updateView()
{
	//Reset the scene
	setRenderHint(QPainter::Antialiasing, !EditorConfig::fastDraw());
	scene()->clear();

	//Clear the domains and arrows
	_domains.clear();
	_arrows.clear();
	unhighlight();

	ConfigurationStack &stack=ConfigurationStack::instance();
	const Configuration &inst=stack.current();

	//Check if items should be hidden because of the hide mode
	bool hideItems=EditorConfig::hideMode()==EditorConfig::RemoveFromView;

	//Populate the domains list
	QList<Domain> domList=inst.domainList();
	for(QList<Domain>::const_iterator it=domList.begin(); it!=domList.end(); ++it)
	{
		const Domain &dom=*it;

		//Create a GraphicsDomain and map its activated signal
		if(!hideItems || !stack.isDomainHidden(dom.name()))
		{
			GraphicsDomain *item=new GraphicsDomain(dom, 0, 0, scene());
			connect(item, SIGNAL(activated(QString)), this, SIGNAL(domainActivated(QString)));
			_domains[dom.name()]=item;
		}
	}

	//Create a graphviz graph and add a node to it for each domain that we have
	GVGraph g("Context Editor", font(), GraphicsDomain::EllipseDiameter, GraphicsArrowProgram::IconSize);
	g.addNodes(_domains.keys());
	g.setRootNode(inst.defaultDomain().name());
	g.setFont(font());

	//Setup a list of all the rules, and go through it
	QVector<Rule> rules=inst.allRules();
	for(int i=0; i<rules.size(); ++i)
	{
		const Rule &rule=rules[i];
		const QStringList &doms=rule.matchedDomainNames(inst.domainNameList());
		QString targetName=rule.transitTo().name();

		//If the rule's target domain is visible or if hidden items are allowed
		if(!hideItems || !stack.isDomainHidden(targetName))
		{
			//For each domain in the rule, check if there is an arrow
			for(int i=0; i<doms.size(); ++i)
			{
				QString domName=doms.at(i);

				//Don't show arrows when one of the domains is hidden or when both domains are identical
				if((domName!=targetName || EditorConfig::displayLoops()) && (!hideItems || !stack.isDomainHidden(domName)))
				{
					GraphicsArrow *arrow=0;
					QPair<QString, QString> key(domName, targetName);

					//If the arrow doesn't exist, create it
					if(!_arrows.contains(key))
					{
						GraphicsDomain *gDom=_domains[domName];
						GraphicsDomain *tDom=_domains[targetName];

						//Make sure both domains are valid before adding an arrow
						if(gDom && tDom)
						{
							arrow=new GraphicsArrow(gDom, tDom, scene());
							connect(arrow, SIGNAL(activated(QString,QString,QString)), this, SIGNAL(transitionArrowActivated(QString,QString,QString)));
							_arrows[key]=arrow;
						}
						else if(!gDom)
							qWarning("GraphicsView: could not find GraphicsDomain for domain %s", qPrintable(domName));
						else
							qWarning("GraphicsView: could not find GraphicsDomain for domain %s", qPrintable(targetName));
					}
					else
						arrow=_arrows[key];

					//If the arrow was properly created, add the rule to it so that it fetches programs and mrules
					if(arrow!=0 && (!hideItems || (!stack.isRuleHidden(rule.id()) && !stack.isProgramHidden(rule.appName()))))
						arrow->addRule(AbstractRulePtr(new Rule(rule)));
				}
			}
		}
	}

	//Add the loops to the arrows too, if allowed by the settings
	if(EditorConfig::displayLoops())
	{
		QVector<Loop> loops=inst.allLoops();
		for(int i=0; i<loops.size(); ++i)
		{
			const Loop &loop=loops[i];
			const QStringList &doms=loop.matchedDomainNames(inst.domainNameList());

			//For each domain in the loop, check if there is an arrow
			for(int i=0; i<doms.size(); ++i)
			{
				QString domName=doms.at(i);

				//Don't show arrows when the domain is hidden
				if(!hideItems || !stack.isDomainHidden(domName))
				{
					GraphicsArrow *arrow=0;
					QPair<QString, QString> key(domName, domName);

					//If the arrow doesn't exist, create it
					if(!_arrows.contains(key))
					{
						GraphicsDomain *gDom=_domains[domName];

						//Make sure the domain is valid before adding an arrow
						if(gDom && gDom)
						{
							arrow=new GraphicsArrow(gDom, gDom, scene());
							connect(arrow, SIGNAL(activated(QString,QString,QString)), this, SIGNAL(transitionArrowActivated(QString,QString,QString)));
							_arrows[key]=arrow;
						}
						else
							qWarning("GraphicsView: could not find GraphicsDomain for domain %s", qPrintable(domName));
					}
					else
						arrow=_arrows[key];

					//If the arrow was properly created, add the loop to it so that it fetches programs and mrules
					if(arrow!=0 && (!hideItems || (!stack.isRuleHidden(loop.id()) && !stack.isProgramHidden(loop.appName()))))
						arrow->addRule(AbstractRulePtr(new Loop(loop)));
				}
			}
		}
	}

	//When in hideItems mode, remove arrows that have no rule/program
	for(QMap<QPair<QString, QString>, GraphicsArrow *>::iterator it=_arrows.begin(); it!=_arrows.end();)
	{
		if(it.value()->programCount()==0)
			it=_arrows.erase(it);
		else
			++it;
	}


	//Add an edge to the graphviz graph for each created GraphicsArrow
	if(EditorConfig::arrowProgramMode()==EditorConfig::Popup)
		for(QMap<QPair<QString, QString>, GraphicsArrow *>::const_iterator it=_arrows.begin(); it!=_arrows.end(); ++it)
			g.addEdge(it.key().first, it.key().second);
	else if(EditorConfig::arrowProgramMode()==EditorConfig::HorizontalIcons)
		for(QMap<QPair<QString, QString>, GraphicsArrow *>::const_iterator it=_arrows.begin(); it!=_arrows.end(); ++it)
			g.addEdge(it.key().first, it.key().second, it.value()->programCount());

	//Apply the dot layout to the graphviz graph for optimal position calculation
	g.applyLayout();

	//Position the nodes in the QGraphicsScene using the coordinates from the graphviz graph
	QList<GVNode> nodes=g.nodes();
	for(int i=0; i<nodes.size(); ++i)
	{
		const GVNode &node=nodes.at(i);
		_domains[node.name]->setCenterPos(node.centerPos);
	}

	//Draw the arrows in the QGraphicsScene using the bezier curves from the graphviz graph
	QList<GVEdge> edges=g.edges();
	for(int i=0; i<edges.size(); ++i)
	{
		const GVEdge &edge=edges.at(i);
		QPair<QString, QString> key(edge.source, edge.target);


		if(_arrows.contains(key))
		{
			_arrows[key]->setPath(edge.path, edge.labelPos);
		}
		else
			qWarning("GraphicsView: could not find GraphicsArrow from %s to %s", qPrintable(edge.source), qPrintable(edge.target));
	}

	//Set the scene's rectangle depending on the graph's one (plus extra margin, especially at bottom for domain labels)
	QRectF rectf=g.boundingRect();
	rectf.setWidth(rectf.width()+50);
	rectf.moveLeft(-25);
	rectf.setHeight(rectf.height()+100);
	rectf.moveTop(-25);

	setSceneRect(rectf);
}

void GraphicsView::highlightDomain(QString name)
{
	unhighlight();

	if(_domains.contains(name))
	{
		_domains[name]->setHighlighted(true);
		ensureVisible(_domains[name]);
		_hl_domains.append(name);
	}
}

void GraphicsView::highlightTransitionArrow(QString source, QString dest)
{
	unhighlight();

	QPair<QString, QString> key(source, dest);

	if(_domains.contains(source))
	{
		_domains[source]->setHighlighted(true);
		_hl_domains.append(source);
	}

	if(_domains.contains(dest))
	{
		_domains[dest]->setHighlighted(true);
		_hl_domains.append(dest);
	}

	if(_arrows.contains(key))
	{
		_arrows[key]->setHighlighted(true);
		ensureVisible(_arrows[key]);
		_hl_arrows.append(key);
	}
}

void GraphicsView::highlightRule(int id)
{
	unhighlight();

	//Find the rule, and use an AbstractRule pointer for code factorisation
	ConfigurationStack &stack=ConfigurationStack::instance();
	AbstractRulePtr abs=stack.findAbstractRule(id);
	RulePtr rule=abs.dynamicCast<Rule>();

	if(!rule.isNull())
	{
		//Highlight the target domain if ruleType is Rule
		if(_domains.contains(rule->transitTo().name()))
		{
			_domains[rule->transitTo().name()]->setHighlighted(true);
			_hl_domains.append(rule->transitTo().name());
			ensureVisible(_domains[rule->transitTo().name()]);
		}
	}

	//Highlight the source domains and arrows
	QStringList domains=abs->matchedDomainNames(stack.current().domainNameList());
	for(int i=0; i<domains.size(); ++i)
	{
		//Highlight source domains
		QString source=domains[i];

		if(_domains.contains(source))
		{
			_domains[source]->setHighlighted(true);
			_hl_domains.append(source);
		}

		//Highlight arrows (self arrow if loop, or to target domain if rule)
		QPair<QString, QString> key(source, source);
		if(!rule.isNull())
			key.second=rule->transitTo().name();

		if(_arrows.contains(key))
		{
			_arrows[key]->setHighlighted(true);
			_hl_arrows.append(key);
		}
	}
}

void GraphicsView::unhighlight()
{
	for(int i=0; i<_hl_domains.size(); ++i)
		if(_domains.contains(_hl_domains.at(i)))
			_domains[_hl_domains.at(i)]->setHighlighted(false);

	_hl_domains.clear();

	for(int i=0; i<_hl_arrows.size(); ++i)
		if(_arrows.contains(_hl_arrows.at(i)))
			_arrows[_hl_arrows.at(i)]->setHighlighted(false);

	_hl_arrows.clear();
}

void GraphicsView::zoom(int mode, qreal value)
{
	static QMatrix matrix(this->matrix());
	static const qreal scales[]={0.125, 0.25, 0.333, 0.5, 0.667, 1, 1.5, 2, 4, 8, 16};
	static const int default_index=5;
	static int scale_index=default_index;

	//If fitting an already calculated value, then update the index and use the value
	if(mode==ZoomBest)
	{
		qreal dif=qAbs(value-scales[0]); int best_index=0;

		for(int i=1; i<10; ++i) //Update this value if you change scales[]
		{
			if(qAbs(value-scales[i])<dif)
			{
				best_index=i;
				dif=qAbs(value-scales[i]);
			}
		}

		scale_index=best_index;

		QMatrix copy=matrix;
		copy.scale(value, value);
		setMatrix(copy);
	}
	//Else update the scale accordingly to the instruction (zoom up/down/original value), and then scale
	else
	{
		if(mode==ZoomOriginal)
			scale_index=default_index;
		else if(mode==ZoomIn && scale_index<10)
			++scale_index;
		else if(mode==ZoomOut && scale_index>0)
			--scale_index;

		QMatrix copy=matrix;
		copy.scale(scales[scale_index], scales[scale_index]);
		setMatrix(copy);
	}
}

void GraphicsView::setupMenus()
{
	//Setup the main view menu
	_menu=new QMenu("View Menu", this);

	//Add actions for creating new items
	QMenu *newAction=_menu->addMenu(QIcon(":/icons/new"), "&New");
	newAction->menuAction()->setIconVisibleInMenu(true);
	newAction->addAction("&Transition rule", this, SIGNAL(newTransition()));
	newAction->addAction("&Domain", this, SIGNAL(newDomain()));
	newAction->addAction("&Program", this, SIGNAL(newProgram()));
	_menu->addSeparator();

	//Add action for showing all the items in the view
	QAction *showAllAction=_menu->addAction(QIcon(":/icons/eye"), "&Show all items", this, SLOT(onShowAllItems()));
	showAllAction->setIconVisibleInMenu(true);
	_menu->addSeparator();

	//Add actions for zoom management
	QAction *zoomInAction=_menu->addAction(QIcon(":/icons/zoom-in"), "Zoom &in", this, SLOT(onZoomIn()), QKeySequence::ZoomIn);
	QAction *zoomOutAction=_menu->addAction(QIcon(":/icons/zoom-out"), "Zoom &out", this, SLOT(onZoomOut()), QKeySequence::ZoomOut);
	QAction *zoomOneAction=_menu->addAction(QIcon(":/icons/zoom-original"), "&Normal size", this, SLOT(onZoomOriginalSize()), QKeySequence(QKeySequence(Qt::CTRL + Qt::Key_0)));
	QAction *zoomBestAction=_menu->addAction(QIcon(":/icons/zoom-fit-best"), "&Best fit", this, SLOT(onZoomFitBest()));
	zoomInAction->setIconVisibleInMenu(true);
	zoomOutAction->setIconVisibleInMenu(true);
	zoomOneAction->setIconVisibleInMenu(true);
	zoomBestAction->setIconVisibleInMenu(true);



	//Setup the menu for domain items
	_domain_menu=new QMenu("Domain Menu", this);

	//Add action for highlighting the domain
	QAction *domainHighlightAction=_domain_menu->addAction("&Show in dock");
	domainHighlightAction->setObjectName("domainHighlightAction");
	domainHighlightAction->setIconVisibleInMenu(true);
	_domain_menu->addSeparator();

	//Add actions for editing and deleting the domain
	QAction *domainEditAction=_domain_menu->addAction(QIcon(":/icons/edit"), "&Edit");
	domainEditAction->setObjectName("domainEditAction");
	domainEditAction->setIconVisibleInMenu(true);

	QAction *domainDeleteAction=_domain_menu->addAction(QIcon(":/icons/delete"), "&Delete");
	domainDeleteAction->setObjectName("domainDeleteAction");
	domainDeleteAction->setIconVisibleInMenu(true);

	_domain_menu->addSeparator();

	//Add actions for showing the domain and focusing on it
	QAction *domainVisibleAction=_domain_menu->addAction("&Visible");
	domainVisibleAction->setObjectName("domainVisibleAction");
	domainVisibleAction->setCheckable(true);

	QAction *domainFocusAction=_domain_menu->addAction(QIcon(":/icons/find"), "&Focus on domain");
	domainFocusAction->setObjectName("domainFocusAction");
	domainFocusAction->setIconVisibleInMenu(true);



	//Setup the menu for program items
	_program_menu=new QMenu("Program Menu", this);

	//Add a menu for showing data in the dock (program and transition info)
	QMenu *programHighlightMenuAction=_program_menu->addMenu("&Show in dock");
	QAction *programHighlightAction=programHighlightMenuAction->addAction("&Program");
	programHighlightAction->setObjectName("programHighlightAction");
	programHighlightMenuAction->addSeparator();

	QAction *rulesHighlightAction=programHighlightMenuAction->addAction("&Transitions");
	rulesHighlightAction->setObjectName("rulesHighlightAction");

	_program_menu->addSeparator();

	//Add a menu for editing the program and individual rules
	QMenu *rulesEditMenuAction=_program_menu->addMenu(QIcon(":/icons/edit"), "&Edit");
	rulesEditMenuAction->menuAction()->setObjectName("rulesEditMenuAction");
	rulesEditMenuAction->menuAction()->setIconVisibleInMenu(true);

	//Add a menu for deleting the program and individual rules
	QMenu *rulesDeleteMenuAction=_program_menu->addMenu(QIcon(":/icons/delete"), "&Delete");
	rulesDeleteMenuAction->menuAction()->setObjectName("rulesDeleteMenuAction");
	rulesDeleteMenuAction->menuAction()->setIconVisibleInMenu(true);
	_program_menu->addSeparator();

	//Add a menu for deleting the program and individual rules
	QMenu *rulesVisibleMenuAction=_program_menu->addMenu("&Visible");
	rulesVisibleMenuAction->menuAction()->setObjectName("rulesVisibleMenuAction");

	QAction *programVisibleAction=rulesVisibleMenuAction->addAction("&Program");
	programVisibleAction->setObjectName("programVisibleAction");
	programVisibleAction->setCheckable(true);
	rulesVisibleMenuAction->addSeparator()->setObjectName("separator");

	_program_menu->addSeparator();

	//Add an action for focusing on the program
	QAction *programFocusAction=_program_menu->addAction(QIcon(":/icons/find"), "&Focus on program");
	programFocusAction->setObjectName("programFocusAction");
	programFocusAction->setIconVisibleInMenu(true);
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	//Fetch the item below the cursor
	QGraphicsItem *item=scene()->itemAt(mapToScene(event->pos()));
	QAction *action;
	GraphicsArrowProgram *progItem=dynamic_cast<GraphicsArrowProgram *>(item);

	if(item)
		while(item->parentItem())
			item=item->parentItem();

	GraphicsDomain *domItem=dynamic_cast<GraphicsDomain *>(item);


	//If the item is a program icon
	if(progItem && (!progItem->isHiddenByUser() || EditorConfig::hideMode()!=EditorConfig::LowerOpacityAndForbidSelection))
	{
		//Build a sorted list of all the rules for this item
		QVector<AbstractRulePtr> abs=progItem->rules();
		qSort(abs.begin(), abs.end(), AbstractRule::hasHigherPriority);

		//Fetch the program matching this item
		const Program &prog=stack.current().getProgramByName(progItem->appName(), true);

		//Set the "Visible" check menu items' value depending on the item
		action=_program_menu->findChild<QAction *>("rulesVisibleMenuAction");
		if(EditorConfig::hideMode()!=EditorConfig::RemoveFromView)
		{
			//Find the program item, and (un)check it, and remove other items
			QList<QAction*> childActions=action->menu()->actions();
			for(int j=0; j<childActions.size(); ++j)
			{
				QAction *childAction=childActions.at(j);
				if(childAction->objectName()=="programVisibleAction")
					childAction->setChecked(!stack.isProgramHidden(progItem->appName()));
				else if(childAction->objectName()!="separator")
					action->menu()->removeAction(childAction);
			}

			//Setup items for each rule/loop
			for(int j=0; j<abs.size(); ++j)
			{
				QAction *child=action->menu()->addAction(abs[j]->displayName());
				child->setCheckable(true);
				child->setChecked(!stack.isRuleHidden(abs[j]->id()));
				child->setObjectName(QString("ruleVisibleAction"));
				child->setData(abs[j]->id());
			}

			action->setVisible(true);
		}
		else
			action->setVisible(false);

		//Setup the edit menu
		action=_program_menu->findChild<QAction *>("rulesEditMenuAction");
		action->menu()->clear();
		action->menu()->addAction("&Program")->setObjectName("programEditAction");
		action->menu()->addSeparator()->setObjectName("separator");

		for(int j=0; j<abs.size(); ++j)
		{
			QAction *child=action->menu()->addAction(abs[j]->displayName());
			child->setObjectName(QString("ruleEditAction"));
			child->setData(abs[j]->id());
			child->setEnabled(prog.isValid());
		}

		//Setup the delete menu
		action=_program_menu->findChild<QAction *>("rulesDeleteMenuAction");
		action->menu()->clear();
		action->menu()->addAction("&Program")->setObjectName("programDeleteAction");
		action->menu()->addSeparator()->setObjectName("separator");

		for(int j=0; j<abs.size(); ++j)
		{
			QAction *child=action->menu()->addAction(abs[j]->displayName());
			child->setObjectName(QString("ruleDeleteAction"));
			child->setData(abs[j]->id());
		}

		//Enable some program menu items only if the program is valid
		if(prog.isValid())
		{
			_program_menu->findChild<QAction *>("programHighlightAction")->setEnabled(true);
			_program_menu->findChild<QAction *>("programEditAction")->setEnabled(true);
			_program_menu->findChild<QAction *>("programDeleteAction")->setEnabled(true);
		}
		else
		{
			_program_menu->findChild<QAction *>("programHighlightAction")->setEnabled(false);
			_program_menu->findChild<QAction *>("programEditAction")->setEnabled(false);
			_program_menu->findChild<QAction *>("programDeleteAction")->setEnabled(false);
		}


		//Execute the menu
		QAction *act=_program_menu->exec(event->globalPos());
		if(!act)
			return;

		//Perform the action that was asked by the user
		if(act->objectName()=="programHighlightAction")
			emit programActivated(progItem->appName());
		else if(act->objectName()=="rulesHighlightAction")
		{
			QString source=progItem->parentArrow()->source()->domain().name();
			QString target=progItem->parentArrow()->target()->domain().name();

			emit transitionArrowActivated(progItem->appName(), source, target);
		}
		else if(act->objectName()=="programDeleteAction")
		{
			stack.deleteProgram(progItem->appName());
		}
		else if(act->objectName()=="ruleDeleteAction")
		{
			int id=act->data().toInt();

			stack.currentForModifying().deleteAbstractRule(id);
		}
		else if(act->objectName()=="programEditAction")
		{
			const Program &prog=stack.current().getProgramByName(progItem->appName(), true);
			AppDialog dialog(prog, this);

			dialog.show();
			dialog.exec();

			if(dialog.result()==AppDialog::Edited)
				stack.updateProgram(prog.name(), dialog.fetchProgram());
		}
		else if(act->objectName()=="ruleEditAction")
		{
			int id=act->data().toInt();
			AbstractRulePtr abs=stack.findAbstractRule(id);

			if(!abs.dynamicCast<Loop>().isNull())
				LoopDialog::editLoop(id);
			else
				RuleDialog::editRule(id);
		}
		else if(act->objectName()=="ruleVisibleAction")
		{
			int id=act->data().toInt();
			if(stack.isRuleHidden(id))
				stack.removeHiddenRule(id);
			else
				stack.addHiddenRule(id);
		}
		else if(act->objectName()=="programVisibleAction")
		{
			QString name=progItem->appName();

			if(stack.isProgramHidden(name))
				stack.removeHiddenProgram(name);
			else
				stack.addHiddenProgram(name);
		}
		else if(act->objectName()=="programFocusAction")
			onProgramFocused(progItem->appName());
	}


	//If the item is a domain, or a child widget of the domain
	else if(domItem && (!domItem->isHiddenByUser() || EditorConfig::hideMode()!=EditorConfig::LowerOpacityAndForbidSelection))
	{
		//Set the "Visible" check menu item's value depending on the item
		action=_domain_menu->findChild<QAction *>("domainVisibleAction");
		if(EditorConfig::hideMode()!=EditorConfig::RemoveFromView)
		{
			action->setChecked(!stack.isDomainHidden(domItem->domain().name()));
			action->setVisible(true);
		}
		else
			action->setVisible(false);

		//Execute the menu
		action=_domain_menu->exec(event->globalPos());
		if(!action)
			return;

		//Perform the action that was asked by the user
		if(action->objectName()=="domainHighlightAction")
			emit domainActivated(domItem->domain().name());
		else if(action->objectName()=="domainDeleteAction")
		{
			QString name=domItem->domain().name();

			//If the deleted domain was the default one, and if there is more than one domain left
			if(stack.current().defaultDomain().name()==name && stack.current().domainCount()>1)
			{

				SelectDefaultDomainDialog dialog(this->window());
				dialog.show();
				dialog.exec();

				if(dialog.result()==QDialog::Accepted)
					stack.currentForModifying().deleteDomain(name, dialog.newDefaultDomainName());
			}
			else
				stack.currentForModifying().deleteDomain(name);
		}
		else if(action->objectName()=="domainEditAction")
		{
			DomainDialog::editDomain(domItem->domain().name());
		}
		else if(action->objectName()=="domainVisibleAction")
		{
			QString name=domItem->domain().name();

			if(stack.isDomainHidden(name))
				stack.removeHiddenDomain(name);
			else
				stack.addHiddenDomain(name);
		}
		else if(action->objectName()=="domainFocusAction")
			onDomainFocused(domItem->domain().name());
	}


	//If there is no item or if the item is something else
	else
		_menu->exec(event->globalPos());
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
	if(event->delta()>0)
		zoom(ZoomIn);
	else if(event->delta()<0)
		zoom(ZoomOut);
}

void GraphicsView::onConfigurationChanged()
{
	updateView();
}

void GraphicsView::onHiddenDomainChanged()
{
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
		updateView();
}

void GraphicsView::onHiddenProgramChanged()
{
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
		updateView();
}

void GraphicsView::onHiddenRuleChanged()
{
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
		updateView();
}

void GraphicsView::onShowAllItems()
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	//Temporary bypass event handlers to improve performance
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
	{
		disconnect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
		disconnect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));
		disconnect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));
	}

	//Show all items
	QStringList hiddenDoms=stack.hiddenDomains();
	for(QStringList::const_iterator it=hiddenDoms.begin(); it!=hiddenDoms.end(); ++it)
		stack.removeHiddenDomain(*it);

	QStringList hiddenProgs=stack.hiddenPrograms();
	for(QStringList::const_iterator it=hiddenProgs.begin(); it!=hiddenProgs.end(); ++it)
		stack.removeHiddenProgram(*it);

	QList<int> hiddenRules=stack.hiddenRules();
	for(QList<int>::const_iterator it=hiddenRules.begin(); it!=hiddenRules.end(); ++it)
		stack.removeHiddenRule(*it);

	//Connect signals again and update the view
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
	{
		connect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
		connect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));
		connect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));

		updateView();
	}
}

void GraphicsView::onZoomIn()
{
	zoom(ZoomIn);
}

void GraphicsView::onZoomOut()
{
	zoom(ZoomOut);
}
void GraphicsView::onZoomFitBest()
{
	int w=width()-10, h=height()-10;

	if(w>0 && h>0)
		zoom(ZoomBest, qMin(h/sceneRect().height(), w/sceneRect().width()));
}

void GraphicsView::onZoomOriginalSize()
{
	zoom(ZoomOriginal);
}

void GraphicsView::onDomainFocused(QString name)
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	//Temporary bypass event handlers to improve performance
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
	{
		disconnect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
		disconnect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));
		disconnect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));
	}


	//Go through all rules involving this domain and list the source/target domains of these rules
	QStringList reachedDomains(name);

	QVector<Rule> rules=stack.current().rulesForDomain(stack.current().getDomainByName(name));
	for(int i=0; i<rules.count(); ++i)
	{
		const Rule &rule=rules[i];
		bool found=false;

		const QStringList &matchedDomains=rule.matchedDomainNames(stack.current().domainNameList());
		for(int j=0; j<matchedDomains.count(); ++j)
		{
			const QString &domName=matchedDomains[j];
			if(domName==name)
			{
				reachedDomains << rule.transitTo().name();
				found=true;
			}
			else if(rule.transitTo().name()==name)
				reachedDomains << domName;
		}
	}
	reachedDomains.removeDuplicates();


	//Hide each domain that is not in the reached list, show those that are in the list
	QStringList allDomains=stack.current().domainNameList();
	for(QStringList::const_iterator it=allDomains.begin(); it!=allDomains.end(); ++it)
	{
		if(reachedDomains.contains(*it))
			stack.removeHiddenDomain(*it);
		else
			stack.addHiddenDomain(*it);
	}


	//Show all programs and all rules
	QStringList hiddenProgs=stack.hiddenPrograms();
	for(QStringList::const_iterator it=hiddenProgs.begin(); it!=hiddenProgs.end(); ++it)
		stack.removeHiddenProgram(*it);

	QList<int> hiddenRules=stack.hiddenRules();
	for(QList<int>::const_iterator it=hiddenRules.begin(); it!=hiddenRules.end(); ++it)
		stack.removeHiddenRule(*it);


	//Connect signals again and update the view
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
	{
		connect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
		connect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));
		connect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));

		updateView();
	}
}

void GraphicsView::onProgramFocused(QString name)
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	//Temporary bypass event handlers to improve performance
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
	{
		disconnect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
		disconnect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));
		disconnect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));
	}


	//Show all rules
	QList<int> hiddenRules=stack.hiddenRules();
	for(QList<int>::const_iterator it=hiddenRules.begin(); it!=hiddenRules.end(); ++it)
		stack.removeHiddenRule(*it);


	//Hide all programs and then show again the one we want to highlight
	QStringList programs=stack.current().programNameList();
	for(QStringList::const_iterator it=programs.begin(); it!=programs.end(); ++it)
		stack.addHiddenProgram(*it);

	stack.removeHiddenProgram(name);


	//Hide all unreached domains (first find all reached domains, then hide the other ones)
	QStringList reachedDomains;

	QVector<Loop> loops=stack.current().loopsForProgram(name);
	for(int i=0; i<loops.count(); ++i)
	{
		const Loop &loop=loops[i];

		const QStringList &matchedDomains=loop.matchedDomainNames(stack.current().domainNameList());
		for(int j=0; j<matchedDomains.count(); ++j)
			reachedDomains << matchedDomains[j];
	}

	QVector<Rule> rules=stack.current().rulesForProgram(name);
	for(int i=0; i<rules.count(); ++i)
	{
		const Rule &rule=rules[i];

		const QStringList &matchedDomains=rule.matchedDomainNames(stack.current().domainNameList());
		for(int j=0; j<matchedDomains.count(); ++j)
			reachedDomains << matchedDomains[j];

		reachedDomains << rule.transitTo().name();
	}
	reachedDomains.removeDuplicates();

	QStringList allDomains=stack.current().domainNameList();
	for(QStringList::const_iterator it=allDomains.begin(); it!=allDomains.end(); ++it)
	{
		if(reachedDomains.contains(*it))
			stack.removeHiddenDomain(*it);
		else
			stack.addHiddenDomain(*it);
	}


	//Connect signals again and update the view
	if(EditorConfig::hideMode()==EditorConfig::RemoveFromView)
	{
		connect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
		connect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));
		connect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));

		updateView();
	}
}

void GraphicsView::onExportGraphAsPNG()
{
	//Ask where to save the graph
	QString path=QFileDialog::getSaveFileName(this, "Export graph as PNG...", EditorConfig::lastSavedPath(),
											  "PNG Images (*.png);;All files (*)");

	//Abort if the user leaves the file dialog
	if(path==QString())
		return;
	else
		EditorConfig::setLastSavedPath(path);

	//Check that writing is possible before going further
	if(!QFileInfo(QFileInfo(path).dir().path()).isWritable())
	{
		QMessageBox::critical(this, "Error", "<b>Could not export graph to " + path +
						   ".</b><br /><br />You do not have the sufficient rights to write to this location.",
						   QMessageBox::Ok);
		return;
	}

	//Create a new image
	QSize size=QSize(ceil(scene()->width()), ceil(scene()->height()));

	QImage img(size, QImage::Format_ARGB32);

	//Create a painter for this image
	QPainter painter(&img);
	painter.setRenderHint(QPainter::Antialiasing, true);

	//Render the scene in the image
	scene()->render(&painter);
	painter.end();

	//Save the image
	img.save(path, "PNG");
}
