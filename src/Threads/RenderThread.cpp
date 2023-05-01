#include "RenderThread.h"

#include <iostream>

RenderThread::RenderThread(QWidget *parent) : QThread(parent) {

    curProcess = nullptr;
    count = 0;
    active = false;
}

RenderThread::~RenderThread() {
    qDebug() << "To Remove Render Thread." << number;
    // 结束后删除进程
    if (curProcess) {
        delete curProcess;
        curProcess = nullptr;
    }
}

void RenderThread::addProcess(QString dir, QString program, const QStringList &arguments) {
    if (arguments.isEmpty()) {
        return;
    }

    AbstractProcess p;
    p.workDir = dir;
    p.programName = program;
    p.argsList = arguments;

    processList.push_back(p);
}

void RenderThread::setSerial(int n) {
    number = n;
}

void RenderThread::setActive(bool value) {
    active = value;
}

bool RenderThread::isActive() {
    return active;
}

QProcess *RenderThread::getProcess() {
    return curProcess;
}

void RenderThread::run() {
    active = true;
    if (!curProcess) {
        curProcess = new QProcess(0);
        // connect(curProcess, &QProcess::readyReadStandardOutput, this, &RenderThread::readOutput);
    }

    // 一个接一个进行
    for (int i = 0; i < processList.size(); ++i) {
        curProcess->setWorkingDirectory(processList[i].workDir);
        curProcess->setProgram(processList[i].programName);
        curProcess->setArguments(processList[i].argsList);

        curProcess->start();
        curProcess->waitForFinished(180 * 1000);
        curProcess->close();
        if (!active) {
            break;
        }
        emit singleDone(number); // 完成了一个
        ++count;
    }

    // 必须等到进程结束才发送结束信息
    emit totalDone(number);
}

void RenderThread::readOutput() {

    if (!curProcess) {
        return;
    }

    QString cardText = curProcess->readAllStandardOutput();
    std::cout << qstr2str(cardText) << endl;
}
