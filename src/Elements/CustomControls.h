#ifndef CUSTOMCONTROLS_H
#define CUSTOMCONTROLS_H

#include <QComboBox>
#include <QDockWidget>
#include <QEnterEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QStyle>
#include <QSvgRenderer>
#include <QWidget>

#include <QUtils.h>

#include "ContinuousCurves.h"
#include "CustomObjects.h"
#include "CustomScrolls.h"
#include "NoteOperation.h"
#include "SimpleNote.h"
#include "VectorButton.h"

class SlideControl;
class TextControl;
class ComboControl;

/*--------------------------------------滑块控制--------------------------------------*/
class SlideControl : public QWidget {
    Q_OBJECT
public:
    explicit SlideControl(QWidget *parent = NULL);
    explicit SlideControl(QString text, QWidget *parent = NULL);
    explicit SlideControl(QString text, double value, QWidget *parent = NULL);

    void setMaximum(double value);
    void setMinimum(double value);
    void setSingleStep(double value);
    void setValue(double value);
    double getValue();

    QDoubleSpinBox *getSpinBox() {
        return pSpinBox;
    }
    QSlider *getSlider() {
        return pSlider;
    }

    void setCaption(QString caption) {
        pLabel->setText(caption);
        pLabel->adjustSize();
    }
    QString caption() {
        return pLabel->text();
    }

    void setProportion(int a, int b);
    void setMargin(int n);

    void setUnmodified(bool value);
    bool isUnmodified();

private:
    double value;
    double maximum, minimum, singleStep;

    void InitSlideControl(QString text, double value);
    void onModifyAction();

    QDoubleSpinBox *pSpinBox;
    QSlider *pSlider;
    QLabel *pLabel;
    QGridLayout *pLayout;

    bool bUnmodified;

public slots:
    void onSliderChanged(int n);
    void onTextChanged(double n);
};

/*--------------------------------------文本控制--------------------------------------*/
class TextControl : public QWidget {
    Q_OBJECT
public:
    explicit TextControl(QWidget *parent = NULL);
    explicit TextControl(QString text, QWidget *parent = NULL);
    explicit TextControl(QString text, QString value, QWidget *parent = NULL);

    void setBlankValue(double value) {
        blankValue = value;
    }

    void setCaption(QString caption) {
        pLabel->setText(caption);
        pLabel->adjustSize();
    }

    QString caption() {
        return pLabel->text();
    }

    void setProportion(int a, int b);
    void setMargin(int n);

    void setValue(QString value);
    QString getValue();

    FixedLineEdit *Text() {
        return pText;
    }

    void setUnmodified(bool value);
    bool isUnmodified();
    void setPlaceholder(QString modi, QString unmo);
    void setValidator(QDoubleValidator *v);

private:
    QString value;

    void InitTextControl(QString text, QString value);
    void onModifyAction();
    void onTextChanged(QString s);

    FixedLineEdit *pText;
    QLabel *pLabel;
    QHBoxLayout *pLayout;
    QDoubleValidator *pValidator;

    double blankValue;
    bool bUnmodified;
    QString placeholderModified, placeholderUnmodified;
};

/*--------------------------------------列表框控制--------------------------------------*/
class ComboControl : public QWidget {
    Q_OBJECT
public:
    explicit ComboControl(QWidget *parent = NULL);
    explicit ComboControl(QString text, QWidget *parent = NULL);
    explicit ComboControl(QString text, const QStringList &values, QWidget *parent = NULL);

    void setCaption(QString caption) {
        pLabel->setText(caption);
        pLabel->adjustSize();
    }
    QString caption() {
        return pLabel->text();
    }

    void setProportion(int a, int b);
    void setMargin(int n);

    void setUnmodified(bool value);
    bool isUnmodified();

    QComboBox *getCombo() {
        return pCombo;
    }

private:
    QString value;
    void InitComboControl(QString text, const QStringList &values);
    void onModifyAction();

    QComboBox *pCombo;
    QLabel *pLabel;

    QHBoxLayout *pLayout;

    bool bUnmodified;
};

/*--------------------------------------框选--------------------------------------*/
class CropView : public QWidget {
    Q_OBJECT
public:
    explicit CropView(QPoint pos, QWidget *parent = nullptr) : QWidget(parent) {
        m_start = pos;
        updateRect(pos + QPoint(1, 1));

        setAttribute(Qt::WA_TransparentForMouseEvents, true); // 本身不响应鼠标事件
    }

    void updateRect(QPoint pos) {
        m_end = pos;

        int m_left = qMin(m_start.x(), m_end.x());
        int m_width = qAbs(m_start.x() - m_end.x());
        int m_top = qMin(m_start.y(), m_end.y());
        int m_height = qAbs(m_start.y() - m_end.y());

        setGeometry(m_left, m_top, m_width, m_height);
    }

    bool hitTestRect(QWidget *rc1, QWidget *rc2 = nullptr) {
        if (!rc1) {
            return false;
        }
        if (!rc2) {
            rc2 = this;
        }

        if (rc1->x() + rc1->width() > rc2->x() && rc2->x() + rc2->width() > rc1->x() &&
            rc1->y() + rc1->height() > rc2->y() && rc2->y() + rc2->height() > rc1->y()) {
            return true;
        } else {
            return false;
        }
    }

    bool hitTestPoint(QPoint p, QWidget *w = nullptr) {
        if (!w) {
            w = this;
        }
        return (p.x() >= w->x() && p.x() <= w->x() + w->width() && p.y() >= w->y() &&
                p.y() <= w->y() + w->height());
    }


private:
    QPoint m_start, m_end;

    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.drawRect(0, 0, width(), height());
    }
};

/*--------------------------------------播放头--------------------------------------*/
class PlayHead : public QWidget {
    Q_OBJECT
public:
    explicit PlayHead(int radius, double thickness, QWidget *parent = nullptr)
        : QWidget(parent), radius(radius), thickness(thickness) {
        setFixedWidth(radius * 2);

        setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
    explicit PlayHead(int radius, QWidget *parent = nullptr)
        : QWidget(parent), radius(radius), thickness(2) {
        setFixedWidth(radius * 2);

        setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
    explicit PlayHead(QWidget *parent = nullptr) : QWidget(parent), radius(5), thickness(2) {
        setFixedWidth(radius * 2);

        setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }

    void setPosition(int x) {
        move(x - radius, y());
    }

    int getPosition() {
        return x() + radius;
    }

    int getRadius() {
        return radius;
    }

private:
    int radius;
    double thickness;

    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawRect(radius - thickness / 2, 0, thickness, height());
    }
};

/*--------------------------------------滑块文本控制--------------------------------------*/
class SlideTextControl : public QWidget {
    Q_OBJECT
public:
    explicit SlideTextControl(QWidget *parent = NULL);
    explicit SlideTextControl(QString text, QWidget *parent = NULL);
    explicit SlideTextControl(QString text, double value, QWidget *parent = NULL);

    void setCaption(QString caption) {
        pLabel->setText(caption);
        pLabel->adjustSize();
    }
    QString caption() {
        return pLabel->text();
    }

    void setBlankValue(double value);
    void setMaximum(double value);
    void setMinimum(double value);
    void setSingleStep(double value);
    void setValue(double value);
    double getValue();

    void setText(QString value);
    QString getText();

    FixedLineEdit *Text() {
        return pText;
    }
    QSlider *getSlider() {
        return pSlider;
    }

    void setProportion(int a, int b);
    void setMargin(int n);

    void setUnmodified(bool value);
    bool isUnmodified();

    void setPlaceholder(QString modi, QString unmo);

private:
    double value;
    double maximum, minimum, singleStep;

    QString placeholderModified, placeholderUnmodified;

    void InitSlideControl(QString text, double value);
    void onModifyAction();

    FixedLineEdit *pText;
    QDoubleValidator *pValidator;

    QSlider *pSlider;
    QLabel *pLabel;
    QGridLayout *pLayout;

    bool bUnmodified;
    double blankValue;

    bool isTextEditing;
    bool isSliderMoving;

public slots:
    void onSliderChanged(int n);
    void onTextChanged(QString s);
};

#endif // CUSTOMCONTROLS_H
