#include "VectorButton.h"

VectorDblButton::VectorDblButton(QWidget *parent, bool single, QSize size, QSizeF padding)
    : QLabel(parent), isSingle(single), size(size), padding(padding) {

    setFocusPolicy(Qt::ClickFocus);

    setFixedSize(size);

    status = false;
    rollState = 0;
    bmpUp[0] = bmpUp[1] = nullptr;
    bmpOver[0] = bmpOver[1] = nullptr;
    bmpDown[0] = bmpDown[1] = nullptr;
}

void VectorDblButton::setValues(bool single, QSize size, QSizeF padding) {
    isSingle = single;
    this->size = size;
    this->padding = padding;
}

void VectorDblButton::setIcons(QString on_up, QString off_up, QString on_over, QString off_over,
                               QString on_down, QString off_down) {

    QSize iconSize(size.width() - padding.width() * 2, size.height() - padding.height() * 2);

    if (isSingle) {
        bmpUp[0] = createBitmapFromSVG(on_up, iconSize);

        if (!off_up.isEmpty()) {
            bmpOver[0] = createBitmapFromSVG(off_up, iconSize);
        } else {
            bmpOver[0] = bmpUp[0];
        }

        if (!on_over.isEmpty()) {
            bmpDown[0] = createBitmapFromSVG(on_over, iconSize);
        } else {
            bmpDown[0] = bmpOver[0];
        }
    } else {

        bmpUp[0] = createBitmapFromSVG(on_up, iconSize);
        bmpUp[1] = createBitmapFromSVG(off_up, iconSize);

        if (!on_over.isEmpty() && !off_over.isEmpty()) {
            bmpOver[0] = createBitmapFromSVG(on_over, iconSize);
            bmpOver[1] = createBitmapFromSVG(off_over, iconSize);
        } else {
            bmpOver[0] = bmpUp[0];
            bmpOver[1] = bmpUp[1];
        }

        if (!on_down.isEmpty() && !off_down.isEmpty()) {
            bmpDown[0] = createBitmapFromSVG(on_down, iconSize);
            bmpDown[1] = createBitmapFromSVG(off_down, iconSize);
        } else {
            bmpDown[0] = bmpOver[0];
            bmpDown[1] = bmpOver[1];
        }
    }
    setIcon2();
}

void VectorDblButton::setIcon2() {

    int index = status ? 1 : 0;

    if (isSingle) {
        index = 0;
    }

    // label->setAlignment(Qt::AlignCenter);
    this->setAlignment(Qt::AlignCenter);

    if (rollState == 0) {
        // label->setPixmap(*bmpUp[index]);
        this->setPixmap(*bmpUp[index]);
    } else if (rollState == 1) {
        // label->setPixmap(*bmpOver[index]);
        this->setPixmap(*bmpOver[index]);
    } else {
        // label->setPixmap(*bmpDown[index]);
        this->setPixmap(*bmpDown[index]);
    }
}


/*--------------------------------------工具栏按钮--------------------------------------*/
ToolMultiButton::ToolMultiButton(QWidget *parent) : QWidget(parent) {
    InitializeComponents();
}


ToolMultiButton::ToolMultiButton(QString text, QWidget *parent) : QWidget(parent) {
    InitializeComponents(text);
}

void ToolMultiButton::setCaption(QString text) {
    label->setText(text);
}

void ToolMultiButton::setHead(bool value) {
    m_head = value;
    if (value) {
        label->setFont(uiFont());
    } else {
        label->setFont(normalFont());
    }
    update();
}

void ToolMultiButton::addIcon(bool single, QSize size, QSizeF padding) {
    iconBtn = new VectorDblButton(this, single, size, padding);
    iconBtn->installEventFilter(this);

    if (m_head) {
        mainLayout->insertWidget(1, iconBtn);
    } else {
        mainLayout->insertWidget(0, iconBtn);
    }
}

bool ToolMultiButton::eventFilter(QObject *obj, QEvent *event) {
    if (obj == iconBtn) {
        return true;
    } else {
        return QWidget::eventFilter(obj, event);
    }
}

void ToolMultiButton::InitializeComponents(QString text) {
    rollState = 0;
    m_head = false;

    iconBtn = nullptr;

    label = new QLabel(text, this);
    label->setFont(normalFont());
    label->setAlignment(Qt::AlignVCenter);
    setLabelColor(label);

    mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(label);
    this->setLayout(mainLayout);
}

void ToolMultiButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    if (m_head) {
        double w = 1;
        painter.setBrush(QColor(Qt::white));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width(), w);
        painter.drawRect(0, height() - 2, width(), w);
    }

    if (rollState == 1) {
        painter.setBrush(QColor(255, 255, 255, 80));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width(), height());
    } else if (rollState == 2) {
        painter.setBrush(QColor(255, 255, 255, 48));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width(), height());
    }
}
