#include "VolumePoint.h"
#include "SimpleEnvelope.h"
#include "Widgets/VolumeArea.h"

#include "mainwindow.h"

VolumePoint::VolumePoint(SimpleScope *scope, EditorAreaWidget *parent)
    : SimplePoint(scope, parent) {
}

QPoint VolumePoint::limitArea(QPoint origin) {
    Qt::KeyboardModifiers c = QApplication::keyboardModifiers();

    bool cs = c == (Qt::ShiftModifier | Qt::ControlModifier);
    bool c1 = c == Qt::ControlModifier;
    bool s1 = c == Qt::ShiftModifier;

    if (!m_area) {
        return origin;
    }

    if (m_move && (cs || s1)) {
        SimplePoint *np = m_area->getNearestPoint(this, origin.x());

        double voiceOverlapPlus = m_area->getCurNote()->getInOverlapPoint();
        double voiceOverlapEnd = m_area->getCurNote()->getOutOverlapPoint();

        int overlapX = m_area->convertValueToPosX(voiceOverlapPlus) - m_radius;
        int noteX = m_area->getCurNote()->getStaticX() - m_radius;
        int endX = m_area->convertValueToPosX(voiceOverlapEnd) - m_radius;

        if (np && qAbs(origin.x() - np->x()) < qAbs(origin.x() - overlapX) &&
            qAbs(origin.x() - np->x()) < qAbs(origin.x() - noteX) &&
            qAbs(origin.x() - np->x()) < qAbs(origin.x() - endX)) {
            origin.setX(np->x());
        } else if (qAbs(origin.x() - overlapX) < qAbs(origin.x() - noteX) &&
                   qAbs(origin.x() - overlapX) < qAbs(origin.x() - endX)) {
            origin.setX(overlapX);
        } else if (qAbs(origin.x() - endX) < qAbs(origin.x() - noteX)) {
            origin.setX(endX);
        } else {
            origin.setX(noteX);
        }
    }
    // 默认限制 x
    if (origin.x() + m_radius < m_area->getActiveScope().left()) {
        origin.setX(m_area->getActiveScope().left() - m_radius);
    } else if (origin.x() + m_radius > m_area->getActiveScope().right()) {
        origin.setX(m_area->getActiveScope().right() - m_radius);
    }

    if (m_move && (cs || c1)) {
        origin.setY(m_windowPoint.y());
    }

    // 默认限制 y
    if (origin.y() + m_radius < m_area->getActiveScope().top()) {
        origin.setY(m_area->getActiveScope().top() - m_radius);
    } else if (origin.y() + m_radius > m_area->getActiveScope().bottom()) {
        origin.setY(m_area->getActiveScope().bottom() - m_radius);
    }

    return origin;
}

void VolumePoint::onRightClick() {
    m_tpoint.mY = 100.0;

    m_area->saveAndUpdate();
}

void VolumePoint::onDoubleClick() {
    if (m_area->getPointsList().size() == 5) {
        triggerDeletePoint();
    } else {
        int index = m_area->getPointsList().indexOf(this);
        if (index < 0) {
            return;
        } else if (index == m_area->getPointsList().size() - 1) {
            index--;
        }
        QPoint pos(
            (m_area->getPointsList()[index]->getX() + m_area->getPointsList()[index + 1]->getX()) /
                2,
            (m_area->getPointsList()[index]->getY() + m_area->getPointsList()[index + 1]->getY()) /
                2);

        m_area->addAndSavePoint(pos);
    }
}

void VolumePoint::triggerDeletePoint() {
    // 调用父亲把自己删了
    m_area->removePoint(this);
}

void VolumePoint::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    drawPoint(painter, MainWindow::volumePoint_coreColor, MainWindow::volumePoint_ringColor);
}
