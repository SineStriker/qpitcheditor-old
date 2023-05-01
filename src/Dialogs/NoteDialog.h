#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QWidget>

#include "Elements/CustomControls.h"
#include "EventDialog.h"

class NoteDialog : public EventDialog {
    Q_OBJECT
public:
    explicit NoteDialog(QWidget *parent);

    void getCurrentValues();
    void saveCurrentValues();

    static QString ambiguousValue;

protected:
    QPushButton *btnCancel, *btnOK;
    QLabel *lbCaption;

    SlideTextControl *scIntensity, *scModulation;
    TextControl *tcFlags;

    TextControl *tcVelocity, *tcStartPoint;
    TextControl *tcPreUtterance, *tcOverlap;

private:
    double firstInt, firstMod, firstPre, firstOve, firstVel, firstStp;
    double firstIntSt, firstModSt, firstPreSt, firstOveSt, firstVelSt;

    bool intDif, modDif, flagsDif, preDif, oveDif, velDif, stpDif;
    QString firstFlags;

    void onCancelClicked();
    void onOKClicked();

    void saveIntensity(SimpleNote *p, double cur);
    void saveModulation(SimpleNote *p, double cur);
    void savePreUtterance(SimpleNote *p, double cur);
    void saveOverlap(SimpleNote *p, double cur);
    void saveVelocity(SimpleNote *p, double cur);
    void saveStartPoint(SimpleNote *p, double cur);
    void saveFlags(SimpleNote *p, QString cur);

    QString convertDouble(double value, bool dif);

signals:
};

#endif // NOTEDIALOG_H
