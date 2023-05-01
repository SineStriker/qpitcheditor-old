#include "ContinuousCurves.h"
#include "Widgets/NotesArea.h"

ContinuousCurves::ContinuousCurves(NotesArea *parent) : SimpleObject(parent) {
    m_parent = parent;

    setAttribute(Qt::WA_TransparentForMouseEvents, true); // 本身不响应鼠标事件
}

void ContinuousCurves::initPoints() {

    return;

    int i, j;
    int tick = 0;
    double pbstart;

    int stepTick = 5;
    int pbstartTick, pbyCur;

    QVector<SimpleNote *> &arr = NotesArea::Stage->notesList;
    QMap<int, int> map;

    qDebug() << "Start init" << arr.size();

    for (i = 0; i < arr.size(); ++i) {
        pbstart = (arr[i]->m_note.m_pbstart == NODEF_DOUBLE) ? arr[i]->getPBStart()
                                                             : (arr[i]->m_note.m_pbstart);

        pbstartTick = arr[i]->convertTimeToTick(pbstart) + arr[i]->getPosition();

        for (j = 0; j < arr[i]->m_note.m_pitches.size(); ++j) {
            tick = pbstartTick + j * stepTick;
            pbyCur =
                (arr[i]->m_note.m_pitches[j] == NODEF_DOUBLE) ? 0 : arr[i]->m_note.m_pitches[j];

            pbyCur += arr[i]->m_note.m_noteNum * 100;

            if (map.contains(tick)) {
                map[tick] = (map[tick] + pbyCur) / 2;
            } else {
                map.insert(tick, pbyCur);
            }
        }
        // 空的
        while (tick < arr[i]->getPosition() + arr[i]->m_note.m_length) {
            tick = arr[i]->getPosition();
            pbyCur = 0;

            if (map.contains(tick)) {
                map[tick] = (map[tick] + pbyCur) / 2;
            } else {
                map.insert(tick, pbyCur);
            }
        }
    }

    tick = 0;
    PitchBend.clear();

    int totalLength = arr.back()->getPosition() + arr.back()->m_note.m_length;
    map.insert(totalLength + stepTick, 0);

    QMap<int, int>::iterator it1, it2;

    it1 = it2 = map.begin();

    while (tick < totalLength) {
        if (it2.key() < tick) {
            it2++;
        }

        QMap<int, int>::iterator it3 = it2;
        it3--;
        while (it1.key() != it3.key()) {
            it1--;
        }
    }
}

void ContinuousCurves::paintEvent(QPaintEvent *event) {
    qDebug() << event;
}
