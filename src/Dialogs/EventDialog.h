#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QCloseEvent>
#include <QEventLoop>
#include <QWidget>

#include "QUtils.h"

class EventDialog : public QWidget {
    Q_OBJECT
public:
    explicit EventDialog(QWidget *parent = nullptr);

    int exec();
    void terminate(int code);
    virtual void release();

protected:
    void setResult(int n);
    void stopLoop();

    virtual void doSomething();
    void closeEvent(QCloseEvent *event) override;

private:
    QEventLoop *m_Loop;
    int m_Result;

signals:
};

#endif // EVENTDIALOG_H
