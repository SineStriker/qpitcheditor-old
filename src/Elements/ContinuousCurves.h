#ifndef CONTINUOUSCURVES_H
#define CONTINUOUSCURVES_H

#include <QObject>
#include <QWidget>

#include "SimpleNote.h"

class ContinuousCurves : public SimpleObject {
    Q_OBJECT
public:
    explicit ContinuousCurves(NotesArea *parent = NULL);
    void initPoints();

private:
    void paintEvent(QPaintEvent *event) override;

private:
    NotesArea *m_parent;
    QVector<short int> PitchBend;
};

#endif // CONTINUOUSCURVES_H
