#ifndef SIMPLEPOINT_H
#define SIMPLEPOINT_H

#include <QWidget>

#include "SimpleCurves.h"
#include "SimpleNote.h"
#include "SimpleScope.h"

#include "Widgets/BaseWidgets.h"

/*--------------------------------------音高控制点--------------------------------------*/
class SimplePoint : public SimpleObject {
    Q_OBJECT
public:
    explicit SimplePoint(EditorAreaWidget *parent = NULL);
    explicit SimplePoint(SimpleScope *scope, EditorAreaWidget *parent = NULL);
    ~SimplePoint();

    void adjustLayer();

    void setSelected(bool b) override;

    // 其他控制点在拖动中
    bool isTemporary() {
        return m_temp;
    }

    // 拖动中
    bool isMoving() {
        return m_moving;
    }

    void setScopeArea(SimpleScope *scope) {
        m_area = scope;
    }

    SimpleScope *getScopeArea() {
        return m_area;
    }

    void setPosition(int x, int y) {
        move(x - m_radius, y - m_radius);
    }

    void setPosition(QPoint p) {
        move(p.x() - m_radius, p.y() - m_radius);
    }

    QPoint getPosition() {
        return QPoint(x() + m_radius, y() + m_radius);
    }

    void setSpecialPos(specialPointPositions s) {
        specialPos = s;
    }

    specialPointPositions getSpecialPos() {
        return specialPos;
    }

    specialPointPositions getStaticSpecialPos() {
        if (m_move || m_temp) {
            return m_startFlag;
        } else {
            return specialPos;
        }
    }

    double getStaticX() {
        if (m_moving || m_temp) {
            return m_windowPoint.x() + m_radius;
        } else {
            return getX();
        }
    }

    double getStaticY() {
        if (m_moving || m_temp) {
            return m_windowPoint.y() + m_radius;
        } else {
            return getY();
        }
    }

    double getGeneralX(bool constraint = false) {
        if (m_moving || constraint) {
            return m_area->convertPosToValueX(getX());
        } else {
            return m_tpoint.mX;
        }
    }

    double getGeneralY(bool constraint = false) {
        if (m_moving || constraint) {
            return m_area->convertPosToValueY(getY());
        } else {
            return m_tpoint.mY;
        }
    }

    int getX() {
        return x() + m_radius;
    }
    int getY() {
        return y() + m_radius;
    }

    QCtrlPoint &getTPoint() {
        return m_tpoint;
    }

    void setTPoint(QCtrlPoint p) {
        m_tpoint = p;
    }

    void setTPointXY(double x, double y) {
        m_tpoint.mX = x;
        m_tpoint.mY = y;
    }

    void setTPointX(double x) {
        m_tpoint.mX = x;
    }

    void setTPointY(double y) {
        m_tpoint.mY = y;
    }

    void linkPressEvent(QMouseEvent *event) override;
    void linkMoveEvent(QMouseEvent *event) override;
    void linkReleaseEvent(QMouseEvent *event) override;
    void autoReleaseEvent(QMouseEvent *event, bool flag = false) override;

    void tempSaveEvent() {
        m_temp = true;
        m_windowPoint = this->frameGeometry().topLeft();
    }
    void tempMoveEvent() {
        if (!m_temp) {
            return;
        }
        move(limitArea(m_windowPoint));
    }

    void externLimitArea() {
        move(limitArea(frameGeometry().topLeft()));
    }

    void tempReleaseEvent() {
        m_temp = false;
    }

    // 未知原因回归原位
    void tempAbandonEvent() {
        m_temp = false;
        move(m_windowPoint);
    }

protected:
    QPoint limitArea(QPoint origin) override;
    virtual void onRightClick();
    virtual void onDoubleClick();

    virtual void drawPoint(QPainter &painter, QColor coreCL, QColor ringCL);

    void enterEvent(QEvent *event) override {
        rollOver = true;
        update();
    }
    void leaveEvent(QEvent *event) override {
        rollOver = false;
        update();
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void triggerDeletePoint();

    EditorAreaWidget *m_parent;

    int m_radius; // 半径
    double m_pointRadius;
    double m_pointPadding;
    double m_ringWidth;

    bool m_selected;
    bool m_move;
    bool m_moving;
    bool m_temp;
    bool rollOver;

    QCtrlPoint m_tpoint;
    specialPointPositions specialPos;

    QPoint m_startPoint;  // 拖动时保存本体起始坐标
    QPoint m_windowPoint; // 拖动时保存鼠标起始坐标
    specialPointPositions m_startFlag;

    SimpleScope *m_area; // 被控

signals:
    void updatePoint(SimplePoint *p, pointMoveEvent e = pointMoving);
};

#endif // SIMPLEPOINT_H
