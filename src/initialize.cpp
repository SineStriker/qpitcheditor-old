#include "mainwindow.h"

QMenu *MainWindow::fileMenu = nullptr;
QAction *MainWindow::exitFileAction = nullptr;
QAction *MainWindow::saveFileAction = nullptr;
QAction *MainWindow::restoreFileAction = nullptr;
QAction *MainWindow::exportFileAction = nullptr;
QAction *MainWindow::exportWaveAction = nullptr;

QMenu *MainWindow::editMenu = nullptr;
QAction *MainWindow::undoEditAction = nullptr;
QAction *MainWindow::redoEditAction = nullptr;
QAction *MainWindow::selectAllEditAction = nullptr;
QAction *MainWindow::deselectEditAction = nullptr;

QMenu *MainWindow::viewMenu = nullptr;
QAction *MainWindow::deleteEditAction = nullptr;
QAction *MainWindow::zoomInVerticallyAction = nullptr;
QAction *MainWindow::zoomOutVerticallyAction = nullptr;
QAction *MainWindow::zoomInHorizontallyAction = nullptr;
QAction *MainWindow::zoomOutHorizontallyAction = nullptr;
QAction *MainWindow::showPitchAction = nullptr;
QAction *MainWindow::showParaAction = nullptr;

QMenu *MainWindow::toolMenu = nullptr;
QAction *MainWindow::otherToolAction = nullptr;
QAction *MainWindow::noteToolAction = nullptr;
QAction *MainWindow::settingToolAction = nullptr;

QMenu *MainWindow::helpMenu = nullptr;
QAction *MainWindow::instructHelpAction = nullptr;
QAction *MainWindow::checkUpdateAction = nullptr;
QAction *MainWindow::aboutThisAction = nullptr;
QAction *MainWindow::aboutQtAction = nullptr;

QMenu *MainWindow::adsorbMidAction = nullptr;
QActionGroup *MainWindow::adsorbActionGroup = nullptr;
QAction *MainWindow::fullAdsorbAction = nullptr;
QAction *MainWindow::halfAdsorbAction = nullptr;
QAction *MainWindow::quarterAdsorbAction = nullptr;
QAction *MainWindow::sixthAdsorbAction = nullptr;
QAction *MainWindow::eighthAdsorbAction = nullptr;
QAction *MainWindow::twelfthAdsorbAction = nullptr;
QAction *MainWindow::sixteenthAdsorbAction = nullptr;
QAction *MainWindow::twentyForthAdsorbAction = nullptr;
QAction *MainWindow::thirtySecondAdsorbAction = nullptr;
QAction *MainWindow::noAdsorbAction = nullptr;

QMenu *MainWindow::previewMidAction = nullptr;
QAction *MainWindow::playPreviewAction = nullptr;
QAction *MainWindow::replayPreviewAction = nullptr;
QAction *MainWindow::stopPreviewAction = nullptr;
QAction *MainWindow::moveToStartPreviewAction = nullptr;
QAction *MainWindow::moveToEndPreviewAction = nullptr;
QAction *MainWindow::removeCacheAction = nullptr;

QMenu *MainWindow::beatMidAction = nullptr;
QActionGroup *MainWindow::beatActionGroup = nullptr;
QAction *MainWindow::fourInFourBeatAction = nullptr;
QAction *MainWindow::threeInFourBeatAction = nullptr;

QColor MainWindow::normalTextColor = Qt::white;

/*--------------------------------------菜单栏设置--------------------------------------*/
void MainWindow::initMenuBar() {

    // 添加菜单项
    fileMenu = new QMenu(tr("File(&F)"), menuBar());
    editMenu = new QMenu(tr("Edit(&E)"), menuBar());
    viewMenu = new QMenu(tr("View(&V)"), menuBar());
    toolMenu = new QMenu(tr("Tools(&T)"), menuBar());
    helpMenu = new QMenu(tr("Help(&H)"), menuBar());

    // 初始化菜单项 File
    saveFileAction = new QAction(tr("Save and exit"), menuBar());
    saveFileAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    saveFileAction->setStatusTip(tr("Feed contents in QPitchEditor back to UTAU and exit."));
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::saveTriggered);

    exportFileAction = new QAction(tr("Export to UST file"), menuBar());
    exportFileAction->setStatusTip(tr("Export contents in QPitchEditor to UST file."));
    connect(exportFileAction, &QAction::triggered, this, &MainWindow::exportFileTriggered);

    exportWaveAction = new QAction(tr("Export previous audio"), menuBar());
    exportWaveAction->setShortcut(QKeySequence("Ctrl+P"));
    exportWaveAction->setStatusTip(tr("Export contents in QPitchEditor to UST file."));
    connect(exportWaveAction, &QAction::triggered, this, &MainWindow::exportWaveTriggered);

    restoreFileAction = new QAction(tr("Restore"), menuBar());
    restoreFileAction->setStatusTip(tr("Restore to the original state."));
    connect(restoreFileAction, &QAction::triggered, this, &MainWindow::restoreFileTriggered);

    exitFileAction = new QAction(tr("Exit"), menuBar());
    exitFileAction->setShortcut(QKeySequence("Ctrl+Q"));
    exitFileAction->setStatusTip(tr("Quit QPitchEditor without saving."));
    connect(exitFileAction, &QAction::triggered, this, &MainWindow::close);

    // 初始化菜单项 Edit
    undoEditAction = new QAction(tr("Undo"), menuBar());
    undoEditAction->setShortcut(QKeySequence::Undo);
    undoEditAction->setStatusTip(tr("Undo"));
    connect(undoEditAction, &QAction::triggered, this, &MainWindow::undoTriggered);

    redoEditAction = new QAction(tr("Redo"), menuBar());
    redoEditAction->setShortcuts(
        QList<QKeySequence>{QKeySequence::Redo, QKeySequence("Ctrl+Shift+Z")});
    redoEditAction->setStatusTip(tr("Redo"));
    connect(redoEditAction, &QAction::triggered, this, &MainWindow::redoTriggered);

    selectAllEditAction = new QAction(tr("Select all"), menuBar());
    selectAllEditAction->setShortcut(QKeySequence::SelectAll);
    selectAllEditAction->setStatusTip(tr("Select all notes."));
    connect(selectAllEditAction, &QAction::triggered, this, &MainWindow::seleteAllTriggered);

    deselectEditAction = new QAction(tr("Deselect"), menuBar());
    deselectEditAction->setStatusTip(tr("Cancel the selection."));
    connect(deselectEditAction, &QAction::triggered, this, &MainWindow::deselectTriggered);

    deleteEditAction = new QAction(tr("Delete selection"), menuBar());
    deleteEditAction->setShortcuts(
        QList<QKeySequence>{QKeySequence::Delete, QKeySequence(Qt::Key_Backspace)});
    deleteEditAction->setStatusTip(tr("Delete the selected control points."));
    connect(deleteEditAction, &QAction::triggered, this, &MainWindow::deleteTriggered);

    // 初始化菜单项 View
    zoomInVerticallyAction = new QAction(tr("Zoom in vertically"), menuBar());
    zoomInVerticallyAction->setShortcut(QKeySequence("Ctrl+="));
    zoomInVerticallyAction->setStatusTip(tr("Zoom in vertically."));
    connect(zoomInVerticallyAction, &QAction::triggered, this,
            &MainWindow::zoomInVerticallyTriggered);

    zoomOutVerticallyAction = new QAction(tr("Zoom out vertically"), menuBar());
    zoomOutVerticallyAction->setShortcut(QKeySequence("Ctrl+-"));
    zoomOutVerticallyAction->setStatusTip(tr("Zoom out vertically."));
    connect(zoomOutVerticallyAction, &QAction::triggered, this,
            &MainWindow::zoomOutVerticallyTriggered);

    zoomInHorizontallyAction = new QAction(tr("Zoom in horizontally"), menuBar());
    zoomInHorizontallyAction->setShortcut(QKeySequence("Ctrl+Shift+="));
    zoomInHorizontallyAction->setStatusTip(tr("Zoom in horizontally."));
    connect(zoomInHorizontallyAction, &QAction::triggered, this,
            &MainWindow::zoomInHorizontallyTriggered);

    zoomOutHorizontallyAction = new QAction(tr("Zoom out horizontally"), menuBar());
    zoomOutHorizontallyAction->setShortcut(QKeySequence("Ctrl+Shift+-"));
    zoomOutHorizontallyAction->setStatusTip(tr("Zoom out horizontally."));
    connect(zoomOutHorizontallyAction, &QAction::triggered, this,
            &MainWindow::zoomOutHorizontallyTriggered);

    showPitchAction = new QAction(tr("Show vibrato"), menuBar());
    showPitchAction->setShortcut(QKeySequence(Qt::Key_X));
    showPitchAction->setStatusTip(tr("Show vibrato curves on notes."));
    // connect(showPitchAction, &QAction::triggered, this, &MainWindow::showPitchBend);

    showParaAction = new QAction(tr("Show parameters"), menuBar());
    showParaAction->setShortcut(QKeySequence(Qt::Key_Z));
    showParaAction->setStatusTip(tr("Show parameters on notes."));
    // connect(showParaAction, &QAction::triggered, this, &MainWindow::showParameter);

    // 初始化菜单项 Tools
    otherToolAction = new QAction(tr("Other tools"), menuBar());
    otherToolAction->setStatusTip(tr("Other specific manipulations to the notes."));
    // connect(otherToolAction, &QAction::triggered, this, &MainWindow::showParameter);

    noteToolAction = new QAction(tr("Note Properties"), menuBar());
    noteToolAction->setShortcut(QKeySequence("Ctrl+E"));
    noteToolAction->setStatusTip(tr("Open note properties panel."));
    connect(noteToolAction, &QAction::triggered, this, &MainWindow::noteToolTriggered);

    settingToolAction = new QAction(tr("Options"), menuBar());
    settingToolAction->setStatusTip(tr("Set render mode."));
    connect(settingToolAction, &QAction::triggered, this, &MainWindow::settingToolTriggered);

    // 初始化菜单项 Help
    instructHelpAction = new QAction(tr("Instructions"), menuBar());
    instructHelpAction->setStatusTip(tr("How to use QPitchEditor."));
    connect(instructHelpAction, &QAction::triggered, this, &MainWindow::instructionTriggered);

    // 初始化菜单项 Help
    checkUpdateAction = new QAction(tr("Check updates"), menuBar());
    checkUpdateAction->setStatusTip(tr("Check the update."));
    connect(checkUpdateAction, &QAction::triggered, this, &MainWindow::checkUpdateTriggered);

    aboutThisAction = new QAction(tr("About QPitchEditor(&A)"), menuBar());
    aboutThisAction->setStatusTip(tr("About QPitchEditor."));
    connect(aboutThisAction, &QAction::triggered, this, &MainWindow::aboueThisTriggered);

    aboutQtAction = new QAction(tr("About Qt"), menuBar());
    aboutQtAction->setStatusTip(tr("About Qt."));
    connect(aboutQtAction, &QAction::triggered, this, &MainWindow::aboutQtTriggered);

    // 初始化中间菜单项
    fullAdsorbAction = new QAction(tr("Quarter"), menuBar());
    connect(fullAdsorbAction, &QAction::triggered, this, &MainWindow::fullAdsorbTriggered);

    halfAdsorbAction = new QAction(tr("1/2 Quarter"), menuBar());
    connect(halfAdsorbAction, &QAction::triggered, this, &MainWindow::halfAdsorbTriggered);

    quarterAdsorbAction = new QAction(tr("1/4 Quarter"), menuBar());
    connect(quarterAdsorbAction, &QAction::triggered, this, &MainWindow::quarterAdsorbTriggered);

    sixthAdsorbAction = new QAction(tr("1/6 Quarter"), menuBar());
    connect(sixthAdsorbAction, &QAction::triggered, this, &MainWindow::sixthAdsorbTriggered);

    eighthAdsorbAction = new QAction(tr("1/8 Quarter"), menuBar());
    connect(eighthAdsorbAction, &QAction::triggered, this, &MainWindow::eighthAdsorbTriggered);

    twelfthAdsorbAction = new QAction(tr("1/12 Quarter"), menuBar());
    connect(twelfthAdsorbAction, &QAction::triggered, this, &MainWindow::twelfthAdsorbTriggered);

    sixteenthAdsorbAction = new QAction(tr("1/16 Quarter"), menuBar());
    connect(sixteenthAdsorbAction, &QAction::triggered, this,
            &MainWindow::sixteenthAdsorbTriggered);

    twentyForthAdsorbAction = new QAction(tr("1/24 Quarter"), menuBar());
    connect(twentyForthAdsorbAction, &QAction::triggered, this,
            &MainWindow::twentyForthAdsorbTriggered);

    thirtySecondAdsorbAction = new QAction(tr("1/32 Quarter"), menuBar());
    connect(thirtySecondAdsorbAction, &QAction::triggered, this,
            &MainWindow::thirtySecondAdsorbTriggered);

    noAdsorbAction = new QAction(tr("None"), menuBar());
    connect(noAdsorbAction, &QAction::triggered, this, &MainWindow::noAdsorbTriggered);

    // 预览菜单
    playPreviewAction = new QAction(tr("Play/Pause"), menuBar());
    playPreviewAction->setStatusTip(
        tr("Play or pause the sound.If the sound has stopped, then render the selection."));
    playPreviewAction->setShortcuts(
        QList<QKeySequence>{QKeySequence(Qt::Key_F5), QKeySequence(Qt::Key_Space)});
    connect(playPreviewAction, &QAction::triggered, this, &MainWindow::playPreviewTriggered);

    replayPreviewAction = new QAction(tr("Replay"), menuBar());
    replayPreviewAction->setStatusTip(tr("Play the sound from the beginning."));
    replayPreviewAction->setShortcuts(
        QList<QKeySequence>{QKeySequence("Shift+F5") /*, QKeySequence(Qt::Key_Return)*/});
    connect(replayPreviewAction, &QAction::triggered, this, &MainWindow::replayPreviewTriggered);

    stopPreviewAction = new QAction(tr("Stop"), menuBar());
    stopPreviewAction->setStatusTip(tr("Stop the sound."));
    stopPreviewAction->setShortcut(QKeySequence(Qt::Key_F7));
    connect(stopPreviewAction, &QAction::triggered, this, &MainWindow::stopPreviewTriggered);

    moveToStartPreviewAction = new QAction(tr("Move to start"), menuBar());
    moveToStartPreviewAction->setStatusTip(tr("Move to the first note of the rendered area."));
    connect(moveToStartPreviewAction, &QAction::triggered, this,
            &MainWindow::moveToStartPreviewTriggered);

    moveToEndPreviewAction = new QAction(tr("Move to end"), menuBar());
    moveToEndPreviewAction->setStatusTip(tr("Move to the last note of the rendered area."));
    connect(moveToEndPreviewAction, &QAction::triggered, this,
            &MainWindow::moveToEndPreviewTriggered);

    removeCacheAction = new QAction(tr("Remove cache"), menuBar());
    removeCacheAction->setStatusTip(tr("Remove all caches."));
    connect(removeCacheAction, &QAction::triggered, this, &MainWindow::removeCacheTriggered);

    // 节拍
    fourInFourBeatAction = new QAction(tr("4/4 Beats"), menuBar());
    fourInFourBeatAction->setStatusTip(tr("A measure consists of four quarter notes."));
    connect(fourInFourBeatAction, &QAction::triggered, this, &MainWindow::fourInFourBeatTriggered);

    threeInFourBeatAction = new QAction(tr("3/4 Beats"), menuBar());
    threeInFourBeatAction->setStatusTip(tr("A measure consists of three quarter notes."));
    connect(threeInFourBeatAction, &QAction::triggered, this,
            &MainWindow::threeInFourBeatTriggered);

    // 添加中间项
    adsorbMidAction = new QMenu(tr("Length Unit"), menuBar());
    adsorbActionGroup = new QActionGroup(menuBar());

    adsorbMidAction->addAction(fullAdsorbAction);
    adsorbMidAction->addAction(halfAdsorbAction);
    adsorbMidAction->addAction(quarterAdsorbAction);
    adsorbMidAction->addAction(sixteenthAdsorbAction);
    adsorbMidAction->addAction(eighthAdsorbAction);
    adsorbMidAction->addAction(twelfthAdsorbAction);
    adsorbMidAction->addAction(sixteenthAdsorbAction);
    adsorbMidAction->addAction(twentyForthAdsorbAction);
    adsorbMidAction->addAction(thirtySecondAdsorbAction);
    adsorbMidAction->addSeparator();
    adsorbMidAction->addAction(noAdsorbAction);

    beatMidAction = new QMenu(tr("Time Signature"), menuBar());
    beatActionGroup = new QActionGroup(menuBar());

    beatMidAction->addAction(fourInFourBeatAction);
    beatMidAction->addAction(threeInFourBeatAction);

    // 设置可选
    fullAdsorbAction->setCheckable(true);
    halfAdsorbAction->setCheckable(true);
    quarterAdsorbAction->setCheckable(true);
    sixteenthAdsorbAction->setCheckable(true);
    eighthAdsorbAction->setCheckable(true);
    twelfthAdsorbAction->setCheckable(true);
    sixteenthAdsorbAction->setCheckable(true);
    twentyForthAdsorbAction->setCheckable(true);
    thirtySecondAdsorbAction->setCheckable(true);
    noAdsorbAction->setCheckable(true);

    fourInFourBeatAction->setCheckable(true);
    threeInFourBeatAction->setCheckable(true);

    // 设置默认选项
    fullAdsorbAction->setChecked(true);

    fourInFourBeatAction->setChecked(true);

    // 加入菜单组
    fullAdsorbAction->setActionGroup(adsorbActionGroup);
    halfAdsorbAction->setActionGroup(adsorbActionGroup);
    quarterAdsorbAction->setActionGroup(adsorbActionGroup);
    sixteenthAdsorbAction->setActionGroup(adsorbActionGroup);
    eighthAdsorbAction->setActionGroup(adsorbActionGroup);
    twelfthAdsorbAction->setActionGroup(adsorbActionGroup);
    sixteenthAdsorbAction->setActionGroup(adsorbActionGroup);
    twentyForthAdsorbAction->setActionGroup(adsorbActionGroup);
    thirtySecondAdsorbAction->setActionGroup(adsorbActionGroup);
    noAdsorbAction->setActionGroup(adsorbActionGroup);

    fourInFourBeatAction->setActionGroup(beatActionGroup);
    threeInFourBeatAction->setActionGroup(beatActionGroup);

    // 添加预览组件
    previewMidAction = new QMenu(tr("Preview(&P)"), menuBar());

    previewMidAction->addAction(playPreviewAction);
    previewMidAction->addAction(replayPreviewAction);
    previewMidAction->addAction(stopPreviewAction);
    previewMidAction->addAction(moveToStartPreviewAction);
    previewMidAction->addAction(moveToEndPreviewAction);
    previewMidAction->addSeparator();
    previewMidAction->addAction(removeCacheAction);
    previewMidAction->addSeparator();
    previewMidAction->addAction(exportWaveAction);

    // 向菜单中添加这些项
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(exportFileAction);
    // fileMenu->addAction(exportWaveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(restoreFileAction); // 懒得做
    fileMenu->addAction(exitFileAction);

    editMenu->addAction(undoEditAction);
    editMenu->addAction(redoEditAction);
    editMenu->addSeparator();
    editMenu->addAction(deleteEditAction);
    editMenu->addSeparator();
    editMenu->addAction(selectAllEditAction);
    editMenu->addAction(deselectEditAction);

    viewMenu->addMenu(adsorbMidAction); // 中间菜单
    viewMenu->addSeparator();
    // viewMenu->addAction(fourInFourBeatAction);
    // viewMenu->addAction(threeInFourBeatAction);
    viewMenu->addMenu(beatMidAction); // 中间菜单
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInVerticallyAction);
    viewMenu->addAction(zoomOutVerticallyAction);
    viewMenu->addAction(zoomInHorizontallyAction);
    viewMenu->addAction(zoomOutHorizontallyAction);
    // viewMenu->addSeparator();
    // viewMenu->addAction(showPitchAction);
    // viewMenu->addAction(showParaAction);

    // toolMenu->addMenu(previewMidAction); // 中间菜单
    toolMenu->addSeparator();
    // toolMenu->addAction(otherToolAction);
    toolMenu->addAction(noteToolAction);
    toolMenu->addSeparator();
    toolMenu->addAction(settingToolAction);

    helpMenu->addAction(instructHelpAction);
    helpMenu->addSeparator();
    helpMenu->addAction(checkUpdateAction);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutThisAction);
    helpMenu->addAction(aboutQtAction);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(previewMidAction);
    menuBar()->addMenu(toolMenu);
    menuBar()->addMenu(helpMenu);

    menuBar()->setStyleSheet(menuStyleSheet);
}

// 加载字符串
void MainWindow::reInitializeStrings() {
    qDebug() << "MainWindow Retranslate";

    // 主菜单
    fileMenu->setTitle(tr("File(&F)"));
    editMenu->setTitle(tr("Edit(&E)"));
    viewMenu->setTitle(tr("View(&V)"));
    toolMenu->setTitle(tr("Tools(&T)"));
    helpMenu->setTitle(tr("Help(&H)"));

    // 初始化菜单项 File
    saveFileAction->setText(tr("Save and exit"));
    saveFileAction->setStatusTip(tr("Feed contents in QPitchEditor back to UTAU and exit."));
    exportFileAction->setText(tr("Export to UST file"));
    exportFileAction->setStatusTip(tr("Export contents in QPitchEditor to UST file."));
    exportWaveAction->setText(tr("Export previous audio"));
    exportWaveAction->setStatusTip(tr("Export contents in QPitchEditor to UST file."));
    restoreFileAction->setText(tr("Restore"));
    restoreFileAction->setStatusTip(tr("Restore to the original state."));
    exitFileAction->setText(tr("Exit"));
    exitFileAction->setStatusTip(tr("Quit QPitchEditor without saving."));
    undoEditAction->setText(tr("Undo"));
    undoEditAction->setStatusTip(tr("Undo"));
    redoEditAction->setText(tr("Redo"));
    redoEditAction->setStatusTip(tr("Redo"));
    selectAllEditAction->setText(tr("Select all"));
    selectAllEditAction->setStatusTip(tr("Select all notes."));
    deselectEditAction->setText(tr("Deselect"));
    deselectEditAction->setStatusTip(tr("Cancel the selection."));
    deleteEditAction->setText(tr("Delete selection"));
    deleteEditAction->setStatusTip(tr("Delete the selected control points."));

    // 初始化菜单项 View
    zoomInVerticallyAction->setText(tr("Zoom in vertically"));
    zoomInVerticallyAction->setStatusTip(tr("Zoom in vertically."));
    zoomOutVerticallyAction->setText(tr("Zoom out vertically"));
    zoomOutVerticallyAction->setStatusTip(tr("Zoom out vertically."));
    zoomInHorizontallyAction->setText(tr("Zoom in horizontally"));
    zoomInHorizontallyAction->setStatusTip(tr("Zoom in horizontally."));
    zoomOutHorizontallyAction->setText(tr("Zoom out horizontally"));
    zoomOutHorizontallyAction->setStatusTip(tr("Zoom out horizontally."));
    showPitchAction->setText(tr("Show vibrato"));
    showPitchAction->setStatusTip(tr("Show vibrato curves on notes."));
    showParaAction->setText(tr("Show parameters"));
    showParaAction->setStatusTip(tr("Show parameters on notes."));

    // 初始化菜单项 Tools
    otherToolAction->setText(tr("Other tools"));
    otherToolAction->setStatusTip(tr("Other specific manipulations to the notes."));
    noteToolAction->setText(tr("Note Properties"));
    noteToolAction->setStatusTip(tr("Open note properties panel."));
    settingToolAction->setText(tr("Options"));
    settingToolAction->setStatusTip(tr("Set render mode."));

    // 初始化菜单项 Help
    instructHelpAction->setText(tr("Instructions"));
    instructHelpAction->setStatusTip(tr("How to use QPitchEditor."));
    checkUpdateAction->setText(tr("Check updates"));
    checkUpdateAction->setStatusTip(tr("Check the update."));
    aboutThisAction->setText(tr("About QPitchEditor(&A)"));
    aboutThisAction->setStatusTip(tr("About QPitchEditor."));
    aboutQtAction->setText(tr("About Qt"));
    aboutQtAction->setStatusTip(tr("About Qt."));

    // 初始化中间菜单项
    fullAdsorbAction->setText(tr("Quarter"));
    halfAdsorbAction->setText(tr("1/2 Quarter"));
    quarterAdsorbAction->setText(tr("1/4 Quarter"));
    sixthAdsorbAction->setText(tr("1/6 Quarter"));
    eighthAdsorbAction->setText(tr("1/8 Quarter"));
    twelfthAdsorbAction->setText(tr("1/12 Quarter"));
    sixteenthAdsorbAction->setText(tr("1/16 Quarter"));
    twentyForthAdsorbAction->setText(tr("1/24 Quarter"));
    thirtySecondAdsorbAction->setText(tr("1/32 Quarter"));
    noAdsorbAction->setText(tr("None"));

    playPreviewAction->setText(tr("Play/Pause"));
    playPreviewAction->setStatusTip(
        tr("Play or pause the sound.If the sound has stopped, then render the selection."));
    replayPreviewAction->setText(tr("Replay"));
    replayPreviewAction->setStatusTip(tr("Play the sound from the beginning."));
    stopPreviewAction->setText(tr("Stop"));
    stopPreviewAction->setStatusTip(tr("Stop the sound."));
    moveToStartPreviewAction->setText(tr("Move to start"));
    moveToStartPreviewAction->setStatusTip(tr("Move to the first note of the rendered area."));
    moveToEndPreviewAction->setText(tr("Move to end"));
    moveToEndPreviewAction->setStatusTip(tr("Move to the last note of the rendered area."));
    removeCacheAction->setText(tr("Remove cache"));
    removeCacheAction->setStatusTip(tr("Remove all caches."));

    fourInFourBeatAction->setText(tr("4/4 Beats"));
    fourInFourBeatAction->setStatusTip(tr("A measure consists of four quarter notes."));
    threeInFourBeatAction->setText(tr("3/4 Beats"));
    threeInFourBeatAction->setStatusTip(tr("A measure consists of three quarter notes."));

    adsorbMidAction->setTitle(tr("Length Unit"));
    beatMidAction->setTitle(tr("Time Signature"));
    previewMidAction->setTitle(tr("Preview(&P)"));

    // 后续操作
    updateHistoryActions();
    updateDeleteAction();
    updateDeselectAction();

    qDebug() << "retrans over 1";

    // 子对象
    mainWidget->reInitializeStrings();
    qDebug() << "retrans over";
}

/*--------------------------------------菜单栏响应对象--------------------------------------*/

void MainWindow::saveTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setChanged(false);

    mainWidget->Interface()->feedbackNotes();

    if (!aData.ExportTotal(aTempFile)) {
        QString content = QObject::tr("Unable to write file, please check permissions.");
        msgboxExclaim(content);
        return;
    }
    close();
}

void MainWindow::exportFileTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    QString Dialog_Filter_Save = tr("UTAU Script Files(*.ust);;All Files(*.*)");
    QString fileName = tr("Project.ust");

    if (!aProjectPath.isEmpty()) {
        fileName = PathFindFileName(aProjectPath);
    }

    QString path =
        QFileDialog::getSaveFileName(this, tr("Export UST file"), fileName, Dialog_Filter_Save);

    // 写入 settings
    QUtauSectionSettings *aSettings = MainWindow::aData.SectionSettings();

    // 全局Flags
    if (!aGlobalFlags.isEmpty()) {
        aSettings->SetValue(KEY_NAME_FLAGS, aGlobalFlags);
    }
    // 引擎
    aSettings->SetValue("Tool1", aTool1Path);
    aSettings->SetValue("Tool2", aTool2Path);

    if (!path.isEmpty()) {
        mainWidget->Interface()->feedbackNotes();
        if (!aData.ExportToUst(path)) {
            QString content = QObject::tr("Unable to write file, please check permissions.");

            msgboxExclaim(content);
            return;
        }
    }
}

void MainWindow::exportWaveTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    QString tempFileName = renderingDir + "\\" + tempAudioName;

    if (!isFileExist(tempFileName)) {
        QString content = QObject::tr("No audio files have been generated recently.");
        msgboxExclaim(content);
        return;
    }

    QString Dialog_Filter_ExportWav = tr("Wave Files(*.wav);;All Files(*.*)");
    QString path = QFileDialog::getSaveFileName(this, tr("Export wave file"), tempAudioName,
                                                Dialog_Filter_ExportWav);

    if (!path.isEmpty()) {
        if (!QFile::copy(tempFileName, path)) {
            QString content = QObject::tr("Unable to copy file, please check permissions.");
            msgboxExclaim(content);
            return;
        }
    } else {
        // QMessageBox::warning(this, tr("Path"), tr("You canceled the saving."));
    }

    qDebug() << "Save path : " << path;
}

void MainWindow::restoreFileTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    removeAllSelectedNotes();
    removeAllSelectedPoints();

    while (!isEarliestStep()) {
        historyPrevStep();
    }
    while (!isLatestStep()) {
        historyList.back()->release();
        historyList.pop_back();
    }
    updateHistoryActions();
}

void MainWindow::undoTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    removeAllSelectedNotes();
    removeAllSelectedPoints();
    historyPrevStep();
}

void MainWindow::redoTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    removeAllSelectedNotes();
    removeAllSelectedPoints();
    historyNextStep();
}

void MainWindow::deleteTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    // 删除所有控制点
    if (stageSelectedPoints.empty()) {
        return;
    }

    QVector<SimplePoint *> arr;

    for (auto iter = stageSelectedPoints.begin(); iter != stageSelectedPoints.end(); ++iter) {
        (*iter)->setSelected(false); // 先取消选择
        arr.push_back(*iter);
    }
    stageSelectedPoints.clear();

    if (arr[0]->getScopeArea()) {
        arr[0]->getScopeArea()->removePoints(arr);
    }
    removeAllSelectedNotes();
}

void MainWindow::seleteAllTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    if (MainWindow::stageDraggedObjects.empty() && !mainWidget->Interface()->isCropActive()) {
        mainWidget->Interface()->selectAllNotes();
    }
}

void MainWindow::deselectTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    removeAllSelectedNotes();
    removeAllSelectedPoints();
}

/*--------------------------------------菜单事件--------------------------------------*/


void MainWindow::instructionTriggered() {
}

void MainWindow::checkUpdateTriggered() {
    checkUpdate(true);
}


// 显示关于此
void MainWindow::aboueThisTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    QMessageBox::about(this, QString::fromLocal8Bit("关于"),
                       QString::fromLocal8Bit(R"(<h2>QPitchEditor for UTAU</h2>
                                                <p>Copyright Sinestr. 2020-2021 <a href="https://space.bilibili.com/87172593">(主页)</a></p>

                                                <h3>关于</h3>
                                                <p>本程序是为简化UTAU绘制音高线而设计的辅助工具，以<span style="font-weight: bold;">UTAU插件形式</span>发布。</p>

                                                <h3>许可</h3>
                                                <p>本程序的发布旨在能够派上用场，<span style="font-weight: bold;">但并不对此做出任何担保</span>。</p>
                                                <p>本程序是共享免费软件，您可以免费使用本程序，也可以<span style="font-weight: bold;">在不修改本程序任何原始文件的条件下</span>自由复制、分发本程序。</p>

                                                <h3>本程序使用的开源软件库</h3>
                                                <ul>
                                                <li>Qt, The Qt Company Ltd, under LGPL v3.</li>
                                                <li>LibUTAU 216, Copyright (C) 2010-2011 by SHINTA（翔星 P）,under Attribution-NonCommercial-Inheritance 2.1</li>
                                                </ul>
                                                )"));
}

// 显示关于Qt
void MainWindow::aboutQtTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    StylizedDialog::aboutQt(this, tr("About Qt"));
}

// 显示选项

void MainWindow::noteToolTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    // 必须选中一个有效的音符
    bool flag = false;
    for (auto iter = stageSelectedNotes.begin(); iter != stageSelectedNotes.end(); ++iter) {
        if (!(*iter)->isRest()) {
            flag = true;
        }
    }

    if (flag) {
        showNoteProperties();
    }
}

void MainWindow::settingToolTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    showOptions();
}

// 小节分子分母
void MainWindow::fourInFourBeatTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    mainWidget->Editor()->Scaleplate()->setBeat(4, 4);
}

void MainWindow::threeInFourBeatTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    mainWidget->Editor()->Scaleplate()->setBeat(3, 4);
}

// 缩放
void MainWindow::zoomInVerticallyTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    mainWidget->Editor()->linkZoomInVertically();
}

void MainWindow::zoomOutVerticallyTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    mainWidget->Editor()->linkZoomOutVertically();
}

void MainWindow::zoomInHorizontallyTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    mainWidget->Editor()->linkZoomInHorizontally();
}

void MainWindow::zoomOutHorizontallyTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    mainWidget->Editor()->linkZoomOutHorizontally();
}

// 吸附单位
void MainWindow::fullAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(1);
}
void MainWindow::halfAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(2);
}
void MainWindow::quarterAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(4);
}
void MainWindow::sixthAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(6);
}
void MainWindow::eighthAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(8);
}
void MainWindow::twelfthAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(12);
}
void MainWindow::sixteenthAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(16);
}
void MainWindow::twentyForthAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(24);
}
void MainWindow::thirtySecondAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(32);
}
void MainWindow::noAdsorbTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    setLengthUnit(0);
}

// 预览组件
void MainWindow::playPreviewTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }

    if (mainWidget->Editor()->isVisible()) {
        mainWidget->EditorPanel()->onPlayBtnClick();
    }
}

void MainWindow::replayPreviewTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    if (mainWidget->Editor()->isVisible()) {
        replayAudio();
    }
}

void MainWindow::stopPreviewTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }

    if (mainWidget->Editor()->isVisible()) {
        mainWidget->EditorPanel()->onStopBtnClick();
    }
}

void MainWindow::moveToStartPreviewTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    if (mainWidget->Editor()->isVisible()) {
        mainWidget->EditorPanel()->onMoveToStartBtnClick();
    }
}

void MainWindow::moveToEndPreviewTriggered() {
    if (!stageDraggedObjects.empty()) {
        return;
    }
    if (mainWidget->Editor()->isVisible()) {
        mainWidget->EditorPanel()->onMoveToEndBtnClick();
    }
}

void MainWindow::removeCacheTriggered() {
    violentDelDir(renderingDir + "\\" + cacheFolderName);
}

void MainWindow::modifyAdsorbOptions(int n) {
    qDebug() << "Modify" << n;
    cancelAllAdsorbActions();
    switch (n) {
    case 1:
        fullAdsorbAction->setChecked(true);
        break;
    case 2:
        halfAdsorbAction->setChecked(true);
        break;
    case 4:
        quarterAdsorbAction->setChecked(true);
        break;
    case 8:
        eighthAdsorbAction->setChecked(true);
        break;
    case 12:
        twelfthAdsorbAction->setChecked(true);
        break;
    case 16:
        sixteenthAdsorbAction->setChecked(true);
        break;
    case 24:
        twentyForthAdsorbAction->setChecked(true);
        break;
    case 32:
        thirtySecondAdsorbAction->setChecked(true);
        break;
    default:
        noAdsorbAction->setChecked(true);
        n = 0;
        break;
    }
    setLengthUnit(n);
}

void MainWindow::cancelAllAdsorbActions() {
    fullAdsorbAction->setChecked(false);
    halfAdsorbAction->setChecked(false);
    quarterAdsorbAction->setChecked(false);
    sixteenthAdsorbAction->setChecked(false);
    eighthAdsorbAction->setChecked(false);
    twelfthAdsorbAction->setChecked(false);
    sixteenthAdsorbAction->setChecked(false);
    twentyForthAdsorbAction->setChecked(false);
    thirtySecondAdsorbAction->setChecked(false);
    noAdsorbAction->setChecked(false);
}

/*--------------------------------------菜单栏更新--------------------------------------*/
void MainWindow::updateHistoryActions() {
    if (undoEditAction) {
        if (!isEarliestStep()) {
            undoEditAction->setText(QString("%1%2%3").arg(
                tr("Undo"), " ", historyList[historyIndex - 1]->description()));
            undoEditAction->setEnabled(true);
        } else {
            undoEditAction->setText(tr("Undo"));
            undoEditAction->setEnabled(false);
        }
    }
    if (redoEditAction) {
        if (!isLatestStep()) {
            redoEditAction->setText(
                QString("%1%2%3").arg(tr("Redo"), " ", historyList[historyIndex]->description()));
            redoEditAction->setEnabled(true);
        } else {
            redoEditAction->setText(tr("Redo"));
            redoEditAction->setEnabled(false);
        }
    }

    if (isEarliestStep()) {
        setChanged(false);
    } else {
        setChanged(true);
    }
}

void MainWindow::updateDeleteAction() {
    if (stageSelectedPoints.empty()) {
        deleteEditAction->setEnabled(false);
    } else {
        deleteEditAction->setEnabled(true);
    }
}

void MainWindow::updateDeselectAction() {
    if (stageSelectedNotes.empty() && stageSelectedPoints.empty()) {
        deselectEditAction->setEnabled(false);
    } else {
        deselectEditAction->setEnabled(true);
    }
}

/*--------------------------------------程序设定--------------------------------------*/

// 检测语言
void MainWindow::detectLanguage() {
    QLocale locale;

    if (locale.language() == QLocale::Chinese) {
        languageIndex = 1; // 简体中文
    } else if (locale.language() == QLocale::Japanese) {
        languageIndex = 2; // 日语
    } else {
        languageIndex = 0; // 其余都用英语
    }
}

// 应用语言
void MainWindow::applyLanguage() {

    QString qmFile;

    if (mainTranslator) {
        App->removeTranslator(mainTranslator);
        mainTranslator->deleteLater();
        mainTranslator = nullptr;
        qDebug() << "After Delete.";
    }

    if (languageIndex == 0) {
        // qmFile = getWorkPath() + "\\translations\\QPitchEditor_EN.qm";
        return;
    } else if (languageIndex == 1) {
        // qmFile = getWorkPath() + "\\translations\\QPitchEditor_CN.qm";
        qmFile = ":/language/QPitchEditor_CN.qm";
    } else {
        // qmFile = getWorkPath() + "\\translations\\QPitchEditor_JP.qm";
        qmFile = ":/language/QPitchEditor_JP.qm";
    }

    mainTranslator = new QTranslator();

    if (!mainTranslator->load(qmFile)) {
        mainTranslator->deleteLater();
        mainTranslator = nullptr;
        return;
    }

    App->installTranslator(mainTranslator);
}

// 读取设定
void MainWindow::readSetting() {
    QFile file(getWorkPath() + "\\setting.txt");
    int eq;
    QString line, str;

    memoryIsMaximumed = false;
    memoryLengthUnit = -1;
    memoryWindowGeometry = noRect;
    detectLanguage();

    if (file.open(QFile::ReadOnly | QIODevice::Text)) {
        qDebug() << "General Settings Detected.";
        QTextStream txtInput(&file);

        while (!txtInput.atEnd()) {
            line = txtInput.readLine().simplified();

            if (line.startsWith("MainWindow=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 初始窗口的坐标与尺寸
                QStringList cords = str.split(",");
                QRect rect(cords[0].toInt(), cords[1].toInt(), cords[2].toInt(), cords[3].toInt());

                memoryWindowGeometry = rect;
                // setInitGeometry(rect);
            } else if (line.startsWith("Maximized=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 是否最大化
                /*if (str == "1") {
                    setWindowState(Qt::WindowMaximized);
                }*/
                memoryIsMaximumed = (str == "1");
            } else if (line.startsWith("Snapping=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 最小长度单位
                memoryLengthUnit = str.toInt();
                // modifyAdsorbOptions(str.toInt());
            } else if (line.startsWith("Language=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 语言
                languageIndex = str.toInt();
            } else if (line.startsWith("Charset=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 编码
                localeIndex = str.toInt();
            } else if (line.startsWith("Volume=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 音量
                stageVolume = str.toInt();
            } else if (line.startsWith("DoubleClickToAddPoint=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 是否双击添加控制点
                if (str == "0") {
                    doubleClickToAdd = false;
                }
            } else if (line.startsWith("RenderThreads=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 合成所用线程数
                renderMode = str.toInt();
            } else if (line.startsWith("ShowLength=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 合成所用线程数
                bReflect = (str == "1");
            } else if (line.startsWith("ShowOverlap=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 合成所用线程数
                bOverlap = (str == "1");
            } else if (line.startsWith("ShowParameters=")) {
                eq = line.indexOf("=") + 1;
                str = line.mid(eq, line.size() - eq);

                // 合成所用线程数
                bShowFlags = (str == "1");
            }
        }
        file.close();
    } else {
        qDebug() << "General Settings Missing.";
    }
}

// 保存程序自身信息
void MainWindow::saveSetting() {
    QFile file(getWorkPath() + "\\setting.txt");

    int strWindowState = (windowState() == Qt::WindowMaximized) ? 1 : 0;
    int strDoubleClickToAdd = doubleClickToAdd ? 1 : 0;
    int strShowReflect = bReflect ? 1 : 0;
    int strShowOverlap = bOverlap ? 1 : 0;
    int strShowFlags = bShowFlags ? 1 : 0;

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // 文件头
        out << "[QPitchEditor]" << endl;
        // 初始窗口的坐标与尺寸
        out << "MainWindow=" << geometry().x() << "," << geometry().y() << "," << width() << ","
            << height() << endl;
        // 是否最大化
        out << "Maximized=" << strWindowState << endl;
        // 最小长度单位
        out << "[General]" << endl;
        out << "Snapping=" << snappingUnit << endl;
        // 语言
        out << "Language=" << languageIndex << endl;
        // 编码
        out << "Charset=" << localeIndex << endl;
        // 音量
        out << "Volume=" << stageVolume << endl;

        // 是否双击加点
        out << "DoubleClickToAddPoint=" << strDoubleClickToAdd << endl;

        // 线程数
        out << "RenderThreads=" << renderMode << endl;

        out << "[View]" << endl;
        // 实际长度
        out << "ShowLength=" << strShowReflect << endl;
        // 重叠
        out << "ShowOverlap=" << strShowOverlap << endl;
        // 参数
        out << "ShowParameters=" << strShowFlags << endl;

        file.close();
    }

    qDebug() << "Setting Saved";
}

void MainWindow::applySetting() {
    if (memoryIsMaximumed) {
        setWindowState(Qt::WindowMaximized);
    } else if (memoryWindowGeometry != noRect) {
        setInitGeometry(memoryWindowGeometry);
    }

    if (memoryLengthUnit >= 0) {
        modifyAdsorbOptions(memoryLengthUnit);
    }
}

// 设置初始尺寸
void MainWindow::setInitGeometry(const QRect &rect) {
    this->setGeometry(rect);
    if (pos().x() < 0) {
        move(0, pos().y());
    }
    if (pos().y() < 0) {
        move(pos().x(), 0);
    }
}
