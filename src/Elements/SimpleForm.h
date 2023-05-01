#ifndef SIMPLEFORM_H
#define SIMPLEFORM_H

#include <QLayout>
#include <QPaintEvent>
#include <QWidget>

#include "CustomControls.h"
#include "QUtils.h"

class SimpleFormTitleBar;
class SimpleForm;
class SimpleStretchArea;

/*--------------------------------------窗体标题栏--------------------------------------*/
class SimpleFormTitleBar : public QWidget {
    Q_OBJECT
public:
    explicit SimpleFormTitleBar(SimpleForm *parent = nullptr);
    explicit SimpleFormTitleBar(QString title, SimpleForm *parent = nullptr);
    explicit SimpleFormTitleBar(QString title, int h, SimpleForm *parent = nullptr);

    static QColor titlebar_topColor;
    static QColor titlebar_fillColor;

    // 返回标题栏文字
    QString getTitle() {
        return m_title;
    }

    // 返回标题栏高度
    int getHeight() {
        return m_height;
    }

    // 返回标题栏上边框高度
    int getTop() {
        return m_top;
    }

    // 设置标题栏上边框高度
    void setTop(double t) {
        m_top = t;
        setFixedHeight(m_top + m_height);
        update();
    }

    // 设置标题栏文字
    void setTitle(QString title) {
        m_title = title;
        lbTitle->setText(m_title);
        lbTitle->adjustSize();
        update();
    }

    // 设置标题栏高度
    void setHeight(int h) {
        m_height = h;
        setFixedHeight(m_top + m_height);
        update();
    }

    void setTopVisible(bool value) {
        topVisible = value;
        update();
    }

    // 添加元件
    void addItem(QWidget *item, int rightToLeft = 0) {
        if (rightToLeft == 0) {
            rightToLeft = item->x();
        }
        itemsList.append(titleBarItem(item, rightToLeft));
    }

    // 删除元件
    void removeItem(QWidget *item) {
        for (int i = 0; i != itemsList.size(); ++i) {
            if (itemsList.at(i).item == item) {
                itemsList.removeAt(i);
                break;
            }
        }
    }

    // 隐藏所有元件
    void hideItems() {
        setItemsVisible(false);
    }

    // 显示所有元件
    void showItems() {
        setItemsVisible(true);
    }

    void setItemsVisible(bool value) {
        for (int i = 0; i != itemsList.size(); ++i) {
            QWidget *p = itemsList.at(i).item;
            if (p) {
                p->setVisible(value);
            }
        }
    }

    // 返回折叠按钮指针
    VectorDblButton *getFoldButton() {
        return btnFold;
    }

protected:
    struct titleBarItem {
        QWidget *item;
        int pos;

        titleBarItem(QWidget *w, int p) : item(w), pos(p) {
        }
    };

    SimpleForm *parentForm;

    int m_height;
    double m_top;
    QString m_title;

    VectorDblButton *btnFold;
    QLabel *lbTitle;

    QList<titleBarItem> itemsList;

    bool topVisible;

private:
    void paintEvent(QPaintEvent *event) override;
    void initializeComponents(QString title, int h, int t);

signals:
};

/*--------------------------------------简单窗体--------------------------------------*/
class SimpleForm : public QWidget {
    Q_OBJECT
public:
    explicit SimpleForm(QWidget *parent = nullptr);
    explicit SimpleForm(QWidget *w, SimpleFormTitleBar *bar, QWidget *parent = nullptr);

    void setWidget(QWidget *w);
    void setDefaultTitleBar();
    void setTitleBar(SimpleFormTitleBar *bar);

    void setFormVisible(bool value);
    bool formUnfolded() const {
        return isUnfolded;
    }

    QWidget *getWidget() {
        return mainWidget;
    }
    QVBoxLayout *getLayout() {
        return mainLayout;
    }

    SimpleFormTitleBar *getTitleBar() {
        return titleBar;
    }

    void setTitle(QString title) {
        titleBar->setTitle(title);
    }
    void reverseFold();

private:
    QVBoxLayout *mainLayout;

    SimpleFormTitleBar *titleBar;
    QWidget *mainWidget;

    bool isUnfolded;

    void initializeForm();

signals:
    void visibilityChanged(bool vi);
    void heightChanged(int h);

private slots:
    void onFoldEvent();

signals:
};

/*--------------------------------------标题栏伸缩区域--------------------------------------*/
class SimpleStretchArea : public QWidget {
    Q_OBJECT
public:
    explicit SimpleStretchArea(SimpleForm *parent = nullptr);
    explicit SimpleStretchArea(int h, SimpleForm *parent = nullptr);

    // 返回上边框高度
    int getTop() {
        return m_top;
    }

    // 设置上边框高度
    void setTop(int t);

private:
    SimpleForm *parentForm;
    int m_top;

    QPoint m_firstPos, m_nextPos;
    bool m_move;

    void setStretch(QPoint gloPoint);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // SIMPLEFORM_H
