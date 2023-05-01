#ifndef VOLUMEPOINT_H
#define VOLUMEPOINT_H

#include "SimpleNote.h"
#include "SimplePoint.h"

#include <QObject>
#include <QWidget>

class VolumeArea;
class SimpleEnvelope;

class VolumePoint : public SimplePoint {
    Q_OBJECT
public:
    explicit VolumePoint(SimpleScope *scope, EditorAreaWidget *parent = NULL);

private:
    QPoint limitArea(QPoint origin) override;
    void onRightClick() override;
    void onDoubleClick() override;

    void triggerDeletePoint();

    void paintEvent(QPaintEvent *event) override;
};

#endif // VOLUMEPOINT_H
