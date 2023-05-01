#include "EditorWidget.h"
#include "mainwindow.h"

/*--------------------------------------下面板中心区域--------------------------------------*/
EditorOuterArea::EditorOuterArea(QWidget *parent) : EditorArea(parent) {
    mainLayout = new QGridLayout(this);

    // 钢琴窗
    pianoScroll = new NoneScrollArea(this);
    piano = new PianoArea(pianoScroll);
    piano->setParent(this);

    pianoScroll->setWidget(piano);
    PianoArea::Stage = piano;

    // 标尺窗
    scaleplateScroll = new NoneScrollArea(this);
    scaleplate = new ScaleplateArea(scaleplateScroll);
    scaleplate->setParent(this);

    scaleplateScroll->setWidget(scaleplate);
    ScaleplateArea::Stage = scaleplate;

    // 主窗体
    mainScroll = new NotesScrollArea(this);
    mainArea = new NotesArea(mainScroll);
    mainArea->setParent(this); // 没用

    mainScroll->setFrameShape(QFrame::NoFrame);
    mainScroll->setWidget(mainArea);
    NotesArea::Stage = mainArea;
    NotesArea::Scroll = mainScroll;

    // 功能窗
    funcScroll = new BasicScrollArea(this);
    functions = new ListArea(funcScroll);
    functions->setParent(this);

    funcScroll->setFrameShape(QScrollArea::NoFrame);
    funcScroll->setWidget(functions);
    ListArea::Stage = functions;
    ListArea::Scroll = funcScroll;

    // 音量窗
    volumeArea = new VolumeForm(this);
    VolumeArea::Stage = Volume();

    // 用来分隔音量窗与音符窗
    mainSplitter = new FormSplitter(this);
    mainSplitter->setTopWidget(mainScroll);
    mainSplitter->addForm(volumeArea);
    mainSplitter->setStretchFactor(mainScroll, 1);
    mainSplitter->setStretchFactor(volumeArea, 1);
    mainSplitter->setSizes({3000, 1000}); // 强行设置比例

    // volumeArea->setFormVisible(false);

    // 空窗体用来占位置
    emptyRow = new QWidget(this);
    emptyRect = new QWidget(this);
    emptyRect2 = new QWidget(this);

    mainLayout->addWidget(emptyRow, 0, 0, 1, 2);
    mainLayout->addWidget(emptyRect, 1, 0);
    mainLayout->addWidget(emptyRect2, 0, 2, 1, 1);
    mainLayout->addWidget(scaleplateScroll, 1, 1);
    mainLayout->addWidget(pianoScroll, 2, 0, 2, 1);
    // mainLayout->addWidget(mainScroll, 2, 1);
    // mainLayout->addWidget(volumeArea, 3, 1);
    mainLayout->addWidget(mainSplitter, 2, 1, 2, 1);

    mainLayout->addWidget(funcScroll, 1, 2, 3, 1);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    connect(mainArea, &NotesArea::onMoveEvent, this, &EditorOuterArea::posLinkChange);
    connect(mainArea, &NotesArea::onSumChanged, this, &EditorOuterArea::sumLinkChange);
    connect(mainArea, &NotesArea::onCropDrawing, this, &EditorOuterArea::cropTimerEvent);
    connect(mainArea, &NotesArea::onCanvasMoving, this, &EditorOuterArea::canvasLinkEvent);
    connect(mainArea, &NotesArea::onPlayheadMoving, this, &EditorOuterArea::playheadLinkChange);
    connect(mainArea, &NotesArea::onCheckOnStage, this, &EditorOuterArea::checkShowOnStage);

    connect(functions, &ListArea::onReizeEvent, this, &EditorOuterArea::sizeLinkChange);

    barTimer = new QTimer(this);
    connect(barTimer, &QTimer::timeout, this, &EditorOuterArea::cropTimerEvent);

    // 初始化
    emptyRow->setFixedHeight(15);

    pianoScroll->setFixedWidth(piano->width());
    scaleplateScroll->setFixedHeight(15);

    mainArea->zoomResize(scaleplate->width(), piano->height());
    Volume()->zoomResize(scaleplate->width());

    functions->setFixedWidth(175);

    updateSingleStep();
    setShowFunctions(false);
}

void EditorOuterArea::paintEvent(QPaintEvent *) {
}

// 滚轮移动
void EditorOuterArea::wheelEvent(QWheelEvent *event) {

    if (!Root->stageDraggedObjects.isEmpty() || mainArea->isMoving()) {
        return;
    }

    QPoint mainPos = mainScroll->mapFromGlobal(QCursor::pos());
    QPoint scalePos = scaleplateScroll->mapFromGlobal(QCursor::pos());
    QPoint pianoPos = pianoScroll->mapFromGlobal(QCursor::pos());
    QPoint listPos = funcScroll->mapFromGlobal(QCursor::pos());
    QPoint volumePos = volumeArea->mapFromGlobal(QCursor::pos());

    if (funcScroll->rect().contains(listPos)) {
        return;
    }

    Qt::KeyboardModifiers c = QApplication::keyboardModifiers(); // 当前辅助组合键
    int para = event->angleDelta().y();                          // 滚轮移动方向

    if (c == Root->zoomHorizontallyKey) {
        // 框选或编辑歌词时不准缩放
        if (mainArea->isCropActive() || mainArea->isLyricEditing()) {
            return;
        }
        if (para > 0) {
            linkZoomInHorizontally();
        } else {
            linkZoomOutHorizontally();
        }
    } else if (c == Root->zoomVerticallyKey) {
        // 框选或编辑歌词时不准缩放
        if (mainArea->isCropActive() || mainArea->isLyricEditing() ||
            volumeArea->rect().contains(volumePos)) {
            return;
        }
        if (para > 0) {
            linkZoomInVertically();
        } else {
            linkZoomOutVertically();
        }
    } else if (c == Root->moveHorizontallyKey) {
        if (para > 0) {
            linkMoveLeft();
        } else {
            linkMoveRight();
        }
    } else if (c == Root->moveVerticallyKey) {
        if (para > 0) {
            linkMoveUp();
        } else {
            linkMoveDown();
        }
    }
}

// 移动联动
void EditorOuterArea::posLinkChange() {
    scaleplate->move(mainArea->x(), scaleplate->y());
    piano->move(piano->x(), mainArea->y());

    Volume()->move(mainArea->x(), Volume()->y()); // 移动音量

    // 移动播放头
    PlayHead *p = mainArea->getPlayhead();
    p->setGeometry(p->x(), -mainArea->y(), p->width(), mainScroll->height());
}

void EditorOuterArea::sizeLinkChange() {
    if (!showFunctions) {
        return;
    }
    funcScroll->setFixedWidth(funcScroll->frameWidth() * 2 + functions->width() + 9);
}

void EditorOuterArea::sumLinkChange(int n) {
    int sectionCount = n / 480 / 4;
    int num = sectionCount + 40;

    if (num < minimumSections) {
        num = minimumSections;
    }

    if (scaleplate->setScaleplates(num)) {
        mainArea->setFixedWidth(scaleplate->width());
    }
}

void EditorOuterArea::cropLinkChange() {
    if (!mainScroll) {
        return;
    }

    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    QPoint cursor = QCursor::pos();
    QPoint scrollCursor = mainScroll->mapFromGlobal(cursor);
    QScrollBar *bar;

    int value;
    int offset;
    double ratio = 0.5;
    QRect frameSize(20, 20, 20, 20);

    if (c != Qt::ControlModifier) {

        bar = mainScroll->horizontalScrollBar();
        value = bar->value();

        if (scrollCursor.x() > mainScroll->width() - frameSize.right()) {
            offset = scrollCursor.x() - (mainScroll->width() - frameSize.right());
            bar->setValue(value + offset * ratio);
        } else if (scrollCursor.x() < frameSize.left()) {
            offset = scrollCursor.x() - frameSize.left();
            bar->setValue(value + offset * ratio);
        }

        bar = mainScroll->verticalScrollBar();
        value = bar->value();

        if (scrollCursor.y() > mainScroll->height() - frameSize.bottom()) {
            offset = scrollCursor.y() - (mainScroll->height() - frameSize.bottom());
            bar->setValue(value + offset * ratio);
        } else if (scrollCursor.y() < frameSize.top()) {
            offset = scrollCursor.y() - frameSize.top();
            bar->setValue(value + offset * ratio);
        }
    }

    mainArea->updateCropArea();
}

void EditorOuterArea::cropTimerEvent() {
    if (mainArea->isCropActive()) {
        if (!barTimer->isActive()) {
            barTimer->start(10);
        }
        cropLinkChange();
    } else {
        barTimer->stop();
    }
}

void EditorOuterArea::canvasLinkEvent(QPoint p) {
    qDebug() << "Canvas" << p;
    mainScroll->horizontalScrollBar()->setValue(mainScroll->horizontalScrollBar()->value() - p.x());
    mainScroll->verticalScrollBar()->setValue(mainScroll->verticalScrollBar()->value() - p.y());
}

void EditorOuterArea::playheadLinkChange() {
    PlayHead *p = mainArea->getPlayhead();

    p->setGeometry(p->x(), -mainArea->y(), p->width(), mainScroll->height());
    p->update();

    checkShowOnStage(p);
}

void EditorOuterArea::checkShowOnStage(QWidget *w, bool right) {
    QScrollBar *bar = mainScroll->horizontalScrollBar();
    QPoint q = mainArea->mapTo(mainScroll, w->pos());

    if (q.x() < 0) {
        bar->setValue(w->x());
    } else if (q.x() + w->width() > mainScroll->viewport()->width()) {
        if (right) {
            bar->setValue(w->x() + w->width() - mainScroll->viewport()->width());
        } else {
            bar->setValue(w->x());
        }
    }
}

// 滚轮单位长度更新为单个半音高度与四分音符长度
void EditorOuterArea::updateSingleStep() {
    QScrollBar *bar = mainScroll->horizontalScrollBar();
    bar->setSingleStep(noteUnitWidthArr[Root->noteUnitWIndex] / 4);
    bar = mainScroll->verticalScrollBar();
    bar->setSingleStep(noteUnitHeightArr[Root->noteUnitHIndex]);
}

/*--------------------------------------区域横向缩放--------------------------------------*/
void EditorOuterArea::linkZoomInHorizontally() {
    if (Root->noteUnitWIndex < noteUnitWidthArr.size() - 1) {

        QPoint absPos = mainArea->mapFromGlobal(QCursor::pos());
        QPoint relativePos = mainScroll->mapFromGlobal(QCursor::pos());

        unsigned prev = noteUnitWidthArr[Root->noteUnitWIndex];
        unsigned next = noteUnitWidthArr[Root->noteUnitWIndex + 1];

        int fromX = relativePos.x();

        if (fromX < 0) {
            fromX = 0;
        } else if (fromX > mainScroll->width()) {
            fromX = mainScroll->width();
        }

        int fromAbs = absPos.x() - (relativePos.x() - fromX);
        int toX = fromX - fromAbs * double(next) / prev;

        if (toX > 0) {
            toX = 0;
        }

        // 改变全局单位
        Root->noteUnitWIndex++;

        // 标尺区域尺寸调整
        scaleplate->adjustSections();

        // 编辑区域尺寸调整
        mainArea->zoomResize(scaleplate->width(), piano->height(), true);
        Volume()->zoomResize(scaleplate->width());

        // 相对坐标调整
        QScrollBar *bar = mainScroll->horizontalScrollBar();
        bar->setValue(-toX);

        updateSingleStep();
    }
}

void EditorOuterArea::linkZoomOutHorizontally() {
    if (Root->noteUnitWIndex > 0) {

        // 保存先后的单位长度与鼠标指针坐标
        QPoint absPos = mainArea->mapFromGlobal(QCursor::pos());
        QPoint relativePos = mainScroll->mapFromGlobal(QCursor::pos());
        unsigned prev = noteUnitWidthArr[Root->noteUnitWIndex];
        unsigned next = noteUnitWidthArr[Root->noteUnitWIndex - 1];

        // 计算变化后widget的目标坐标
        int fromX = relativePos.x();

        if (fromX < 0) {
            fromX = 0;
        } else if (fromX > mainScroll->width()) {
            fromX = mainScroll->width();
        }

        int fromAbs = absPos.x() - (relativePos.x() - fromX);
        int toX = fromX - fromAbs * double(next) / prev;

        if (toX > 0) {
            toX = 0;
        }

        // 改变全局单位
        Root->noteUnitWIndex--;

        // 标尺区域尺寸调整
        scaleplate->adjustSections();

        // 编辑区域尺寸调整
        mainArea->zoomResize(scaleplate->width(), piano->height(), true);
        Volume()->zoomResize(scaleplate->width());

        // 相对坐标调整
        QScrollBar *bar = mainScroll->horizontalScrollBar();
        bar->setValue(-toX);

        updateSingleStep();
    }
}
/*--------------------------------------区域纵向缩放--------------------------------------*/
void EditorOuterArea::linkZoomInVertically() {
    if (Root->noteUnitHIndex < noteUnitHeightArr.size() - 1) {

        QPoint absPos = mainArea->mapFromGlobal(QCursor::pos());
        QPoint relativePos = mainScroll->mapFromGlobal(QCursor::pos());

        unsigned prev = noteUnitHeightArr[Root->noteUnitHIndex];
        unsigned next = noteUnitHeightArr[Root->noteUnitHIndex + 1];

        int fromY = relativePos.y();

        if (fromY < 0) {
            fromY = 0;
        } else if (fromY > mainScroll->height()) {
            fromY = mainScroll->height();
        }

        int fromAbs = absPos.y() - (relativePos.y() - fromY);
        int toY = fromY - fromAbs * double(next) / prev;

        if (toY > 0) {
            toY = 0;
        }

        // 改变全局单位
        Root->noteUnitHIndex++;

        // 钢琴区域尺寸调整
        piano->adjustHeight();

        // 编辑区域尺寸调整
        mainArea->zoomResize(scaleplate->width(), piano->height());

        // 相对坐标调整
        QScrollBar *bar = mainScroll->verticalScrollBar();
        bar->setValue(-toY);

        updateSingleStep();
    }
}

void EditorOuterArea::linkZoomOutVertically() {
    if (Root->noteUnitHIndex > 0) {

        QPoint absPos = mainArea->mapFromGlobal(QCursor::pos());
        QPoint relativePos = mainScroll->mapFromGlobal(QCursor::pos());

        unsigned prev = noteUnitHeightArr[Root->noteUnitHIndex];
        unsigned next = noteUnitHeightArr[Root->noteUnitHIndex - 1];

        int fromY = relativePos.y();

        if (fromY < 0) {
            fromY = 0;
        } else if (fromY > mainScroll->height()) {
            fromY = mainScroll->height();
        }

        int fromAbs = absPos.y() - (relativePos.y() - fromY);
        int toY = fromY - fromAbs * double(next) / prev;

        if (toY > 0) {
            toY = 0;
        }

        // 改变全局单位
        Root->noteUnitHIndex--;

        // 钢琴区域尺寸调整
        piano->adjustHeight();

        // 编辑区域尺寸调整
        mainArea->zoomResize(scaleplate->width(), piano->height());

        // 相对坐标调整
        QScrollBar *bar = mainScroll->verticalScrollBar();
        bar->setValue(-toY);

        updateSingleStep();
    }
}

/*--------------------------------------滚轮事件--------------------------------------*/
void EditorOuterArea::linkMoveLeft() {
    QScrollBar *bar = mainScroll->horizontalScrollBar();

    bar->triggerAction(QAbstractSlider::SliderSingleStepSub);
}

void EditorOuterArea::linkMoveRight() {
    QScrollBar *bar = mainScroll->horizontalScrollBar();

    bar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}

void EditorOuterArea::linkMoveUp() {
    QScrollBar *bar = mainScroll->verticalScrollBar();

    bar->triggerAction(QAbstractSlider::SliderSingleStepSub);
}

void EditorOuterArea::linkMoveDown() {
    QScrollBar *bar = mainScroll->verticalScrollBar();

    bar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}

void EditorOuterArea::reInitializeStrings() {
    volumeArea->setTitle(tr("Volume Panel"));
    functions->reInitializeStrings();
}

void EditorOuterArea::adjustHorizontalView() {
}

bool EditorOuterArea::getShowFunctions() const {
    return showFunctions;
}

void EditorOuterArea::setShowFunctions(bool value) {
    showFunctions = value;
    emptyRect2->setVisible(value);
    funcScroll->setVisible(value);
}


/*--------------------------------------下面板整体--------------------------------------*/
EditorWidget::EditorWidget(QWidget *parent) : SimpleForm(parent) {

    QSizeF playGroupPadding(6, 6);

    setWindowTitle(tr("Editor Panel"));

    editor = new EditorOuterArea(this);

    setDefaultTitleBar();
    setWidget(editor);
    setTitle(tr("Editor Panel"));

    getLayout()->setMargin(0);

    // 前面俩模式键
    btnLine = new VectorDblButton(getTitleBar(), false, QSize(72, 24), QSize(2, 2));
    btnNote = new VectorDblButton(getTitleBar(), false, QSize(72, 24), QSize(2, 2));

    // 播放键
    btnPlay = new VectorDblButton(getTitleBar(), false, QSize(24, 24), playGroupPadding);
    btnStop = new VectorDblButton(getTitleBar(), true, QSize(24, 24), playGroupPadding);
    btnMoveToStart = new VectorDblButton(getTitleBar(), true, QSize(24, 24), playGroupPadding);
    btnMoveToEnd = new VectorDblButton(getTitleBar(), true, QSize(24, 24), playGroupPadding);

    // 五个开关键
    btnMode1 = new VectorDblButton(getTitleBar(), false, QSize(36, 24), QSize(0, 2));
    btnMode2 = new VectorDblButton(getTitleBar(), false, QSize(36, 24), QSize(0, 2));
    btnOverlap = new VectorDblButton(getTitleBar(), false, QSize(36, 24), QSize(0, 2));
    btnReflect = new VectorDblButton(getTitleBar(), false, QSize(36, 24), QSize(0, 2));
    btnFlags = new VectorDblButton(getTitleBar(), false, QSize(36, 24), QSize(0, 2));

    // 右侧菜单键
    btnLocaleMenu = new VectorDblButton(this, true, QSize(24, 24), QSizeF(4, 4));

    btnLine->setIcons(QString(":/images/LineTypeMode2.svg"), QString(":/images/LineTypeMode1.svg"));
    btnNote->setIcons(QString(":/images/NoteTypeMode2.svg"), QString(":/images/NoteTypeMode1.svg"));

    btnPlay->setIcons(QString(":/images/PlayGroupBtn1.svg"), QString(":/images/PlayGroupBtn5.svg"),
                      QString(":/images/PlayGroupBtn1_Over.svg"),
                      QString(":/images/PlayGroupBtn5_Over.svg"));
    btnStop->setIcons(QString(":/images/PlayGroupBtn2.svg"),
                      QString(":/images/PlayGroupBtn2_Over.svg"));
    btnMoveToStart->setIcons(QString(":/images/PlayGroupBtn4.svg"),
                             QString(":/images/PlayGroupBtn4_Over.svg"));
    btnMoveToEnd->setIcons(QString(":/images/PlayGroupBtn3.svg"),
                           QString(":/images/PlayGroupBtn3_Over.svg"));

    btnMode1->setIcons(QString(":/images/ModeGroupBtn1.svg"),
                       QString(":/images/ModeGroupBtn1_2.svg"));
    btnMode2->setIcons(QString(":/images/ModeGroupBtn2.svg"),
                       QString(":/images/ModeGroupBtn2_2.svg"));
    btnOverlap->setIcons(QString(":/images/ModeGroupBtn3.svg"),
                         QString(":/images/ModeGroupBtn3_2.svg"));
    btnReflect->setIcons(QString(":/images/ModeGroupBtn4.svg"),
                         QString(":/images/ModeGroupBtn4_2.svg"));
    btnFlags->setIcons(QString(":/images/ModeGroupBtn5.svg"),
                       QString(":/images/ModeGroupBtn5_2.svg"));

    btnLocaleMenu->setIcons(QString(":/images/RightMenuBtn1.svg"),
                            QString(":/images/RightMenuBtn1_2.svg"));

    int playGroupSpace = 30;
    int modeGroupSpace = 37;

    btnLine->move(175, 0);
    btnNote->move(btnLine->x() + btnLine->width() + 20, 0);

    btnMode1->move(btnNote->x() + btnNote->width() + 20, 0);
    btnMode2->move(btnMode1->x() + modeGroupSpace, 0);
    btnOverlap->move(btnMode2->x() + modeGroupSpace, 0);
    btnReflect->move(btnOverlap->x() + modeGroupSpace, 0);
    btnFlags->move(btnReflect->x() + modeGroupSpace, 0);

    btnPlay->move(btnFlags->x() + btnFlags->width() + 20, 0);
    btnStop->move(btnPlay->x() + playGroupSpace, 0);
    btnMoveToStart->move(btnStop->x() + playGroupSpace, 0);
    btnMoveToEnd->move(btnMoveToStart->x() + playGroupSpace, 0);

    getTitleBar()->addItem(btnNote);
    getTitleBar()->addItem(btnLine);

    getTitleBar()->addItem(btnPlay);
    getTitleBar()->addItem(btnStop);
    getTitleBar()->addItem(btnMoveToStart);
    getTitleBar()->addItem(btnMoveToEnd);

    getTitleBar()->addItem(btnMode1);
    getTitleBar()->addItem(btnMode2);
    getTitleBar()->addItem(btnOverlap);
    getTitleBar()->addItem(btnReflect);
    getTitleBar()->addItem(btnFlags);

    getTitleBar()->addItem(btnLocaleMenu, -5);

    connect(btnLine, &VectorDblButton::clicked, this, &EditorWidget::onLineBtnClick);
    connect(btnNote, &VectorDblButton::clicked, this, &EditorWidget::onNoteBtnClick);

    connect(btnPlay, &VectorDblButton::clicked, this, &EditorWidget::onPlayBtnClick);
    connect(btnStop, &VectorDblButton::clicked, this, &EditorWidget::onStopBtnClick);
    connect(btnMoveToStart, &VectorDblButton::clicked, this, &EditorWidget::onMoveToStartBtnClick);
    connect(btnMoveToEnd, &VectorDblButton::clicked, this, &EditorWidget::onMoveToEndBtnClick);

    connect(btnMode1, &VectorDblButton::clicked, this, &EditorWidget::onMode1BtnClick);
    connect(btnMode2, &VectorDblButton::clicked, this, &EditorWidget::onMode2BtnClick);
    connect(btnOverlap, &VectorDblButton::clicked, this, &EditorWidget::onOverlapBtnClick);
    connect(btnReflect, &VectorDblButton::clicked, this, &EditorWidget::onReflectBtnClick);
    connect(btnFlags, &VectorDblButton::clicked, this, &EditorWidget::onFlagsBtnClick);

    connect(btnLocaleMenu, &VectorDblButton::clicked, this, &EditorWidget::onLocaleMenuBtnClick);

    editor->MainArea()->updateMode();
    updateButtons();
    initLocaleMenu();
}

void EditorWidget::updateButtons() {
    btnLine->setStatus(Root->bLineMode);
    btnNote->setStatus(!Root->bNoteMode);

    btnPlay->setStatus(Root->bPlayMode);

    btnMode1->setStatus(Root->bShowMode1);
    btnMode2->setStatus(Root->bShowMode2);
    btnOverlap->setStatus(Root->bOverlap);
    btnReflect->setStatus(Root->bReflect);
    btnFlags->setStatus(Root->bShowFlags);
}

void EditorWidget::onLineBtnClick() {
    Root->setLineMode(!Root->bLineMode);
    updateButtons();
}

void EditorWidget::onNoteBtnClick() {
    Root->setNoteMode(!Root->bNoteMode);
    updateButtons();
}

void EditorWidget::onPlayBtnClick() {

    int result;

    if (!Root->bPlayMode) {
        // 播放声音
        result = Root->renderAudio();
    } else {
        result = Root->pauseAudio();
    }

    qDebug() << "Play button" << result;
}

void EditorWidget::onStopBtnClick() {
    int result;
    result = Root->stopAudio();

    qDebug() << "Stop button" << result;
}

void EditorWidget::onMoveToStartBtnClick() {

    if (!editor->MainArea()->getNoteAt(Root->playingRange.x())) {
        return;
    }

    int left = editor->MainArea()->getNoteAt(Root->playingRange.x())->x();

    editor->MainScroll()->horizontalScrollBar()->setValue(left);
}

void EditorWidget::onMoveToEndBtnClick() {
    if (!editor->MainArea()->getNoteAt(Root->playingRange.y())) {
        return;
    }

    int right = editor->MainArea()->getNoteAt(Root->playingRange.y())->x();

    editor->MainScroll()->horizontalScrollBar()->setValue(right);
}

void EditorWidget::reInitializeStrings() {
    setWindowTitle(tr("Editor Panel"));
    setTitle(tr("Editor Panel"));

    // 子对象初始化
    editor->reInitializeStrings();
}

void EditorWidget::onMode1BtnClick() {
    Root->setMode1Status(!Root->bShowMode1);
    updateButtons();
}

void EditorWidget::onMode2BtnClick() {
    Root->setMode2Status(!Root->bShowMode2);
    updateButtons();
}

void EditorWidget::onOverlapBtnClick() {
    Root->setOverlapStatus(!Root->bOverlap);
    updateButtons();
}

void EditorWidget::onReflectBtnClick() {
    Root->setReflectStatus(!Root->bReflect);
    updateButtons();
}

void EditorWidget::onFlagsBtnClick() {
    Root->setFlagsStatus(!Root->bShowFlags);
    updateButtons();
}

void EditorWidget::onLocaleMenuBtnClick() {
    QPoint pos = this->mapToGlobal(btnLocaleMenu->geometry().bottomLeft());
    m_menu->exec(pos);
}

void EditorWidget::initLocaleMenu() {
    // 新建菜单
    m_menu = new QMenu(this);

    // 新建选项
    systemLocaleAction = new QAction(tr("System Code"), this);
    shiftJISLocaleAction = new QAction(tr("Shift-JIS"), this);

    systemLocaleAction->setCheckable(true);
    shiftJISLocaleAction->setCheckable(true);

    // 添加到选项组
    localeActionGroup = new QActionGroup(this);
    localeActionGroup->addAction(systemLocaleAction);
    localeActionGroup->addAction(shiftJISLocaleAction);

    // 添加到菜单
    m_menu->addAction(systemLocaleAction);
    m_menu->addAction(shiftJISLocaleAction);

    if (Root->localeIndex == 0) {
        systemLocaleAction->setChecked(true);
    } else {
        shiftJISLocaleAction->setChecked(true);
    }

    // 应用样式
    m_menu->setStyleSheet(menuStyleSheet);

    connect(systemLocaleAction, &QAction::triggered, this, &EditorWidget::onSystemLocaleTriggered);
    connect(shiftJISLocaleAction, &QAction::triggered, this,
            &EditorWidget::onShiftJISLocaleTriggered);
}

void EditorWidget::onSystemLocaleTriggered() {
    Root->setCodeLocale(0);
}

void EditorWidget::onShiftJISLocaleTriggered() {
    Root->setCodeLocale(1);
}
