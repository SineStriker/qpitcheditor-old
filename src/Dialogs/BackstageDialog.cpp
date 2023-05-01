#include "BackstageDialog.h"
#include "mainwindow.h"

BackstageDialog::BackstageDialog(QWidget *parent) : EventDialog(parent) {

    // 窗口初始化
    setWindowTitle(childWindowTitle);
    setFixedSize(420, 140);

    textWavtool = tr("Wavtool Process: ");
    textResampler = tr("Resampler Process: ");

    // 添加标签和按钮
    QFont font(normalFont());

    lbCaption = new QLabel(tr("Rendering..."), this);
    lbProcess = new QLabel(tr("Loading..."), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    lbCaption->setFont(font);
    lbProcess->setFont(font);
    btnCancel->setFont(font);

    lbCaption->move(25, 25);
    lbProcess->move(25, 55);

    lbProcess->setGeometry(lbProcess->x(), lbProcess->y(), width() - lbProcess->x(),
                           lbProcess->height());

    btnCancel->setFixedSize(115, 35);
    btnCancel->move(width() - btnCancel->width() - 15, height() - btnCancel->height() - 15);

    btnCancel->setFocus();
    btnCancel->setDefault(true);

    btnCancel->setShortcut(QKeySequence(Qt::Key_Escape));

    connect(btnCancel, &QPushButton::clicked, this, &BackstageDialog::onCancelClicked);

    setStyleSheet("QWidget{"
                  "background:#3C3C3C;"
                  "}"
                  "QLabel{"
                  "color:white;"
                  "}"
                  "QPushButton{"
                  "background:#404040;"
                  "border:1px solid #2D2D2D;"
                  "border-radius:3px;"
                  //"color:#878787;"
                  "color:white;"
                  "}"
                  "QPushButton:default{"
                  "border-width:2px;"
                  "}"
                  "QPushButton:hover{"
                  "background:#494949;"
                  "}");

    manualClose = false;
    acceptClose = false;
    isSynthesizing = false;
    finishedThreads = 0;

    rangeZero = 0;

    sumThreads = 12;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BackstageDialog::onTimer);
}

BackstageDialog::~BackstageDialog() {

    qDebug() << "To Remove Backstage Dialog.";
}

void BackstageDialog::setNormalBegin(int begin) {
    rangeZero = begin;
}

void BackstageDialog::setThreads(int n) {
    sumThreads = n;
}

void BackstageDialog::release() {
    // 启动计时器当所有线程结束时把自己删了

    qDebug() << "Timer start";
    timer->start(100);
}

void BackstageDialog::closeEvent(QCloseEvent *event) {

    if (!acceptClose) {
        // 如果是手动退出
        if (!manualClose) {
            setResult(-1); // 强制退出，返回设为 -1
            manualClose = true;
            killProcess();
        }

        event->ignore();
        return;
    }

    stopLoop();
    event->accept();
}

void BackstageDialog::doSomething() {
    show();
    runProcess();
}

bool BackstageDialog::killProcess() {
    for (int i = 0; i < threadList.size(); ++i) {
        if (threadList[i]->getProcess()) {
            threadList[i]->setActive(false);
            threadList[i]->getProcess()->terminate();
        }
    }

    return true;
}

void BackstageDialog::onRenderOver() {
    setResult(0);
}

void BackstageDialog::onRenderFailed() {
    acceptClose = true;
    setResult(1);
    close();
}

void BackstageDialog::onCancelClicked() {
    close();
}

void BackstageDialog::runProcess() {
    if (MainWindow::resamplerArguments.isEmpty() && MainWindow::wavtoolArguments.isEmpty()) {
        onRenderFailed();
    }

    preOperation(); // 准备

    resampCount = 0; // 清空重采样计数器
    syntheCount = 0; // 清空拼接计数器

    launchResamp();
}

void BackstageDialog::launchResamp() {

    // 启动重采样进程
    int i, j;

    int rangeStart = MainWindow::playingRange.x();
    int rangeEnd = MainWindow::playingRange.y();

    int aGroupCount = ceil(double(rangeEnd - rangeStart + 1) / sumThreads);

    resampSum = 0;
    thread = nullptr;

    for (i = 0; i <= rangeEnd - rangeStart; ++i) {

        if (i % aGroupCount == 0) {
            thread = new RenderThread();
            thread->setSerial(i / aGroupCount + 1);

            connect(thread, &RenderThread::singleDone, this, &BackstageDialog::onResampComplete);
            connect(thread, &RenderThread::totalDone, this, &BackstageDialog::onThreadComplete);

            threadList.push_back(thread);
        }

        j = rangeZero + rangeStart + i;

        if (MainWindow::resamplerArguments[j].isEmpty()) {
            continue;
        }
        if (MainWindow::resamplerArguments[j].size() >= 2 &&
            isFileExist(MainWindow::resamplerArguments[j][1])) {
            continue;
        }

        thread->addProcess(MainWindow::renderingDir, MainWindow::aTool2Path,
                           MainWindow::resamplerArguments[j]);
        resampSum++;
    }

    // 无论如何进行重采样线程，若没有要合成的瞬间完成
    for (i = 0; i < threadList.size(); ++i) {
        threadList[i]->start();
    }
}

void BackstageDialog::launchSynthe() {

    // 启动拼接进程
    int i, j;
    int rangeStart = MainWindow::playingRange.x();
    int rangeEnd = MainWindow::playingRange.y();

    syntheSum = 0;

    thread = new RenderThread();
    thread->setSerial(sumThreads + 1);
    connect(thread, &RenderThread::singleDone, this, &BackstageDialog::onSyntheComplete);
    connect(thread, &RenderThread::totalDone, this, &BackstageDialog::onThreadComplete);

    threadList.push_back(thread);

    for (i = 0; i <= rangeEnd - rangeStart; ++i) {
        j = rangeZero + rangeStart + i;

        if (MainWindow::wavtoolArguments[j].isEmpty()) {
            continue;
        }

        thread->addProcess(MainWindow::renderingDir, MainWindow::aTool1Path,
                           MainWindow::wavtoolArguments[j]);
        syntheSum++;
    }

    thread->start();
}

void BackstageDialog::onTimer() {
    int i;
    bool flag = false;
    for (i = 0; i < threadList.size(); ++i) {
        if (threadList[i]->isRunning()) {
            return;
            // flag = true;
        }
    }

    if (flag) {
        return;
    }

    if (timer->isActive()) {
        timer->stop();
    }
    // 删除所有线程
    for (int i = 0; i < threadList.size(); ++i) {
        delete threadList[i];
    }
    delete this;
}

void BackstageDialog::onThreadComplete(int code) {

    // 当且仅当线程全部结束才可退出
    finishedThreads++;

    qDebug() << "Thread" << code << "Finished.";

    if (finishedThreads != threadList.size()) {
        return;
    }

    // 在重采样状态下，并且没有手动关闭
    if (!isSynthesizing && !manualClose) {
        isSynthesizing = true;

        // 启动合成线程
        launchSynthe();
        return;
    }

    // 在合成状态下，并且没有手动关闭
    if (!manualClose) {
        postOperation();
    }

    acceptClose = true;

    close();
}

void BackstageDialog::onResampComplete(int code) {

    resampCount++;
    lbProcess->setText(textResampler + QString::number(resampCount) + "/" +
                       QString::number(resampSum));
}

void BackstageDialog::onSyntheComplete(int code) {

    ++syntheCount;
    lbProcess->setText(textWavtool + QString::number(syntheCount) + "/" +
                       QString::number(syntheSum));
}

// 准备
void BackstageDialog::preOperation() {
    QString targetFileName = MainWindow::renderingDir + "\\" + MainWindow::tempAudioName;
    QString cacheDirName = MainWindow::renderingDir + "\\" + MainWindow::cacheFolderName;

    // 创建缓存文件夹
    deleteFile(targetFileName);
    violentMakeDir(cacheDirName);
}

// 结束
void BackstageDialog::postOperation() {

    // 合并 whd 与 dat 文件
    QString whdFileName = MainWindow::renderingDir + "\\" + MainWindow::tempAudioName + ".whd";
    QString datFileName = MainWindow::renderingDir + "\\" + MainWindow::tempAudioName + ".dat";
    QString targetFileName = MainWindow::renderingDir + "\\" + MainWindow::tempAudioName;

    if (isFileExist(whdFileName) && isFileExist(datFileName)) {
        combineFile(whdFileName, datFileName, targetFileName);
        deleteFile(whdFileName);
        deleteFile(datFileName);
    }

    onRenderOver();
}
