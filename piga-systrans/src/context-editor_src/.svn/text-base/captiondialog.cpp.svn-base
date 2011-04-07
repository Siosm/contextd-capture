#include <QPaintEngine>
#include "colors.h"
#include "configurationstack.h"
#include "editorconfig.h"
#include "graphicsdomain.h"
#include "captiondialog.h"
#include "ui_captiondialog.h"

CaptionDialog::CaptionDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::CaptionDialog)
{
	ui->setupUi(this);
	ui->domView->setScene(new QGraphicsScene());
	ui->arrowView->setScene(new QGraphicsScene());

	ui->domNormalColor->setAutoFillBackground(true);
	ui->domNormalColorBg->setAutoFillBackground(true);
	ui->domSelectedColor->setAutoFillBackground(true);
	ui->domSelectedColorBg->setAutoFillBackground(true);
	ui->domUnreachableColor->setAutoFillBackground(true);
	ui->domUnreachableColorBg->setAutoFillBackground(true);
	ui->domGradientColor->setAutoFillBackground(true);
	ui->domGradientColorBg->setAutoFillBackground(true);

	ui->arrowBypassedColor->setAutoFillBackground(true);
	ui->arrowConfidentialityColor->setAutoFillBackground(true);
	ui->arrowDuplicateColor->setAutoFillBackground(true);
	ui->arrowInvalidMRuleColor->setAutoFillBackground(true);
	ui->arrowNormalColor->setAutoFillBackground(true);
	ui->arrowReliabilityColor->setAutoFillBackground(true);
	ui->arrowSelectedColor->setAutoFillBackground(true);
	ui->arrowUndefinedActionsColor->setAutoFillBackground(true);
	ui->arrowUnknownProgramColor->setAutoFillBackground(true);
	ui->arrowUnreachableColor->setAutoFillBackground(true);


	//Normal domain color
	QPalette pal=palette();

	pal.setColor(backgroundRole(), Colors::DomainNormalForeground);
	ui->domNormalColor->setPalette(pal);
	pal.setColor(backgroundRole(), Colors::DomainNormalBackground);
	ui->domNormalColorBg->setPalette(pal);

	//Selected domain color
	pal.setColor(backgroundRole(), Colors::DomainSelectedForeground);
	ui->domSelectedColor->setPalette(pal);
	pal.setColor(backgroundRole(), Colors::DomainSelectedBackground);
	ui->domSelectedColorBg->setPalette(pal);


	//Unreachable domain color
	DomainWarningColorAnimation &warnDomAnim=DomainWarningColorAnimation::instance();
	QPair<QColor, QColor> colors=warnDomAnim.getColorsFromDomainWarningType(Unreachable);

	pal.setColor(backgroundRole(), colors.first);
	ui->domUnreachableColor->setPalette(pal);
	pal.setColor(backgroundRole(), colors.second);
	ui->domUnreachableColorBg->setPalette(pal);


	//Gradient for domain confidentiality/reliability
	DomainGradientColorAnimation &gradAnim=DomainGradientColorAnimation::instance();
	QLinearGradient grad(ui->domGradientColor->rect().left(), 0, ui->domGradientColor->rect().right(), 0);
	QLinearGradient gradBg(ui->domGradientColorBg->rect().left(), 0, ui->domGradientColorBg->rect().right(), 0);
	qreal steps[]={0, 0.25, 0.5, 0.75, 1};

	for(int i=0; i<5; ++i)
	{
		colors=gradAnim.getColorsFromRatio(steps[i]);
		grad.setColorAt(1-steps[i], colors.first);
		gradBg.setColorAt(1-steps[i], colors.second);
	}

	pal.setBrush(backgroundRole(), QBrush(grad));
	ui->domGradientColor->setPalette(pal);
	pal.setBrush(backgroundRole(), QBrush(gradBg));
	ui->domGradientColorBg->setPalette(pal);


	//Normal arrow color
	pal.setColor(backgroundRole(), Colors::ArrowNormalPen);
	ui->arrowNormalColor->setPalette(pal);

	//Selected arrow color
	pal.setColor(backgroundRole(), Colors::ArrowSelectedPen);
	ui->arrowSelectedColor->setPalette(pal);


	//Unreachable arrow color
	ArrowColorAnimation &anim=ArrowColorAnimation::instance();

	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(Unreachable));
	ui->arrowUnreachableColor->setPalette(pal);

	//Bypassed arrow color
	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(Bypassed));
	ui->arrowBypassedColor->setPalette(pal);

	//Confidentiality arrow color
	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(ConfidentialityLoss));
	ui->arrowConfidentialityColor->setPalette(pal);

	//Reliability arrow color
	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(ReliabilityLoss));
	ui->arrowReliabilityColor->setPalette(pal);

	//UnknownProgram arrow color
	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(UnknownProgram));
	ui->arrowUnknownProgramColor->setPalette(pal);

	//UndefinedMRuleProgramData arrow color
	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(UndefinedMRuleProgramData));
	ui->arrowUndefinedActionsColor->setPalette(pal);

	//InvalidMRuleProgramData arrow color
	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(InvalidMRuleProgramData));
	ui->arrowInvalidMRuleColor->setPalette(pal);

	//MatchedSomewhereElse arrow color
	pal.setColor(backgroundRole(), anim.getColorFromRuleWarningType(MatchedSomewhereElse));
	ui->arrowDuplicateColor->setPalette(pal);
}

CaptionDialog::~CaptionDialog()
{
    delete ui;
}

void CaptionDialog::updateViews()
{
	//Clear scenes and set hints
	ui->domView->scene()->clear();
	ui->arrowView->scene()->clear();
	ui->domView->setRenderHint(QPainter::Antialiasing, !EditorConfig::fastDraw());
	ui->arrowView->setRenderHint(QPainter::Antialiasing, !EditorConfig::fastDraw());
	_domains.clear();


	//Fill domains view
	Domain dom1("default", "Default", ":/icons/security-low", QString(),
				Domain::ConfidentialityDefault, Domain::ReliabilityDefault);
	GraphicsDomain *gDom1=new GraphicsDomain(dom1, 0, 0, ui->domView->scene());
	gDom1->drawDefaultDomainAttributes(true);
	gDom1->setPenAndBrush();
	connect(gDom1, SIGNAL(activated(QString)), this, SLOT(onDomainActivated(QString)));
	_domains["default"]=gDom1;

	Domain dom2("medium", "Average", ":/icons/security-medium", QString(),
				(Domain::ConfidentialityMax-Domain::ConfidentialityMin)/2,
				(Domain::ReliabilityMax-Domain::ReliabilityMin)/2);
	_domains["medium"]=new GraphicsDomain(dom2, 100, 0, ui->domView->scene());
	connect(_domains["medium"], SIGNAL(activated(QString)), this, SLOT(onDomainActivated(QString)));

	Domain dom3("secure", "Secure", ":/icons/security-high", QString(),
				Domain::ConfidentialityMax, Domain::ReliabilityMax);
	_domains["secure"]=new GraphicsDomain(dom3, 200, 0, ui->domView->scene());
	connect(_domains["secure"], SIGNAL(activated(QString)), this, SLOT(onDomainActivated(QString)));

	ui->domView->setSceneRect(ui->domView->scene()->itemsBoundingRect());

	//Fill arrow view
	GraphicsDomain *src=new GraphicsDomain(dom2, 0, 0, ui->arrowView->scene());
	GraphicsDomain *dest=new GraphicsDomain(dom3, 150, 80, ui->arrowView->scene());
	_arrow=new GraphicsArrow(src, dest, ui->arrowView->scene());

	Program prog("login", "Login", QString(), QString(), QString());
	AbstractRulePtr abs=AbstractRulePtr(new Rule(prog, dom3, "Caption Sample Rule"));
	_arrow->addRule(abs);
	_arrow->makeLinePath();
	_arrow->_prog_position=QPoint(110, 45);
	_arrow->positionPrograms();

	ui->arrowView->setSceneRect(ui->arrowView->scene()->itemsBoundingRect());
}

void CaptionDialog::onDomainActivated(QString name)
{
	for(QMap<QString, GraphicsDomain*>::iterator it=_domains.begin(); it!=_domains.end(); ++it)
		it.value()->setHighlighted(it.key()==name);
}
