#ifndef RENDERDIALOG_H
#define RENDERDIALOG_H

#include <QCloseEvent>
#include <QDebug>
#include <QEventLoop>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QWidget>

#include "EventDialog.h"
#include "QUtils.h"

// Windows
#include "Windows.h"

class RenderDialog : public EventDialog {
    Q_OBJECT
public:
    explicit RenderDialog(QWidget *parent = nullptr);
    ~RenderDialog();

    void runInCmd();

protected:
    QPushButton *btnCancel;
    QLabel *lbCaption;

    PROCESS_INFORMATION *m_pRender;
    QTimer *timer;

private:
    void closeEvent(QCloseEvent *event) override;
    void doSomething() override;

    void onTimer();
    bool killProcess();
    void onRenderOver();
    void onRenderFailed();

    void onCancelClicked();

signals:
};

#endif // RENDERDIALOG_H
