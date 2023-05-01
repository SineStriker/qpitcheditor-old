#include "SimpleEnvelope.h"
#include "mainwindow.h"

#include "Widgets/VolumeArea.h"

SimpleEnvelope::SimpleEnvelope(SimpleNote *note, VolumeArea *parent) : SimpleScope(note, parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents, true); // 本身不响应鼠标事件

    adjustLayer();
    initAndAdjustPoints();
}

void SimpleEnvelope::reInitializeStyles() {
}

void SimpleEnvelope::adjustGeometry() {
    QRect rect = getActiveScope();
    // rect.setBottom(m_parent->height());

    if (geometry() != rect) {
        setGeometry(rect);
    }
}

void SimpleEnvelope::saveToNote(QVector<QCtrlPoint> arr) {

    // 建立记录变量
    NoteOperation *n = new NoteOperation(envChange);

    // 设定修改前的数据
    n->setOrg(curNote);

    // 将控制点改动保存到音符
    if (arr.isEmpty()) {
        curNote->m_note.m_env = fromCorrectPoints();
        curNote->m_note.envSort();
    } else {
        curNote->m_note.m_env = arr;
    }

    // 设定修改后的数据
    n->setNew(curNote);
    Root->historyAddStep(n);
}

void SimpleEnvelope::rearrangePoints() {
    SimplePoint *p;
    int i;

    QVector<QCtrlPoint> arr = toCorrectPoints();
    for (i = 0; i < points.size(); ++i) {
        p = points[i];
        p->setTPoint(arr[i]);
    }
    updateVisibility();
}

void SimpleEnvelope::adjustPoints() {
    SimpleScope::adjustPoints();
}

QVector<QCtrlPoint> SimpleEnvelope::toCorrectPoints() {
    QVector<QCtrlPoint> &arr = curNote->m_note.m_env;
    int p3 = (arr.size() >= 4) ? (arr.size() - 2) : arr.size();

    int i;
    double pos;
    QVector<QCtrlPoint> points2, points3, result;

    pos = curNote->getInPoint();
    for (i = 0; i < p3; ++i) {
        pos += arr[i].mX;
        points2.push_back(QCtrlPoint(pos, arr[i].mY));
    }
    pos = curNote->getOutPoint();
    for (i = arr.size() - 1; i >= p3; --i) {
        pos -= arr[i].mX;
        points3.push_back(QCtrlPoint(pos, arr[i].mY));
    }

    for (i = 0; i < points2.size(); ++i) {
        result.push_back(points2[i]);
    }
    for (i = points3.size() - 1; i >= 0; --i) {
        result.push_back(points3[i]);
    }

    AbstractNote::arrSort(result);

    return result;
}

QVector<QCtrlPoint> SimpleEnvelope::fromCorrectPoints() {
    int p3 = (points.size() >= 4) ? (points.size() - 2) : points.size();

    int i;
    double interval, prev;
    QVector<QCtrlPoint> points2, points3, result;

    prev = curNote->getInPoint();
    for (i = 0; i < p3; ++i) {
        interval = points[i]->getTPoint().mX - prev;
        prev = points[i]->getTPoint().mX;
        points2.push_back(QCtrlPoint(interval, points[i]->getTPoint().mY));
    }
    prev = curNote->getOutPoint();
    for (i = points.size() - 1; i >= p3; --i) {
        interval = prev - points[i]->getTPoint().mX;
        prev = points[i]->getTPoint().mX;
        points3.push_back(QCtrlPoint(interval, points[i]->getTPoint().mY));
    }

    for (i = 0; i < points2.size(); ++i) {
        result.push_back(points2[i]);
    }
    for (i = points3.size() - 1; i >= 0; --i) {
        result.push_back(points3[i]);
    }
    return result;
}

double SimpleEnvelope::convertPosToValueX(int x) {
    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    double unit = 120.0 / curNote->getTempo() / 0.96; // 从毫秒到ticks
    double prevUnit = getPrevNote() ? (120.0 / getPrevNote()->getTempo() / 0.96) : unit;

    int noteX = curNote->getStaticX();
    double tick = double(x - noteX) * 480 / currentWidth;
    double tx = (tick < 0) ? (tick * prevUnit) : (tick * unit);

    return tx;
}

double SimpleEnvelope::convertPosToValueY(int y) {
    int valueY = (-(y - m_parent->height() / 2.0) / VolumeArea::standardHeight + 1) * 100;
    return valueY;
}

int SimpleEnvelope::convertValueToPosX(double x) {
    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;
    double unit = 120.0 / curNote->getTempo() / 0.96; // 从毫秒到ticks
    double prevUnit = getPrevNote() ? (120.0 / getPrevNote()->getTempo() / 0.96) : unit;

    int noteX = curNote->getStaticX();
    double tick = (x < 0) ? (x / prevUnit) : (x / unit);
    int toX = noteX + tick / 480 * currentWidth;

    return toX;
}

int SimpleEnvelope::convertValueToPosY(double y) {
    int posY = m_parent->height() / 2.0 - (y / 100 - 1) * VolumeArea::standardHeight;
    return posY;
}

void SimpleEnvelope::removePoint(SimplePoint *p) {
    SimpleScope::removePoint(p);
}

void SimpleEnvelope::removePoints(QVector<SimplePoint *> arr) {
    // 只能一次删除一个
}

void SimpleEnvelope::initPoints(QVector<QCtrlPoint> arr) {
    if (arr.empty()) {
        arr = toCorrectPoints();
    }

    SimpleScope::initPoints(arr);

    updateVisibility();
}

void SimpleEnvelope::initAndAdjustPoints(QVector<QCtrlPoint> arr) {
    adjustGeometry();
    initPoints(arr);

    adjustPoints();

    // 重新排序
    updateSpecialPositions();
    // 限制最左边与最右边的控制点的位置
    limitAllPoints();

    update();
}

void SimpleEnvelope::updateVisibility() {
    if (curNote->isRest()) {
        setPointsVisible(false);
    } else {
        setPointsVisible(true);
    }
    update();
}

QRect SimpleEnvelope::getActiveScope() {
    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    double unit = 120.0 / curNote->getTempo() / 0.96; // 从毫秒到ticks
    double prevUnit = getPrevNote() ? (120.0 / getPrevNote()->getTempo() / 0.96) : unit;
    double nextUnit = getNextNote() ? (120.0 / getNextNote()->getTempo() / 0.96) : unit;

    double voiceStartPlus = -curNote->getInPoint();
    double voiceEndPlus = curNote->getOutPoint() - curNote->getDuration();

    QRectF realNoteRect;

    double voiceStartPlusTime =
        (voiceStartPlus > 0) ? (voiceStartPlus / prevUnit) : (voiceStartPlus / unit);
    double voiceEndPlusTime =
        (voiceEndPlus > 0) ? (voiceEndPlus / nextUnit) : (voiceEndPlus / unit);

    realNoteRect.setLeft(curNote->getStaticX() - voiceStartPlusTime / 480 * currentWidth);
    realNoteRect.setRight(curNote->getStaticX() + curNote->width() +
                          voiceEndPlusTime / 480 * currentWidth);

    realNoteRect.setTop(m_parent->height() / 2 - VolumeArea::standardHeight);
    realNoteRect.setBottom(m_parent->height() / 2 + VolumeArea::standardHeight);

    return realNoteRect.toRect();
}

SimplePoint *SimpleEnvelope::createNewPoint() {
    SimplePoint *p = new VolumePoint(this, m_parent);

    connect(p, SIGNAL(updatePoint(SimplePoint *, pointMoveEvent)), this,
            SLOT(onUpdatePoint(SimplePoint *, pointMoveEvent)));

    return p;
}

void SimpleEnvelope::drawPreUtteranceAndOverlap(QPainter &painter) {

    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    QPen overlapPen(MainWindow::overlap_lineColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen preuttrPen(MainWindow::preUttr_lineColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    double voiceOverlapPlus = -curNote->getInOverlapPoint();
    double voicePreUttrPlus = -curNote->getInPoint();
    double voiceOverlapPlusTime = curNote->convertTimeToTick(voiceOverlapPlus);
    double voicePreUttrPlusTime = curNote->convertTimeToTick(voicePreUttrPlus);

    double voiceOverlapLineX =
        curNote->getStaticX() - voiceOverlapPlusTime / 480 * currentWidth - this->x();
    double voicePreUttrLineX =
        curNote->getStaticX() - voicePreUttrPlusTime / 480 * currentWidth - this->x();

    // 画重叠线
    QLineF oveLine(voiceOverlapLineX, 0, voiceOverlapLineX, height());
    painter.setBrush(Qt::NoBrush);
    painter.setPen(overlapPen);
    painter.drawLine(oveLine);

    // 画先行线
    QLineF preLine(voicePreUttrLineX, 0, voicePreUttrLineX, height());
    painter.setBrush(Qt::NoBrush);
    painter.setPen(preuttrPen);
    painter.drawLine(preLine);
}

void SimpleEnvelope::drawBackFill(QPainter &painter) {

    // 画填充
    QPainterPath fillPath;

    QPointF veryStart(getActiveScope().bottomLeft() - pos());
    QPointF veryEnd(getActiveScope().bottomRight() - pos());

    if (Root->bShowEnvFill) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(MainWindow::envelope_fillColor_enabled);
        // fillPath.moveTo(QPointF(veryStart.x(), rect().bottomLeft().y()));
        fillPath.moveTo(veryStart);
        for (int i = 0; i < points.size(); ++i) {
            fillPath.lineTo(QPointF(points[i]->getPosition() - pos()));
        }
        fillPath.lineTo(veryEnd);
        // fillPath.lineTo(QPointF(veryEnd.x(), rect().bottomRight().y()));
        painter.drawPath(fillPath);
    }
}

void SimpleEnvelope::drawEnvelope(QPainter &painter) {

    QPointF prev, curr;
    QCtrlPoint prevT, currT;

    QPainterPath path;

    double thickness = m_solid ? 1.5 : 1.5;
    QColor solidLineColor = m_solid ? MainWindow::envelope_solidLineColor_enabled
                                    : MainWindow::envelope_solidLineColor_disabled;
    QColor dashLineCOlor = m_solid ? MainWindow::envelope_dashLineColor_enabled
                                   : MainWindow::envelope_dashLineColor_disabled;

    QPen solidPen(solidLineColor, thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen dashPen(dashLineCOlor, thickness, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

    QPointF veryStart(getActiveScope().bottomLeft() - pos());
    QPointF veryEnd(getActiveScope().bottomRight() - pos());

    if (points.size() > 0) {
        prev = veryStart;
        prevT = QCtrlPoint(curNote->getInPoint(), 0);
    } else {
        return;
    }

    int prevPenState = -1;
    int curPenState;

    // 画边框
    painter.setBrush(Qt::NoBrush);
    for (int i = 0; i <= points.size(); ++i) {

        if (i < points.size()) {
            curr = QPointF(points[i]->getPosition() - pos());
            currT = points[i]->getTPoint();
        } else {
            curr = veryEnd;
            currT = QCtrlPoint(curNote->getOutPoint(), 0);
        }

        double curTick = curNote->convertTimeToTick(currT.mX);
        double prevTick = curNote->convertTimeToTick(prevT.mX);

        bool tooShort = qAbs(curTick - prevTick) < 2; // 小于 2 tick

        // 开始描线
        curPenState = !tooShort;
        if (curPenState != prevPenState) {
            if (!path.isEmpty()) {
                painter.drawPath(path);
            }
            path.clear();
            path.moveTo(prev);
        }

        painter.setPen(tooShort ? dashPen : solidPen);
        path.lineTo(curr);

        if (i == points.size()) {
            if (!path.isEmpty()) {
                painter.drawPath(path);
            }
        }
        // 结束描线

        prev = curr;
        prevT = currT;
        prevPenState = curPenState;
    }
}

void SimpleEnvelope::paintEvent(QPaintEvent *event) {

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    double unit = 120 / curNote->getTempo() / 0.96;
    double prevUnit = getPrevNote() ? (120.0 / getPrevNote()->getTempo() / 0.96) : unit;
    double nextUnit = getNextNote() ? (120.0 / getNextNote()->getTempo() / 0.96) : unit;

    if (!curNote->isRest()) {
        if (Root->bShowEnvFill) {
            drawBackFill(painter);
        }
        if (Root->bShowEnvLines) {
            drawPreUtteranceAndOverlap(painter);
        }
        drawEnvelope(painter);
    }
}

void SimpleEnvelope::onUpdatePoint(SimplePoint *p, pointMoveEvent e) {
    int index = points.indexOf(p);
    if (index < 0) {
        return;
    }

    if (e == pointPress) {
        for (SimplePoint *it : points) {
            if (!Root->stageDraggedObjects.contains(it)) {
                it->tempSaveEvent();
            }
        }
    } else if (e == pointMoving) {
        updateSpecialPositions();

        for (SimplePoint *it : points) {
            if (it->isTemporary()) {
                it->tempMoveEvent();
            }
        }
    } else if (e == pointRelease || e == pointStable) {
        updateSpecialPositions();
        for (SimplePoint *it : points) {
            if (it->isTemporary()) {
                it->tempReleaseEvent();
            }
        }
        if (e != pointStable) {
            saveMovingPoints();
            saveToNote();
        }
    } else if (e == pointAbandon) {
        for (SimplePoint *it : points) {
            if (it->isTemporary()) {
                it->tempAbandonEvent();
            }
        }
        updateSpecialPositions();
    }

    // 更新前后音符
    update();
    if (curNote->m_prev && curNote->m_prev->m_envelope) {
        curNote->m_prev->m_envelope->update();
    }
}
