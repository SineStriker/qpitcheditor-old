#include "mainwindow.h"

#include <QAudioFormat>
#include <QAudioOutput>

QSound *MainWindow::sound = nullptr;
QMediaPlayer *MainWindow::player = nullptr;

QVector<QStringList> MainWindow::resamplerArguments = {};
QVector<QStringList> MainWindow::wavtoolArguments = {};

QString MainWindow::renderingDir;
QString MainWindow::tempAudioName;
QString MainWindow::cacheFolderName;

QPoint MainWindow::playingRange;

void MainWindow::initMediaComponents() {
    resamplerArguments = {};
    wavtoolArguments = {};

    renderingDir = getWorkPath() + "\\synthesize";
    tempAudioName = "temp.wav";
    cacheFolderName = "cache";

    playingRange = QPoint(0, 0);

    player = new QMediaPlayer();
    player->setNotifyInterval(20);

    connect(player, &QMediaPlayer::stateChanged, this, &MainWindow::onSoundStateChanged);
    connect(player, &QMediaPlayer::positionChanged, mainWidget->Interface(),
            &NotesArea::onPositionChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onPlayerStatusChanged);
}

// 渲染音频
int MainWindow::renderAudio() {

    if (player && player->state() != QMediaPlayer::StoppedState) {
        return playAudio();
    }

    QPoint range;

    // 选中连续区域并保存音符信息
    if (stageSelectedNotes.empty() || !mainWidget || !mainWidget->Interface()) {
        return 0;
    } else {
        range = mainWidget->Interface()->selectContinuousNotes();
    }

    playingRange = range;

    // 初始化播放
    qDebug() << "Initialize Rendering...";

    // 创建工作文件夹
    violentMakeDir(renderingDir);

    int batProcessCode;
    int code = -1;

    RenderDialog *dlg = nullptr;     // Windows 进程
    BackstageDialog *dlg2 = nullptr; // QProcess 多线程

    // 产生批处理文件
    batProcessCode = RENDER_InitializeProcess();

    if (batProcessCode == 0) {
        // 产生批处理文件成功
        if (renderMode > 0) {
            dlg2 = new BackstageDialog();

            dlg2->setThreads(renderMode);
            dlg2->setNormalBegin(0);

            code = dlg2->exec();

            if (dlg2) {
                dlg2->release();
                dlg2 = nullptr;
            }
        } else {
            dlg = new RenderDialog();
            code = dlg->exec();

            if (dlg) {
                dlg->release();
                dlg = nullptr;
            }
        }
    } else if (batProcessCode == -1) {
        // 未知错误
        QString title = childWindowTitle;
        QString content = tr("Unknown Exception.");

        QMessageBox::warning(NULL, title, content);
        return 0;

    } else if (batProcessCode == -2) {

        // 无法写入文件
        QString title = childWindowTitle;
        QString content = tr("Unable to write file, please check permissions.");

        QMessageBox::warning(NULL, title, content);
        return 0;
    }

    if (code != 0) {
        qDebug() << "Render failed.";
        return 0;
    }

    qDebug() << "Rendering over.";

    // 播放前准备
    QString audioFilePath = renderingDir + "\\" + tempAudioName;

    if (!isFileExist(audioFilePath)) {
        QString title = childWindowTitle;
        QString content = tr("Wave file rendering failed, please check the settings.");

        QMessageBox::warning(NULL, title, content);
        return 0;
    }

    playWavFile(audioFilePath);

    return 1;
}

// 播放音频
int MainWindow::playAudio() {
    if (!player) {
        return 0;
    }

    if (player->state() == QMediaPlayer::PausedState) {
        player->play();
    }

    return 1;
}

int MainWindow::pauseAudio() {
    if (!player) {
        return 1;
    }

    if (player->state() == QMediaPlayer::PlayingState) {
        player->pause();
    }

    return 1;
}

int MainWindow::stopAudio() {
    if (!player) {
        return 1;
    }

    if (player->state() != QMediaPlayer::StoppedState) {
        player->stop();
    }

    return 1;
}

int MainWindow::replayAudio() {
    if (!player) {
        return 0;
    }

    player->setPosition(0);
    player->play();

    return 1;
}

void MainWindow::updatePlayButtons() {
    if (mainWidget && mainWidget->Editor()) {
        if (mainWidget->Editor()->isVisible()) {
            mainWidget->EditorPanel()->updateButtons();
        }
    }
}

void MainWindow::onSoundStateChanged(QMediaPlayer::State newState) {

    // 同样发送一个事件
    mainWidget->Interface()->onPositionChanged(player->position());

    if (newState == QMediaPlayer::StoppedState) {
        if (player->mediaStatus() == QMediaPlayer::EndOfMedia) {
            qDebug() << "Media Complete";

            setPlayMode(false);
            updatePlayButtons();
        } else {
            qDebug() << "Media Stop";

            setPlayMode(false);
            updatePlayButtons();
        }
    } else if (newState == QMediaPlayer::PlayingState) {
        qDebug() << "Media Play";

        setPlayMode(true);
        updatePlayButtons();
    } else if (newState == QMediaPlayer::PausedState) {
        qDebug() << "Media Pause";

        setPlayMode(false);
        updatePlayButtons();
    }
}

void MainWindow::onPlayerStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::InvalidMedia) {
        QString title = childWindowTitle;
        QString content = tr("Wave file format error, please check the settings.");

        QMessageBox::warning(NULL, title, content);
    }
}

int MainWindow::playWavFile(QString fileName) {

    if (!player) {
        return 0;
    }

    // 设置空
    player->setMedia(QMediaContent());

    // 加载声音
    player->setMedia(QMediaContent(QUrl::fromLocalFile(fileName)));

    // 播放声音
    player->setVolume(stageVolume);
    player->play();

    // 第一次播放
    setPlayMode(true);
    updatePlayButtons();

    return replayAudio();
}
