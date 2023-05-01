#include "SimpleForm.h"
#include "mainwindow.h"

QColor SimpleFormTitleBar::titlebar_topColor = QColor(0x272727);
QColor SimpleFormTitleBar::titlebar_fillColor = QColor(0x373737);

/*--------------------------------------窗体标题栏--------------------------------------*/
SimpleFormTitleBar::SimpleFormTitleBar(SimpleForm *parent) : QWidget(parent), parentForm(parent) {
    initializeComponents("Untitled", 30, 1);
}

SimpleFormTitleBar::SimpleFormTitleBar(QString title, SimpleForm *parent)
    : QWidget(parent), parentForm(parent) {
    initializeComponents(title, 30, 1);
}

SimpleFormTitleBar::SimpleFormTitleBar(QString title, int h, SimpleForm *parent)
    : QWidget(parent), parentForm(parent) {
    initializeComponents(title, h, 1);
}

// 初始化标题栏
void SimpleFormTitleBar::initializeComponents(QString title, int h, int t) {
    m_height = h;
    m_title = title;
    m_top = t;

    topVisible = true;

    lbTitle = new QLabel(m_title, this);
    lbTitle->setFont(normalFont());
    setLabelColor(lbTitle);

    lbTitle->adjustSize();

    btnFold = new VectorDblButton(this, false, QSize(24, 24), QSizeF(5.5, 5.5));

    btnFold->setIcons(QString(":/images/WidgetFold.svg"), QString(":/images/WidgetUnfold.svg"),
                      QString(":/images/WidgetFold_Over.svg"),
                      QString(":/images/WidgetUnfold_Over.svg"));

    setFixedHeight(m_top + m_height);
}

void SimpleFormTitleBar::paintEvent(QPaintEvent *event) {

    QPainter painter(this);
    QRect rect = this->rect();

    // 画背景
    painter.setPen(Qt::transparent);

    if (topVisible) {
        painter.setBrush(QBrush(titlebar_topColor, Qt::SolidPattern));
    } else {
        painter.setBrush(QBrush(titlebar_fillColor, Qt::SolidPattern));
    }
    painter.drawRect(rect.left(), rect.top(), rect.width(), m_top);

    painter.setBrush(QBrush(titlebar_fillColor, Qt::SolidPattern));
    painter.drawRect(rect.left(), rect.top() + m_top, rect.width(), rect.height() - m_top);

    // 标题栏元件中心对齐
    btnFold->move(5, m_top + (rect.height() - m_top - btnFold->height()) / 2);
    lbTitle->move(40, m_top + (rect.height() - m_top - lbTitle->height()) / 2);
    for (int i = 0; i != itemsList.size(); ++i) {
        QWidget *p = itemsList.at(i).item;
        int x = itemsList.at(i).pos;
        if (p) {
            if (x >= 0) {
                p->move(x, m_top + (rect.height() - m_top - p->height()) / 2);
            } else {
                p->move(width() - p->width() + x,
                        m_top + (rect.height() - m_top - p->height()) / 2);
            }
        }
    }
}

/*--------------------------------------窗体--------------------------------------*/

SimpleForm::SimpleForm(QWidget *parent) : QWidget(parent) {
    isUnfolded = true;
    mainWidget = nullptr;
    titleBar = nullptr;
}

SimpleForm::SimpleForm(QWidget *w, SimpleFormTitleBar *bar, QWidget *parent) : QWidget(parent) {
    isUnfolded = true;
    titleBar = bar;
    mainWidget = w;

    initializeForm();
}

void SimpleForm::setWidget(QWidget *w) {
    if (mainWidget) {
        return;
    }

    mainWidget = w;

    if (titleBar && mainWidget) {
        initializeForm();
    }
}

void SimpleForm::setDefaultTitleBar() {
    setTitleBar(new SimpleFormTitleBar(this));
}

void SimpleForm::setTitleBar(SimpleFormTitleBar *bar) {
    if (titleBar) {
        return;
    }
    titleBar = bar;
    if (titleBar && mainWidget) {
        initializeForm();
    }
}

void SimpleForm::setFormVisible(bool value) {
    isUnfolded = value;
    mainWidget->setVisible(value);
    titleBar->setItemsVisible(value);
    titleBar->getFoldButton()->setStatus(!value);

    if (value) {
        setMaximumHeight(10000);
    } else {
        setMaximumHeight(titleBar->height());
    }
    adjustSize();

    emit visibilityChanged(value);
}

// 展开或折叠窗口
void SimpleForm::reverseFold() {
    setFormVisible(!isUnfolded);
}

// 初始化窗体
void SimpleForm::initializeForm() {
    mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(mainWidget);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    connect(titleBar->getFoldButton(), &VectorDblButton::clicked, this, &SimpleForm::onFoldEvent);
}

void SimpleForm::onFoldEvent() {
    reverseFold();
}

/*--------------------------------------标题栏伸缩区域--------------------------------------*/

SimpleStretchArea::SimpleStretchArea(SimpleForm *parent) : QWidget(parent), parentForm(parent) {
    m_move = false;
    m_firstPos = m_nextPos = QPoint(0, 0);

    setTop(0);
}

SimpleStretchArea::SimpleStretchArea(int h, SimpleForm *parent)
    : QWidget(parent), parentForm(parent) {
    m_move = false;
    m_firstPos = m_nextPos = QPoint(0, 0);

    setTop(h);
}

void SimpleStretchArea::setTop(int t) {
    m_top = t;
    if (m_top == 0) {
        setFixedHeight(1);
        hide();
    } else {
        setFixedHeight(t);
        show();
    }
}

// 以下作废
void SimpleStretchArea::setStretch(QPoint gloPoint) {
    if (parentForm) {
        emit parentForm->heightChanged(-gloPoint.y());
    }
}

void SimpleStretchArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_firstPos = event->pos();
        m_move = true;
    }
}

void SimpleStretchArea::mouseMoveEvent(QMouseEvent *event) {
    if (m_move) {
        m_nextPos = event->pos();
        setStretch(m_nextPos - m_firstPos);
    }
}

void SimpleStretchArea::mouseReleaseEvent(QMouseEvent *event) {
    m_move = false;
}

void SimpleStretchArea::enterEvent(QEvent *event) {
    // 设置垂直缩放图标
    this->setCursor(QCursor(Qt::SizeVerCursor));
}

void SimpleStretchArea::leaveEvent(QEvent *event) {
    // 设置箭头图标
    this->setCursor(QCursor(Qt::ArrowCursor));
    m_move = false;
}
