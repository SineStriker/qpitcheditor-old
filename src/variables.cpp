#include "mainwindow.h"

bool MainWindow::bLineMode = false;
bool MainWindow::bNoteMode = false;
bool MainWindow::bPlayMode = false;

bool MainWindow::bShowMode1 = false;
bool MainWindow::bShowMode2 = true;
bool MainWindow::bOverlap = false;
bool MainWindow::bReflect = true;
bool MainWindow::bShowFlags = false;

bool MainWindow::bShowEnvLines = true;
bool MainWindow::bShowEnvFill = true;

int MainWindow::snappingUnit = 1; // 最小长度单位

int MainWindow::languageIndex = 0; // 语言
int MainWindow::localeIndex = 0;   // 语言
QVector<QString> MainWindow::languageFiles = {};

int MainWindow::renderMode = 0;    // 渲染模式 （0：用命令行；>0：QProcess多线程）
int MainWindow::stageVolume = 100; // 舞台音量

bool MainWindow::doubleClickToAdd = true; // 单击添加控制点

Qt::KeyboardModifiers MainWindow::zoomHorizontallyKey = (Qt::ShiftModifier | Qt::ControlModifier);
Qt::KeyboardModifiers MainWindow::zoomVerticallyKey = Qt::ControlModifier;
Qt::KeyboardModifiers MainWindow::moveHorizontallyKey = Qt::ShiftModifier;
Qt::KeyboardModifiers MainWindow::moveVerticallyKey = Qt::NoModifier;

/*--------------------------------------与内部窗口连接的变量--------------------------------------*/
void MainWindow::setLineMode(bool value) {
    bLineMode = value;
    mainWidget->Interface()->updateMode();
}

void MainWindow::setNoteMode(bool value) {
    bNoteMode = value;
    mainWidget->Interface()->updateMode();
}

void MainWindow::setPlayMode(bool value) {
    bPlayMode = value;
    mainWidget->Interface()->updateMode();
}

void MainWindow::setMode1Status(bool value) {
    bShowMode1 = value;
    mainWidget->Interface()->updateAllNotes();
}

void MainWindow::setMode2Status(bool value) {
    bShowMode2 = value;
    mainWidget->Interface()->updateAllNotes();
}

void MainWindow::setOverlapStatus(bool value) {
    bOverlap = value;
    mainWidget->Interface()->updateAllNotes();
}

void MainWindow::setReflectStatus(bool value) {
    bReflect = value;
    if (mainWidget && mainWidget->Interface()) {
        mainWidget->Interface()->updateAllNotes();
    }
}

void MainWindow::setFlagsStatus(bool value) {
    bShowFlags = value;
    mainWidget->Interface()->updateAllNotes();
}

void MainWindow::setEnvLinesStatus(bool value) {
    bShowEnvLines = value;
    mainWidget->Interface()->updateAllNotes();
}

void MainWindow::setEnvFillStatus(bool value) {
    bShowEnvFill = value;
    mainWidget->Interface()->updateAllNotes();
}

// 更新音长单位
void MainWindow::setLengthUnit(int value) {
    snappingUnit = value;
    mainWidget->Interface()->update();
}

// 设置语言
void MainWindow::setLanguage(const int n) {
    languageIndex = n;
}

void MainWindow::setCodeLocale(const int n) {
    if (localeIndex == n) {
        return;
    } else if (localeIndex == 0 && n == 1) {
        localeIndex = n;
        for (int i = 0; i < NotesArea::Stage->notesList.size(); ++i) {
            SimpleNote *p = NotesArea::Stage->notesList.at(i);
            p->m_note.m_lyric = JisToSystem(p->m_note.m_lyric);
            p->updateNoteStatus();
        }
    } else {
        localeIndex = n;
        for (int i = 0; i < NotesArea::Stage->notesList.size(); ++i) {
            SimpleNote *p = NotesArea::Stage->notesList.at(i);
            p->m_note.m_lyric = systemToJis(p->m_note.m_lyric);
            p->updateNoteStatus();
        }
    }
}
