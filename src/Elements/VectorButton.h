#ifndef VECTORBUTTON_H
#define VECTORBUTTON_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QToolButton>

#include "QUtils.h"
#include "SimpleNote.h"

class VectorDblButton;

// 绘制/点选模式，音符/曲线模式，连续曲线/控制点曲线/重叠线/实际长度/音量包络/参数

/*--------------------------------------矢量双状态按钮--------------------------------------*/
class VectorDblButton : public QLabel {
    Q_OBJECT

public:
    explicit VectorDblButton(QWidget *parent = nullptr, bool single = true,
                             QSize size = QSize(30, 30), QSizeF padding = QSizeF(0, 0));

    void setValues(bool single = true, QSize size = QSize(30, 30), QSizeF padding = QSizeF(0, 0));

    void setIcons(QString on_up, QString off_up = "", QString on_over = "", QString off_over = "",
                  QString on_down = "", QString off_down = "");

    void setStatus(bool b) {
        if (isSingle) {
            status = false;
        } else {
            status = b;
        }
        setIcon2();
    }

    bool getStatus() {
        return status;
    }

    friend class ToolMultiButton;

protected:
    void enterEvent(QEvent *event) override {
        rollState = 1;
        setIcon2();
    }
    void leaveEvent(QEvent *event) override {
        rollState = 0;
        setIcon2();
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() != Qt::LeftButton) {
            return;
        }
        if (rollState == 0) {
            return;
        }
        rollState = 2;
        setIcon2();

        emit press();
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() != Qt::LeftButton) {
            return;
        }
        rollState = 1;
        setIcon2();

        emit clicked();
    }

private:
    bool status, isSingle;
    int rollState;
    QSize size;
    QSizeF padding;

    // QLabel *label; // 用来显示图标的

    QPixmap *bmpUp[2], *bmpOver[2], *bmpDown[2];

    void setIcon2();

signals:
    void clicked();
    void press();
};

/*--------------------------------------工具栏按钮--------------------------------------*/
// 功能：填词，插入休止符，编组/解散，查找/替换，合并音符，填充休止符
// 单独音符属性：歌词（功能），音阶（功能），音长（功能），音量（功能），移调（功能）
// 组合音符属性：音量/移调/先行/重叠/辅音速度/Flags
// 滑音
// 颤音

class ToolMultiButton : public QWidget {
    Q_OBJECT
public:
    explicit ToolMultiButton(QWidget *parent = nullptr);
    explicit ToolMultiButton(QString text, QWidget *parent = nullptr);

    void setCaption(QString text);
    void setHead(bool value);

    QString caption() {
        return label->text();
    }

    void enterEvent(QEvent *event) override {
        if (iconBtn) {
            iconBtn->enterEvent(event);
        }
        rollState = 1;
        update();
    }
    void leaveEvent(QEvent *event) override {
        if (iconBtn) {
            iconBtn->leaveEvent(event);
        }
        rollState = 0;
        update();
    }
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() != Qt::LeftButton) {
            return;
        }
        if (iconBtn) {
            iconBtn->mousePressEvent(event);
        }
        rollState = 2;
        update();

        emit press();
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() != Qt::LeftButton) {
            return;
        }
        if (iconBtn) {
            iconBtn->mouseReleaseEvent(event);
        }
        rollState = 1;
        update();

        emit clicked();
    }

    // 添加图标
    void addIcon(bool single = true, QSize size = QSize(30, 30), QSizeF padding = QSizeF(0, 0));

private:
    bool eventFilter(QObject *obj, QEvent *event) override;

    VectorDblButton *iconBtn;
    QLabel *label;
    QHBoxLayout *mainLayout;

    int rollState;
    bool m_head;

    void InitializeComponents(QString text = "");

private:
    void paintEvent(QPaintEvent *event) override;

signals:
    void clicked();
    void press();
};

#endif // VECTORBUTTON_H
