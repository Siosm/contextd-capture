#include "notifyprompt.h"

#include <QDesktopWidget>

#include<QTimer>

NotifyPrompt::NotifyPrompt(QWidget *parent) :
	QDialog(parent, Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint),
	timer(new QTimer())
{
	setupUi(this);

	// Add a timeout
	timer->setSingleShot(true);
	connect(timer, SIGNAL(timeout()), this, SLOT(reject()));
}

void NotifyPrompt::changeEvent(QEvent *e)
{
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void NotifyPrompt::prompt(const DomainList::Domain_p& domain_from, const DomainList::Domain_p& domain_to, const QString& appName)
{
	// Get the question
	QString question=tr("<p>Your computer will switch from the domain '<b>%1</b>' to the domain '<b>%2</b>' because of the application '<b>%3</b>'.</p>"
									   "<p>Remember to <b>save your data</b> before accepting.</p><p>Do you agree ? Answer No if you don't know.</p>").arg(domain_from.display_name).arg(domain_to.display_name).arg(appName);

	// Set the icons
	domain_from_lbl->setPixmap(QPixmap(domain_from.icon).scaledToWidth(48));
	domain_to_lbl->setPixmap(QPixmap(domain_to.icon).scaledToWidth(48));

	// Set the title
	this->setWindowIcon(QIcon(":/icons/images/systrayicon.png"));
	this->setWindowTitle(tr("Do you want to transit ?"));

	// Wait for 19.5s before rejecting
	timer->start(19500);

	// Make it scroll
	QRect ScreenSize=QApplication::desktop()->screen()->frameGeometry();

	question_lbl->setText(question);
	//this->adjustSize();

	// Add a scrolling effect
	move(ScreenSize.width()-width(), ScreenSize.height()-20);
	show();
	for(int h=0;h<height();h++)
	{
		move(ScreenSize.width()-width(), ScreenSize.height()-h-20);
		QCoreApplication::processEvents();
	}
	move(ScreenSize.width()-width(), ScreenSize.height()-height()-20);

	// Exec the loop
	exec();
}
