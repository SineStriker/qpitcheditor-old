#include "mainwindow.h"
#include "ui_mainwindow.h"

// 初始化所有的全局变量
QString MainWindow::CurrentVersion = "0.1.16";

QUtauData MainWindow::aData;
QString MainWindow::aTempFile;

QString MainWindow::aVoiceDir = "";
QString MainWindow::aProjectPath = "";
QString MainWindow::aTool1Path = "";
QString MainWindow::aTool2Path = "";
QString MainWindow::aGlobalFlags = "";

int MainWindow::noteUnitWIndex = 14;
int MainWindow::noteUnitHIndex = 3;

QSet<SimpleObject *> MainWindow::stageDraggedObjects = {};
QSet<SimplePoint *> MainWindow::stageSelectedPoints = {};
QSet<SimpleNote *> MainWindow::stageSelectedNotes = {};

QVector<NoteOperation *> MainWindow::historyList = {};
int MainWindow::historyIndex = 0;
int MainWindow::historySequence = 0;
QTranslator *MainWindow::mainTranslator = nullptr;

QTabWidget *MainWindow::mainTab = nullptr;
CentralWidget *MainWindow::mainWidget = nullptr;

int MainWindow::debugConstant = 0;
bool MainWindow::isLoaded = false;
bool MainWindow::isChanged = false;

// 加载临时文件之前
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    mainWidget = nullptr;
    isLoaded = false;
    isChanged = false;

    setWindowTitle(fullWindowTitle());
    setStyleSheet("QMainWindow {background-color:#373737;}");

    // 读取程序记忆设定
    detectLanguage();
    readSetting();
}

// 退出窗口时
MainWindow::~MainWindow() {
    exitWindow();
}

/*--------------------------------------初始化窗口--------------------------------------*/

void MainWindow::setFirstWindow() {

    // checkUpdate();

    // 设置主窗口

    // mainTab = new QTabWidget(this);

    mainWidget = new CentralWidget(this);
    setCentralWidget(mainWidget);

    // mainTab->addTab(mainWidget, aTempFile);
    // setCentralWidget(mainTab);

    // qDebug()<<tabifyDockWidget()

    // 初始化播放组件
    initMediaComponents();

    // 检查上次删了没
    violentDelDir(renderingDir);

    // 初始化菜单项
    initMenuBar();

    // 显示状态栏
    // statusBar()->show();

    // 显示窗口
    resize(1280, 720);
    applySetting();

    // 更新菜单选项
    updateHistoryActions();
    updateDeleteAction();
    updateDeselectAction();

    // 显示窗口
    show();

    // 加载音符
    initEditor();

    // 显示窗口
    // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
}

void MainWindow::exitWindow() {
    if (isLoaded) {
        violentDelDir(renderingDir);
        if (player) {
            delete player;
            player = nullptr;
        }
        saveSetting();
    }
}

/*--------------------------------------静态全局函数--------------------------------------*/

// 解析 character.txt
void MainWindow::getCharacter(QString &name, QString &image) {

    QFile fs(aVoiceDir + "\\character.txt");

    if (!fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream txtInput(&fs);
    QString line;

    int eq;

    while (!txtInput.atEnd()) {
        line = txtInput.readLine();

        if (line.startsWith("name=")) {
            eq = line.indexOf("=") + 1;
            name = line.mid(eq, line.size() - eq);
        }

        else if (line.startsWith("image=")) {
            eq = line.indexOf("=") + 1;
            image = line.mid(eq, line.size() - eq);
        }
    }

    fs.close();
}

QString MainWindow::fullWindowTitle() {
    return mainWindowTitle + " v" + CurrentVersion;
}

int MainWindow::msgboxInfo(QString content, QString title) {
    if (title.isEmpty()) {
        title = childWindowTitle;
    }
    return QMessageBox::information(this, title, content, tr("Confirm"));
}

int MainWindow::msgboxExclaim(QString content, QString title) {
    if (title.isEmpty()) {
        title = childWindowTitle;
    }
    return QMessageBox::warning(this, title, content, tr("Confirm"));
}

int MainWindow::msgboxError(QString content, QString title) {
    if (title.isEmpty()) {
        title = childWindowTitle;
    }
    return QMessageBox::critical(this, title, content, tr("Confirm"));
}

int MainWindow::msgboxInfoQuestion(QString content, QString title, int def) {
    if (title.isEmpty()) {
        title = childWindowTitle;
    }

    def = (def == QMessageBox::Yes) ? 0 : 1;

    int result =
        QMessageBox::information(this, title, content, tr("Yes"), tr("No"), nullptr, def, 1);

    return (result == 0) ? QMessageBox::Yes : QMessageBox::No;
}

void MainWindow::checkUpdate(bool showLatest) {
    QString page = "https://sinestriker.github.io";
    QString branches = "softwares/QPitchEditor/version.json";
    QString site = page + "/" + branches;

    OnlineUpdater *updater = new OnlineUpdater(this);
    updater->setVersion(CurrentVersion);
    updater->setAutoDelete(true);
    updater->setShowLatest(showLatest);
    updater->setOnlineUrl(site);
}

/*--------------------------------------拖动和选中--------------------------------------*/

// 处理拖动的对象
void MainWindow::addDraggedObject(SimpleObject *w) {
    stageDraggedObjects.insert(w);
}

void MainWindow::removeDraggedObject(SimpleObject *w) {
    auto it = stageDraggedObjects.find(w);
    if (it != stageDraggedObjects.end()) {
        stageDraggedObjects.erase(it);
    }
}
void MainWindow::removeAllDraggedObjects() {
    for (auto iter = stageDraggedObjects.begin(); iter != stageDraggedObjects.end(); ++iter) {
        (*iter)->autoReleaseEvent(NULL, true);
    }
    stageDraggedObjects.clear();
}

// 处理选中的音符
void MainWindow::autoAddSelectedNote(SimpleNote *w) {
    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    if (c == Qt::ShiftModifier) {
        if (stageSelectedNotes.empty()) {
            addSelectedNotes(w);
        } else {
            addSelectedNotes(w);
            mainWidget->Interface()->selectContinuousNotes();
        }
    } else if (c == Qt::ControlModifier) {
        if (stageSelectedNotes.contains(w)) {
            removeSelectedNotes(w);
        } else {
            addSelectedNotes(w);
        }
    } else {
        if (!stageSelectedNotes.contains(w)) {
            removeAllSelectedNotes();
            addSelectedNotes(w);
        }
    }

    updateDeleteAction();
    updateDeselectAction();
}
void MainWindow::addSelectedNotes(SimpleNote *w) {
    w->setSelected(true);
    stageSelectedNotes.insert(w);

    updateDeleteAction();
    updateDeselectAction();
}
void MainWindow::removeSelectedNotes(SimpleNote *w) {
    auto it = stageSelectedNotes.find(w);
    if (it != stageSelectedNotes.end()) {
        (*it)->setSelected(false);
        stageSelectedNotes.erase(it);
    }

    updateDeleteAction();
    updateDeselectAction();
}
void MainWindow::removeAllSelectedNotes() {
    // qDebug() << "remove all";
    for (auto iter = stageSelectedNotes.begin(); iter != stageSelectedNotes.end(); ++iter) {
        (*iter)->setSelected(false);
    }
    stageSelectedNotes.clear();

    updateDeleteAction();
    updateDeselectAction();
}

// 处理选中的控制点
void MainWindow::autoAddSelectedPoint(SimplePoint *w) {
    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    if (c == Qt::ControlModifier) {
        if (stageSelectedPoints.contains(w)) {
            removeSelectedPoints(w);
        } else {
            addSelectedPoints(w);
        }
    } else {
        if (!stageSelectedPoints.contains(w)) {
            removeAllSelectedPoints();
            addSelectedPoints(w);
        }
    }

    // 检查是否是同一个音符的控制点
    bool flag = true;
    auto st = stageSelectedPoints.begin();
    for (auto iter = stageSelectedPoints.begin(); iter != stageSelectedPoints.end(); ++iter) {
        if ((*iter)->getScopeArea() != (*st)->getScopeArea()) {
            flag = false;
            break;
        }
    }
    if (!flag) {
        removeAllSelectedPoints();
    }

    updateDeleteAction();
    updateDeselectAction();
}
void MainWindow::addSelectedPoints(SimplePoint *w) {
    w->setSelected(true);
    stageSelectedPoints.insert(w);

    updateDeleteAction();
    updateDeselectAction();
}
void MainWindow::removeSelectedPoints(SimplePoint *w) {
    auto it = stageSelectedPoints.find(w);
    if (it != stageSelectedPoints.end()) {
        (*it)->setSelected(false);
        stageSelectedPoints.erase(it);
    }

    updateDeleteAction();
    updateDeselectAction();
}
void MainWindow::removeAllSelectedPoints() {
    for (auto iter = stageSelectedPoints.begin(); iter != stageSelectedPoints.end(); ++iter) {
        (*iter)->setSelected(false);
    }
    stageSelectedPoints.clear();

    updateDeleteAction();
    updateDeselectAction();
}

/*--------------------------------------历史记录--------------------------------------*/
void MainWindow::historyAddStep(NoteOperation *n) {

    try {
        if (!n->differ()) {
            qDebug() << "Note Operation refused.";
            n->release();
            return;
        }

        // 删除后面所有的操作
        while (!isLatestStep()) {
            historyList.back()->release();
            historyList.pop_back();
        }

        if (historySequence == 0) {
            // 单个操作
            qDebug() << "Single Operation.";
            historyList.push_back(n);
            historyIndex++;
        } else if (historySequence == 1) {
            // 多个操作的起始
            qDebug() << "Multiple Operation Start.";
            historyList.push_back(n);
            historyIndex++;
            historySequence = 2;
        } else if (historySequence == 2) {
            // 多个操作的后续
            qDebug() << "Multiple Operation Next.";
            historyList.back()->setNext(n);
        }

        updateHistoryActions();
    } catch (...) {
        msgboxExclaim(tr("Unknown exception!"));
    }
}


void MainWindow::historyNextStep(bool execute) {
    if (isLatestStep()) {
        return;
    }
    try {
        if (execute) {
            historyList[historyIndex]->redo();
        }
        historyIndex++;

        updateHistoryActions();
    } catch (...) {
        msgboxExclaim(tr("Unknown exception!"));
    }
}

void MainWindow::historyPrevStep(bool execute) {
    if (isEarliestStep()) {
        return;
    }
    try {
        if (execute) {
            historyList[historyIndex - 1]->undo();
        }
        historyIndex--;

        updateHistoryActions();
    } catch (...) {
        msgboxExclaim(tr("Unknown exception!"));
    }
}

bool MainWindow::isLatestStep() {
    return (historyIndex == historyList.size());
}

bool MainWindow::isEarliestStep() {
    return (historyIndex == 0);
}

/*--------------------------------------加载临时文件--------------------------------------*/
void MainWindow::loadTempFile(QString &filename) {

    aTempFile = filename;

    // 检查音源与引擎
    initPath();

    // 新建加载窗口
    InitDialog *loadingDlg = new InitDialog();
    int code = loadingDlg->exec();

    if (loadingDlg) {
        loadingDlg->release();
        loadingDlg = nullptr;
    }

    if (code == -1) {
        exit(ERROR_CANCELLED);
    } else {
        // 判断加载文件是否有效
        if (code < MINOR_ERROR) {
            QString content = QString("%1%2%3").arg(tr("Failed to load \""), aTempFile, tr("\"."));
            msgboxExclaim(content);
            exit(ERROR_OPEN_FAILED);
        }

        isLoaded = true;
        setFirstWindow();
    }
}

void MainWindow::setChanged(bool value) {

    isChanged = value;

    if (isChanged) {
        setWindowTitle(fullWindowTitle() + "(*)");
    } else {
        setWindowTitle(fullWindowTitle());
    }
}

void MainWindow::initEditor() {

    // 插入音符
    mainWidget->Interface()->initializeNotes();

    // 移动到第一个音符附近
    SimpleNote *firstNote = mainWidget->Interface()->getNoteAt(0);
    int containerHeight = mainWidget->Container()->height();
    if (firstNote) {
        mainWidget->Container()->verticalScrollBar()->setValue(firstNote->y() -
                                                               containerHeight / 2);
    }
}

/*--------------------------------------其他函数--------------------------------------*/

// 显示选项
void MainWindow::showOptions() {
    OptionsDialog *dlg = new OptionsDialog(NULL);
    int code = -1;

    code = dlg->exec();

    if (dlg) {
        dlg->release();
        dlg = nullptr;
    }
}

void MainWindow::showNoteProperties() {
    NoteDialog *dlg = new NoteDialog(NULL);
    int code = -1;

    code = dlg->exec();

    if (dlg) {
        dlg->release();
        dlg = nullptr;
    }
}

/*--------------------------------------主窗口其他事件--------------------------------------*/

// 鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
}

// 重绘事件
void MainWindow::paintEvent(QPaintEvent *event) {
}

// 关闭事件
void MainWindow::closeEvent(QCloseEvent *event) {
    if (isChanged) {
        QString content = tr("You haven't saved yet, sure to exit?");
        int result = msgboxInfoQuestion(content, "", QMessageBox::No);

        switch (result) {
        case QMessageBox::Yes:
            event->accept();
            break;
        case QMessageBox::No:
            event->ignore();
            break;
        default:
            break;
        }
    }
}

void MainWindow::changeEvent(QEvent *event) {
    /*switch (event->type()) {
    case QEvent::LanguageChange:
        reInitializeStrings();
        break;
    default:
        break;
    }*/
}
