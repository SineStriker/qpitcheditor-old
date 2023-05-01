#include "OptionsDialog.h"
#include "mainwindow.h"

OptionsDialog::OptionsDialog(QWidget *parent) : EventDialog(parent) {

    // 窗口初始化
    setWindowTitle(childWindowTitle);
    setFixedSize(450, 375);

    // 添加标签和按钮
    QFont font(normalFont());
    QFont font2(normalHFont());

    lbCaption = new QLabel(tr("Options"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);
    btnOK = new QPushButton(tr("OK"), this);

    lbCaption->setFont(font2);
    btnCancel->setFont(font);
    btnOK->setFont(font);

    lbCaption->move(25, 20);

    btnCancel->setFixedSize(115, 35);
    btnCancel->move(width() - btnCancel->width() - 20, height() - btnCancel->height() - 20);

    btnOK->setFixedSize(115, 35);
    btnOK->move(width() - btnCancel->width() - 145, height() - btnCancel->height() - 20);

    btnOK->setFocus();
    btnOK->setDefault(true);

    btnOK->setShortcut(QKeySequence(Qt::Key_Return));
    btnCancel->setShortcut(QKeySequence(Qt::Key_Escape));

    connect(btnOK, &QPushButton::clicked, this, &OptionsDialog::onOKClicked);
    connect(btnCancel, &QPushButton::clicked, this, &OptionsDialog::onCancelClicked);

    QStringList languages{"English", QString::fromLocal8Bit("简体中文"),
                          QString::fromLocal8Bit("日本語")};
    QStringList doubleClicks{tr("Double-Click"), tr("Click")};
    QStringList threads;

    threads.push_back(tr("Use cmd"));
    for (int i = 0; i < 12; ++i) {
        threads.push_back(QString::number(i + 1));
    }

    ccLanguage = new ComboControl(tr("Language"), languages, this);
    ccDoubleClick = new ComboControl(tr("Add control point by"), doubleClicks, this);
    ccThreads = new ComboControl(tr("Rendering threads"), threads, this);

    scVolume = new SlideControl(tr("Master volume"), this);
    scVolume->setSingleStep(1);
    scVolume->setMinimum(0);
    scVolume->setMaximum(stageMaxVolume);

    ccLanguage->setFixedHeight(60);
    ccDoubleClick->setFixedHeight(60);
    ccThreads->setFixedHeight(60);
    scVolume->setFixedHeight(80);

    int elemetHeight = 40;

    ccLanguage->setGeometry(15, 50, 420, elemetHeight);
    ccDoubleClick->setGeometry(ccLanguage->x(), ccLanguage->y() + ccLanguage->height(),
                               ccLanguage->width(), elemetHeight);
    ccThreads->setGeometry(ccLanguage->x(), ccDoubleClick->y() + ccDoubleClick->height(),
                           ccLanguage->width(), elemetHeight);
    scVolume->setGeometry(ccLanguage->x(), ccThreads->y() + ccThreads->height(),
                          ccLanguage->width(), elemetHeight * 2);

    getCurrentValues();

    /*setStyleSheet("QWidget{"
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
                  "}");*/
}

void OptionsDialog::getCurrentValues() {
    ccLanguage->getCombo()->setCurrentIndex(MainWindow::languageIndex);
    ccDoubleClick->getCombo()->setCurrentIndex(1 - MainWindow::doubleClickToAdd);
    ccThreads->getCombo()->setCurrentIndex(MainWindow::renderMode);
    scVolume->setValue(MainWindow::stageVolume);
}

void OptionsDialog::saveCurrentValues() {

    int prevLanguaeIndex = MainWindow::languageIndex;

    int curLanguageIndex = ccLanguage->getCombo()->currentIndex();
    int curDoubleClickToAdd = 1 - ccDoubleClick->getCombo()->currentIndex();
    int curRenderMode = ccThreads->getCombo()->currentIndex();
    int curStageVolume = scVolume->getValue();

    // 保存设定
    MainWindow::languageIndex = curLanguageIndex;
    MainWindow::doubleClickToAdd = curDoubleClickToAdd;
    MainWindow::renderMode = curRenderMode;
    MainWindow::stageVolume = curStageVolume;

    // 设定音量
    if (MainWindow::player) {
        MainWindow::player->setVolume(MainWindow::stageVolume);
    }

    if (prevLanguaeIndex != curLanguageIndex) {
        /*QString title = childWindowTitle;
        QString content =
            tr("You need to restart QPitchEditor for language settings to take effect.");

        // 提示参数不足
        QMessageBox::information(NULL, title, content);*/

        Root->applyLanguage();
        Root->reInitializeStrings();
    }
}

void OptionsDialog::onCancelClicked() {
    close();
}

void OptionsDialog::onOKClicked() {
    setResult(1);

    saveCurrentValues();

    close();
}
