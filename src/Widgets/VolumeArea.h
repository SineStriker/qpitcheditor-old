#ifndef VOLUMEAREA_H
#define VOLUMEAREA_H

#include <QWidget>

#include "BaseWidgets.h"
#include "Elements/SimpleForm.h"

class VolumeArea;
class EditorArea;
class VolumeOuterArea;

class VolumeForm : public SimpleForm {
    Q_OBJECT
public:
    explicit VolumeForm(QWidget *parent = nullptr);

    void setParent(EditorArea *w) {
        parentArea = w;
    }

    EditorArea *Parent() {
        return parentArea;
    }

    VolumeOuterArea *getWidget() {
        return mainArea;
    }

protected:
    EditorArea *parentArea;
    VolumeOuterArea *mainArea;

private:
    VectorDblButton *btnEase23, *btnEase14, *btnReset;
    VectorDblButton *btnLines, *btnBackFill;

public:
    void onEase23BtnClick();
    void onEase14BtnClick();
    void onResetBtnClick();

    void onLinesBtnClick();
    void onFillBtnClick();

    void updateButtons();
signals:
};

class VolumeOuterArea : public EditorArea {
    Q_OBJECT
public:
    explicit VolumeOuterArea(QWidget *parent);

    VolumeArea *Volume() {
        return volume;
    }

    NoneScrollArea *VolumeScroll() {
        return volumeScroll;
    }

private:
    void onScrollResize(QResizeEvent *event);

    VolumeArea *volume;
    NoneScrollArea *volumeScroll;

    QHBoxLayout *mainLayout;
};

class VolumeArea : public EditorAreaWidget {
    Q_OBJECT
public:
    explicit VolumeArea(QWidget *parent = nullptr);

    void zoomResize(int w);

    QWidget *getNoteSegLayer() override {
        return noteTopLayer;
    }

    QWidget *getCurvesSegLayer() override {
        return curvesTopLayer;
    }

    QWidget *getPointSegLayer() override {
        return pointTopLayer;
    }

    QWidget *getVibratoSegLayer() override {
        return vibratoTopLayer;
    }

    static unsigned standardHeight;
    static VolumeArea *Stage;

private:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    QWidget *noteTopLayer, *curvesTopLayer, *pointTopLayer, *vibratoTopLayer; // 空图层
signals:
};

#endif // VOLUMEAREA_H
