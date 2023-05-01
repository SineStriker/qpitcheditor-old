#ifndef SIMPLECURVES_H
#define SIMPLECURVES_H

#include <QWidget>

#include "Widgets/BaseWidgets.h"

#include "QEvent"
#include "QUtils.h"
#include "SimpleScope.h"
#include "SimpleVibrato.h"

class NotesArea;

class SimpleNote;
class SimplePoint;

/*--------------------------------------Mode2音高曲线--------------------------------------*/

class SimpleCurves : public SimpleScope {
    Q_OBJECT
public:
    // 建立音高曲线必须指定前驱音符与当前音符，前驱为空则为NULL
    explicit SimpleCurves(SimpleNote *curr, EditorAreaWidget *parent = NULL);
    ~SimpleCurves();

    void reInitializeStyles() override;

    void adjustGeometry() override; // 调整曲线显示区域大小
    void adjustPoints() override;

    void adjustVibratoLayer(QWidget *w);

    // 滑音部分
public:
    void saveToNote(QVector<QCtrlPoint> arr = {}) override;

    double convertPosToValueX(int x) override;
    double convertPosToValueY(int y) override;

    int convertValueToPosX(double x) override;
    int convertValueToPosY(double y) override;

    void removePoint(SimplePoint *p) override;
    void removePoints(QVector<SimplePoint *> arr) override;

    void addPortamento();    // 对没有滑音的音符添加滑音
    void removePortamento(); // 删除滑音（删除所有控制点)

    void initPoints(QVector<QCtrlPoint> arr = {}) override;

    QRect getActiveScope() override;

    bool hasVBREditor() {
        return m_showVBR;
    }

    bool isSolid() {
        return m_solid;
    }

    void setSolid(bool value);

    const static QVector<QCtrlPoint> defaultPortamento;

private:
    int activeWidth; // 设置控制点活动宽度
    bool m_solid;

    void paintEvent(QPaintEvent *event) override;

    void drawReflect(QPainter &painter);
    void drawOverlap(QPainter &painter);
    void drawMode1(QPainter &painter);
    void drawMode2(QPainter &painter);
    void drawParameters(QPainter &painter);

    void drawVibrato(QPainter &painter);
    void drawVibratoEditor(QPainter &painter);

public slots:
    void onUpdatePoint(SimplePoint *p, pointMoveEvent e = pointMoving) override;

    // 颤音部分

public:
    const static double minPeriod;
    const static double maxPeriod;

    void initVibratoButtons();
    void removeVibrtoButtons();

    void adjustVibrato();
    void updateVibratoLength(); // 改动区域

    void updateVibratoGraph();

    void updateVibratoHeight();
    void updateVibratoEase();
    void updateVibratoOffset();

    void saveMovingVibrato();

    QPoint pVibratoStart();
    QPoint pVibratoEaseInTop();
    QPoint pVibratoEaseInBottom();
    QPoint pVibratoEaseOutTop();
    QPoint pVibratoEaseOutBottom();
    QPoint pVibratoEaseInCenter();
    QPoint pVibratoEaseOutCenter();
    QPoint pVibratoEnd();

    QPoint pVibratoStandardStart();
    QPoint pVibratoStandardEnd();

private:
    PointButton *btnSide, *btnPhase, *btnFrequency; // 相位，频率按钮
    LineButton *btnEaseIn, *btnEaseOut, *btnTop, *btnBottom, *btnCenter;

    bool m_showVBR;

    QVector<double> tempVBR; // 保存临时的颤音序列（连接变量2）

    void updateFrequencyBySide();
    void updatePhaseByFrequency();

    void updateOthersBySide(); // 根据区域调整其他所有
    void updateHeightBySide();

    void updateEaseByHeight(); // 根据峰谷调整淡入淡出
    void updateHeightByEase(); // 根据淡入淡出调整峰谷

    void updateOthersByOffset();

    void restrictHeightByOffset(); // 限制峰谷
    void relieveHeightByOffset();  // 暂时解除峰谷的上下限制

    void abandonVibrato();
};

#endif // SIMPLECURVES_H
