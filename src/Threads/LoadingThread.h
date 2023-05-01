#ifndef LoadingThread_H
#define LoadingThread_H

#include <QDebug>
#include <QThread>

#include "LibUTAUQt/QUtauData.h"

class LoadingThread : public QThread {
    Q_OBJECT
public:
    explicit LoadingThread(QWidget *parent = nullptr);

protected:
    void run();

signals:
    void isDone(int code);
};

#endif // LoadingThread_H
