#include "CustomScrolls.h"

BasicScrollArea::BasicScrollArea(QWidget *parent) : QScrollArea(parent) {
    QString qssG = "QScrollArea{ background:transparent; }";

    QString qssV = "QScrollBar{background:transparent; width:8px; margin:0px 0px 0px 0px;}"
                   "QScrollBar::handle{background:rgba(223, 223, 225, 200); border:0px; "
                   "border-radius:4px; margin:0px 0px 0px 0px; min-height:40px;}"
                   "QScrollBar::handle:hover{background:lightgray;}"
                   "QScrollBar::handle:pressed{background:rgba(200, 200, 200, 255);}"
                   "QScrollBar::sub-page{background:transparent;}"
                   "QScrollBar::add-page{background:transparent;}"
                   "QScrollBar::up-arrow{background:transparent;}"
                   "QScrollBar::down-arrow{background:transparent;}"
                   "QScrollBar::sub-line{background:transparent; height:0px;}"
                   "QScrollBar::add-line{background:transparent; height:0px;}";

    QString qssH = "QScrollBar{background:transparent; height:8px; margin:0px 0px 0px 0px;}"
                   "QScrollBar::handle{background:rgba(223, 223, 225, 200); border:0px; "
                   "border-radius:4px; margin:0px 0px 0px 0px; min-width:40px;}"
                   "QScrollBar::handle:hover{background:lightgray;}"
                   "QScrollBar::handle:pressed{background:rgba(200, 200, 200, 255);}"
                   "QScrollBar::sub-page{background:transparent;}"
                   "QScrollBar::add-page{background:transparent;}"
                   "QScrollBar::left-arrow{background:transparent;}"
                   "QScrollBar::right-arrow{background:transparent;}"
                   "QScrollBar::sub-line{background:transparent; height:0px;}"
                   "QScrollBar::add-line{background:transparent; height:0px;}";

    verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

    verticalScrollBar()->setStyleSheet(qssV);
    horizontalScrollBar()->setStyleSheet(qssH);
    this->setStyleSheet(qssG);
}

NotesScrollArea::NotesScrollArea(QWidget *parent) : BasicScrollArea(parent) {

    verticalScrollBar()->installEventFilter(this);
    horizontalScrollBar()->installEventFilter(this);

    verticalScrollBar()->setMinimumHeight(18);
    horizontalScrollBar()->setMinimumWidth(18);
}

NotesScrollArea::~NotesScrollArea() {
}

// 屏蔽滚轮与键盘事件
bool NotesScrollArea::eventFilter(QObject *obj, QEvent *event) {

    if (obj == verticalScrollBar() || obj == horizontalScrollBar()) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            qDebug() << "Ate key press" << keyEvent->key();
            // qDebug() << "ate key";
            return true;
        } else if (event->type() == QEvent::Wheel) {
            // qDebug() << "Ate mouse wheel";
            // event->ignore();
            return true;
        } else {
            return false;
        }
    } else {
        // pass the event on to the parent class
        return BasicScrollArea::eventFilter(obj, event);
    }
}
