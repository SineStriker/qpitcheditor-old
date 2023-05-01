#ifndef BACKSTAGEDIALOG_H
#define BACKSTAGEDIALOG_H

#include "EventDialog.h"
#include "Threads/RenderThread.h"

#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QTimer>

class BackstageDialog : public EventDialog {
    Q_OBJECT
public:
    explicit BackstageDialog(QWidget *parent = nullptr);
    ~BackstageDialog();

    void setNormalBegin(int begin);
    void setThreads(int n);

    void release() override;

protected:
    QString textWavtool, textResampler;
    QPushButton *btnCancel;
    QLabel *lbCaption;
    QLabel *lbProcess;

    RenderThread *thread;
    QVector<RenderThread *> threadList;

    QTimer *timer;

    int rangeZero;

    int resampCount, syntheCount;
    int sumThreads, resampSum, syntheSum;

    bool manualClose, acceptClose, isSynthesizing;
    int finishedThreads;

private:
    void closeEvent(QCloseEvent *event) override;
    void doSomething() override;

    bool killProcess();
    void onRenderOver();
    void onRenderFailed();

    void onCancelClicked();

    void runProcess();

    void launchResamp(); // 启动重采样线程
    void launchSynthe(); // 启动合成线程

    void onTimer();

    void onThreadComplete(int code);

    void onResampComplete(int code); // 重采样结束
    void onSyntheComplete(int code); // 合成结束

    void preOperation();  // 合成前
    void postOperation(); // 结束后

signals:
};

#endif // BACKSTAGEDIALOG_H
