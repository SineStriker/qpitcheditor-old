#ifndef SIMPLESCOPE_H
#define SIMPLESCOPE_H

#include <QWidget>

#include "LibUTAUQt/QUtauUtils.h"
#include "Widgets/BaseWidgets.h"

#include "QUtils.h"

class SimpleNote;
class SimplePoint;

class SimpleScope : public QWidget {
    Q_OBJECT
public:
    explicit SimpleScope(SimpleNote *curr, EditorAreaWidget *parent = NULL);
    virtual ~SimpleScope();

    virtual void reInitializeStyles();
    virtual void adjustLayer();
    virtual void adjustGeometry(); // 调整曲线显示区域大小
    virtual void adjustPoints();   // 调整控制点的位置
    virtual void limitAllPoints(); // 限制所有控制点位置

public:
    virtual void saveMovingPoints(); // 根据控制点坐标保存控制点（移动后保存）
    virtual void saveSinglePoint(SimplePoint *p); // 根据控制点坐标保存控制点（保存单个）
    virtual void addAndSavePoint(QPoint pos); // 根据鼠标坐标添加并保存控制点

    virtual double convertPosToValueX(int x);
    virtual double convertPosToValueY(int y);

    virtual int convertValueToPosX(double x);
    virtual int convertValueToPosY(double y);

    virtual void saveToNote(QVector<QCtrlPoint> arr = {});
    virtual void saveAndUpdate();

    virtual void removePoint(SimplePoint *p);
    virtual void removePoints(QVector<SimplePoint *> arr);

    virtual void initPoints(QVector<QCtrlPoint> arr = {});
    virtual void initAndAdjustPoints(QVector<QCtrlPoint> arr = {});

    virtual SimplePoint *createNewPoint();

    virtual SimplePoint *getNearestPoint(SimplePoint *p, int limitedPos);

    QVector<SimplePoint *> &getPointsList() {
        return points;
    }

    SimpleNote *getPrevNote();
    SimpleNote *getCurNote();
    SimpleNote *getNextNote();

    SimplePoint *firstPoint(); // 返回最左边的控制点
    SimplePoint *lastPoint();  // 返回最右边的控制点

    void setPointsVisible(bool value);

    virtual QRect getActiveScope();

    bool isEmpty() {
        return points.isEmpty();
    }

    bool detectOver(SimplePoint *p); // 查重

    void updateSpecialPositions(); // 更新控制点特殊位置标记

protected:
    EditorAreaWidget *m_parent;

    SimpleNote *curNote;           // 保存前驱音符与后继音符
    QVector<SimplePoint *> points; // 保存控制点（连接变量1）
    bool m_solid;

    void removeAllPoints();

public slots:
    virtual void onUpdatePoint(SimplePoint *p, pointMoveEvent e = pointMoving);

private:
    void InsertSort(); // 选择稳定的排序

signals:
};

#endif // SIMPLESCOPE_H
