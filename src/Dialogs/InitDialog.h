#ifndef InitDialog_H
#define InitDialog_H

#include <QCloseEvent>
#include <QDebug>
#include <QEventLoop>
#include <QLabel>
#include <QPushButton>
#include <QThread>
#include <QTimer>
#include <QWidget>

#include "Threads/LoadingThread.h"
#include "EventDialog.h"

class InitDialog : public EventDialog {
    Q_OBJECT
public:
    explicit InitDialog(QWidget *parent = nullptr);

private:
    QPushButton *btnCancel;
    QLabel *lbCaption;

    QTimer *timer;
    LoadingThread *thread;

    void doSomething() override;
    void threadRun();

    void onCancelClicked();
    void onThreadComplete(int code);
    void onWaitingTimeOut();
};

#endif // InitDialog_H
