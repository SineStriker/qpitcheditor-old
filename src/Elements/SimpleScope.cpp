#include "SimpleScope.h"
#include "SimplePoint.h"

SimpleScope::SimpleScope(SimpleNote *curr, EditorAreaWidget *parent)
    : QWidget(parent), m_parent(parent), curNote(curr) {
    m_solid = true;
}

SimpleScope::~SimpleScope() {
    removeAllPoints();
}

void SimpleScope::reInitializeStyles() {
}

void SimpleScope::adjustLayer() {
    if (m_parent->getCurvesSegLayer()) {
        this->stackUnder(m_parent->getCurvesSegLayer());
    }
}

void SimpleScope::adjustGeometry() {
    // 参考了前后音符的长度，必须在前后音符尺寸更新后调用

    int startX = curNote->m_prev ? curNote->m_prev->x() : curNote->x();
    int prevWidth = curNote->m_prev ? curNote->m_prev->width() : 0;
    int nextWidth = curNote->m_next ? curNote->m_next->width() : 0;

    QRect rect(startX, 0, prevWidth + curNote->width() + nextWidth, m_parent->height());

    if (geometry() != rect) {
        setGeometry(rect);
        update();
    }
}

void SimpleScope::adjustPoints() {
    SimplePoint *p;
    int i;

    for (i = 0; i < points.size(); ++i) {
        p = points[i];
        if (!p) {
            continue;
        }

        QCtrlPoint tp = p->getTPoint();
        int posX = convertValueToPosX(tp.mX);
        int posY = convertValueToPosY(tp.mY);
        p->setPosition(posX, posY);
    }

    // 对最左和最有右的有必要限制一次
    limitAllPoints();
}

void SimpleScope::limitAllPoints() {
    int i;
    SimplePoint *p;
    for (i = 0; i < points.size(); ++i) {
        p = points[i];
        p->externLimitArea();
    }
}

void SimpleScope::saveMovingPoints() {
    SimplePoint *p;

    int i;

    for (i = 0; i < points.size(); ++i) {
        p = points[i];
        if (!p) {
            continue;
        }

        // 没被拖并且特殊位置无变化就不存
        if (!p->isMoving() && p->getSpecialPos() == p->getStaticSpecialPos()) {
            continue;
        }

        double tx = p->getGeneralX();
        double ty = p->getGeneralY();

        // 纠正可能产生的误差，保证通过位置计算得出的mX夹在前后两个点之间
        if (i > 0 && tx < points[i - 1]->getGeneralX()) {
            tx = points[i - 1]->getGeneralX();
        } else if (i < points.size() - 1 && tx > points[i + 1]->getGeneralX()) {
            tx = points[i + 1]->getGeneralX();
        }

        // 没移动的只存 y
        if (p->isMoving()) {
            p->setTPointX(tx);
        }
        p->setTPointY(ty);
    }
}

void SimpleScope::saveSinglePoint(SimplePoint *p) {
    int i = points.indexOf(p);

    if (i < 0) {
        return;
    }

    double tx = p->getGeneralX(true);
    double ty = p->getGeneralY(true);

    // 纠正可能产生的误差，保证通过位置计算得出的mX夹在前后两个点之间
    if (i > 0 && tx < points[i - 1]->getGeneralX()) {
        tx = points[i - 1]->getGeneralX();
    } else if (i < points.size() - 1 && tx > points[i + 1]->getGeneralX()) {
        tx = points[i + 1]->getGeneralX();
    }

    p->setTPointX(tx);
    p->setTPointY(ty);
}

void SimpleScope::addAndSavePoint(QPoint pos) {
    QCtrlPoint tp(0, 0);

    SimplePoint *p, *q;

    if (points.empty()) {
        return;
    }

    // 新建控制点
    p = createNewPoint();
    p->setPosition(pos);
    p->setTPoint(tp);

    if (m_solid) {
        p->show();
    } else {
        p->hide();
    }

    // 将点插入倒数第二个位置
    q = points.back();
    points.pop_back();
    points.push_back(p);
    points.push_back(q);

    // 对所有控制点重新排序
    updateSpecialPositions();
    // 限制最左边与最右边的控制点的位置
    limitAllPoints();

    // 必涉及改动
    saveSinglePoint(p);
    saveToNote();

    update();
}

double SimpleScope::convertPosToValueX(int x) {
    return x;
}

double SimpleScope::convertPosToValueY(int y) {
    return y;
}

int SimpleScope::convertValueToPosX(double x) {
    return x;
}

int SimpleScope::convertValueToPosY(double y) {
    return y;
}

void SimpleScope::saveToNote(QVector<QCtrlPoint> arr) {
    // 不知道怎么存
}

void SimpleScope::saveAndUpdate() {
    saveToNote();
    adjustPoints();
    update();
}

void SimpleScope::removePoint(SimplePoint *p) {
    points.removeOne(p);
    delete p;
    p = nullptr;

    // 删除控制点后需要重新排序
    updateSpecialPositions();

    // 限制所有控制点位置
    limitAllPoints();

    // 保存到音符
    saveToNote();

    // 更新曲线形状
    update();
}

void SimpleScope::removePoints(QVector<SimplePoint *> arr) {

    SimplePoint *p;

    for (int i = 0; i < arr.size(); ++i) {
        p = arr[i];
        qDebug() << "Remove Point : " << p;
        points.removeOne(p);
        delete p;
        p = nullptr;
    }

    // 删除控制点后需要重新排序
    updateSpecialPositions();

    // 限制所有控制点位置
    limitAllPoints();

    // 保存到音符
    saveToNote();

    // 更新曲线形状
    update();
}

void SimpleScope::initPoints(QVector<QCtrlPoint> arr) {
    SimplePoint *p;

    int i;
    bool minus = false;

    // 如果已有的点多于数组中的 则删
    while (points.size() > arr.size()) {
        minus = true;
        qDebug() << points.size() << arr.size() << "Delete some points.";
        p = points.back();
        delete p;
        p = nullptr;

        points.pop_back();
    }
    if (minus) {
        updateSpecialPositions();
    }

    // 改点
    for (i = 0; i < points.size(); ++i) {
        p = points[i];
        p->setTPoint(arr[i]);
    }

    // 加点
    i = points.size();
    while (i < arr.size()) {
        p = createNewPoint(); // 将控制点的父窗口设为编辑区域
        p->setTPoint(arr[i]);
        points.push_back(p);

        if (m_solid) {
            p->show();
        } else {
            p->hide();
        }

        ++i;
    }
}

void SimpleScope::initAndAdjustPoints(QVector<QCtrlPoint> arr) {
    adjustGeometry();
    initPoints(arr);

    adjustPoints();

    // 重新排序
    updateSpecialPositions();
    // 限制最左边与最右边的控制点的位置
    limitAllPoints();

    update();
}

SimplePoint *SimpleScope::createNewPoint() {
    SimplePoint *p = new SimplePoint(this, m_parent);

    connect(p, SIGNAL(updatePoint(SimplePoint *, pointMoveEvent)), this,
            SLOT(onUpdatePoint(SimplePoint *, pointMoveEvent)));

    return p;
}

SimplePoint *SimpleScope::getNearestPoint(SimplePoint *p, int limitedPos) {
    if (points.empty()) {
        return nullptr;
    }
    double min = qAbs(points[0]->x() - limitedPos);
    int minIndex = 0;

    for (int i = 1; i < points.size(); ++i) {
        if (points[i] && points[i] != p) {
            double cur = qAbs(points[i]->x() - limitedPos);
            if (cur < min) {
                min = cur;
                minIndex = i;
            }
        }
    }
    return points[minIndex];
}

SimpleNote *SimpleScope::getPrevNote() {
    return curNote->m_prev;
}

SimpleNote *SimpleScope::getCurNote() {
    return curNote;
}

SimpleNote *SimpleScope::getNextNote() {
    return curNote->m_next;
}

SimplePoint *SimpleScope::firstPoint() {
    if (points.empty()) {
        return nullptr;
    }
    return points[0];
}
SimplePoint *SimpleScope::lastPoint() {
    if (points.empty()) {
        return nullptr;
    }
    return points.back();
}

void SimpleScope::setPointsVisible(bool value) {
    for (SimplePoint *p : points) {
        p->setVisible(value);
    }
}

QRect SimpleScope::getActiveScope() {
    return rect();
}

bool SimpleScope::detectOver(SimplePoint *p) {
    int i;
    bool flag = false;
    for (i = 0; i < points.size(); ++i) {
        if (points[i] == p) {
            continue;
        }
        if (points[i]->getPosition() == p->getPosition()) {
            flag = true;
            break;
        }
    }
    if (flag) {
        qDebug() << "Remove Overlapped point" << p;
    }
    return flag;
}

void SimpleScope::InsertSort() {
    int i, j;
    SimplePoint *key;

    for (j = 1; j < points.size(); ++j) {
        key = points[j];
        i = j - 1;

        while (i >= 0 && points[i]->getX() > key->getX()) {
            points[i + 1] = points[i];
            i = i - 1;
        }
        points[i + 1] = key;
    }
}

void SimpleScope::updateSpecialPositions() {
    if (points.isEmpty()) {
        return;
    }

    InsertSort();

    if (points.size() < 2) {
        return;
    }

    // 最左边的点
    points[0]->setSpecialPos(pointLeftmost);
    // 最右边的点
    points.back()->setSpecialPos(pointRightmost);
    // 中间的点
    for (int j = 1; j < points.size() - 1; ++j) {
        points[j]->setSpecialPos(pointBetween);
    }
}

void SimpleScope::removeAllPoints() {
    // 曲线析构时连带控制点一起析构
    for (int i = 0; i < points.size(); ++i) {
        if (points[i]) {
            delete points[i];
        }
        points[i] = nullptr;
    }
    points.clear();
}

void SimpleScope::onUpdatePoint(SimplePoint *p, pointMoveEvent e) {
}
