#ifndef PIANOAREA_H
#define PIANOAREA_H

#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QEnterEvent>
#include <QEvent>
#include <QGraphicsScene>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QVector>
#include <QWidget>

#include <QUtils.h>

#include "BaseWidgets.h"
#include "Elements/CustomControls.h"

class PianoKey;
class PianoArea;

/*--------------------------------------钢琴键--------------------------------------*/
class PianoKey : public QWidget {
    Q_OBJECT
public:
    explicit PianoKey(QWidget *parent = nullptr, int x = 0, int y = 0, int w = 60, int h = 30,
                      QColor c = Qt::black, QString s = "");
    ~PianoKey();

    void adjustHeight();

    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    unsigned m_y, m_height, keyHeight;
    QColor color;
    QString name;

    bool rollOver;
};

/*--------------------------------------钢琴区域--------------------------------------*/
class PianoArea : public EditorAreaWidget {
    Q_OBJECT
public:
    explicit PianoArea(QWidget *parent = nullptr);

    void adjustHeight();
    void updateNames();

    static PianoArea *Stage;

private:
    QVector<PianoKey *> keyList;
    QTimer *timer;
    QLabel *noteName;
    int curPos;

    void addPiano();
    void paintEvent(QPaintEvent *event) override;
    void onTimer();

signals:
};

#endif // PIANOAREA_H
