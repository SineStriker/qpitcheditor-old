#ifndef SIMPLEENVELOPE_H
#define SIMPLEENVELOPE_H

#include <QWidget>

#include "SimpleScope.h"
#include "VolumePoint.h"
#include "Widgets/VolumeArea.h"

class SimpleEnvelope : public SimpleScope {
    Q_OBJECT
public:
    explicit SimpleEnvelope(SimpleNote *note, VolumeArea *parent = nullptr);

    void reInitializeStyles() override;

    void adjustGeometry() override; // 调整曲线显示区域大小

public:
    void saveToNote(QVector<QCtrlPoint> arr = {}) override;

    void rearrangePoints();
    void adjustPoints() override;

    QVector<QCtrlPoint> toCorrectPoints();
    QVector<QCtrlPoint> fromCorrectPoints();

    double convertPosToValueX(int x) override;
    double convertPosToValueY(int y) override;

    int convertValueToPosX(double x) override;
    int convertValueToPosY(double y) override;

    void removePoint(SimplePoint *p) override;
    void removePoints(QVector<SimplePoint *> arr) override;

    QRect getActiveScope() override;

    void initPoints(QVector<QCtrlPoint> arr) override;
    void initAndAdjustPoints(QVector<QCtrlPoint> arr = {}) override;

    void updateVisibility();

private:
    SimplePoint *createNewPoint() override;

    void drawPreUtteranceAndOverlap(QPainter &painter);
    void drawBackFill(QPainter &painter);
    void drawEnvelope(QPainter &painter);
    void paintEvent(QPaintEvent *event) override;

public slots:
    void onUpdatePoint(SimplePoint *p, pointMoveEvent e = pointMoving) override;

signals:
};

#endif // SIMPLEENVELOPE_H
