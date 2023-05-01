#include "PianoArea.h"
#include "mainwindow.h"

PianoArea *PianoArea::Stage = nullptr;

/*--------------------------------------钢琴键元件--------------------------------------*/
PianoKey::PianoKey(QWidget *parent, int x, int y, int w, int h, QColor c, QString s)
    : QWidget(parent), m_y(y), m_height(h), color(c), name(s) {

    rollOver = false;

    setFixedSize(w, h);
    move(x, y);

    adjustHeight();
}

PianoKey::~PianoKey() {
}


void PianoKey::adjustHeight() {
    unsigned currentHeight = noteUnitHeightArr[MainWindow::noteUnitHIndex];

    keyHeight = double(height()) / m_height * 30;

    if (keyHeight != currentHeight) {
        keyHeight = currentHeight;

        setFixedHeight(double(keyHeight) / 30 * m_height);
        move(x(), double(m_y) / 30 * keyHeight);
        update();
    }
}

void PianoKey::paintEvent(QPaintEvent *event) {

    unsigned currentHeight = noteUnitHeightArr[MainWindow::noteUnitHIndex];

    double padding = 0.5;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    if (!rollOver) {
        painter.setBrush(color);
    } else {
        painter.setBrush(QColor(0x4794CB));
    }
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(padding, padding, width() - 2 * padding, height() - 2 * padding, 3, 3);

    if (!name.isEmpty()) {
        painter.setPen(Qt::black);
        painter.setBrush(Qt::transparent);
        painter.setFont(uiFont());

        painter.drawText(60, height() - currentHeight, width() - 60, currentHeight, Qt::AlignCenter,
                         name);
    }
}

void PianoKey::enterEvent(QEvent *event) {
    rollOver = true;
    update();
}

void PianoKey::leaveEvent(QEvent *event) {
    rollOver = false;
    update();
}

void PianoKey::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // click
    }
}

/*--------------------------------------钢琴区域--------------------------------------*/

PianoArea::PianoArea(QWidget *parent) : EditorAreaWidget(parent) {
    unsigned currentHeight = noteUnitHeightArr[MainWindow::noteUnitHIndex];

    addPiano();
    adjustSize();
    setFixedHeight(currentHeight * 84);

    timer = new QTimer(this);
    timer->start(10);

    noteName = new QLabel("ABCD", this);
    noteName->setStyleSheet("background-color:transparent;");
    noteName->setFont(uiFont());
    noteName->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    noteName->adjustSize();
    noteName->move(10, 0);
    onTimer();

    curPos = -1;

    connect(timer, &QTimer::timeout, this, &PianoArea::onTimer);
}

void PianoArea::onTimer() {

    unsigned currentHeight = noteUnitHeightArr[MainWindow::noteUnitHIndex];

    QPoint mouse = QCursor::pos();
    QPoint thisMouse = mapFromGlobal(mouse);

    int index = int(thisMouse.y() / currentHeight);
    int number = 84 - index;

    if (index == curPos) {
        return;
    } else {
        curPos = index;
    }

    if (number < 1 || number > 84) {
        if (noteName->isVisible()) {
            noteName->setVisible(false);
        }
        return;
    } else if (!noteName->isVisible()) {
        noteName->setVisible(true);
    }

    int toY = index * currentHeight;

    QString name = noteNames[number % 12];
    int order = int((number - 1) / 12) + 1;

    noteName->move(noteName->x(), toY + (currentHeight - noteName->height()) / 2);
    noteName->setText(name + QString::number(order));

    if (name.contains('#')) {
        noteName->setPalette(QPalette(QPalette::WindowText, Qt::black));
    } else {
        noteName->setPalette(QPalette(QPalette::WindowText, Qt::white));
    }
}

void PianoArea::addPiano() {
    PianoKey *p;
    int totalPos;
    int num;
    unsigned int i, j;

    totalPos = 0;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {

            int h;
            if (j == 1 || j == 2 || j == 5) {
                h = 60;
            } else {
                h = 45;
            }

            num = 107 - (totalPos + 20) / 30;

            QString notePrefix = noteNames[(num + 1) % 12];
            int order = int(num / 12) - 1;
            QString noteName = notePrefix + QString::number(order);

            if (notePrefix == "C") {
                p = new PianoKey(this, 0, totalPos, 120, h, Qt::white, noteName);
            } else {
                p = new PianoKey(this, 0, totalPos, 120, h, Qt::white);
            }

            keyList.push_back(p);

            totalPos += h;
        }
    }

    qDebug() << "Piano Height :" << totalPos;

    totalPos = 30;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 5; j++) {
            int h = 30;

            num = 107 - (totalPos + 20) / 30;
            p = new PianoKey(this, 0, totalPos, 60, h);

            keyList.push_back(p);

            if (j == 0 || j == 1 || j == 3) {
                totalPos += 60;
            } else {
                totalPos += 90;
            }
        }
    }
}

void PianoArea::adjustHeight() {
    unsigned currentHeight = noteUnitHeightArr[MainWindow::noteUnitHIndex];

    for (PianoKey *it : keyList) {
        if (it) {
            it->adjustHeight();
        }
    }
    noteName->move(noteName->x(), 0);
    curPos = -1;

    setFixedHeight(currentHeight * 84);
    onTimer();
}

void PianoArea::updateNames() {
}

void PianoArea::paintEvent(QPaintEvent *event) {
    /*QPainter painter(this);

    painter.setBrush(QColor(0x232323));
    painter.drawRect(0, 0, 120, 2520);*/
}
