#ifndef CUSTOMSCROLLS_H
#define CUSTOMSCROLLS_H

#include <QEvent>
#include <QKeyEvent>
#include <QScrollArea>
#include <QScrollBar>

#include "QUtils.h"

/*--------------------------------------滚动区域基类--------------------------------------*/
class BasicScrollArea : public QScrollArea {
    Q_OBJECT
public:
    explicit BasicScrollArea(QWidget *parent = nullptr);
};

/*--------------------------------------主编辑区的滚动区域--------------------------------------*/
class NotesScrollArea : public BasicScrollArea {
    Q_OBJECT
public:
    explicit NotesScrollArea(QWidget *parent = nullptr);

    ~NotesScrollArea();

protected:
private:
    bool eventFilter(QObject *obj, QEvent *event) override;
signals:
};

/*--------------------------------------不呈现任何状态的滚动区--------------------------------------*/
class NoneScrollArea : public QWidget {
    Q_OBJECT
public:
    explicit NoneScrollArea(QWidget *parent = nullptr) : QWidget(parent) {
        w = nullptr;
    }

    void setWidget(QWidget *widget) {
        w = widget;

        widget->move(0, 0);
    }
    QWidget *widget() {
        return w;
    }

    void setPercentageX(double value) {
        if (!w || w->width() <= this->width()) {
            return;
        }
        double toX = (value / 100.0) * (this->width() - w->width());
        w->move(toX, w->y());
    }
    void setPercentageY(double value) {
        if (!w || w->height() <= this->height()) {
            return;
        }
        double toY = (value / 100.0) * (this->height() - w->height());
        w->move(w->x(), toY);
    }
    double getPercentageX() {
        if (!w || w->width() <= this->width()) {
            return 0;
        }
        return w->x() / (this->width() - w->width()) * 100.0;
    }
    double getPercentageY() {
        if (!w || w->height() <= this->height()) {
            return 0;
        }
        return w->y() / (this->height() - w->height()) * 100.0;
    }

protected:
private:
    QWidget *w;

private:
    void resizeEvent(QResizeEvent *event) override {
        emit onResizeEvent(event);
    }
signals:
    void onResizeEvent(QResizeEvent *);
};

class MoveWidget : public QWidget {
    Q_OBJECT
public:
    explicit MoveWidget(QWidget *parent = nullptr) : QWidget(parent) {
    }

private:
    void moveEvent(QMoveEvent *event) override {
        emit onMoveEvent();
    }

signals:
    void onMoveEvent();
};

#endif // CUSTOMSCROLLS_H
