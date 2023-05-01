#ifndef SIMPLEVIBRATO_H
#define SIMPLEVIBRATO_H

#include <QWidget>

#include "NoteOperation.h"
#include "QUtils.h"
#include "VectorButton.h"

/*--------------------------------------颤音按钮手柄--------------------------------------*/
class PointButton : public SimpleObject {
    Q_OBJECT
public:
    explicit PointButton(QWidget *parent = nullptr, QSize size = QSize(30, 30),
                         QSizeF padding = QSizeF(0, 0));

    void setText(QString text);
    void setColor(QColor solid, QColor blank);

    void setLimitArea(QRect rect);

    void setPosition(int x, int y) {
        move(x - width() / 2, y - width() / 2);
    }

    void setPosition(QPoint p) {
        move(p.x() - width() / 2, p.y() - width() / 2);
    }

    QPoint getPosition() {
        return QPoint(x() + width() / 2, y() + width() / 2);
    }

    bool isMoving() {
        return m_move;
    }

    QPointF getValue();

    void setValue(QPointF values);
    void setValueX(double value);
    void setValueY(double value);

protected:
    QPoint limitArea(QPoint origin);
    QPoint limitArea(int x, int y);

private:
    QPoint m_windowPoint, m_startPoint;
    bool m_move;
    QRect m_region;

    QString m_text;
    QSize m_size;
    QSizeF m_padding;

    QColor solidColor, blankColor;

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void autoReleaseEvent(QMouseEvent *event, bool flag = false) override;

    void paintEvent(QPaintEvent *event) override;

signals:
    void onButtonMove();
    void onButtonClick();
    void onButtonRestore();
};

/*--------------------------------------颤音边框手柄--------------------------------------*/
class LineButton : public SimpleObject {
    Q_OBJECT
public:
    explicit LineButton(bool isHorizontal, QWidget *parent = nullptr);
    explicit LineButton(bool isHorizontal, int w, QWidget *parent = nullptr);
    explicit LineButton(bool isHorizontal, int w, QColor c, QWidget *parent = nullptr);

    void setColor(QColor c);

    void setLimitArea(QRect rect = noRect);
    QRect getLimitArea();
    void setWidth(int w);
    void showLine(bool b);

    double getValue();
    void setValue(double value);

    int getPosition();
    void setPosition(int n);

    bool isMoving() {
        return m_move;
    }

protected:
    QPoint limitArea(QPoint origin);
    QPoint limitArea(int x, int y);

private:
    QPoint m_windowPoint, m_startPoint;
    bool m_move;
    bool m_show;

    bool m_horizontal;
    QRect m_region;
    int m_width;

    QColor color;

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void autoReleaseEvent(QMouseEvent *event, bool flag = false) override;

    void paintEvent(QPaintEvent *event) override;

signals:
    void onButtonMove();
    void onButtonClick();
    void onButtonRestore();
};

/*--------------------------------------颤音管理--------------------------------------*/

// 在 SimpleCurves 部分

#endif // SIMPLEVIBRATO_H
