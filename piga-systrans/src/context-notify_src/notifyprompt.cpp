#include "notifyprompt.h"
#include <QDesktopWidget>

NotifyPrompt::NotifyPrompt(QWidget *parent) :
    QDialog(parent, Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint)
{
    setupUi(this);
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

void NotifyPrompt::prompt(QString question)
{
	QRect ScreenSize=QApplication::desktop()->screen()->frameGeometry();

	question_lbl->setText(question);
	//this->adjustSize();

	move(ScreenSize.width()-width(), ScreenSize.height()-20);
	show();
	for(int h=0;h<height();h++)
	{
		move(ScreenSize.width()-width(), ScreenSize.height()-h-20);
		QCoreApplication::processEvents();
	}
	move(ScreenSize.width()-width(), ScreenSize.height()-height()-20);
}
