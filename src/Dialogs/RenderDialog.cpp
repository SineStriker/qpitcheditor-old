#include "RenderDialog.h"
#include "mainwindow.h"

RenderDialog::RenderDialog(QWidget *parent) : EventDialog(parent) {

    // 窗口初始化
    setWindowTitle(childWindowTitle);
    setFixedSize(420, 140);

    // 添加标签和按钮
    QFont font(normalFont());

    lbCaption = new QLabel(tr("Rendering..."), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    lbCaption->setFont(font);
    btnCancel->setFont(font);

    lbCaption->move(25, 25);

    btnCancel->setFixedSize(115, 35);
    btnCancel->move(width() - btnCancel->width() - 15, height() - btnCancel->height() - 15);

    btnCancel->setFocus();
    btnCancel->setDefault(true);

    btnCancel->setShortcut(QKeySequence(Qt::Key_Escape));

    connect(btnCancel, &QPushButton::clicked, this, &RenderDialog::onCancelClicked);

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

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RenderDialog::onTimer);
}

RenderDialog::~RenderDialog() {
    qDebug() << "To remove cmd render dialog.";
}

void RenderDialog::runInCmd() {

    QString workDir = MainWindow::renderingDir;

    //------------------------------------------------------------------------
    m_pRender = new PROCESS_INFORMATION();

    PROCESS_INFORMATION &proInfo = *m_pRender;
    STARTUPINFO startInfo;

    QString tempPath = workDir + "\\temp.bat";

    wstring ws_bat = tempPath.toStdWString();
    wstring ws_dir = workDir.toStdWString();

    memset(&startInfo, 0, sizeof(startInfo));
    memset(&proInfo, 0, sizeof(proInfo));
    startInfo.cb = sizeof(startInfo);

    qDebug() << "workDir temp" << tempPath;

    // 启动进程
    BOOL bRet = CreateProcess(NULL, (LPWSTR) ws_bat.c_str(), NULL, NULL, FALSE,
                              CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, ws_dir.c_str(),
                              &startInfo, &proInfo);

    if (bRet == FALSE) {
        onRenderFailed();
    }
    //------------------------------------------------------------------------

    timer->start(100);
}

void RenderDialog::doSomething() {
    show();
    runInCmd();
}

void RenderDialog::closeEvent(QCloseEvent *event) {
    if (!killProcess()) {
        QString title = QObject::tr("QPitchEditor");
        QString content = QObject::tr("Fail to stop rendering.");

        QMessageBox::warning(NULL, title, content);

        event->ignore();
        return;
    }

    stopLoop();
    event->accept();
}

void RenderDialog::onTimer() {
    if (m_pRender) {
        DWORD exitCode;
        GetExitCodeProcess(m_pRender->hProcess, &exitCode);

        if (exitCode != STILL_ACTIVE) {
            CloseHandle(m_pRender->hProcess);
            CloseHandle(m_pRender->hThread);

            timer->stop();

            delete m_pRender;
            m_pRender = nullptr;

            onRenderOver();
        }
    }
}

bool RenderDialog::killProcess() {
    if (m_pRender) {
        DWORD exitCode = 0;
        GetExitCodeProcess(m_pRender->hProcess, &exitCode);

        if (exitCode == STILL_ACTIVE) {
            if (!TerminateProcess(m_pRender->hProcess, exitCode)) {
                qDebug() << "Fail to terminate process.";
                return 0;
            }
        }

        if (timer) {
            timer->stop();
        }

        CloseHandle(m_pRender->hProcess);
        CloseHandle(m_pRender->hThread);

        delete m_pRender;
        m_pRender = nullptr;
    }

    return -1;
}

void RenderDialog::onRenderOver() {
    setResult(0);
    close();
}

void RenderDialog::onRenderFailed() {
    setResult(1);
    close();
}

void RenderDialog::onCancelClicked() {
    close();
}
