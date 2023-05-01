#include "ScaleplateArea.h"
#include "mainwindow.h"

ScaleplateArea *ScaleplateArea::Stage = nullptr;

/*--------------------------------------小节线元件--------------------------------------*/
SectionLabel::SectionLabel(QWidget *parent) : QWidget(parent) {
    setText("");
}

SectionLabel::SectionLabel(QString str, QWidget *parent) : QWidget(parent) {
    setText(str);
}

SectionLabel::~SectionLabel() {
}

void SectionLabel::setText(QString str) {
    text = str;
    update();

    setFixedHeight(15);
}

QString SectionLabel::getText() {
    return text;
}

void SectionLabel::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QFont font(uiFont());

    painter.setFont(font);
    painter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    painter.drawLine(0, 0, 0, 15);
    painter.drawText(2, 0, text);

    painter.drawText(5, 0, 50, 15, Qt::AlignBottom, text);
}


/*--------------------------------------标尺区域--------------------------------------*/

ScaleplateArea::ScaleplateArea(QWidget *parent) : EditorAreaWidget(parent) {
    count = minimumSections;
    setBeat(4, 4);

    fillBackground(this);
}

void ScaleplateArea::adjustSections() {
    unsigned currentWidth = noteUnitWidthArr[MainWindow::noteUnitWIndex];

    for (int i = 0; i < lineList.size(); ++i) {
        SectionLabel *it = lineList[i];
        it->move(i * currentWidth * double(sectionWidth.x()) / sectionWidth.y(), it->y());
    }

    adjustWidth();
}

void ScaleplateArea::adjustWidth() {
    unsigned currentWidth = noteUnitWidthArr[MainWindow::noteUnitWIndex];
    this->setFixedWidth(count * currentWidth);
}

int ScaleplateArea::setScaleplates(int n) {
    if (n == count) {
        return 0;
    }
    count = n;

    return setBeat();
}

int ScaleplateArea::setBeat(int numerator, int denominator) {
    sectionWidth = QPoint(numerator, denominator);
    return setBeat();
}

int ScaleplateArea::setBeat() {
    int n = ceil(double(count) / sectionWidth.x() * sectionWidth.y());

    if (n == lineList.size()) {
        return 0;
    }

    addScaleplates(n);
    return 1;
}

void ScaleplateArea::addScaleplates(int n) {

    unsigned currentWidth = noteUnitWidthArr[MainWindow::noteUnitWIndex];

    int i;
    SectionLabel *p;

    if (n < lineList.size()) {
        while (lineList.size() > n) {
            p = lineList.back();
            if (p) {
                delete p;
                p = nullptr;
            }
            lineList.pop_back();
        }
    } else {
        while (lineList.size() < n) {
            i = lineList.size() + 1;

            p = new SectionLabel(QString::number(i), this);
            p->move(currentWidth * (i - 1) * double(sectionWidth.x()) / sectionWidth.y(), 0);

            lineList.push_back(p);
            p->show();
        }
    }

    adjustSections();
}

void ScaleplateArea::paintEvent(QPaintEvent *event) {
}
