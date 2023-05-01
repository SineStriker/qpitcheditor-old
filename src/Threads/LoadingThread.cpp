#include "LoadingThread.h"
#include "mainwindow.h"

#include "QUtils.h"

LoadingThread::LoadingThread(QWidget *parent) : QThread(parent) {
}

void LoadingThread::run() {
    int result = MainWindow::aData.LoadFromFile(MainWindow::aTempFile);
    emit isDone(result);
}
