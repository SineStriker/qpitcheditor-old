#include "EventDialog.h"

EventDialog::EventDialog(QWidget *parent) : QWidget(parent) {
    m_Result = 0;
    m_Loop = nullptr;

    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
}

int EventDialog::exec() {
    m_Result = -1;
    doSomething();

    m_Loop = new QEventLoop(this);
    m_Loop->exec();

    return m_Result;
}

void EventDialog::terminate(int code) {
    m_Result = code;
    stopLoop();
}

void EventDialog::release() {
    delete this;
}

void EventDialog::setResult(int n) {
    m_Result = n;
}

void EventDialog::stopLoop() {
    if (m_Loop) {
        m_Loop->exit();
    }
}

void EventDialog::doSomething() {
    show();
}

void EventDialog::closeEvent(QCloseEvent *event) {
    stopLoop();
    event->accept();
}
