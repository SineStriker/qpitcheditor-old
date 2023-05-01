#include <QTime>

#include "LibUTAUQt/QUtauUtils.h"
#include "SimpleCurves.h"
#include "SimpleEnvelope.h"
#include "SimpleNote.h"
#include "SimplePoint.h"
#include "SimpleVibrato.h"
#include "mainwindow.h"

/*--------------------------------------音符--------------------------------------*/

SimpleNote::SimpleNote(NotesArea *parent, int pos) : SimpleObject(parent), pos(pos) {
    m_parent = parent;
    adjustLayer();

    resetValues();

    m_selected = false;
    m_movable = true;
    m_move = false;
    m_moving = false;
    rollOver = false;
    m_pitch = nullptr;
    m_envelope = nullptr;
    m_prev = m_next = nullptr;

    m_menu = nullptr;
    portamentoAction = nullptr;
    vibratoAction = nullptr;
    vbrEditAction = nullptr;
    propertyAction = nullptr;

    initRightMenu();

    adjustNoteSize();
}

SimpleNote::SimpleNote(QString lrc, NotesArea *parent, int pos) : SimpleObject(parent), pos(pos) {
    m_parent = parent;
    adjustLayer();

    resetValues();

    m_selected = false;
    m_movable = true;
    m_move = false;
    m_moving = false;
    rollOver = false;
    m_pitch = nullptr;
    m_envelope = nullptr;
    m_prev = m_next = nullptr;

    m_menu = nullptr;
    portamentoAction = nullptr;
    vibratoAction = nullptr;
    propertyAction = nullptr;
    initRightMenu();

    m_note.m_lyric = lrc;

    updateNoteStatus();
    adjustNoteSize();
}

SimpleNote::SimpleNote(QUtauSectionNote *aNote, NotesArea *parent, int pos)
    : SimpleObject(parent), pos(pos) {
    m_parent = parent;
    adjustLayer();

    resetValues();

    m_selected = false;
    m_movable = true;
    m_move = false;
    m_moving = false;
    rollOver = false;
    m_pitch = nullptr;
    m_envelope = nullptr;
    m_prev = m_next = nullptr;

    m_menu = nullptr;
    portamentoAction = nullptr;
    vibratoAction = nullptr;
    propertyAction = nullptr;
    initRightMenu();

    m_note.setValuesFromNote(aNote);

    updateNoteStatus();
    adjustNoteSize();
}

SimpleNote::~SimpleNote() {
    // 若存在音高线则释放音高线
    if (m_pitch) {
        delete m_pitch;
        m_pitch = nullptr;
    }
}

void SimpleNote::adjustLayer() {
    if (m_parent->getNoteSegLayer()) {
        this->stackUnder(m_parent->getNoteSegLayer());
    }
}

void SimpleNote::initCurves() {
    if (m_pitch) {
        return;
    }

    SimpleCurves *curve = new SimpleCurves(this, m_parent);

    // 保存曲线
    m_pitch = curve;

    curve->setSolid(!isMovable());
    curve->show();
}

void SimpleNote::initEnvelope() {
    if (m_envelope) {
        return;
    }

    SimpleEnvelope *envelope = new SimpleEnvelope(this, VolumeArea::Stage);

    // 保存曲线
    m_envelope = envelope;
    envelope->show();
}

// 重置默认音符
void SimpleNote::resetValues() {
    utauNote = nullptr;

    m_status = NoteUnlistStatus;
    startTime = 0;

    m_note.resetValues();
}

// 调整尺寸
void SimpleNote::adjustNoteSize() {
    unsigned w = noteUnitWidthArr[Root->noteUnitWIndex];
    unsigned h = noteUnitHeightArr[Root->noteUnitHIndex];

    int toX = double(pos) / 480 * double(w) / 4;
    int toY = h * (107 - m_note.m_noteNum);
    int toW = double(m_note.m_length) / 480 * double(w) / 4;
    int toH = h;

    setGeometry(toX, toY, toW, toH);
    update();
}

void SimpleNote::saveNoteNumFromPosition() {
    unsigned h = noteUnitHeightArr[Root->noteUnitHIndex];

    int prevNoteNum = m_note.m_noteNum;
    int curNoteNum = 107 - int(double(this->y()) / h + 0.5);

    // 没变
    if (prevNoteNum == curNoteNum) {
        return;
    }

    // 建立记录变量
    NoteOperation *n = new NoteOperation(noteNumChange);

    // 设定修改前的数据
    n->setOrg(this);

    // 将音阶改动保存到音符
    m_note.m_noteNum = curNoteNum;

    // 设定修改后的数据
    n->setNew(this);
    Root->historyAddStep(n);

    updateNoteStatus();
    updateCurves();
}

void SimpleNote::saveLyric(QString lyric) {
    // 没变
    if (lyric == m_note.m_lyric) {
        return;
    }

    // 建立记录变量
    NoteOperation *n = new NoteOperation(lrcChange);

    // 设定修改前的数据
    n->setOrg(this);

    // 将音阶改动保存到音符
    m_note.m_lyric = lyric;

    // 设定修改后的数据
    n->setNew(this);
    Root->historyAddStep(n);

    updateNoteStatus();
    updateCurves();
}

void SimpleNote::updateNoteStatus(bool detectVoice) {
    // 检测休止符
    if (isRestNoteLyric(m_note.m_lyric)) {
        m_status = NoteRestStatus;
    } else if (detectVoice) {
        // 检测是否在音源中
        m_status = m_note.updateGenons() ? NoteListStatus : NoteUnlistStatus;
    }
    updateEnvelopes(true);
    update();
}

void SimpleNote::updateCurves() {
    if (m_pitch) {
        m_pitch->adjustGeometry();
        m_pitch->adjustPoints();
        m_pitch->update();
    }
    if (m_next && m_next->m_pitch) {
        m_next->m_pitch->adjustGeometry();
        m_next->m_pitch->adjustPoints();
        m_next->m_pitch->update();
    }
    if (m_prev && m_prev->m_pitch) {
        m_prev->m_pitch->adjustGeometry();
        m_prev->m_pitch->adjustPoints();
        m_prev->m_pitch->update();
    }
}

void SimpleNote::updateEnvelopes(bool rearrange) {
    if (m_envelope) {
        m_envelope->adjustGeometry();
        if (rearrange) {
            m_envelope->rearrangePoints();
        }
        m_envelope->adjustPoints();
        m_envelope->update();
    }
    if (m_next && m_next->m_envelope) {
        m_next->m_envelope->adjustGeometry();
        if (rearrange) {
            m_next->m_envelope->rearrangePoints();
        }
        m_next->m_envelope->adjustPoints();
        m_next->m_envelope->update();
    }
    if (m_prev && m_prev->m_envelope) {
        m_prev->m_envelope->adjustGeometry();
        if (rearrange) {
            m_prev->m_envelope->rearrangePoints();
        }
        m_prev->m_envelope->adjustPoints();
        m_prev->m_envelope->update();
    }
}

void SimpleNote::initializeNote(bool resetCurves) {
    updateNoteStatus();
    adjustNoteSize();

    if (m_pitch) {
        if (resetCurves) {
            // 将音高曲线重载
            m_pitch->initAndAdjustPoints();
        }
        updateCurves();
    }
}

void SimpleNote::changeValues() {
    updateNoteStatus();
    updateCurves();
}

void SimpleNote::changeLength() {
}

void SimpleNote::changeVibrato() {
    if (!m_pitch) {
        return;
    }
    if (m_note.m_vbr.empty() && m_pitch->hasVBREditor()) {
        m_pitch->removeVibrtoButtons();
    }

    m_pitch->adjustVibrato();
    m_pitch->update();
}

void SimpleNote::changeEnvelope() {
    if (m_envelope) {
        m_envelope->initAndAdjustPoints();
        // 无需重新调整
        updateEnvelopes();
    }
}

void SimpleNote::changeTempo() {
}

QPoint SimpleNote::getStaticPos() {
    if (m_move) {
        return m_windowPoint;
    } else {
        return QPoint(x(), y());
    }
}

int SimpleNote::getStaticX() {
    if (m_move) {
        return m_windowPoint.x();
    } else {
        return x();
    }
}

int SimpleNote::getStaticY() {
    if (m_move) {
        return m_windowPoint.y();
    } else {
        return y();
    }
}

double SimpleNote::getInPoint() {
    return -getCorrectGenon().PreUtterance;
}

double SimpleNote::getOutPoint() {
    double endPlus = 0;
    if (m_next) {
        CorrectGenon nextGenon = m_next->getCorrectGenon();
        double nextDuration = m_next->getDuration();

        endPlus = -nextGenon.PreUtterance + nextGenon.VoiceOverlap;
        if (endPlus > nextDuration) {
            endPlus = nextDuration;
        }
    }
    return getDuration() + endPlus;
}

double SimpleNote::getInOverlapPoint() {
    CorrectGenon genon = getCorrectGenon();
    return -genon.PreUtterance + genon.VoiceOverlap;
}

double SimpleNote::getOutOverlapPoint() {
    double endPlus = 0;
    if (m_next) {
        CorrectGenon nextGenon = m_next->getCorrectGenon();
        endPlus = -nextGenon.PreUtterance;
    }
    return getDuration() + endPlus;
}

double SimpleNote::getPBStart() {
    return -m_note.m_preUttr;
}

int SimpleNote::convertTimeToTick(double time) {
    double unit = 120.0 / getTempo() / 0.96; // 从毫秒到ticks
    double prevUnit = m_prev ? (120.0 / m_prev->getTempo() / 0.96) : unit;
    double tick = (time < 0) ? (time / prevUnit) : (time / unit);

    return tick;
}

double SimpleNote::convertTickToTime(double tick) {
    double unit = 120.0 / getTempo() / 0.96; // 从毫秒到ticks
    double prevUnit = m_prev ? (120.0 / m_prev->getTempo() / 0.96) : unit;
    double time = (tick < 0) ? (tick * prevUnit) : (tick * unit);

    return time;
}

void SimpleNote::setSelected(bool b) {
    m_selected = b;

    update();
}

void SimpleNote::setMovable(bool b) {
    m_movable = b;
    if (m_pitch) {
        m_pitch->setSolid(!b);
    }
}

double SimpleNote::getCorrectDuration() {
    CorrectGenon curGenon = getCorrectGenon();
    CorrectGenon nextGenon = m_next ? getCorrectGenon() : CorrectGenon();

    double d =
        getDuration() + curGenon.PreUtterance - nextGenon.PreUtterance + nextGenon.VoiceOverlap;

    return d;
}

// 咬合校正（核心)
CorrectGenon SimpleNote::getCorrectGenon() {
    if (!isListed()) {
        return CorrectGenon(0, 0, 0);
    }

    double CorrectRate = 1;

    double tmpPreUttr = m_note.m_preUttr;
    double tmpOverlap = m_note.m_overlap;

    double tmpSTPoint = (m_note.m_stp == NODEF_DOUBLE) ? 0 : m_note.m_stp;
    double velocity = (m_note.m_vel == NODEF_DOUBLE) ? 100 : m_note.m_vel;

    double velocityRate = pow(2, 1 - velocity / 100);

    tmpPreUttr *= velocityRate;
    tmpOverlap *= velocityRate;

    if (!m_prev) {
        return CorrectGenon(tmpPreUttr, tmpOverlap, tmpSTPoint);
    }

    double prevDuration = m_prev->getDuration(); // 前驱音符时长
    double curDuration = this->getDuration();    // 当前音符时长
    double MaxOccupy = (m_prev->isRest()) ? prevDuration : (prevDuration / 2);

    if (tmpPreUttr - tmpOverlap > MaxOccupy) {
        CorrectRate = MaxOccupy / (tmpPreUttr - tmpOverlap);
    }

    double CorrecetPreUttr = CorrectRate * tmpPreUttr;
    double CorrecetOverlap = CorrectRate * tmpOverlap;
    double CorrecetSTPoint = tmpPreUttr - CorrecetPreUttr;

    CorrecetSTPoint += tmpSTPoint;

    // 保证重叠不过大
    if (CorrecetOverlap - CorrecetPreUttr > curDuration) {
        CorrecetOverlap = CorrecetPreUttr + curDuration;
    }

    return CorrectGenon(CorrecetPreUttr, CorrecetOverlap, CorrecetSTPoint);
}

// 联动移动
void SimpleNote::linkPressEvent(QMouseEvent *event) {

    raise();
    adjustLayer();

    Root->addDraggedObject(this);
    m_move = true;

    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());

    m_startPoint = global_pos;
    m_windowPoint = this->frameGeometry().topLeft();
}

void SimpleNote::linkMoveEvent(QMouseEvent *event) {
    m_moving = true;

    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
    QPoint relative_pos = global_pos - m_startPoint;

    this->move(limitArea(m_windowPoint + relative_pos));
}

void SimpleNote::linkReleaseEvent(QMouseEvent *event) {
    Root->removeDraggedObject(this);
    m_move = false;
    m_moving = false;

    saveNoteNumFromPosition();
}

void SimpleNote::autoReleaseEvent(QMouseEvent *event, bool flag) {

    if (!flag) {
        Root->removeDraggedObject(this);
    }

    this->move(m_windowPoint);
    m_move = false;
    m_moving = false;
}

QPoint SimpleNote::limitArea(QPoint origin) {
    unsigned h = noteUnitHeightArr[Root->noteUnitHIndex];
    origin.setY(int(double(origin.y()) / h + 0.5) * h);
    origin.setX(m_windowPoint.x());

    if (origin.y() < 0) {
        origin.setY(0);
    } else if (origin.y() > 83.5 * h) {
        origin.setY(83 * h);
    }

    return origin;
}

void SimpleNote::initRightMenu() {
    m_menu = new QMenu(this);

    portamentoAction = new QAction(tr("Portamento"), this);
    vibratoAction = new QAction(tr("Vibrato"), this);
    vbrEditAction = new QAction(tr("Vibrato Editor"), this);
    propertyAction = new QAction(tr("Note Properties"), this);

    portamentoAction->setCheckable(true);
    vibratoAction->setCheckable(true);
    vbrEditAction->setCheckable(true);

    // 给颤音编辑器添加快捷键
    // vbrEditAction->setShortcut(QKeySequence(Qt::Key_V));

    m_menu->addAction(portamentoAction);
    m_menu->addSeparator();
    m_menu->addAction(vibratoAction);
    m_menu->addAction(vbrEditAction);
    m_menu->addSeparator();
    m_menu->addAction(propertyAction);

    m_menu->setStyleSheet(menuStyleSheet);

    connect(portamentoAction, &QAction::triggered, this, &SimpleNote::triggerAddPitch);
    connect(vibratoAction, &QAction::triggered, this, &SimpleNote::triggerAddVibrato);
    connect(vbrEditAction, &QAction::triggered, this, &SimpleNote::triggerVBREditor);
    connect(propertyAction, &QAction::triggered, this, &SimpleNote::triggerShowProperty);

    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void SimpleNote::addVibrato(bool showEditor) {
    if (!m_note.m_vbr.empty()) {
        return;
    }
    // 建立记录变量
    NoteOperation *n = new NoteOperation(vbrChange);

    // 设定修改前的数据
    n->setOrg(this);

    m_note.m_vbr = {65, 180, 35, 20, 20, 0, 0, 0};

    // 设定修改后的数据
    n->setNew(this);
    Root->historyAddStep(n);

    if (showEditor && !m_movable) {
        m_pitch->initVibratoButtons();
    }

    m_pitch->update();
}

void SimpleNote::removeVibrato() {
    if (m_note.m_vbr.empty()) {
        return;
    }
    // 建立记录变量
    NoteOperation *n = new NoteOperation(vbrChange);

    // 设定修改前的数据
    n->setOrg(this);

    m_pitch->removeVibrtoButtons();
    m_note.m_vbr = {};

    // 设定修改后的数据
    n->setNew(this);
    Root->historyAddStep(n);

    m_pitch->update();
}

void SimpleNote::paintEvent(QPaintEvent *event) {

    unsigned w = noteUnitWidthArr[Root->noteUnitWIndex];
    unsigned h = noteUnitHeightArr[Root->noteUnitHIndex];

    double padding = 0.25;
    double innerPadding = 2;
    double border = h / 15;

    border = border > 4 ? 4 : border;

    QRect rect(double(pos) / 480 * double(w) / 4, h * (107 - m_note.m_noteNum),
               double(m_note.m_length) / 480 * double(w) / 4, h);

    QPainter painter(this);

    QColor selectColor, deselectColor;

    painter.setRenderHint(QPainter::Antialiasing);

    if (isListed()) {
        deselectColor = QColor(0x0099FF);
        selectColor = QColor(0x40D9FF);
    } else if (isRest()) {
        deselectColor = QColor(0xCCCCCC);
        selectColor = QColor(0xEEEEEE);
    } else {
        deselectColor = QColor(0x105685);
        selectColor = QColor(0x1881C7);
    }

    QRectF entityRect(padding, padding, width() - 2 * padding, height() - 2 * padding);
    QRectF textRect(entityRect);
    textRect.setX(entityRect.x() + 5 + w / 128.0);

    painter.setBrush(deselectColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(entityRect, border * 1.5, border * 1.5);

    if (m_selected) {
        painter.setBrush(selectColor);
        QRectF innerRect(padding + innerPadding, padding + innerPadding,
                         width() - 2 * (padding + innerPadding),
                         height() - 2 * (padding + innerPadding));
        painter.drawRoundedRect(innerRect, border * 1.5, border * 1.5);
    }

    painter.setPen(MainWindow::note_lyricNameColor);
    painter.setBrush(Qt::NoBrush);
    painter.setFont(uiFont());

    painter.drawText(textRect, Qt::AlignVCenter, m_note.m_lyric, &entityRect);
    // painter.drawText(10, 7 + height() / 2, m_note.m_lyric);
}

void SimpleNote::MyMouseEvent(QMouseEvent *e) {
    /*if (this->parentWidget()) {
        // 将自己设为鼠标事件透明并重新搜索是否有后面的控件会响应鼠标事件。
        this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        QPoint pt = this->mapTo(this->parentWidget(), e->pos());
        QWidget *w = this->parentWidget();
        if (w) {
            pt = w->mapFrom(this->parentWidget(), pt);
            QMouseEvent *event =
                new QMouseEvent(e->type(), pt, e->button(), e->buttons(), e->modifiers());
            QApplication::postEvent(w, event);
        }
        // 将自己设为鼠标事件不透明，以免影响button的功能
        this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }*/

    if (m_parent) {
        QPoint pt = this->mapTo(m_parent, e->pos());

        QMouseEvent *event =
            new QMouseEvent(e->type(), pt, e->button(), e->buttons(), e->modifiers());
        m_parent->addFromCursorEvent(event);
    }
}

void SimpleNote::mouseDoubleClickEvent(QMouseEvent *event) {
    // 移动属性不可用时像父对象发送双击事件
    if (event->button() == Qt::LeftButton) {
        if (!m_movable) {
            qDebug() << "Mouse Event Through..";
            MyMouseEvent(event);
        } else {
            m_parent->editNoteLyric(this);
        }
    }
}
void SimpleNote::mousePressEvent(QMouseEvent *event) {

    // 选择音符时自动忽略控制点
    Root->removeAllSelectedPoints();

    if (!m_movable) {
    }

    if (event->button() == Qt::LeftButton) {
        Root->autoAddSelectedNote(this);
        update();

        for (SimpleNote *it : Root->stageSelectedNotes) {
            it->linkPressEvent(event);
        }
    }
}

void SimpleNote::mouseMoveEvent(QMouseEvent *event) {
    if (!m_move /*|| !m_movable*/) {
        return;
    }
    if (event->buttons() & Qt::LeftButton) {
        // 只有可移动或双击加点的情况下才可拖动
        for (SimpleNote *it : Root->stageSelectedNotes) {
            it->linkMoveEvent(event);
        }
    }
}

void SimpleNote::mouseReleaseEvent(QMouseEvent *event) {
    /*if (!m_movable) {
        return;
    }*/

    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());

    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    if (event->button() == Qt::LeftButton) {

        // 当且仅当没有拖动时
        if (!m_movable && !m_moving) {
            if (!Root->doubleClickToAdd && c == Qt::NoModifier) {
                qDebug() << "Mouse Event Through..";
                MyMouseEvent(event);
            }
        }

        Root->historySequence = 1;
        for (SimpleNote *it : Root->stageSelectedNotes) {
            it->linkReleaseEvent(event);
        }
        Root->historySequence = 0;
    }
}

void SimpleNote::contextMenuEvent(QContextMenuEvent *event) {
    // 右键触发时检查自己是否被选中
    if (!Root->stageSelectedNotes.contains(this)) {
        Root->removeAllSelectedNotes();
        Root->addSelectedNotes(this);
    }

    if (!m_pitch) {
        return;
    }

    if (m_pitch->isEmpty()) {
        portamentoAction->setChecked(false);
    } else {
        portamentoAction->setChecked(true);
    }

    if (!m_pitch->hasVBREditor()) {
        vbrEditAction->setChecked(false);
    } else {
        vbrEditAction->setChecked(true);
    }

    if (m_note.m_vbr.empty()) {
        vibratoAction->setChecked(false);
        vbrEditAction->setEnabled(false);
    } else {
        vibratoAction->setChecked(true);
        vbrEditAction->setEnabled(true);
    }

    if (m_movable) {
        vbrEditAction->setEnabled(false);
        vbrEditAction->setChecked(false);
    }

    m_menu->exec(QCursor::pos());
    event->accept();
}

void SimpleNote::triggerAddPitch() {
    if (!m_pitch) {
        return;
    }

    if (m_pitch->isEmpty()) {
        if (Root->stageSelectedNotes.empty()) {
            m_pitch->addPortamento();
        } else {
            Root->historySequence = 1;
            for (SimpleNote *it : Root->stageSelectedNotes) {
                if (!it->m_pitch) {
                    continue;
                }
                it->m_pitch->addPortamento();
            }
            Root->historySequence = 0;
        }
    } else {
        if (Root->stageSelectedNotes.empty()) {
            m_pitch->removePortamento();
        } else {
            Root->historySequence = 1;
            for (SimpleNote *it : Root->stageSelectedNotes) {
                if (!it->m_pitch) {
                    continue;
                }
                it->m_pitch->removePortamento();
            }
            Root->historySequence = 0;
        }
    }
}

void SimpleNote::triggerAddVibrato() {
    if (!m_pitch) {
        return;
    }

    if (m_note.m_vbr.empty()) {
        if (Root->stageSelectedNotes.empty()) {
            addVibrato(true);
        } else {
            Root->historySequence = 1;
            for (SimpleNote *it : Root->stageSelectedNotes) {
                if (it == this) {
                    it->addVibrato(true);
                } else {
                    it->addVibrato();
                }
            }
            Root->historySequence = 0;
        }
    } else {
        if (Root->stageSelectedNotes.empty()) {
            removeVibrato();
        } else {
            Root->historySequence = 1;
            for (SimpleNote *it : Root->stageSelectedNotes) {
                it->removeVibrato();
            }
            Root->historySequence = 0;
        }
    }
}

void SimpleNote::triggerVBREditor() {
    if (!m_pitch) {
        return;
    }
    if (!m_pitch->hasVBREditor()) {
        m_pitch->initVibratoButtons();
    } else {
        m_pitch->removeVibrtoButtons();
    }
    m_pitch->update();
}

void SimpleNote::triggerShowProperty() {
    Root->showNoteProperties();
}
