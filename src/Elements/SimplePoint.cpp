#include "SimplePoint.h"
#include "SimpleCurves.h"
#include "mainwindow.h"

/*--------------------------------------音高控制点--------------------------------------*/

SimplePoint::SimplePoint(EditorAreaWidget *parent) : SimpleObject(parent) {
    m_parent = parent;
    adjustLayer();

    m_area = nullptr;
    m_radius = 10;
    m_ringWidth = 2;
    m_pointPadding = 2;
    m_pointRadius = 3;

    specialPos = pointBetween;
    m_selected = false;
    m_move = false;
    m_moving = false;
    m_temp = false;
    rollOver = false;

    setFixedSize(m_radius * 2, m_radius * 2);
    show();
}

SimplePoint::SimplePoint(SimpleScope *scope, EditorAreaWidget *parent) : SimpleObject(parent) {
    m_parent = parent;
    adjustLayer();

    m_area = scope;
    m_radius = 10;
    m_ringWidth = 2;
    m_pointPadding = 2;
    m_pointRadius = 3;

    specialPos = pointBetween;
    m_selected = false;
    m_move = false;
    m_moving = false;
    m_temp = false;
    rollOver = false;

    setFixedSize(m_radius * 2, m_radius * 2);
    show();
}

SimplePoint::~SimplePoint() {
}

void SimplePoint::adjustLayer() {
    if (m_parent->getPointSegLayer()) {
        this->stackUnder(m_parent->getPointSegLayer());
    }
}

void SimplePoint::setSelected(bool b) {
    m_selected = b;
    update();
}

// 联动移动
void SimplePoint::linkPressEvent(QMouseEvent *event) {
    raise();
    adjustLayer();

    Root->addDraggedObject(this);
    m_move = true;

    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
    m_startFlag = specialPos;
    m_startPoint = global_pos;
    m_windowPoint = this->frameGeometry().topLeft();
}

void SimplePoint::linkMoveEvent(QMouseEvent *event) {
    if (m_area) {
        m_area->update();
    }

    m_moving = true;

    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
    QPoint relative_pos = global_pos - m_startPoint;

    this->move(limitArea(m_windowPoint + relative_pos));
}

void SimplePoint::linkReleaseEvent(QMouseEvent *event) {
    Root->removeDraggedObject(this);

    if (m_area) {
        m_area->update();
    }
    m_move = false;
    m_moving = false;
}

void SimplePoint::autoReleaseEvent(QMouseEvent *event, bool flag) {

    emit updatePoint(this, pointAbandon);

    if (!flag) {
        Root->removeDraggedObject(this);
    }

    if (m_area) {
        m_area->update();
    }

    // 改变移动状态.
    this->move(m_windowPoint);
    m_move = false;
    m_moving = false;
}

void SimplePoint::mouseDoubleClickEvent(QMouseEvent *event) {
    // 忽略所有控制点
    Root->removeAllSelectedPoints();

    if (event->button() == Qt::LeftButton) {
        // 双击删除控制点
        onDoubleClick();
    } else if (event->button() == Qt::RightButton) {
        onRightClick();
    }
}

void SimplePoint::mousePressEvent(QMouseEvent *event) {

    qDebug() << "this" << m_tpoint.mX << m_tpoint.mY << specialPos;

    // 选择控制点时自动忽略音符
    Root->removeAllSelectedNotes();

    if (event->button() == Qt::LeftButton) {
        Root->autoAddSelectedPoint(this);
        update();

        emit updatePoint(this, pointPress);
        for (SimplePoint *it : Root->stageSelectedPoints) {
            it->linkPressEvent(event);
        }

    } else if (event->button() == Qt::RightButton && !m_move) {
        onRightClick();

        // 忽略所有控制点
        Root->removeAllSelectedPoints();
    }
}

void SimplePoint::mouseMoveEvent(QMouseEvent *event) {
    if (!m_move) {
        return;
    }
    if (event->buttons() & Qt::LeftButton) {
        emit updatePoint(this, pointMoving); // 发送移动控制点信号
        for (SimplePoint *it : Root->stageSelectedPoints) {
            it->linkMoveEvent(event);
        }
    }
}

void SimplePoint::mouseReleaseEvent(QMouseEvent *event) {

    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
    QPoint relative_pos = global_pos - m_startPoint;

    if (event->button() == Qt::LeftButton) {
        if (!m_moving) {
            emit emit updatePoint(this, pointStable);
        } else {
            emit updatePoint(this, pointRelease); // 发送释放控制点信号
        }
        for (SimplePoint *it : Root->stageSelectedPoints) {
            it->linkReleaseEvent(event);
        }
        /*// 检测重叠
        if (m_area && m_area->detectOver(this)) {
            triggerDeletePoint();
        }*/
    }
}

QPoint SimplePoint::limitArea(QPoint origin) {
    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    bool cs = c == (Qt::ShiftModifier | Qt::ControlModifier);
    bool c1 = c == Qt::ControlModifier;
    bool s1 = c == Qt::ShiftModifier;

    unsigned h = noteUnitHeightArr[Root->noteUnitHIndex];
    unsigned h1 = h / 2;

    int unit_y = int(double(origin.y() + m_radius) / h1 - 0.5) * h1 - m_radius + double(h) / 2;

    if (!m_area) {
        return origin;
    }

    if (m_move && (cs || s1)) {
        SimplePoint *np = m_area->getNearestPoint(this, origin.x());
        if (np) {
            origin.setX(np->x());
        }
    }
    // 默认限制 x
    if (origin.x() + m_radius < m_area->getActiveScope().left()) {
        origin.setX(m_area->x() - m_radius);
    } else if (origin.x() + m_radius > m_area->getActiveScope().right()) {
        origin.setX(m_area->getActiveScope().right() - m_radius);
    }

    if (specialPos == pointBetween) {
        // 中间或前驱为休止符的控制点可以自由移动
        if (m_move && (cs || c1)) {
            origin.setY(unit_y);
        }
    } else if (specialPos == pointLeftmost && m_area->getPrevNote()) {
        if (m_area->getPrevNote()->isRest()) {
            if (m_move && (cs || c1)) {
                origin.setY(unit_y);
            }
        } else {
            origin.setY(m_area->getPrevNote()->y() + m_area->getPrevNote()->height() / 2 -
                        m_radius);
        }
    } else {
        origin.setY(m_area->getCurNote()->y() + m_area->getCurNote()->height() / 2 - m_radius);
    }

    // 默认限制 y
    if (origin.y() + m_radius < m_area->getActiveScope().top()) {
        origin.setY(m_area->getActiveScope().top() - m_radius);
    } else if (origin.y() + m_radius > m_area->getActiveScope().bottom()) {
        origin.setY(m_area->getActiveScope().bottom() - m_radius);
    }

    return origin;
}

void SimplePoint::onRightClick() {
    ControlPointType type = m_tpoint.mP;

    // 第一个点没法变
    if (specialPos == pointLeftmost) {
        return;
    }

    if (type == sType) { // S型
        type = lineType;
    } else if (type == rType) { // R型
        type = jType;
    } else if (type == jType) { // J型
        type = sType;
    } else {
        type = rType;
    }

    m_tpoint.mP = type;
    if (m_area) {
        m_area->update();
        m_area->saveToNote();
    }
}

void SimplePoint::onDoubleClick() {
    triggerDeletePoint();
}

void SimplePoint::drawPoint(QPainter &painter, QColor coreCL, QColor ringCL) {
    QRect point(m_radius - m_pointRadius, m_radius - m_pointRadius, m_pointRadius * 2,
                m_pointRadius * 2);
    QRect ring(m_pointPadding, m_pointPadding, m_radius * 2 - m_pointPadding * 2,
               m_radius * 2 - m_pointPadding * 2);

    painter.setRenderHint(QPainter::Antialiasing);

    // 画圆点
    painter.setPen(Qt::transparent);
    painter.setBrush(coreCL);
    painter.drawEllipse(point);

    if (rollOver || m_selected) {
        // 画圆环
        painter.setPen(QPen(ringCL, m_ringWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::transparent);
        painter.drawEllipse(ring);
    }
}

void SimplePoint::triggerDeletePoint() {
    // 调用父亲把自己删了
    m_area->removePoint(this);
}

void SimplePoint::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    drawPoint(painter, MainWindow::pitchPoint_coreColor, MainWindow::pitchPoint_ringColor);
}
