#include "VolumeArea.h"
#include "Elements/SimpleEnvelope.h"
#include "NotesArea.h"
#include "mainwindow.h"

unsigned VolumeArea::standardHeight = 0;
VolumeArea *VolumeArea::Stage = nullptr;

VolumeForm::VolumeForm(QWidget *parent) : SimpleForm(parent) {

    mainArea = new VolumeOuterArea(this);

    setDefaultTitleBar();
    setWidget(mainArea);
    setTitle(tr("Volume Panel"));

    getLayout()->setMargin(0);

    mainArea->setMinimumHeight(60);

    // 三个功能键
    btnEase23 = new VectorDblButton(getTitleBar(), true, QSize(36, 24), QSize(0, 2));
    btnEase14 = new VectorDblButton(getTitleBar(), true, QSize(36, 24), QSize(0, 2));
    btnReset = new VectorDblButton(getTitleBar(), true, QSize(36, 24), QSize(0, 2));

    // 两个开关键
    btnLines = new VectorDblButton(getTitleBar(), false, QSize(36, 24), QSize(0, 2));
    btnBackFill = new VectorDblButton(getTitleBar(), false, QSize(36, 24), QSize(0, 2));

    btnEase23->setIcons(QString(":/images/VolumeGroupBtn1.svg"),
                        QString(":/images/VolumeGroupBtn1_2.svg"));
    btnEase14->setIcons(QString(":/images/VolumeGroupBtn2.svg"),
                        QString(":/images/VolumeGroupBtn2_2.svg"));
    btnReset->setIcons(QString(":/images/VolumeGroupBtn3.svg"),
                       QString(":/images/VolumeGroupBtn3_2.svg"));

    btnLines->setIcons(QString(":/images/VolumeModeBtn1.svg"),
                       QString(":/images/VolumeModeBtn1_2.svg"));
    btnBackFill->setIcons(QString(":/images/VolumeModeBtn2.svg"),
                          QString(":/images/VolumeModeBtn2_2.svg"));

    int modeGroupSpace = 37;

    btnEase23->move(175, 0);
    btnEase14->move(btnEase23->x() + modeGroupSpace, 0);
    btnReset->move(btnEase14->x() + modeGroupSpace, 0);

    btnLines->move(btnReset->x() + btnReset->width() + 20, 0);
    btnBackFill->move(btnLines->x() + modeGroupSpace, 0);

    getTitleBar()->addItem(btnEase23);
    getTitleBar()->addItem(btnEase14);
    getTitleBar()->addItem(btnReset);

    getTitleBar()->addItem(btnLines);
    getTitleBar()->addItem(btnBackFill);

    connect(btnEase23, &VectorDblButton::clicked, this, &VolumeForm::onEase23BtnClick);
    connect(btnEase14, &VectorDblButton::clicked, this, &VolumeForm::onEase14BtnClick);
    connect(btnReset, &VectorDblButton::clicked, this, &VolumeForm::onResetBtnClick);

    connect(btnLines, &VectorDblButton::clicked, this, &VolumeForm::onLinesBtnClick);
    connect(btnBackFill, &VectorDblButton::clicked, this, &VolumeForm::onFillBtnClick);

    updateButtons();
}

void VolumeForm::onEase23BtnClick() {
    if (Root->stageSelectedNotes.isEmpty()) {
        return;
    }

    Root->historySequence = 1;

    for (auto it = Root->stageSelectedNotes.begin(); it != Root->stageSelectedNotes.end(); ++it) {
        SimpleNote *p = *it;
        bool changed = false;

        QVector<QCtrlPoint> list = p->m_note.m_env;

        CorrectGenon curGenon = p->getCorrectGenon();
        CorrectGenon nextGenon = p->m_next ? p->m_next->getCorrectGenon() : CorrectGenon();

        if (p->m_prev && !p->m_prev->isRest() && curGenon.VoiceOverlap > 0) {
            changed = true;

            list[0].mX = list[0].mY = 0;
            list[1].mX = curGenon.VoiceOverlap;
        }
        if (p->m_next && !p->m_next->isRest() && nextGenon.VoiceOverlap > 0) {
            changed = true;

            list[list.size() - 1].mX = list[list.size() - 1].mY = 0;
            list[list.size() - 2].mX = nextGenon.VoiceOverlap;
        }

        // 如果是五点，把第二个点删了
        if (changed && list.size() == 5) {
            list.removeAt(2);
        }
        p->m_envelope->saveToNote(list);
        p->changeEnvelope();
    }

    Root->historySequence = 0;
}

void VolumeForm::onEase14BtnClick() {
    if (Root->stageSelectedNotes.isEmpty()) {
        return;
    }

    Root->historySequence = 1;

    for (auto it = Root->stageSelectedNotes.begin(); it != Root->stageSelectedNotes.end(); ++it) {
        SimpleNote *p = *it;
        bool changed = false;

        QVector<QCtrlPoint> list = p->m_note.m_env;

        // double interval = p->getCorrectDuration() - list[0].mX - list.back().mX;
        // interval = (interval < 0) ? 0 : interval;

        CorrectGenon curGenon = p->getCorrectGenon();
        CorrectGenon nextGenon = p->m_next ? p->m_next->getCorrectGenon() : CorrectGenon();

        if (p->m_prev && !p->m_prev->isRest() && curGenon.VoiceOverlap > 0) {
            changed = true;

            list[0].mX = curGenon.VoiceOverlap;
            list[0].mY = 100;

            list[1].mX = 5;
            list[1].mY = 100;
        }
        if (p->m_next && !p->m_next->isRest() && nextGenon.VoiceOverlap > 0) {
            changed = true;

            list[list.size() - 1].mX = nextGenon.VoiceOverlap;
            list[list.size() - 1].mY = 100;

            list[list.size() - 2].mX = 5;
            list[list.size() - 2].mY = 100;
        }

        // 如果是五点，把第二个点删了
        if (changed && list.size() == 5) {
            list.removeAt(2);
        }
        p->m_envelope->saveToNote(list);
        p->changeEnvelope();
    }

    Root->historySequence = 0;
}

void VolumeForm::onResetBtnClick() {
    if (Root->stageSelectedNotes.isEmpty()) {
        return;
    }

    Root->historySequence = 1;

    for (auto it = Root->stageSelectedNotes.begin(); it != Root->stageSelectedNotes.end(); ++it) {
        QVector<QCtrlPoint> list = AbstractNote::defaultEnvelope();

        SimpleNote *p = *it;

        p->m_envelope->saveToNote(list);
        p->changeEnvelope();
    }

    Root->historySequence = 0;
}

void VolumeForm::onLinesBtnClick() {
    Root->setEnvLinesStatus(!Root->bShowEnvLines);
    updateButtons();
}

void VolumeForm::onFillBtnClick() {
    Root->setEnvFillStatus(!Root->bShowEnvFill);
    updateButtons();
}

void VolumeForm::updateButtons() {
    btnLines->setStatus(Root->bShowEnvLines);
    btnBackFill->setStatus(Root->bShowEnvFill);
}


VolumeOuterArea::VolumeOuterArea(QWidget *parent) : EditorArea(parent) {
    mainLayout = new QHBoxLayout(this);

    // 钢琴窗
    volumeScroll = new NoneScrollArea(this);
    volume = new VolumeArea(volumeScroll);
    volume->setParent(this);

    volumeScroll->setWidget(volume);

    mainLayout->addWidget(volumeScroll);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    connect(volumeScroll, &NoneScrollArea::onResizeEvent, this, &VolumeOuterArea::onScrollResize);
}

void VolumeOuterArea::onScrollResize(QResizeEvent *event) {
    volume->resize(volume->width(), volumeScroll->height());
    VolumeArea::standardHeight = volume->height() / 2.0 * 0.8;
    NotesArea::Stage->adjustEnvelopes();
}

VolumeArea::VolumeArea(QWidget *parent) : EditorAreaWidget(parent) {
    noteTopLayer = new QWidget(this);
    curvesTopLayer = new QWidget(this);
    pointTopLayer = new QWidget(this);
    vibratoTopLayer = new QWidget(this);
}

void VolumeArea::zoomResize(int w) {
    setFixedWidth(w);
}

void VolumeArea::paintEvent(QPaintEvent *event) {
    int i, j;
    QSet<short> deep{1, 3, 5, 8, 10};

    double thinV = 0.5;
    double boldV = 0.8;

    double boldH = 0.5;

    QColor thinC(MainWindow::volume_timeLineColor);
    QColor boldC(MainWindow::volume_sectionLineColor);

    double unit_w = noteUnitWidthArr[Root->noteUnitWIndex] / 4;

    int quarters = int(width() / unit_w) + 1;

    QPainter painter(this);
    painter.setPen(Qt::transparent);

    // 画背景
    painter.setBrush(QColor(MainWindow::volume_backColor));
    painter.drawRect(0, 0, width(), height());

    // 画竖线
    double snap_w = unit_w / Root->snappingUnit;
    double cur_x = 0;
    double halfW;
    QColor tempC;

    int tempLengthUnit = Root->snappingUnit;

    while (snap_w < 8) {
        snap_w *= 2;
        tempLengthUnit /= 2;
    }

    i = 0;

    while (cur_x < quarters * unit_w) {
        halfW = (i % tempLengthUnit == 0) ? boldV : thinV;
        tempC = (i % tempLengthUnit == 0) ? boldC : thinC;

        painter.setBrush(tempC);
        painter.drawRect(cur_x - halfW, 0, 2 * halfW, height());
        cur_x += snap_w;
        i++;
    }
    for (i = -2; i <= 2; ++i) {
        painter.setBrush(MainWindow::volume_standardLineColor);
        painter.drawRect(0, height() / 2 + i * standardHeight - boldH, width(), 2 * boldH);
    }
}

void VolumeArea::resizeEvent(QResizeEvent *event) {
}

void VolumeArea::leaveEvent(QEvent *event) {
}

void VolumeArea::mousePressEvent(QMouseEvent *event) {
}

void VolumeArea::mouseReleaseEvent(QMouseEvent *event) {
    Root->removeAllSelectedNotes();
    Root->removeAllSelectedPoints();
}

void VolumeArea::mouseDoubleClickEvent(QMouseEvent *event) {
}
