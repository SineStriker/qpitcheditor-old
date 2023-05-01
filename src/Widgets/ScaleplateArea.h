#ifndef SCALEPLATEAREA_H
#define SCALEPLATEAREA_H

#include <QPainter>
#include <QWidget>

#include "BaseWidgets.h"
#include "Elements/CustomControls.h"
#include "QUtils.h"

class SectionLabel;
class ScaleplateArea;

/*--------------------------------------小节线--------------------------------------*/
class SectionLabel : public QWidget {
    Q_OBJECT
public:
    explicit SectionLabel(QWidget *parent = nullptr);
    explicit SectionLabel(QString str, QWidget *parent = nullptr);
    ~SectionLabel();

    void setText(QString str);
    QString getText();

private:
    void paintEvent(QPaintEvent *event);
    QString text;
};

/*--------------------------------------标尺区域--------------------------------------*/
class ScaleplateArea : public EditorAreaWidget {
    Q_OBJECT
public:
    explicit ScaleplateArea(QWidget *parent = nullptr);

    void adjustSections();

    void adjustWidth();

    int setScaleplates(int n);
    int setBeat(int numerator, int denominator);

    void setNumber(int n) {
        addScaleplates(n);
        adjustSize();
    }
    int getNumber() {
        return lineList.size();
    }

    static ScaleplateArea *Stage;

private:
    QVector<SectionLabel *> lineList;
    QPoint sectionWidth;

    int count;

    int setBeat(); // 根据设定好的分子分母调整标签数
    void addScaleplates(int n);

    void paintEvent(QPaintEvent *event) override;
};

#endif // SCALEPLATEAREA_H
