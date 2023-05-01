#include "InitDialog.h"

#include "QUtils.h"

InitDialog::InitDialog(QWidget *parent) : EventDialog(parent) {

    // 窗口初始化
    setWindowTitle(childWindowTitle);
    setFixedSize(420, 140);

    // 添加标签和按钮
    QFont font(normalFont());

    lbCaption = new QLabel(tr("Initializing..."), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    lbCaption->setFont(font);
    btnCancel->setFont(font);

    lbCaption->move(25, 25);

    btnCancel->setFixedSize(115, 35);
    btnCancel->move(width() - btnCancel->width() - 15, height() - btnCancel->height() - 15);

    btnCancel->setFocus();
    btnCancel->setDefault(true);

    btnCancel->setShortcut(QKeySequence(Qt::Key_Escape));

    // 添加按钮事件
    connect(btnCancel, &QPushButton::clicked, this, &InitDialog::onCancelClicked);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &InitDialog::onWaitingTimeOut);
}

void InitDialog::doSomething() {
    threadRun();
}

void InitDialog::threadRun() {
    // 创建线程
    thread = new LoadingThread(this);
    connect(thread, &LoadingThread::isDone, this, &InitDialog::onThreadComplete);
    thread->start();

    // 创建计时器
    timer->start(100);
}

void InitDialog::onCancelClicked() {
    qDebug() << "Loading Canceled.";

    if (timer->isActive()) {
        timer->stop();
    }

    thread->terminate();
    close();
}

void InitDialog::onThreadComplete(int code) {
    qDebug() << "Loading Complete.";

    if (timer->isActive()) {
        timer->stop();
    }

    setResult(code);
    stopLoop();
}

void InitDialog::onWaitingTimeOut() {
    qDebug() << "Dialog shows.";

    // 移除计时器
    if (timer->isActive()) {
        timer->stop();
    }

    show();
}
