#include "SimpleCurves.h"
#include "SimpleNote.h"
#include "SimplePoint.h"

#include "mainwindow.h"

const QVector<QCtrlPoint> SimpleCurves::defaultPortamento = {QCtrlPoint(-15, 0), QCtrlPoint(15, 0)};

/*--------------------------------------Mode2音高曲线--------------------------------------*/

SimpleCurves::SimpleCurves(SimpleNote *curr, EditorAreaWidget *parent) : SimpleScope(curr, parent) {

    // 颤音部分
    btnSide = btnPhase = btnFrequency = nullptr;
    btnEaseIn = btnEaseOut = btnTop = btnBottom = btnCenter = nullptr;
    m_showVBR = false;

    setAttribute(Qt::WA_TransparentForMouseEvents, true); // 本身不响应鼠标事件

    adjustLayer();
    initAndAdjustPoints();
}

SimpleCurves::~SimpleCurves() {
    // 曲线析构时连带控制点一起析构
    removeAllPoints();

    if (m_showVBR) {
        removeVibrtoButtons();
    }
}

void SimpleCurves::reInitializeStyles() {
    if (m_showVBR) {
        btnSide->setColor(MainWindow::vibrato_buttonBorderColor,
                          MainWindow::vibrato_buttonFillColor);
        btnPhase->setColor(MainWindow::vibrato_buttonBorderColor,
                           MainWindow::vibrato_buttonFillColor);
        btnFrequency->setColor(MainWindow::vibrato_buttonBorderColor,
                               MainWindow::vibrato_buttonFillColor);
    }
    update();
}

void SimpleCurves::adjustGeometry() {
    // 参考了前后音符的长度，必须在前后音符尺寸更新后调用

    int startX = curNote->m_prev ? curNote->m_prev->x() : curNote->x();
    int prevWidth = curNote->m_prev ? curNote->m_prev->width() : 0;
    int nextWidth = curNote->m_next ? curNote->m_next->width() : 0;

    QRect rect(startX, 0, prevWidth + curNote->width() + nextWidth, m_parent->height());

    activeWidth = prevWidth + curNote->width();

    if (geometry() != rect) {
        setGeometry(rect);
        update();
    }
}

void SimpleCurves::adjustPoints() {
    SimpleScope::adjustPoints();
    adjustVibrato();
}

double SimpleCurves::convertPosToValueX(int x) {
    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    int noteX = curNote->getStaticX();
    double tick = double(x - noteX) * 480 / currentWidth;
    double time = curNote->convertTickToTime(tick);

    return time;
}

double SimpleCurves::convertPosToValueY(int y) {
    unsigned currentHeight = noteUnitHeightArr[Root->noteUnitHIndex];
    int noteY = curNote->getStaticY() + double(currentHeight) / 2;
    double cent = -1 * double(y - noteY) * 10 / currentHeight;
    return cent;
}

int SimpleCurves::convertValueToPosX(double x) {
    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    int noteX = curNote->getStaticX();
    double tick = curNote->convertTimeToTick(x);
    int toX = noteX + tick / 480 * currentWidth;

    return toX;
}

int SimpleCurves::convertValueToPosY(double y) {
    unsigned currentHeight = noteUnitHeightArr[Root->noteUnitHIndex];
    int noteY = curNote->getStaticY() + double(currentHeight) / 2;
    int toY = noteY - y / 10 * currentHeight;

    return toY;
}

// 删除一个点
void SimpleCurves::removePoint(SimplePoint *p) {
    qDebug() << "Remove Point : " << p;

    if (p->getSpecialPos() != pointBetween && points.size() <= 2) {
        qDebug() << "Remove failure.";
        return;
    }

    SimpleScope::removePoint(p);
}


// 删除多个点
void SimpleCurves::removePoints(QVector<SimplePoint *> arr) {
    if (points.size() - arr.size() < 2) {
        int leftIndex = arr.indexOf(firstPoint());
        if (leftIndex >= 0) {
            arr.removeAt(leftIndex);
        }

        int rightIndex = arr.indexOf(lastPoint());
        if (rightIndex >= 0) {
            arr.removeAt(rightIndex);
        }
    }

    // 没删点
    if (arr.isEmpty()) {
        return;
    }

    SimpleScope::removePoints(arr);
}

void SimpleCurves::addPortamento() {
    if (!points.empty()) {
        return;
    }

    initAndAdjustPoints(defaultPortamento);

    // 保存到音符
    saveToNote();
}

void SimpleCurves::removePortamento() {
    if (points.empty()) {
        return;
    }
    qDebug() << "Remove portamento";

    for (int i = 0; i < points.size(); ++i) {
        if (points[i]) {
            delete points[i];
            points[i] = nullptr;
        }
    }
    points.clear();

    // 删除控制点后续操作
    updateSpecialPositions();

    // 保存到音符
    saveToNote();

    update();
}

// 初始化控制点
void SimpleCurves::initPoints(QVector<QCtrlPoint> arr) {
    if (arr.empty()) {
        arr = curNote->m_note.Mode2Pitch;
    }

    SimpleScope::initPoints(arr);
}

QRect SimpleCurves::getActiveScope() {
    QRect rect;

    rect.setLeft(x());
    rect.setRight(x() + activeWidth);

    rect.setTop(y());
    rect.setBottom(y() + height());

    return rect;
}

void SimpleCurves::setSolid(bool value) {
    m_solid = value;
    setPointsVisible(value);
    if (!value) {
        removeVibrtoButtons();
    }
    update();
}

// 涉及历史记录更改
void SimpleCurves::saveToNote(QVector<QCtrlPoint> arr) {
    int i;
    SimplePoint *p;

    // 建立记录变量
    NoteOperation *n = new NoteOperation(mode2Change);

    // 设定修改前的数据
    n->setOrg(curNote);

    if (arr.isEmpty()) {
        // 将控制点改动保存到音符
        curNote->m_note.Mode2Pitch.clear();
        for (i = 0; i < points.size(); ++i) {
            p = points[i];
            // 最左与最右的点直接标记为 y=0
            if (p->getSpecialPos() == pointRightmost) {
                p->setTPointXY(p->getTPoint().mX, 0);
            } else if (p->getSpecialPos() == pointLeftmost &&
                       !(getPrevNote() && getPrevNote()->isRest())) {
                p->setTPointXY(p->getTPoint().mX, 0);
            }
            curNote->m_note.Mode2Pitch.push_back(p->getTPoint());
        }
        curNote->m_note.mode2Sort();
    } else {
        curNote->m_note.Mode2Pitch = arr;
    }

    // 设定修改后的数据
    n->setNew(curNote);
    Root->historyAddStep(n);
}

/*--------------------------------------图像绘制--------------------------------------*/
void SimpleCurves::paintEvent(QPaintEvent *event) {

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // double unit = 120 / curNote->getTempo() / 0.96;
    // double prevUnit = getPrevNote() ? (120.0 / getPrevNote()->getTempo() / 0.96) : unit;
    // double nextUnit = getNextNote() ? (120.0 / getNextNote()->getTempo() / 0.96) : unit;

    // 画实际区域
    if (!curNote->isRest()) {
        if (Root->bReflect) {
            drawReflect(painter);
        }
        if (Root->bOverlap) {
            drawOverlap(painter);
        }
    }

    // 画音高线
    if (Root->bShowMode2) {
        drawMode2(painter);
        drawVibrato(painter);
    }

    if (Root->bShowMode1) {
        drawMode1(painter);
    }

    // 画颤音编辑器
    if (m_showVBR) {
        drawVibratoEditor(painter);
    }

    if (Root->bShowFlags) {
        drawParameters(painter);
    }
}

void SimpleCurves::drawReflect(QPainter &painter) {
    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    QBrush realNoteBrush(MainWindow::reflect_fillColor);

    // 画长度虚影

    double voiceStartPlus = -curNote->getInPoint();
    double voiceEndPlus = curNote->getOutPoint() - curNote->getDuration();

    QRectF realNoteRect;

    double voiceStartPlusTime = curNote->convertTimeToTick(voiceStartPlus);
    double voiceEndPlusTime = getNextNote() ? getNextNote()->convertTimeToTick(voiceEndPlus) : 0;

    realNoteRect.setLeft(curNote->getStaticX() - voiceStartPlusTime / 480 * currentWidth -
                         this->x());
    realNoteRect.setRight(curNote->getStaticX() + curNote->width() +
                          voiceEndPlusTime / 480 * currentWidth - this->x());
    realNoteRect.setTop(curNote->getStaticY() - this->y());
    realNoteRect.setBottom(curNote->getStaticY() + curNote->height() - this->y());

    painter.setPen(Qt::NoPen);
    painter.setBrush(realNoteBrush);
    painter.drawRect(realNoteRect);
}

void SimpleCurves::drawOverlap(QPainter &painter) {
    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    QPen overlapPen(MainWindow::overlap_lineColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    // 画重叠线
    double voiceOverlapPlus = -curNote->getInOverlapPoint();
    double voiceOverlapPlusTime = curNote->convertTimeToTick(voiceOverlapPlus);

    double voiceOverlapLineX =
        curNote->getStaticX() - voiceOverlapPlusTime / 480 * currentWidth - this->x();

    QLineF oveLine(voiceOverlapLineX, curNote->getStaticY() - this->y(), voiceOverlapLineX,
                   curNote->getStaticY() + curNote->height() - this->y());
    painter.setBrush(Qt::NoBrush);
    painter.setPen(overlapPen);
    painter.drawLine(oveLine);
}

void SimpleCurves::drawMode1(QPainter &painter) {
    unsigned w = noteUnitWidthArr[Root->noteUnitWIndex];

    QPointF prev, curr;

    QPainterPath path;

    QColor solidLineColor = !m_solid ? MainWindow::mode1_solidLineColor_enabled
                                     : MainWindow::mode1_solidLineColor_disabled;
    QColor dashLineCOlor = !m_solid ? MainWindow::mode1_dashLineColor_enabled
                                    : MainWindow::mode1_dashLineColor_disabled;

    double thickness = !m_solid ? 1.5 : 1.5;

    double pbstart = (curNote->m_note.m_pbstart == NODEF_DOUBLE) ? curNote->getPBStart()
                                                                 : (curNote->m_note.m_pbstart);
    double pbycur;

    int x0 = convertValueToPosX(pbstart);
    int xn = convertValueToPosX(curNote->getOutPoint());
    double stepx = double(w) / 4 / 480 * 5;

    double x1 = x0 - stepx;
    double y1 = convertValueToPosY(0);

    int i;

    QVector<QPointF> pointList, vectorList;

    QPen solidPen(solidLineColor, thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen dashPen(dashLineCOlor, thickness, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(solidPen);

    // 添加点列
    for (i = 0; i < curNote->m_note.m_pitches.size(); ++i) {
        pbycur = (curNote->m_note.m_pitches[i] == NODEF_DOUBLE) ? 0 : curNote->m_note.m_pitches[i];
        x1 += stepx;
        y1 = convertValueToPosY(pbycur / 10);
        pointList.push_back(QPointF(x1, y1));
    }
    if (pointList.isEmpty()) {
        pointList.push_back(QPointF(x1, y1));
    }
    while (x1 < xn) {
        x1 += stepx;
        pointList.push_back(QPointF(x1, y1));
    }

    // 保存斜率
    for (i = 0; i < pointList.size(); ++i) {
        if (i == 0 || i == pointList.size() - 1) {
            vectorList.push_back(QPointF(0, 0));
            continue;
        }
        vectorList.push_back(pointList[i + 1] - pointList[i - 1]);
    }

    // 画曲线
    QPointF c1, c2;
    for (i = 0; i < pointList.size(); ++i) {
        if (i == 0) {
            path.moveTo(pointList[i] - pos());
            continue;
        }
        c1 = pointList[i - 1] + vectorList[i - 1] / 2;
        c2 = pointList[i] - vectorList[i] / 2;
        // path.cubicTo(c1 - pos(), c2 - pos(), pointList[i] - pos());
        path.lineTo(pointList[i] - pos());
    }

    painter.drawPath(path);
}

void SimpleCurves::drawMode2(QPainter &painter) {

    QPointF prev, curr;
    QCtrlPoint prevT, currT;

    QPainterPath path;
    ControlPointType type;

    double thickness = m_solid ? 1.5 : 1.5;

    QColor solidLineColor = m_solid ? MainWindow::mode2_solidLineColor_enabled
                                    : MainWindow::mode2_solidLineColor_disabled;
    QColor dashLineCOlor = m_solid ? MainWindow::mode2_dashLineColor_enabled
                                   : MainWindow::mode2_dashLineColor_disabled;
    QColor invalidLineColor = m_solid ? MainWindow::mode2_invalidLineColor_enabled
                                      : MainWindow::mode2_invalidLineColor_disabled;

    QPen solidPen(solidLineColor, thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen dashPen(dashLineCOlor, thickness, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QPen invalidPen(invalidLineColor, thickness, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

    painter.setBrush(Qt::NoBrush);

    if (points.size() > 0) {
        prev = QPointF(points[0]->getPosition()) - QPointF(pos());
        prevT = points[0]->getTPoint();
    } else {
        // 起始点设在音符开头
        prev =
            QPointF(curNote->getStaticX(), curNote->getStaticY() + double(curNote->height()) / 2) -
            QPointF(pos());
        prevT = QCtrlPoint(0, 0);
    }

    int prevPenState = -1;
    int curPenState;

    for (int i = 1; i < points.size(); ++i) {

        type = points[i]->getTPoint().mP;

        curr = QPointF(points[i]->getPosition()) - QPointF(pos());
        currT = points[i]->getTPoint();

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

        if (type == sType) { // S型
            pathCosineTo(path, prev, curr);
        } else if (type == rType) { // R型
            pathCosineRTo(path, prev, curr);
        } else if (type == jType) { // J型
            pathCosineJTo(path, prev, curr);
        } else {
            path.lineTo(curr);
        }
        if (i == points.size() - 1) {
            if (!path.isEmpty()) {
                painter.drawPath(path);
            }
        }
        // 结束描线

        prev = curr;
        prevT = currT;
        prevPenState = curPenState;
    }

    // 尝试寻找后继
    SimpleNote *nextNote = getNextNote();
    QPainterPath path2;

    painter.setPen(dashPen);

    // 后继点默认设为音符尾部
    curr = QPointF(curNote->getStaticX() + curNote->width(),
                   curNote->getStaticY() + double(curNote->height()) / 2) -
           QPointF(pos());

    // 不存在后继音符
    if (!nextNote) {
        path2.moveTo(prev);
        pathCosineTo(path2, prev, curr);
        painter.drawPath(path2);

        return;
    }

    SimpleCurves *nextCurve = nextNote->getMode2Pitch();
    SimplePoint *nextFirstPoint = nullptr;

    // 后继音符没有音高线或者获取第一个点失败
    if (!nextCurve || nextCurve->isEmpty() || !(nextFirstPoint = nextCurve->firstPoint())) {
        path2.moveTo(prev);
        pathCosineTo(path2, prev, curr);

        prev = curr;

        // 后继点移动到下一个音符开头
        curr = QPointF(nextNote->getStaticX(),
                       nextNote->getStaticY() + double(nextNote->height()) / 2) -
               QPointF(pos());

        path2.moveTo(prev);
        pathCosineTo(path2, prev, curr);
        painter.drawPath(path2);

        return;
    }

    curr = QPointF(nextFirstPoint->getPosition()) - QPointF(pos());

    path2.moveTo(prev);
    pathCosineTo(path2, prev, curr);

    if (prev.x() > curr.x()) {
        painter.setPen(invalidPen);
    }
    painter.drawPath(path2);
}

void SimpleCurves::drawParameters(QPainter &painter) {

    if (curNote->isRest()) {
        return;
    }

    unsigned w = noteUnitWidthArr[Root->noteUnitWIndex];

    QColor flagsColor = MainWindow::parameters_textColor;

    painter.setPen(flagsColor);
    painter.setBrush(Qt::transparent);
    painter.setFont(QFont("Microsoft YaHei UI", 8));

    QString strMod =
        (curNote->getModulation() == 0) ? "" : ("Mod " + QString::number(curNote->getModulation()));
    QString strFlags = curNote->getFlags();

    QString strPara =
        strMod.isEmpty() ? strFlags : (strMod + (strFlags.isEmpty() ? "" : " / ") + strFlags);

    int f = (curNote->getStaticY() + curNote->height() * 1.5 >= y() + height()) ? -1 : 1;

    QRectF entityRect(curNote->getStaticX() - x(),
                      curNote->getStaticY() + f * curNote->height() - y(), curNote->width(),
                      curNote->height());
    QRectF textRect(entityRect);
    textRect.setX(entityRect.x() + 2 + w / 128.0);

    painter.drawText(textRect, Qt::AlignVCenter, strPara, &entityRect);
}

void SimpleCurves::drawVibrato(QPainter &painter) {

    QVector<double> *arr = nullptr;

    if (m_showVBR) {
        arr = &tempVBR;
    } else {
        arr = &curNote->m_note.m_vbr;
    }

    if ((*arr).size() < 8) {
        return;
    }

    QColor solidLineColor = m_solid ? MainWindow::mode2_solidLineColor_enabled
                                    : MainWindow::mode2_solidLineColor_disabled;
    QPen solidPen(solidLineColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    unsigned currentWidth = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    double amplitude = curNote->height() * (*arr)[2] / 100.0; // 振幅

    double pixel_len = curNote->width() * (*arr)[0] / 100.0; // 颤音像素长度
    QPointF pixel_org(curNote->getStaticX() + curNote->width() - pixel_len,
                      curNote->getStaticY() + curNote->height() / 2);

    double period = (*arr)[1]; // 周期

    if (period < minPeriod) {
        period = minPeriod;
    }
    if (period > maxPeriod) {
        period = maxPeriod;
    }

    double tick_time = curNote->convertTimeToTick(period); // ticks 周期
    double pixel_time = tick_time / 480 * currentWidth;    // 一个周期的像素长度

    double phase = (*arr)[5];

    QPainterPath path;

    double i;
    double j, k, p, y;

    double a, b;
    double c = pixel_org.y();

    k = 1 / pixel_time * 2 * PI;
    p = phase / 100.0 * 2 * PI;

    double step = pixel_time / 32;

    double start = pixel_org.x();
    double end = curNote->getStaticX() + curNote->width();
    double easeIn = (*arr)[3] / 100.0 * pixel_len;
    double easeOut = (1 - (*arr)[4] / 100.0) * pixel_len;

    double offset = (*arr)[6] / 100.0;

    while (step < 0.2) {
        step *= 2.0;
    }
    while (step > 5.0) {
        step /= 2.0;
    }

    i = start;

    path.moveTo(QPointF(i, c) - QPointF(pos()));

    while (i <= end) {
        j = i - start;
        y = amplitude * sin(k * j - p);

        a = 1;
        b = offset;

        // 套上偏移
        y += b * amplitude;

        if (j < easeIn) {
            a *= j / easeIn;
        }
        if (j > easeOut) {
            a *= 1 - (j - easeOut) / (pixel_len - easeOut);
        }

        // 套上包络
        y = a * y;

        path.lineTo(QPointF(i, c - y) - QPointF(pos()));
        i += step;
    }

    // 画最后一个点
    if (i != end) {
        i = end;

        j = i - start;
        y = amplitude * sin(k * j - p);

        a = 1;
        b = offset;

        // 套上偏移
        y += b * amplitude;

        if (j < easeIn) {
            a *= j / easeIn;
        }
        if (j > easeOut) {
            a *= 1 - (j - easeOut) / (pixel_len - easeOut);
        }

        // 套上包络
        y = a * y;

        path.lineTo(QPointF(i, c - y) - QPointF(pos()));
    }
    path.lineTo(QPointF(i, c) - QPointF(pos()));

    painter.setBrush(Qt::NoBrush);
    painter.setPen(solidPen);
    painter.drawPath(path);
}

void SimpleCurves::drawVibratoEditor(QPainter &painter) {
    QColor vbrLineColor = MainWindow::vibrato_frameColor;

    QPen vbrPen(vbrLineColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPainterPath path;

    painter.setBrush(Qt::NoBrush);
    painter.setPen(vbrPen);

    // 上
    path.moveTo(QPointF(pVibratoStandardStart() - pos()));
    path.lineTo(QPointF(pVibratoEaseInTop() - pos()));
    path.lineTo(QPointF(pVibratoEaseOutTop() - pos()));
    path.lineTo(QPointF(pVibratoStandardEnd() - pos()));
    painter.drawPath(path);

    // 中
    path.moveTo(QPointF(pVibratoStandardStart() - pos()));
    path.lineTo(QPointF(pVibratoEaseInCenter() - pos()));
    path.lineTo(QPointF(pVibratoEaseOutCenter() - pos()));
    path.lineTo(QPointF(pVibratoStandardEnd() - pos()));
    painter.drawPath(path);

    // 下
    path.moveTo(QPointF(pVibratoStandardStart() - pos()));
    path.lineTo(QPointF(pVibratoEaseInBottom() - pos()));
    path.lineTo(QPointF(pVibratoEaseOutBottom() - pos()));
    path.lineTo(QPointF(pVibratoStandardEnd() - pos()));
    painter.drawPath(path);

    // 左
    path.moveTo(QPointF(pVibratoEaseInTop() - pos()));
    path.lineTo(QPointF(pVibratoEaseInBottom() - pos()));
    painter.drawPath(path);

    // 右
    path.moveTo(pVibratoEaseOutTop() - pos());
    path.lineTo(pVibratoEaseOutBottom() - pos());
    painter.drawPath(path);
}

/*--------------------------------------事件处理--------------------------------------*/
void SimpleCurves::onUpdatePoint(SimplePoint *p, pointMoveEvent e) {
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
    if (curNote->m_prev && curNote->m_prev->m_pitch) {
        curNote->m_prev->m_pitch->update();
    }
}
