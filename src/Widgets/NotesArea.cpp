#include "NotesArea.h"
#include "EditorWidget.h"
#include "Elements/SimpleEnvelope.h"
#include "Elements/SimplePoint.h"
#include "mainwindow.h"

NotesArea *NotesArea::Stage = nullptr;
NotesScrollArea *NotesArea::Scroll = nullptr;

/*--------------------------------------主编辑区域--------------------------------------*/
NotesArea::NotesArea(QWidget *parent) : EditorAreaWidget(parent) {
    globalTempo = 120.0;

    isMousePressed = false;
    m_view = nullptr;
    m_move = false;

    // 分隔层
    noteTopLayer = new QWidget(this);
    curvesTopLayer = new QWidget(this);
    pointTopLayer = new QWidget(this);
    vibratoTopLayer = new QWidget(this);

    // 播放头
    m_playhead = new PlayHead(5, 1, this);
    m_playhead->hide();

    // 辅助播放变量
    m_playToNote = 0;

    // 歌词编辑
    m_lyricEdit = new FixedLineEdit(this);
    m_lyricEdit->setFocusPolicy(Qt::NoFocus);
    m_lyricEdit->installEventFilter(this);
    m_lyricEdit->hide();
    connect(m_lyricEdit, &FixedLineEdit::loseFocus, this, &NotesArea::onEditLyricFinished);

    m_curEditNote = nullptr;

    // Tab 切换下一个
    tabLyricAction = new QAction(this);
    tabLyricAction->setShortcut(QKeySequence(Qt::Key_Tab));
    connect(tabLyricAction, &QAction::triggered, this, &NotesArea::onLyricSwitch);

    // Tab 切换下一个
    escLyricAction = new QAction(this);
    escLyricAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(escLyricAction, &QAction::triggered, this, &NotesArea::onLyricAbandon);

    tabLyricAction->setEnabled(false);
    escLyricAction->setEnabled(false);

    this->addAction(tabLyricAction);
    this->addAction(escLyricAction);

    // 建立 Mode1
    m_mode1 = new ContinuousCurves(this);
    m_mode1->stackUnder(curvesTopLayer);
}

void NotesArea::zoomResize(int w, int h, bool Horizontal) {
    adjustNotes();
    adjustEnvelopes();
    setFixedSize(w, h);

    onPositionChanged(); // 同时改变播放头
}

void NotesArea::adjustNotes() {
    for (SimpleNote *it : notesList) {
        if (it) {
            it->adjustNoteSize();
        }
    }

    for (SimpleNote *it : notesList) {
        if (it && it->m_pitch) {
            it->m_pitch->adjustGeometry();
            it->m_pitch->adjustPoints();
        }
    }
}

void NotesArea::adjustEnvelopes() {
    for (SimpleNote *it : notesList) {
        if (it && it->m_envelope) {
            it->m_envelope->adjustGeometry();
            it->m_envelope->adjustPoints();
        }
    }
}

void NotesArea::setNotesMovable(bool value) {
    for (SimpleNote *it : notesList) {
        if (it) {
            it->setMovable(value);
            // it->m_pitch->setSolid(!value);
        }
    }
}

void NotesArea::updateAllNotes() {
    for (SimpleNote *it : notesList) {
        if (it) {
            it->update();
            it->m_pitch->update();
            it->m_envelope->update();
        }
    }
}

void NotesArea::addNote(QUtauSectionNote *aNote, int pos, double tempo) {

    SimpleNote *note = new SimpleNote(aNote, this, pos);

    note->setTempo(tempo);

    // 保存音符
    if (notesList.size() > 0) {
        note->m_prev = notesList.back();
        notesList.back()->m_next = note;
    }

    notesList.push_back(note);

    note->setMovable(Root->bNoteMode);
    note->show();
}

void NotesArea::initializeNotes() {

    // 删除所有音符
    for (auto *it : notesList) {
        if (it) {
            delete it;
        }
        it = nullptr;
    }
    notesList.clear();

    // 初始化累加变量
    int currentLength = 0;
    double currentTempo = 120.0;
    globalTempo = currentTempo;

    const QUtauSectionSettings *aSettings = Root->aData.SectionSettings();

    int startNote = Root->aData.SectionNotesNormalBegin();
    int endNote = Root->aData.SectionNotesNormalEnd() - 1;

    int totalNotes = endNote - startNote + 1;

    // 获取全局曲速
    double tempo = 120.0;
    int length = 480;

    if (aSettings->GetValue(KEY_NAME_TEMPO, &tempo)) {
        globalTempo = tempo;
    }
    currentTempo = globalTempo;

    // 添加音符
    for (int i = startNote; i <= endNote; i++) {
        QUtauSectionNote *aNote = Root->aData.SectionNoteAt(i);

        if (aNote->GetValue(KEY_NAME_TEMPO, &tempo)) {
            currentTempo = tempo;
        }
        if (!aNote->GetValue(KEY_NAME_LENGTH, &length)) {
            length = 480;
        }
        addNote(aNote, currentLength, currentTempo);
        currentLength += length;
    }

    // 添加音高曲线
    for (int i = 0; i < notesList.size(); i++) {
        notesList[i]->getTime(); // 计算时间
        notesList[i]->initCurves();
        notesList[i]->initEnvelope();
    }

    m_mode1->initPoints();

    qDebug() << "All Notes Added.";
    emit onSumChanged(currentLength);
}

void NotesArea::feedbackNotes() {
    int startNote = Root->aData.SectionNotesNormalBegin();
    int endNote = Root->aData.SectionNotesNormalEnd() - 1;

    int j = 0;

    for (int i = startNote; i <= endNote; i++) {
        QUtauSectionNote *aNote = Root->aData.SectionNoteAt(i);

        notesList[j]->m_note.setValuesToNote(aNote);
        ++j;
    }
}

void NotesArea::removeNote(SimpleNote *pNote) {
}

void NotesArea::addNote(AbstractNote bNote) {
}

// 由于音符不能完全重叠 可以通过此方法查找音符
SimpleNote *NotesArea::findNote(int pos, int noteNum) {
    SimpleNote *note1 = nullptr;
    for (int i = 0; i < notesList.size(); i++) {
        if (notesList[i]->getPosition() == pos && notesList[i]->m_note.m_noteNum == noteNum) {
            note1 = notesList[i];
            break;
        }
    }
    return note1;
}

/*--------------------------------------框选区域--------------------------------------*/

bool NotesArea::isCropActive() {
    bool flag = false;
    if (m_view) {
        flag = true;
    }
    return flag;
}

bool NotesArea::isMoving() {
    return m_move;
}

bool NotesArea::isLyricEditing() {
    return m_lyricEdit->isVisible();
}

void NotesArea::updateCropArea() {
    if (m_view) {
        m_view->updateRect(mapFromGlobal(QCursor::pos()));
    }
}

void NotesArea::removeCropArea() {

    // qDebug() << "Remove Crop";

    // 发生意外时自动删除框选区域
    if (m_view) {
        delete m_view;
        m_view = nullptr;
    }
}

void NotesArea::cropSelect() {

    if (notesList.empty() || !m_view) {
        return;
    }

    int start, end, i, j;

    SimpleNote *note;
    SimpleCurves *line;
    SimplePoint *ctrl;

    QVector<SimpleNote *> findNotes;
    QVector<SimplePoint *> findPoints;

    start = 0;
    end = notesList.size() - 1;

    // 查找音符
    while (start < notesList.size() &&
           m_view->x() > notesList[start]->x() + notesList[start]->width()) {
        ++start;
    }
    while (end >= 0 && m_view->x() + m_view->width() < notesList[end]->width()) {
        --end;
    }

    for (i = start; i <= end; ++i) {
        note = notesList[i];
        if (note && m_view->hitTestRect(note)) {
            // qDebug() << i << note->m_note.m_lyric << "Selected";
            findNotes.push_back(note);
        }
    }

    if (QApplication::keyboardModifiers() != Qt::ControlModifier) {
        Root->removeAllSelectedNotes();
    }

    for (i = 0; i < findNotes.size(); ++i) {
        Root->addSelectedNotes(findNotes[i]);
    }

    // 非曲线模式不查找控制点
    if (Root->bNoteMode) {
        Root->removeAllSelectedPoints();
        return;
    }

    // 查找控制点（只能选中同一个音符上的控制点）
    bool findCurve, success;

    // 开始与结束前移与后移一个音符
    start = (start > 0) ? (start - 1) : start;
    end = (end < notesList.size() - 1) ? (end + 1) : end;

    findCurve = false;
    success = true;

    for (i = start; i <= end; ++i) {
        note = notesList[i];
        if (!note) {
            continue;
        }
        line = note->m_pitch;

        for (j = 0; j < line->getPointsList().size(); ++j) {
            ctrl = line->getPointsList()[j];

            if (ctrl && m_view->hitTestPoint(ctrl->getPosition())) {
                // 如果前一条曲线已经发现了点
                if (findCurve) {
                    success = false;
                    break;
                } else {
                    findPoints.push_back(ctrl);
                }
            }
        }
        if (!success) {
            break;
        }
        if (!findPoints.empty()) {
            findCurve = true;
        }
    }

    if (QApplication::keyboardModifiers() != Qt::ControlModifier) {
        Root->removeAllSelectedPoints();
    } else if (!Root->stageSelectedPoints.empty() && !findPoints.empty()) {
        // 不是同一条曲线上的
        SimplePoint *p1 = *Root->stageSelectedPoints.begin();
        SimplePoint *p2 = *findPoints.begin();
        if (p1->getScopeArea() != p2->getScopeArea()) {
            success = false;
        }
    }

    if (success && !findPoints.empty()) {
        for (i = 0; i < findPoints.size(); ++i) {
            Root->addSelectedPoints(findPoints[i]);
        }
    }

    qDebug() << "Crop Select.";
}

/*--------------------------------------选择音符--------------------------------------*/
void NotesArea::selectAllNotes() {
    if (notesList.empty()) {
        return;
    }

    Root->removeAllSelectedNotes();
    Root->removeAllSelectedPoints();

    for (int i = 0; i < notesList.size(); ++i) {
        Root->addSelectedNotes(notesList[i]);
    }
}

QPoint NotesArea::selectContinuousNotes() {
    SimpleNote *leftmost, *rightmost, *curr;
    int min, max;

    if (Root->stageSelectedNotes.empty()) {
        return QPoint(0, 0);
    }

    // 找到最左边和最右边的选中的音符
    leftmost = *Root->stageSelectedNotes.begin();
    rightmost = *Root->stageSelectedNotes.begin();

    for (auto iter = Root->stageSelectedNotes.begin(); iter != Root->stageSelectedNotes.end();
         ++iter) {
        if ((*iter)->x() > rightmost->x()) {
            rightmost = (*iter);
        }
        if ((*iter)->x() < leftmost->x()) {
            leftmost = (*iter);
        }
    }

    min = 0;
    max = notesList.size() - 1;
    for (int i = 0; i < notesList.size(); ++i) {
        curr = notesList[i];
        if (curr->x() >= leftmost->x() &&
            curr->x() + curr->width() <= rightmost->x() + rightmost->width() &&
            !Root->stageSelectedNotes.contains(curr)) {
            Root->addSelectedNotes(curr);
        }
        if (curr == leftmost) {
            min = i;
        }
        if (curr == rightmost) {
            max = i;
        }
    }
    return QPoint(min, max);
}

SimpleNote *NotesArea::getNoteAt(int i) {
    if (i >= notesList.size() || i < 0) {
        return nullptr;
    } else {
        return notesList[i];
    }
}

// 从鼠标指针位置确定应该添加曲线的音符
void NotesArea::addNoteFromCursor(QPoint cursor) {

    qDebug() << "Add note from cursor";

    SimpleNote *cur = nullptr, *next = nullptr;
    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    // 搜索最接近鼠标指针的音符
    int i = 0;
    bool flag = false;
    while (i < notesList.size()) {
        if (notesList[i]->x() < cursor.x() &&
            notesList[i]->x() + notesList[i]->width() > cursor.x()) {
            flag = true;
            break;
        }
        ++i;
    }

    if (flag) {
        cur = notesList[i];
        next = cur->m_next;
    } else {
        return;
    }

    // 存在音高线且最后一个点在鼠标指针右侧（按Ctrl可优先检测后继音符）
    if (cur->m_pitch && cur->m_pitch->lastPoint() &&
        cur->m_pitch->lastPoint()->getX() > cursor.x() && c != Qt::ControlModifier) {
        // 直接加点
        cur->m_pitch->addAndSavePoint(cursor);
        return;
    }

    // 存在后继音符且后继音符的第一个点在鼠标指针左侧
    if (next && next->m_pitch && next->m_pitch->firstPoint()) {
        // 对后继音符加点
        if (next->m_pitch->firstPoint()->getX() < cursor.x()) {
            next->m_pitch->addAndSavePoint(cursor);
            next->m_pitch->update();
            return;
        } else if (c == Qt::ControlModifier) {
            next->m_pitch->addAndSavePoint(cursor);
            next->m_pitch->update();
            return;
        }
    }

    // 否则对当前音符加点
    if (cur->m_pitch) {
        cur->m_pitch->addAndSavePoint(cursor);
        cur->m_pitch->update();
    }
}

/*--------------------------------------主编辑区域事件--------------------------------------*/
void NotesArea::paintEvent(QPaintEvent *event) {

    int i, j;
    QSet<short> deep{1, 3, 5, 8, 10};

    double boldH = 0.5;

    double thinV = 0.5;
    double boldV = 0.8;

    QColor thinC(MainWindow::editor_timeLineColor);
    QColor boldC(MainWindow::editor_sectionLineColor);

    double unit_w = noteUnitWidthArr[Root->noteUnitWIndex] / 4;
    double unit_h = noteUnitHeightArr[Root->noteUnitHIndex];

    int quarters = int(width() / unit_w) + 1;

    QPainter painter(this);
    painter.setPen(Qt::transparent);

    QRectF rectf(boldH, boldH, unit_w - 2 * boldH, unit_h - 2 * boldH);

    // 画背景
    for (i = 0; i < 84; ++i) {
        painter.setBrush((deep.contains(i % 12)) ? QColor(MainWindow::editor_backLowColor)
                                                 : QColor(MainWindow::editor_backHighColor));
        painter.drawRect(0, i * unit_h, quarters * unit_w, unit_h);
    }

    // 画八度线
    for (i = 1; i < 7; ++i) {
        painter.setBrush(MainWindow::editor_pitchLineColor);
        painter.drawRect(0, ((i - 1) * 12 + 7) * unit_h - boldH, quarters * unit_w, 2 * boldH);
        painter.setBrush(MainWindow::editor_levelLineColor);
        painter.drawRect(0, i * 12 * unit_h - boldH, quarters * unit_w, 2 * boldH);
    }

    if (Root->snappingUnit == 0) {
        return;
    }

    // 画竖线
    double snap_w = unit_w / Root->snappingUnit;
    double cur_x = 0;
    double halfW;
    QColor tempC;

    int tempLengthUnit = Root->snappingUnit;

    while (snap_w < 8) {
        snap_w *= 2;
        tempLengthUnit /= 2;
    }

    i = 0;

    while (cur_x < quarters * unit_w) {
        halfW = (i % tempLengthUnit == 0) ? boldV : thinV;
        tempC = (i % tempLengthUnit == 0) ? boldC : thinC;

        painter.setBrush(tempC);
        painter.drawRect(cur_x - halfW, 0, 2 * halfW, 84 * unit_h);
        cur_x += snap_w;
        i++;
    }
}

void NotesArea::moveEvent(QMoveEvent *event) {
    emit onMoveEvent();
}

void NotesArea::leaveEvent(QEvent *event) {

    qDebug() << "Notes Area Leave";

    Root->removeAllDraggedObjects();

    // 取消鼠标拖动
    if (isMousePressed) {
        if (m_move) {
            m_move = false;
        }

        if (m_view) {
            cropSelect();
        }
        isMousePressed = false;
    }

    removeCropArea();
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void NotesArea::mousePressEvent(QMouseEvent *event) {
    isMousePressed = true;
    removeCropArea();

    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        if (c == (Qt::ControlModifier | Qt::ShiftModifier)) {
            // 拖动屏幕
            m_move = true;

            QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
            m_startPoint = global_pos;

            this->setCursor(QCursor(Qt::OpenHandCursor));
        } else {

            // 产生框选区域等待鼠标移动
            m_view = new CropView(mapFromGlobal(QCursor::pos()), this);
            m_view->hide();
        }
    }
}

void NotesArea::mouseMoveEvent(QMouseEvent *event) {

    if (m_view && !m_view->isVisible()) {
        // 如果鼠标移动则发送信号给父窗体启动计时器
        m_view->show();
        emit onCropDrawing();

    } else if (m_move) {
        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton) {
            QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
            QPoint relative_pos = global_pos - m_startPoint;

            emit onCanvasMoving(relative_pos);
            m_startPoint = global_pos;
        }
    }
}

void NotesArea::mouseReleaseEvent(QMouseEvent *event) {
    // 必须有点有放
    if (!isMousePressed) {
        return;
    }
    isMousePressed = false;

    if (m_view && m_view->isVisible()) {
        cropSelect();
    } else if (m_move) {
        m_move = false;
    } else {
        Root->removeAllSelectedNotes();
        Root->removeAllSelectedPoints();

        if (Root->bNoteMode) {
            // 音符模式
        } else {
            if (Root->bLineMode) {
                // Mode1 曲线模式

            } else if (!Root->doubleClickToAdd &&
                       QApplication::keyboardModifiers() == Qt::NoModifier) {
                // Mode2 曲线模式
                if (event->button() == Qt::LeftButton) {
                    addFromCursorEvent(event);
                }
            }
        }
    }

    removeCropArea();
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void NotesArea::mouseDoubleClickEvent(QMouseEvent *event) {
    if (Root->bNoteMode) {
        // 音符模式
    } else {
        if (Root->bLineMode) {
            // Mode1 曲线模式

        } else if (event->button() == Qt::LeftButton) {
            // Mode2 曲线模式
            addFromCursorEvent(event);
        }
    }
}

void NotesArea::focusOutEvent(QFocusEvent *event) {
    qDebug() << "Focus Out";
}

bool NotesArea::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_lyricEdit) {
    }

    return false;
}

void NotesArea::addFromCursorEvent(QMouseEvent *event) {
    addNoteFromCursor(event->pos());
}

QPoint NotesArea::limitArea(QPoint origin) {
    if (origin.x() >= 0) {
        origin.setX(0);
    } else if (origin.x() <= -width()) {
        origin.setX(-width());
    }
    if (origin.y() >= 0) {
        origin.setY(0);
    } else if (origin.y() <= -height()) {
        origin.setY(-height());
    }

    return origin;
}

void NotesArea::editNoteLyric(SimpleNote *pNote) {
    Root->removeAllSelectedNotes();
    Root->addSelectedNotes(pNote);

    m_lyricEdit->setText(pNote->m_note.m_lyric);
    m_lyricEdit->resize(100, 25);
    m_lyricEdit->move(pNote->x(), pNote->y() + pNote->height() / 2 - m_lyricEdit->height() / 2);
    m_lyricEdit->show();
    m_lyricEdit->setFocus();
    m_lyricEdit->selectAll();
    m_curEditNote = pNote;

    emit onCheckOnStage(pNote, true);
    emit onCheckOnStage(m_lyricEdit, true);

    tabLyricAction->setEnabled(true);
    escLyricAction->setEnabled(true);
}

void NotesArea::onEditLyricFinished() {
    qDebug() << "Edit finish";
    if (!m_lyricEdit->isVisible() || !m_curEditNote) {
        return;
    }

    m_curEditNote->saveLyric(m_lyricEdit->text());
    m_lyricEdit->hide();

    tabLyricAction->setEnabled(false);
    escLyricAction->setEnabled(false);
}

void NotesArea::editNextLyric() {
    qDebug() << "Edit next";
    if (!m_lyricEdit->isVisible() || !m_curEditNote) {
        return;
    }

    m_curEditNote->saveLyric(m_lyricEdit->text());

    if (m_curEditNote->m_next) {
        editNoteLyric(m_curEditNote->m_next);
    }
}

void NotesArea::onLyricSwitch() {
    qDebug() << QTime::currentTime() << "Tab";
    if (!isLyricEditing()) {
        return;
    }
    editNextLyric();
}

void NotesArea::onLyricAbandon() {
    qDebug() << "Edit Abandon";
    if (!m_lyricEdit->isVisible() || !m_curEditNote) {
        return;
    }
    m_lyricEdit->hide();

    tabLyricAction->setEnabled(false);
    escLyricAction->setEnabled(false);
}

void NotesArea::updateMode() {
    //    if (Root->bNoteMode) {
    //        // 音符模式
    //        setNotesMovable(true);
    //    } else {
    //        // 曲线模式
    //        setNotesMovable(false);
    //    }

    //    if (Root->bLineMode) {
    //        // 绘制模式
    //    } else {
    //        // 移动模式
    //    }
    setNotesMovable(Root->bNoteMode || Root->bLineMode);
}

void NotesArea::onPositionChanged(qint64 position) {

    double unit_w = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    if (!Root->player || Root->player->state() != QMediaPlayer::PlayingState ||
        Root->playingRange.x() >= notesList.size()) {
        m_playhead->hide();
        return;
    }

    if (position < 0) {
        position = Root->player->position();
    }

    position -= notesList[Root->playingRange.x()]->getCorrectGenon().PreUtterance;
    if (position < 0) {
        position = 0;
    }

    // 将 m_playToNote 位置确定
    if (m_playToNote < Root->playingRange.x()) {
        m_playToNote = Root->playingRange.x();
    }
    if (m_playToNote > Root->playingRange.y()) {
        m_playToNote = Root->playingRange.y();
    }

    // 调整范围
    while (m_playToNote <= Root->playingRange.y() &&
           notesList[m_playToNote]->getTime() + notesList[m_playToNote]->getDuration() -
                   notesList[Root->playingRange.x()]->getTime() <
               position) {
        m_playToNote++;
    }

    // 不能超过范围
    if (m_playToNote > qMin(Root->playingRange.y() + 1, notesList.size() - 1)) {
        qDebug() << "Over line.";
        m_playhead->hide();
        return;
    }

    while (m_playToNote >= Root->playingRange.x() &&
           notesList[m_playToNote]->getTime() - notesList[Root->playingRange.x()]->getTime() >
               position) {
        m_playToNote--;
    }

    // 不能小于范围
    if (m_playToNote < Root->playingRange.x()) {
        m_playhead->hide();
        return;
    }

    // 减掉第一个音符的先行声音
    double unit = 120.0 / notesList[m_playToNote]->getTempo() / 0.96;
    double ticks = (position + notesList[Root->playingRange.x()]->getTime() -
                    notesList[m_playToNote]->getTime()) /
                   unit / 480 * unit_w;

    if (!m_playhead->isVisible()) {
        m_playhead->show();
    }

    m_playhead->setPosition(notesList[m_playToNote]->x() + ticks);

    emit onPlayheadMoving(); // 让父窗体同时更新位置
}
