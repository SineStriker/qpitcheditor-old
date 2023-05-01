#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <QWidget>

#include "QUtils.h"

struct AbstractProcess {
    QString workDir;
    QString programName;
    QStringList argsList;
};

class RenderThread : public QThread {
    Q_OBJECT
public:
    RenderThread(QWidget *parent = nullptr);
    ~RenderThread();

    void addProcess(QString dir, QString program, const QStringList &arguments);
    void setSerial(int n);

    void setActive(bool value);
    bool isActive();

    QProcess *getProcess();

protected:
    void run();

    QVector<AbstractProcess> processList;
    QProcess *curProcess;

    void readOutput();

private:
    int number;
    int count;

    bool active;

signals:
    void singleDone(int serial = 0);
    void totalDone(int code = 0);
};

#endif // RENDERTHREAD_H
