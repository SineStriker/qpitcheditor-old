#include "NoteDialog.h"

#include "Elements/SimpleNote.h"
#include "mainwindow.h"

QString NoteDialog::ambiguousValue = " ";

NoteDialog::NoteDialog(QWidget *parent) : EventDialog(parent) {
    // 窗口初始化
    setWindowTitle(childWindowTitle);
    setFixedSize(450, 525);

    // 添加标签和按钮
    QFont font(normalFont());
    QFont font2(normalHFont());

    lbCaption = new QLabel(tr("Note Properties"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);
    btnOK = new QPushButton(tr("OK"), this);

    lbCaption->setFont(font2);
    btnCancel->setFont(font);
    btnOK->setFont(font);

    lbCaption->move(25, 20);

    btnCancel->setFixedSize(115, 35);
    btnCancel->move(width() - btnCancel->width() - 20, height() - btnCancel->height() - 20);

    btnOK->setFixedSize(115, 35);
    btnOK->move(width() - btnCancel->width() - 145, height() - btnCancel->height() - 20);

    btnOK->setFocus();
    btnOK->setDefault(true);

    btnOK->setShortcut(QKeySequence(Qt::Key_Return));
    btnCancel->setShortcut(QKeySequence(Qt::Key_Escape));

    connect(btnOK, &QPushButton::clicked, this, &NoteDialog::onOKClicked);
    connect(btnCancel, &QPushButton::clicked, this, &NoteDialog::onCancelClicked);

    scIntensity = new SlideTextControl(tr("Intensity"), this);
    scIntensity->setProportion(1, 1);
    scIntensity->setSingleStep(0.01);
    scIntensity->setBlankValue(blankIntensity);
    scIntensity->setMinimum(0);   // 最小值0
    scIntensity->setMaximum(200); // 最大值200

    scModulation = new SlideTextControl(tr("Modulation"), this);
    scModulation->setProportion(1, 1);
    scModulation->setSingleStep(0.01);
    scModulation->setBlankValue(blankModulation);
    scModulation->setMinimum(-200); // 最小值-200
    scModulation->setMaximum(200);  // 最大值200

    tcFlags = new TextControl(tr("Flags"), this);
    tcFlags->setProportion(1, 2);

    tcPreUtterance = new TextControl(tr("PreUtterance"), this);
    tcPreUtterance->setProportion(1, 1);

    tcOverlap = new TextControl(tr("Overlap"), this);
    tcOverlap->setProportion(1, 1);

    tcVelocity = new TextControl(tr("Consonant Velocity"), this);
    tcVelocity->setProportion(1, 1);

    tcStartPoint = new TextControl(tr("Start Point"), this);
    tcStartPoint->setProportion(1, 1);

    scIntensity->setFixedHeight(80);
    scModulation->setFixedHeight(80);

    tcFlags->setFixedHeight(50);
    tcPreUtterance->setFixedHeight(50);
    tcOverlap->setFixedHeight(50);
    tcVelocity->setFixedHeight(50);
    tcStartPoint->setFixedHeight(50);

    int elemetHeight = 40;

    tcPreUtterance->setGeometry(15, 50, 420, elemetHeight);

    tcOverlap->setGeometry(tcPreUtterance->x(), tcPreUtterance->y() + tcPreUtterance->height(),
                           tcPreUtterance->width(), elemetHeight);

    tcVelocity->setGeometry(tcOverlap->x(), tcOverlap->y() + tcOverlap->height(),
                            tcOverlap->width(), elemetHeight);

    tcStartPoint->setGeometry(tcVelocity->x(), tcVelocity->y() + tcVelocity->height(),
                              tcVelocity->width(), elemetHeight);

    scIntensity->setGeometry(tcStartPoint->x(), tcStartPoint->y() + tcStartPoint->height(),
                             tcStartPoint->width(), elemetHeight * 2);

    scModulation->setGeometry(scIntensity->x(), scIntensity->y() + scIntensity->height(),
                              scIntensity->width(), elemetHeight * 2);
    tcFlags->setGeometry(scModulation->x(), scModulation->y() + scModulation->height(),
                         scModulation->width(), elemetHeight);

    // 设置删除按钮可见
    scIntensity->Text()->setClearButtonEnabled(true);
    scModulation->Text()->setClearButtonEnabled(true);
    tcFlags->Text()->setClearButtonEnabled(true);
    tcPreUtterance->Text()->setClearButtonEnabled(true);
    tcOverlap->Text()->setClearButtonEnabled(true);
    tcVelocity->Text()->setClearButtonEnabled(true);
    tcStartPoint->Text()->setClearButtonEnabled(true);

    // 添加限制数字
    QDoubleValidator *dblValidatorP = new QDoubleValidator(0, INFINITY, 3, this);
    QDoubleValidator *dblValidatorR = new QDoubleValidator(-INFINITY, INFINITY, 3, this);
    dblValidatorP->setNotation(QDoubleValidator::StandardNotation);
    dblValidatorR->setNotation(QDoubleValidator::StandardNotation);
    tcPreUtterance->setValidator(dblValidatorP);
    tcOverlap->setValidator(dblValidatorR);
    tcVelocity->setValidator(dblValidatorR);
    tcStartPoint->setValidator(dblValidatorP);

    getCurrentValues();
}

void NoteDialog::getCurrentValues() {
    double curInt, curMod, curPre, curOve, curVel, curStp;
    QString curFlags;

    // 找最左边的
    SimpleNote *p = *Root->stageSelectedNotes.begin();

    for (auto iter = Root->stageSelectedNotes.begin(); iter != Root->stageSelectedNotes.end();
         ++iter) {
        if ((*iter)->x() < p->x() || (p->isRest() && !(*iter)->isRest())) {
            p = *iter;
        }
    }

    firstInt = (p->m_note.intStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_int;
    firstMod = (p->m_note.modStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_mod;
    firstPre = (p->m_note.preUttrStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_preUttr;
    firstOve = (p->m_note.overlapStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_overlap;
    firstVel = (p->m_note.velStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_vel;
    firstStp = (p->m_note.stpStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_stp;
    firstFlags = p->m_note.m_flags;

    intDif = modDif = flagsDif = preDif = oveDif = velDif = stpDif = false;

    for (auto iter = Root->stageSelectedNotes.begin(); iter != Root->stageSelectedNotes.end();
         ++iter) {
        p = *iter;

        if (p->isRest()) {
            continue;
        }

        curInt = (p->m_note.intStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_int;
        curMod = (p->m_note.modStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_mod;
        curPre = (p->m_note.preUttrStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_preUttr;
        curOve = (p->m_note.overlapStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_overlap;
        curVel = (p->m_note.velStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_vel;
        curStp = (p->m_note.stpStatus == MFM_OMITTED) ? NODEF_DOUBLE : p->m_note.m_stp;
        curFlags = p->m_note.m_flags;

        if (curInt != firstInt) {
            intDif = true;
        }
        if (curMod != firstMod) {
            modDif = true;
        }
        if (curPre != firstPre) {
            preDif = true;
        }
        if (curOve != firstOve) {
            oveDif = true;
        }
        if (curVel != firstVel) {
            velDif = true;
        }
        if (curStp != firstStp) {
            stpDif = true;
        }
        if (curFlags != firstFlags) {
            flagsDif = true;
        }
    }

    scIntensity->setText(convertDouble(firstInt, intDif));
    scModulation->setText(convertDouble(firstMod, modDif));
    tcPreUtterance->setValue(convertDouble(firstPre, preDif));
    tcOverlap->setValue(convertDouble(firstOve, oveDif));
    tcVelocity->setValue(convertDouble(firstVel, velDif));
    tcStartPoint->setValue(convertDouble(firstStp, stpDif));
    tcFlags->setValue(flagsDif ? ambiguousValue : firstFlags);

    if (intDif) {
        scIntensity->setUnmodified(true);
    }
    if (modDif) {
        scModulation->setUnmodified(true);
    }
    if (preDif) {
        tcPreUtterance->setUnmodified(true);
    }
    if (oveDif) {
        tcOverlap->setUnmodified(true);
    }
    if (velDif) {
        tcVelocity->setUnmodified(true);
    }
    if (stpDif) {
        tcStartPoint->setUnmodified(true);
    }
    if (flagsDif) {
        tcFlags->setUnmodified(true);
    }

    // 设置默认文字
    if (Root->stageSelectedNotes.size() == 1) {
        QGenonSettings g;
        bool r = (*Root->stageSelectedNotes.begin())->m_note.updateGenons(&g);
        if (r) {
            tcPreUtterance->setPlaceholder(QString::number(g.mPreUtterance), ambiguousValue);
            tcOverlap->setPlaceholder(QString::number(g.mVoiceOverlap), ambiguousValue);
        } else {
            tcPreUtterance->setPlaceholder(tr("Not found"), ambiguousValue);
            tcOverlap->setPlaceholder(tr("Not found"), ambiguousValue);
        }
    } else {
        tcPreUtterance->setPlaceholder(tr("oto"), ambiguousValue);
        tcOverlap->setPlaceholder(tr("oto"), ambiguousValue);
    }
    scIntensity->setPlaceholder(QString::number(blankIntensity), ambiguousValue);
    scModulation->setPlaceholder(QString::number(blankModulation), ambiguousValue);
    tcVelocity->setPlaceholder(QString::number(blankVelocity), ambiguousValue);
    tcStartPoint->setPlaceholder(QString::number(blankStartPoint), ambiguousValue);
}

void NoteDialog::saveCurrentValues() {
    SimpleNote *p;

    double curInt, curMod, curPre, curOve, curVel, curStp;
    QString curFlags;

    curInt = scIntensity->getText().isEmpty() ? NODEF_DOUBLE : scIntensity->getText().toDouble();
    curMod = scModulation->getText().isEmpty() ? NODEF_DOUBLE : scModulation->getText().toDouble();
    curPre =
        tcPreUtterance->getValue().isEmpty() ? NODEF_DOUBLE : tcPreUtterance->getValue().toDouble();
    curOve = tcOverlap->getValue().isEmpty() ? NODEF_DOUBLE : tcOverlap->getValue().toDouble();
    curVel = tcVelocity->getValue().isEmpty() ? NODEF_DOUBLE : tcVelocity->getValue().toDouble();
    curStp =
        tcStartPoint->getValue().isEmpty() ? NODEF_DOUBLE : tcStartPoint->getValue().toDouble();
    curFlags = tcFlags->getValue();

    // 一次性修改
    Root->historySequence = 1;

    if (Root->stageSelectedNotes.size() == 1) {
        p = (*Root->stageSelectedNotes.begin());
        if (curInt != firstInt) {
            saveIntensity(p, curInt);
        }
        if (curMod != firstMod) {
            saveModulation(p, curMod);
        }
        if (curPre != firstPre) {
            savePreUtterance(p, curPre);
        }
        if (curOve != firstOve) {
            saveOverlap(p, curOve);
        }
        if (curVel != firstVel) {
            saveVelocity(p, curVel);
        }
        if (curStp != firstStp) {
            saveVelocity(p, curVel);
        }
        if (curFlags != firstFlags) {
            saveFlags(p, curFlags);
        }
        p->changeValues();
    } else {
        if ((!scIntensity->isUnmodified() && intDif) || (curInt != firstInt && !intDif)) {
            for (auto iter = Root->stageSelectedNotes.begin();
                 iter != Root->stageSelectedNotes.end(); ++iter) {
                p = *iter;
                if (p->isRest()) {
                    continue;
                }
                saveIntensity(p, curInt);
            }
        }
        if ((!scModulation->isUnmodified() && modDif) || (curMod != firstMod && !modDif)) {
            for (auto iter = Root->stageSelectedNotes.begin();
                 iter != Root->stageSelectedNotes.end(); ++iter) {
                p = *iter;
                if (p->isRest()) {
                    continue;
                }
                saveModulation(p, curMod);
            }
        }
        if ((!tcPreUtterance->isUnmodified() && preDif) || (curPre != firstPre && !preDif)) {
            for (auto iter = Root->stageSelectedNotes.begin();
                 iter != Root->stageSelectedNotes.end(); ++iter) {
                p = *iter;
                if (p->isRest()) {
                    continue;
                }
                savePreUtterance(p, curPre);
            }
        }
        if ((!tcOverlap->isUnmodified() && oveDif) || (curOve != firstOve && !oveDif)) {
            for (auto iter = Root->stageSelectedNotes.begin();
                 iter != Root->stageSelectedNotes.end(); ++iter) {
                p = *iter;
                if (p->isRest()) {
                    continue;
                }
                saveOverlap(p, curOve);
            }
        }
        if ((!tcVelocity->isUnmodified() && velDif) || (curVel != firstVel && !velDif)) {
            for (auto iter = Root->stageSelectedNotes.begin();
                 iter != Root->stageSelectedNotes.end(); ++iter) {
                p = *iter;
                if (p->isRest()) {
                    continue;
                }
                saveVelocity(p, curVel);
            }
        }
        if ((!tcStartPoint->isUnmodified() && stpDif) || (curStp != firstStp && !stpDif)) {
            for (auto iter = Root->stageSelectedNotes.begin();
                 iter != Root->stageSelectedNotes.end(); ++iter) {
                p = *iter;
                if (p->isRest()) {
                    continue;
                }
                saveStartPoint(p, curStp);
            }
        }
        if ((!tcFlags->isUnmodified() && flagsDif) || (curFlags != firstFlags && !flagsDif)) {
            for (auto iter = Root->stageSelectedNotes.begin();
                 iter != Root->stageSelectedNotes.end(); ++iter) {
                p = *iter;
                if (p->isRest()) {
                    continue;
                }
                saveFlags(p, curFlags);
            }
        }
        for (auto iter = Root->stageSelectedNotes.begin(); iter != Root->stageSelectedNotes.end();
             ++iter) {
            p = *iter;
            if (p->isRest()) {
                continue;
            }
            p->changeValues();
        }
    }

    Root->historySequence = 0;
}

void NoteDialog::onCancelClicked() {
    close();
}

void NoteDialog::onOKClicked() {
    setResult(1);
    saveCurrentValues();
    close();
}

// 保存变量
void NoteDialog::saveIntensity(SimpleNote *p, double cur) {
    // 建立记录变量
    NoteOperation *n = new NoteOperation(intChange);
    // 设定修改前的数据
    n->setOrg(p);
    // 将控制点改动保存到音符
    if (cur == NODEF_DOUBLE) {
        p->m_note.m_int = blankIntensity;
        p->m_note.intStatus = MFM_OMITTED;
    } else {
        p->m_note.m_int = cur;
        p->m_note.intStatus = MFM_MODIFIED;
    }
    // 设定修改后的数据
    n->setNew(p);
    Root->historyAddStep(n);
}

void NoteDialog::saveModulation(SimpleNote *p, double cur) {
    // 建立记录变量
    NoteOperation *n = new NoteOperation(modChange);
    // 设定修改前的数据
    n->setOrg(p);
    // 将控制点改动保存到音符
    if (cur == NODEF_DOUBLE) {
        p->m_note.m_mod = blankModulation;
        p->m_note.modStatus = MFM_OMITTED;
    } else {
        p->m_note.m_mod = cur;
        p->m_note.modStatus = MFM_MODIFIED;
    }
    // 设定修改后的数据
    n->setNew(p);
    Root->historyAddStep(n);
}

void NoteDialog::savePreUtterance(SimpleNote *p, double cur) {
    // 建立记录变量
    NoteOperation *n = new NoteOperation(preChange);
    // 设定修改前的数据
    n->setOrg(p);
    // 将控制点改动保存到音符
    if (cur == NODEF_DOUBLE) {
        p->m_note.m_preUttr = 0;
        p->m_note.preUttrStatus = MFM_OMITTED;
    } else {
        p->m_note.m_preUttr = cur;
        p->m_note.preUttrStatus = MFM_MODIFIED;
    }
    // 设定修改后的数据
    n->setNew(p);
    Root->historyAddStep(n);
}


void NoteDialog::saveOverlap(SimpleNote *p, double cur) {
    // 建立记录变量
    NoteOperation *n = new NoteOperation(oveChange);
    // 设定修改前的数据
    n->setOrg(p);
    // 将控制点改动保存到音符
    if (cur == NODEF_DOUBLE) {
        p->m_note.m_overlap = 0;
        p->m_note.overlapStatus = MFM_OMITTED;
    } else {
        p->m_note.m_overlap = cur;
        p->m_note.overlapStatus = MFM_MODIFIED;
    }
    // 设定修改后的数据
    n->setNew(p);
    Root->historyAddStep(n);
}

void NoteDialog::saveVelocity(SimpleNote *p, double cur) {
    // 建立记录变量
    NoteOperation *n = new NoteOperation(velChange);
    // 设定修改前的数据
    n->setOrg(p);
    // 将控制点改动保存到音符
    if (cur == NODEF_DOUBLE) {
        p->m_note.m_vel = blankVelocity;
        p->m_note.velStatus = MFM_OMITTED;
    } else {
        p->m_note.m_vel = cur;
        p->m_note.velStatus = MFM_MODIFIED;
    }
    // 设定修改后的数据
    n->setNew(p);
    Root->historyAddStep(n);
}

void NoteDialog::saveStartPoint(SimpleNote *p, double cur) {
    // 建立记录变量
    NoteOperation *n = new NoteOperation(stpChange);
    // 设定修改前的数据
    n->setOrg(p);
    // 将控制点改动保存到音符
    if (cur == NODEF_DOUBLE) {
        p->m_note.m_stp = blankStartPoint;
        p->m_note.stpStatus = MFM_OMITTED;
    } else {
        p->m_note.m_stp = cur;
        p->m_note.stpStatus = MFM_MODIFIED;
    }
    // 设定修改后的数据
    n->setNew(p);
    Root->historyAddStep(n);
}

void NoteDialog::saveFlags(SimpleNote *p, QString cur) {
    // 建立记录变量
    NoteOperation *n = new NoteOperation(flagsChange);
    // 设定修改前的数据
    n->setOrg(p);
    // 将控制点改动保存到音符
    if (cur.isEmpty()) {
        p->m_note.m_flags.clear();
        p->m_note.flagsStatus = MFM_OMITTED;
    } else {
        p->m_note.m_flags = cur;
        p->m_note.flagsStatus = MFM_MODIFIED;
    }
    // 设定修改后的数据
    n->setNew(p);
    Root->historyAddStep(n);
}

QString NoteDialog::convertDouble(double value, bool dif) {
    QString s;
    if (dif) {
        s = ambiguousValue;
    } else if (value == NODEF_DOUBLE) {
        s = "";
    } else {
        s = QString::number(value);
    }
    return s;
}
