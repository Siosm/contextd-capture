#ifndef NOTIFYPROMPT_H
#define NOTIFYPROMPT_H

#include "ui_notifyprompt.h"

class NotifyPrompt : public QDialog, private Ui::NotifyPrompt {
    Q_OBJECT
    Q_DISABLE_COPY(NotifyPrompt)
public:
    explicit NotifyPrompt(QWidget *parent = 0);

    void prompt(QString question);

protected:
    virtual void changeEvent(QEvent *e);
};

#endif // NOTIFYPROMPT_H
