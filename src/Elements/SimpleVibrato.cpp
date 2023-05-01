#include "SimpleVibrato.h"
#include "SimpleCurves.h"
#include "mainwindow.h"

const double SimpleCurves::minPeriod = 32;
const double SimpleCurves::maxPeriod = 512;

/*--------------------------------------颤音按钮手柄--------------------------------------*/
PointButton::PointButton(QWidget *parent, QSize size, QSizeF padding)
    : SimpleObject(parent), m_size(size), m_padding(padding) {

    setFixedSize(size);

    m_region = noRect;
    m_move = false;
    m_text = "P";

    solidColor = MainWindow::vibrato_buttonBorderColor;
    blankColor = MainWindow::vibrato_buttonFillColor;
}

void PointButton::setText(QString text) {
    m_text = text;
    update();
}

void PointButton::setColor(QColor solid, QColor blank) {
    solidColor = solid;
    blankColor = blank;

    update();
}

void PointButton::setLimitArea(QRect rect) {
    m_region = rect;

    move(limitArea(x(), y()));
}

QPointF PointButton::getValue() {

    QPointF result;

    result.rx() = double((this->x() + double(width()) / 2) - m_region.left()) /
                  (m_region.right() - m_region.left());
    result.ry() = double((this->y() + double(width()) / 2) - m_region.top()) /
                  (m_region.bottom() - m_region.top());

    return result;
}

void PointButton::setValue(QPointF values) {
    setValueX(values.x());
    setValueY(values.y());
}

void PointButton::setValueX(double value) {
    double toX = 0;
    toX = value * (m_region.right() - m_region.left()) + m_region.left() - double(width()) / 2;
    this->move(limitArea(toX, y()));
}

void PointButton::setValueY(double value) {
    double toY = 0;
    toY = value * (m_region.bottom() - m_region.top()) + m_region.top() - double(height()) / 2;
    this->move(limitArea(x(), toY));
}

QPoint PointButton::limitArea(QPoint origin) {
    if (m_region == noRect) {
        return origin;
    }

    if (origin.x() < m_region.left() - width() / 2) {
        origin.setX(m_region.left() - width() / 2);
    } else if (origin.x() > m_region.right() - width() / 2) {
        origin.setX(m_region.right() - width() / 2);
    }

    if (origin.y() < m_region.top() - width() / 2) {
        origin.setY(m_region.top() - width() / 2);
    } else if (origin.y() > m_region.bottom() - width() / 2) {
        origin.setY(m_region.bottom() - width() / 2);
    }

    return origin;
}

QPoint PointButton::limitArea(int x, int y) {
    return limitArea(QPoint(x, y));
}

void PointButton::enterEvent(QEvent *event) {
    this->setCursor(QCursor(Qt::PointingHandCursor));
}

void PointButton::leaveEvent(QEvent *event) {
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void PointButton::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        Root->addDraggedObject(this);
        m_move = true;

        QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
        m_startPoint = global_pos;
        m_windowPoint = this->frameGeometry().topLeft();
    }
}

void PointButton::mouseMoveEvent(QMouseEvent *event) {

    if (!m_move) {
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
        QPoint relative_pos = global_pos - m_startPoint;

        this->move(limitArea(m_windowPoint + relative_pos));

        emit onButtonMove();
    }
}

void PointButton::mouseReleaseEvent(QMouseEvent *event) {

    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
    QPoint relative_pos = global_pos - m_startPoint;

    if (event->button() == Qt::LeftButton) {
        Root->removeDraggedObject(this);

        if (relative_pos.manhattanLength() >= 1) {
            emit onButtonClick();
        } else {
            emit onButtonRestore();
        }

        m_move = false;
    }
}

void PointButton::autoReleaseEvent(QMouseEvent *event, bool flag) {

    if (!flag) {
        Root->removeDraggedObject(this);
    }
    this->move(m_windowPoint);

    emit onButtonRestore();

    m_move = false;
}

void PointButton::paintEvent(QPaintEvent *event) {

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    QSizeF ring(1.5, 1.5);

    painter.setPen(Qt::NoPen);
    painter.setBrush(solidColor);

    painter.drawEllipse(QRectF(m_padding.width(), m_padding.height(),
                               m_size.width() - m_padding.width() * 2,
                               m_size.height() - m_padding.height() * 2));

    painter.setBrush(blankColor);

    painter.drawEllipse(QRectF(m_padding.width() + ring.width(), m_padding.height() + ring.height(),
                               m_size.width() - m_padding.width() * 2 - ring.width() * 2,
                               m_size.height() - m_padding.height() * 2 - ring.height() * 2));

    painter.setPen(solidColor);
    painter.setBrush(Qt::NoBrush);
    painter.setFont(uiFont());

    painter.drawText(0, 0, this->width(), this->height(), Qt::AlignCenter, m_text);
}

/*--------------------------------------颤音边框手柄--------------------------------------*/

LineButton::LineButton(bool isHorizontal, QWidget *parent)
    : SimpleObject(parent), m_horizontal(isHorizontal), m_width(1), color(QColor(0xCC0000)) {
    m_region = noRect;
    m_show = true;
    m_move = false;

    setLimitArea(m_region);
}

LineButton::LineButton(bool isHorizontal, int w, QWidget *parent)
    : SimpleObject(parent), m_horizontal(isHorizontal), m_width(w), color(QColor(0xCC0000)) {
    m_region = noRect;
    m_show = true;
    m_move = false;
    setLimitArea(m_region);
}

LineButton::LineButton(bool isHorizontal, int w, QColor c, QWidget *parent)
    : SimpleObject(parent), m_horizontal(isHorizontal), m_width(w), color(c) {
    m_region = noRect;
    m_show = true;
    m_move = false;
    setLimitArea(m_region);
}

void LineButton::setColor(QColor c) {
    color = c;
    update();
}

void LineButton::setLimitArea(QRect rect) {
    m_region = rect;

    if (m_region == noRect) {
        hide();
        return;
    } else {
        show();
    }

    if (m_horizontal) {
        move(limitArea(x(), rect.top()));

        setFixedHeight(m_region.height());
        setFixedWidth(m_width);
    } else {
        move(limitArea(rect.left(), y()));

        setFixedWidth(m_region.width());
        setFixedHeight(m_width);
    }
    update();
}

QRect LineButton::getLimitArea() {
    return m_region;
}

void LineButton::setWidth(int w) {
    m_width = w;
    setLimitArea(m_region);
}

void LineButton::showLine(bool b) {
    m_show = b;
    update();
}

double LineButton::getValue() {
    double result;

    if (m_horizontal) {
        result = double(this->x() + double(m_width) / 2 - m_region.left()) /
                 (m_region.right() - m_region.left());
    } else {
        result = double(this->y() + double(m_width) / 2 - m_region.top()) /
                 (m_region.bottom() - m_region.top());
    }

    return result;
}

void LineButton::setValue(double value) {
    double toW = 0;

    if (m_horizontal) {
        toW = value * (m_region.right() - m_region.left()) + m_region.left() - double(m_width) / 2;
        this->move(limitArea(toW, y()));
    } else {
        toW = value * (m_region.bottom() - m_region.top()) + m_region.top() - double(m_width) / 2;
        this->move(limitArea(x(), toW));
    }
}

int LineButton::getPosition() {
    if (m_horizontal) {
        return x() + m_width / 2;
    } else {
        return y() + m_width / 2;
    }
}

void LineButton::setPosition(int n) {
    if (m_horizontal) {
        move(limitArea(n - m_width / 2, y()));
    } else {
        move(limitArea(x(), n - m_width / 2));
    }
}

QPoint LineButton::limitArea(QPoint origin) {
    if (m_region == noRect) {
        return origin;
    }

    unsigned h = noteUnitHeightArr[Root->noteUnitHIndex];
    unsigned h1 = h / 2;
    int unit_y = int(double(origin.y() + double(m_width) / 2) / h1 - 0.5) * h1 -
                 double(m_width) / 2 + double(h) / 2;

    if (m_horizontal) {
        if (origin.x() < m_region.left() - m_width / 2) {
            origin.setX(m_region.left() - m_width / 2);
        } else if (origin.x() > m_region.right() - m_width / 2) {
            origin.setX(m_region.right() - m_width / 2);
        }
        origin.setY(m_region.top());
    } else {

        Qt::KeyboardModifiers c = QApplication::keyboardModifiers();
        bool c1 = c == Qt::ControlModifier;

        if (c1 && m_move) {
            origin.setY(unit_y);
        }

        if (origin.y() < m_region.top() - m_width / 2) {
            origin.setY(m_region.top() - m_width / 2);
        } else if (origin.y() > m_region.bottom() - m_width / 2) {
            origin.setY(m_region.bottom() - m_width / 2);
        }
        origin.setX(m_region.left());
    }

    return origin;
}

QPoint LineButton::limitArea(int x, int y) {
    return limitArea(QPoint(x, y));
}

void LineButton::enterEvent(QEvent *event) {
    this->setCursor(QCursor(Qt::PointingHandCursor));
}

void LineButton::leaveEvent(QEvent *event) {
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void LineButton::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        Root->addDraggedObject(this);
        m_move = true;

        QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
        m_startPoint = global_pos;
        m_windowPoint = this->frameGeometry().topLeft();
    }
}

void LineButton::mouseMoveEvent(QMouseEvent *event) {
    if (!m_move) {
        return;
    }
    if (event->buttons() & Qt::LeftButton) {
        QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
        QPoint relative_pos = global_pos - m_startPoint;

        this->move(limitArea(m_windowPoint + relative_pos));

        emit onButtonMove();
    }
}

void LineButton::mouseReleaseEvent(QMouseEvent *event) {
    QPoint global_pos = parentWidget()->mapFromGlobal(event->globalPos());
    QPoint relative_pos = global_pos - m_startPoint;

    if (event->button() == Qt::LeftButton) {
        Root->removeDraggedObject(this);
        if (relative_pos.manhattanLength() >= 1) {
            emit onButtonClick();
        } else {
            emit onButtonRestore();
        }

        m_move = false;
    }
}

void LineButton::autoReleaseEvent(QMouseEvent *event, bool flag) {
    if (!flag) {
        Root->removeDraggedObject(this);
    }
    this->move(m_windowPoint);

    emit onButtonRestore();

    m_move = false;
}

void LineButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(Qt::NoPen);

    if (!m_show) {
        painter.setBrush(Qt::NoBrush);
    } else {
        painter.setBrush(color);
    }

    painter.drawRect(0, 0, width(), height());
}

/*--------------------------------------颤音管理--------------------------------------*/

void SimpleCurves::adjustVibratoLayer(QWidget *w) {

    if (m_parent->getVibratoSegLayer()) {
        w->stackUnder(m_parent->getVibratoSegLayer());
    }
}

void SimpleCurves::initVibratoButtons() {
    if (m_showVBR) {
        return;
    }

    m_showVBR = true;

    QSize pointBtnSize(18, 18);
    QSize pointBtnPadding(2, 2);

    btnCenter = new LineButton(false, 4, Qt::yellow, m_parent);
    btnEaseIn = new LineButton(true, 4, Qt::yellow, m_parent);
    btnEaseOut = new LineButton(true, 4, Qt::yellow, m_parent);
    btnTop = new LineButton(false, 4, Qt::yellow, m_parent);
    btnBottom = new LineButton(false, 4, Qt::yellow, m_parent);

    btnSide = new PointButton(m_parent, pointBtnSize, pointBtnPadding);
    btnPhase = new PointButton(m_parent, pointBtnSize, pointBtnPadding);
    btnFrequency = new PointButton(m_parent, pointBtnSize, pointBtnPadding);

    btnEaseIn->showLine(false);
    btnEaseOut->showLine(false);
    btnCenter->showLine(false);
    btnTop->showLine(false);
    btnBottom->showLine(false);

    btnSide->setText("S");
    btnPhase->setText("P");
    btnFrequency->setText("F");

    adjustVibratoLayer(btnPhase);
    adjustVibratoLayer(btnEaseIn);
    adjustVibratoLayer(btnEaseOut);
    adjustVibratoLayer(btnCenter);
    adjustVibratoLayer(btnTop);
    adjustVibratoLayer(btnBottom);
    adjustVibratoLayer(btnSide);
    adjustVibratoLayer(btnPhase);
    adjustVibratoLayer(btnFrequency);

    btnEaseIn->show();
    btnEaseOut->show();
    btnCenter->show();
    btnTop->show();
    btnBottom->show();
    btnSide->show();
    btnFrequency->show();
    btnPhase->show();

    // 边界按钮
    connect(btnSide, &PointButton::onButtonMove, this, &SimpleCurves::updateVibratoLength);
    connect(btnSide, &PointButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnSide, &PointButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    // 相位按钮
    connect(btnPhase, &PointButton::onButtonMove, this, &SimpleCurves::updateVibratoGraph);
    connect(btnPhase, &PointButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnPhase, &PointButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    // 频率按钮
    connect(btnFrequency, &PointButton::onButtonMove, this, &SimpleCurves::updateVibratoGraph);
    connect(btnFrequency, &PointButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnFrequency, &PointButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    // 颤音深度
    connect(btnTop, &LineButton::onButtonMove, this, &SimpleCurves::updateVibratoHeight);
    connect(btnTop, &LineButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnTop, &LineButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    connect(btnBottom, &LineButton::onButtonMove, this, &SimpleCurves::updateVibratoHeight);
    connect(btnBottom, &LineButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnBottom, &LineButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    // 颤音淡入淡出
    connect(btnEaseIn, &LineButton::onButtonMove, this, &SimpleCurves::updateVibratoEase);
    connect(btnEaseIn, &LineButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnEaseIn, &LineButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    connect(btnEaseOut, &LineButton::onButtonMove, this, &SimpleCurves::updateVibratoEase);
    connect(btnEaseOut, &LineButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnEaseOut, &LineButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    // 颤音高度（偏移）
    connect(btnCenter, &LineButton::onButtonMove, this, &SimpleCurves::updateVibratoOffset);
    connect(btnCenter, &LineButton::onButtonClick, this, &SimpleCurves::saveMovingVibrato);
    connect(btnCenter, &LineButton::onButtonRestore, this, &SimpleCurves::abandonVibrato);

    adjustVibrato();
}

void SimpleCurves::removeVibrtoButtons() {
    if (!m_showVBR) {
        return;
    }

    m_showVBR = false;

    delete btnSide;
    delete btnPhase;
    delete btnFrequency;

    btnSide = btnPhase = btnFrequency = nullptr;

    delete btnEaseIn;
    delete btnEaseOut;
    delete btnTop;
    delete btnBottom;
    delete btnCenter;

    btnEaseIn = btnEaseOut = btnTop = btnBottom = btnCenter = nullptr;
}

void SimpleCurves::adjustVibrato() {
    if (!m_showVBR) {
        return;
    }

    tempVBR = curNote->m_note.m_vbr; // 保存起初的颤音序列

    // 预调整：调整三个手柄按钮的纵坐标
    btnPhase->setPosition(curNote->getStaticX() + curNote->width() / 2,
                          curNote->getStaticY() + curNote->height() / 2);
    btnFrequency->setPosition(curNote->getStaticX() + curNote->width() / 2,
                              btnPhase->getPosition().y() + btnPhase->height());
    btnSide->setPosition(curNote->getStaticX() + curNote->width() / 2,
                         btnPhase->getPosition().y() - btnSide->height());

    // 限制边界手柄活动范围
    QRect sideRect;
    sideRect.setTopLeft(QPoint(curNote->getStaticX(), btnSide->getPosition().y()));
    sideRect.setBottomRight(
        QPoint(curNote->getStaticX() + curNote->width(), btnSide->getPosition().y()));
    btnSide->setLimitArea(sideRect);

    // 根据颤音调整边界手柄的位置
    btnSide->setValueX(1 - curNote->m_note.m_vbr[0] / 100.0);

    // 调整颤音
    updateVibratoLength();
}

void SimpleCurves::updateVibratoLength() {
    updateOthersBySide();

    // --- 临时变化
    tempVBR[0] = (1 - btnSide->getValue().x()) * 100.0;

    update();
}

void SimpleCurves::updateVibratoGraph() {
    // 重新绘制图像
    if (btnFrequency->isMoving()) {
        updatePhaseByFrequency();
        btnPhase->setValueX(curNote->m_note.m_vbr[5] / 100.0);

        // --- 临时变化
        tempVBR[1] = btnFrequency->getValue().x() * (maxPeriod - minPeriod) + minPeriod;
    } else if (btnPhase->isMoving()) {
        // --- 临时变化
        tempVBR[5] = btnPhase->getValue().x() * 100.0;
    }
    update();
}

void SimpleCurves::updateVibratoHeight() {
    if (btnTop->isMoving()) {
        btnBottom->setPosition(btnCenter->getPosition() +
                               qAbs(btnCenter->getPosition() - btnTop->getPosition()));
    } else {
        btnTop->setPosition(btnCenter->getPosition() -
                            qAbs(btnCenter->getPosition() - btnBottom->getPosition()));
    }

    updateEaseByHeight();

    // --- 临时变化
    double dy =
        -double(btnTop->getPosition() - btnCenter->getPosition()) / curNote->height() * 100.0;
    if (dy == 0) {
        dy = 1;
    }
    tempVBR[2] = dy;
    // 峰谷修改后高度也会跟着变化
    tempVBR[6] =
        -double(btnCenter->getPosition() - (curNote->getStaticY() + curNote->height() / 2)) /
        curNote->height() / (tempVBR[2] / 100.0) * 100.0;
    // --- 临时变化

    update();
}

void SimpleCurves::updateVibratoEase() {
    updateHeightByEase();
    restrictHeightByOffset();

    // --- 临时变化
    if (btnEaseIn->isMoving()) {
        tempVBR[3] = btnEaseIn->getValue() * 100.0;
    } else if (btnEaseOut->isMoving()) {
        tempVBR[4] = (1 - btnEaseOut->getValue()) * 100.0;
    }
    // --- 临时变化

    update();
}

void SimpleCurves::updateVibratoOffset() {
    relieveHeightByOffset();
    updateOthersByOffset();

    // --- 临时变化
    tempVBR[6] =
        -double(btnCenter->getPosition() - (curNote->getStaticY() + curNote->height() / 2)) /
        curNote->height() / (curNote->m_note.m_vbr[2] / 100.0) * 100.0;

    // --- 临时变化

    update();
}

void SimpleCurves::saveMovingVibrato() {

    if (btnCenter->isMoving()) {
        restrictHeightByOffset();
    }

    // 原来长度是0 现在长度也是0 没有保存的意义
    if (curNote->m_note.m_vbr[0] == 0 && tempVBR[0] == 0) {
        qDebug() << "Length = 0 , abandon.";
        abandonVibrato();
        return;
    }

    // 建立记录变量
    NoteOperation *n = new NoteOperation(vbrChange);

    // 设定修改前的数据
    n->setOrg(curNote);

    // 修改数据
    curNote->m_note.m_vbr = tempVBR;

    // 设定修改后的数据
    n->setNew(curNote);
    Root->historyAddStep(n);

    qDebug() << "VBR:" << tempVBR;
}

QPoint SimpleCurves::pVibratoStart() {
    return QPoint(btnSide->getPosition().x(), btnCenter->getPosition());
}

QPoint SimpleCurves::pVibratoEaseInTop() {
    return QPoint(btnEaseIn->getPosition(), btnTop->getPosition());
}

QPoint SimpleCurves::pVibratoEaseInBottom() {
    return QPoint(btnEaseIn->getPosition(), btnBottom->getPosition());
}

QPoint SimpleCurves::pVibratoEaseOutTop() {
    return QPoint(btnEaseOut->getPosition(), btnTop->getPosition());
}

QPoint SimpleCurves::pVibratoEaseOutBottom() {
    return QPoint(btnEaseOut->getPosition(), btnBottom->getPosition());
}

QPoint SimpleCurves::pVibratoEaseInCenter() {
    return QPoint(btnCenter->x(), btnCenter->getPosition());
}

QPoint SimpleCurves::pVibratoEaseOutCenter() {
    return QPoint(btnCenter->x() + btnCenter->width(), btnCenter->getPosition());
}

QPoint SimpleCurves::pVibratoEnd() {
    return QPoint(curNote->getStaticX() + curNote->width(), btnCenter->getPosition());
}

QPoint SimpleCurves::pVibratoStandardStart() {
    return QPoint(btnSide->getPosition().x(), curNote->getStaticY() + curNote->height() / 2);
}

QPoint SimpleCurves::pVibratoStandardEnd() {
    return QPoint(curNote->getStaticX() + curNote->width(),
                  curNote->getStaticY() + curNote->height() / 2);
}

// 内部协调
void SimpleCurves::updateFrequencyBySide() {
    QRect frqRect;

    int x0 = btnSide->getPosition().x();
    int len = curNote->x() + curNote->width() - x0;
    int x1 = x0 + len * minPeriod / maxPeriod;

    frqRect.setTopLeft(QPoint(x1, btnFrequency->getPosition().y()));
    frqRect.setBottomRight(
        QPoint(curNote->getStaticX() + curNote->width(), btnFrequency->getPosition().y()));
    btnFrequency->setLimitArea(frqRect);
}


void SimpleCurves::updatePhaseByFrequency() {
    QRect phaseRect;

    phaseRect.setTopLeft(QPoint(btnSide->getPosition().x(), btnPhase->getPosition().y()));
    phaseRect.setBottomRight(QPoint(btnFrequency->getPosition().x(), btnPhase->getPosition().y()));
    btnPhase->setLimitArea(phaseRect);
}

void SimpleCurves::updateOthersBySide() {
    // 根据边界调整另外两个点按钮
    // 根据颤音信息调整两个点按钮坐标
    updateFrequencyBySide();
    btnFrequency->setValueX((curNote->m_note.m_vbr[1] - minPeriod) / (maxPeriod - minPeriod));

    updatePhaseByFrequency();
    btnPhase->setValueX(curNote->m_note.m_vbr[5] / 100.0);

    // 调整峰谷与中心
    updateHeightBySide();

    // 根据颤音信息调整上下深度的纵坐标
    btnCenter->setPosition(curNote->getStaticY() + curNote->height() / 2 -
                           curNote->m_note.m_vbr[6] / 100.0 * curNote->m_note.m_vbr[2] / 100.0 *
                               curNote->height());

    btnTop->setPosition(btnCenter->getPosition() -
                        curNote->m_note.m_vbr[2] / 100.0 * curNote->height());
    btnBottom->setPosition(btnCenter->getPosition() +
                           curNote->m_note.m_vbr[2] / 100.0 * curNote->height());

    // 根据峰谷调整淡入淡出
    updateEaseByHeight();

    // 根据颤音信息调整淡入淡出的横坐标
    btnEaseIn->setValue(curNote->m_note.m_vbr[3] / 100.0);
    btnEaseOut->setValue(1 - curNote->m_note.m_vbr[4] / 100.0);

    // 根据淡入淡出横坐标重新调整上下深度
    updateHeightByEase();

    // 重新限制上下深度的纵坐标
    restrictHeightByOffset();
}

void SimpleCurves::updateHeightBySide() {
    QRect verticalRect;

    verticalRect.setTopLeft(QPoint(btnSide->getPosition().x(), this->y()));
    verticalRect.setBottomRight(
        QPoint(curNote->getStaticX() + curNote->width(), this->y() + this->height()));

    btnCenter->setLimitArea(verticalRect);
    btnTop->setLimitArea(verticalRect);
    btnBottom->setLimitArea(verticalRect);
}

void SimpleCurves::updateEaseByHeight() {
    QRect horizontalRect;

    horizontalRect.setTopLeft(QPoint(btnSide->getPosition().x(), btnTop->getPosition()));
    horizontalRect.setBottomRight(
        QPoint(curNote->getStaticX() + curNote->width(), btnBottom->getPosition()));

    btnEaseIn->setLimitArea(horizontalRect);
    btnEaseOut->setLimitArea(horizontalRect);
}

void SimpleCurves::updateHeightByEase() {
    QRect verticalRect;

    int btnLeftPos = qMin(btnEaseIn->getPosition(), btnEaseOut->getPosition());
    int btnRightPos = qMax(btnEaseIn->getPosition(), btnEaseOut->getPosition());

    verticalRect.setTopLeft(QPoint(btnLeftPos, this->y()));
    verticalRect.setBottomRight(QPoint(btnRightPos, this->y() + this->height()));

    btnCenter->setLimitArea(verticalRect);
    btnTop->setLimitArea(verticalRect);
    btnBottom->setLimitArea(verticalRect);
}

void SimpleCurves::updateOthersByOffset() {
    int dis = qAbs(btnTop->getPosition() - btnBottom->getPosition());

    btnTop->setPosition(btnCenter->getPosition() - dis / 2);
    btnBottom->setPosition(btnCenter->getPosition() + dis / 2);

    updateEaseByHeight();
}

void SimpleCurves::relieveHeightByOffset() {
    QRect verticalRect2;

    verticalRect2 = btnCenter->getLimitArea();

    btnTop->setLimitArea(verticalRect2);
    btnBottom->setLimitArea(verticalRect2);
}

void SimpleCurves::abandonVibrato() {
    tempVBR = curNote->m_note.m_vbr;

    update();
}

void SimpleCurves::restrictHeightByOffset() {
    QRect verticalRect2;

    verticalRect2 = btnCenter->getLimitArea();
    verticalRect2.setBottom(btnCenter->getPosition());

    btnTop->setLimitArea(verticalRect2);

    verticalRect2 = btnCenter->getLimitArea();
    verticalRect2.setTop(btnCenter->getPosition());

    btnBottom->setLimitArea(verticalRect2);
}
