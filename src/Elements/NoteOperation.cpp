#include "NoteOperation.h"
#include "SimpleCurves.h"
#include "SimpleNote.h"
#include "SimplePoint.h"
#include "mainwindow.h"

void AbstractNote::setValuesToNote(QUtauSectionNote *aNote) {
    if (!aNote) {
        return;
    }

    // 始终存在的条目

    // 转码
    QString lrc;
    if (Root->localeIndex == 1) {
        lrc = systemToJis(m_lyric);
    } else {
        lrc = m_lyric;
    }

    aNote->SetValue(KEY_NAME_LYRIC, lrc);
    aNote->SetValue(KEY_NAME_NOTE_NUM, m_noteNum);
    aNote->SetValue(KEY_NAME_LENGTH, m_length);

    // 音符属性 1
    if (preUttrStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_PRE_UTTERANCE, m_preUttr);
    } else {
        aNote->RemoveKey(KEY_NAME_PRE_UTTERANCE);
    }
    if (overlapStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_VOICE_OVERLAP, m_overlap);
    } else {
        aNote->RemoveKey(KEY_NAME_VOICE_OVERLAP);
    }
    if (stpStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_START_POINT, m_stp);
    } else {
        aNote->RemoveKey(KEY_NAME_START_POINT);
    }

    // 可以省略的条目
    if (intStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_INTENSITY, m_int);
    } else {
        aNote->RemoveKey(KEY_NAME_INTENSITY);
    }
    if (modStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_MODULATION, m_mod);
    } else {
        aNote->RemoveKey(KEY_NAME_MODULATION);
    }
    if (velStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_VELOCITY, m_vel);
    } else {
        aNote->RemoveKey(KEY_NAME_VELOCITY);
    }

    if (flagsStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_FLAGS, m_flags);
    } else {
        aNote->RemoveKey(KEY_NAME_FLAGS);
    }

    if (m_label != NODEF_STRING) {
        aNote->SetValue(KEY_NAME_LABEL, m_label);
    }
    if (m_direct != NODEF_STRING) {
        aNote->SetValue(KEY_NAME_DIRECT, m_direct);
    }
    if (m_patch != NODEF_STRING) {
        aNote->SetValue(KEY_NAME_PATCH, m_patch);
    }
    if (m_region != NODEF_STRING) {
        aNote->SetValue(KEY_NAME_REGION_START, m_region);
    }
    if (m_end != NODEF_STRING) {
        aNote->SetValue(KEY_NAME_REGION_END, m_end);
    }
    if (m_pbstart != NODEF_DOUBLE) {
        aNote->SetValue(KEY_NAME_PB_START, m_pbstart);
    }
    if (m_pbtype != NODEF_STRING) {
        aNote->SetValue(KEY_NAME_PB_TYPE, m_pbtype);
    }

    // 曲速
    if (tempoStatus != MFM_OMITTED) {
        aNote->SetValue(KEY_NAME_TEMPO, m_tempo);
    } else {
        aNote->RemoveKey(KEY_NAME_TEMPO);
    }

    // 颤音
    if (!m_vbr.isEmpty()) {
        aNote->SetValue(KEY_NAME_VBR, qvector_double_to_qstring(m_vbr));
    } else {
        aNote->RemoveKey(KEY_NAME_VBR);
    }

    // Mode2音高曲线
    if (!Mode2Pitch.isEmpty()) {
        aNote->SetCorrectedPortamento(Mode2Pitch);
    } else {
        aNote->RemoveKey(KEY_NAME_PBM);
        aNote->RemoveKey(KEY_NAME_PBS);
        aNote->RemoveKey(KEY_NAME_PBY);
        aNote->RemoveKey(KEY_NAME_PBW);
    }

    // Mode1音高曲线
    if (!m_pitches.isEmpty()) {
        aNote->SetValue(KEY_NAME_PITCH_BEND, qvector_double_to_qstring(m_pitches));
    } else {
        aNote->RemoveKey(KEY_NAME_PB_START);
        aNote->RemoveKey(KEY_NAME_PB_TYPE);
        aNote->RemoveKey(KEY_NAME_PITCH_BEND);
    }

    // 包络
    aNote->SetEnvelope(m_env);
}

void AbstractNote::setValuesFromNote(QUtauSectionNote *aNote) {
    if (!aNote) {
        return;
    }

    QString aStringTmp;
    QString lrc;

    // 始终存在的条目
    aNote->GetValue(KEY_NAME_NOTE_NUM, &m_noteNum);
    aNote->GetValue(KEY_NAME_LENGTH, &m_length);
    aNote->GetValue(KEY_NAME_LYRIC, &lrc);

    // 转码
    if (Root->localeIndex == 1) {
        m_lyric = JisToSystem(lrc);
    } else {
        m_lyric = lrc;
    }

    // 音符属性 1
    aNote->GetValue(KEY_NAME_PRE_UTTERANCE, &m_preUttr, &preUttrStatus); // 默认值从oto读取
    aNote->GetValue(KEY_NAME_VOICE_OVERLAP, &m_overlap, &overlapStatus); // 默认值从oto读取
    aNote->GetValue(KEY_NAME_START_POINT, &m_stp, &stpStatus);           // 默认值 0

    // 音符属性 2
    aNote->GetValue(KEY_NAME_INTENSITY, &m_int, &intStatus);  // 默认值 100
    aNote->GetValue(KEY_NAME_MODULATION, &m_mod, &modStatus); // 默认值 100
    aNote->GetValue(KEY_NAME_VELOCITY, &m_vel, &velStatus);   // 默认值 100

    // 曲速
    aNote->GetValue(KEY_NAME_TEMPO, &m_tempo, &tempoStatus); // 默认值受所有前面音符影响

    // 可以省略的条目
    aNote->GetValue(KEY_NAME_FLAGS, &m_flags, &flagsStatus);
    aNote->GetValue(KEY_NAME_LABEL, &m_label);
    aNote->GetValue(KEY_NAME_DIRECT, &m_direct);
    aNote->GetValue(KEY_NAME_PATCH, &m_patch);
    aNote->GetValue(KEY_NAME_REGION_START, &m_region);
    aNote->GetValue(KEY_NAME_REGION_END, &m_end);

    // Mode1音高曲线
    aNote->GetValue(KEY_NAME_PB_START, &m_pbstart);
    aNote->GetValue(KEY_NAME_PB_TYPE, &m_pbtype);

    aStringTmp = "";
    aNote->GetValue(KEY_NAME_PITCH_BEND, &aStringTmp);
    if (!aStringTmp.isEmpty()) {
        m_pitches = qstring_to_qvector_double(aStringTmp);
    }

    // Mode2音高曲线
    aNote->GetCorrectedPortamento(&Mode2Pitch);

    // 颤音
    aStringTmp = "";
    aNote->GetValue(KEY_NAME_VBR, &aStringTmp);
    if (!aStringTmp.isEmpty()) {
        m_vbr = qstring_to_qvector_double(aStringTmp);
    }

    // 音量包络
    if (!aNote->GetEnvelope(&m_env)) {
        m_env = defaultEnvelope();
    }

    // aNote->GetValue(KEY_NAME_ENVELOPE, &m_strEnvelope);
}

bool AbstractNote::updateGenons(QGenonSettings *g) {
    const QUtauSectionSettings *settings = MainWindow::aData.SectionSettings();
    QGenonSettings genon;

    // 转码
    bool aResult = settings->GenonSettingsFind(Lyric(), &genon, m_noteNum);

    if (g) {
        *g = genon;
    }

    // 如果不是默认状态，那么每次都要更新
    if (preUttrStatus == MFM_OMITTED) {
        m_preUttr = genon.mPreUtterance;
    }
    if (overlapStatus == MFM_OMITTED) {
        m_overlap = genon.mVoiceOverlap;
    }

    return aResult;
}

QString AbstractNote::Lyric() {
    QString lrc;
    if (Root->localeIndex == 1) {
        lrc = systemToJis(m_lyric);
    } else {
        lrc = m_lyric;
    }
    return lrc;
}

NoteOperation::NoteOperation(NoteChangeTypes t) {
    qDebug() << "Note Operation Added.";
    resetValues();
    type = t;
}

NoteOperation::~NoteOperation() {
}

void NoteOperation::release() {
    // 释放数组
    release(qstrs[0], nums[0], reals[0], points[0], count[0]);
    release(qstrs[1], nums[1], reals[1], points[1], count[1]);

    // 递归释放
    if (next) {
        next->release();
        next = nullptr;
    }
    delete this;
}

void NoteOperation::setOrg(SimpleNote *p) {
    pos[0] = p->getPosition();
    noteNum[0] = p->m_note.m_noteNum;
    setValues(p, qstrs[0], nums[0], reals[0], points[0], count[0]);
}

void NoteOperation::setNew(SimpleNote *p) {
    pos[1] = p->getPosition();
    noteNum[1] = p->m_note.m_noteNum;
    setValues(p, qstrs[1], nums[1], reals[1], points[1], count[1]);
}

bool NoteOperation::differ() {
    bool flag = false;

    switch (type) {

    case lrcChange:
        if (*qstrs[0] != *qstrs[1]) {
            flag = true;
            qDebug() << "org" << *qstrs[0] << "new" << *qstrs[1];
        }
        break;
    case flagsChange:
        if (*qstrs[0] != *qstrs[1]) {
            flag = true;
            qDebug() << "org" << *qstrs[0] << "new" << *qstrs[1];
        }
        break;
    case intChange:
    case modChange:
    case preChange:
    case oveChange:
    case stpChange:
    case bpmChange:
    case velChange:
        if (*reals[0] != *reals[1] || count[0] != count[1]) {
            flag = true;
        }
        break;
    case lenChange:
    case noteNumChange:
        if (*nums[0] != *nums[1]) {
            flag = true;
        }
        break;
    case mode2Change:
    case envChange:
        if (count[0] != count[1]) {
            flag = true;
        } else {
            for (int i = 0; i < count[0]; ++i) {
                if (points[0][i].mP != points[1][i].mP) {
                    flag = true;
                    break;
                } else if (Hypotenuse(points[0][i].mX, points[1][i].mX, points[0][i].mY,
                                      points[1][i].mY) > 0.5) {
                    flag = true;
                    break;
                }
            }
        }
        break;
    case vbrChange:
        if (count[0] != count[1]) {
            flag = true;
        } else {
            for (int i = 0; i < count[0]; ++i) {
                if (reals[0][i] != reals[1][i]) {
                    flag = true;
                    break;
                }
            }
        }
        break;
    default:
        flag = true;
        break;
    }

    return flag;
}

QString NoteOperation::description() {
    QString s;

    switch (type) {
    case lrcChange:
        s = QObject::tr("Modify lyric");
        break;

    case flagsChange:
        s = QObject::tr("Modify flags");
        break;

    case lenChange:
        s = QObject::tr("Modify length");
        break;

    case intChange:
        s = QObject::tr("Modify intensity");
        break;

    case modChange:
        s = QObject::tr("Modify modulation");
        break;

    case preChange:
        s = QObject::tr("Modify pre-utterance");
        break;

    case oveChange:
        s = QObject::tr("Modify overlap value");
        break;

    case velChange:
        s = QObject::tr("Modify consonants speed");
        break;

    case stpChange:
        s = QObject::tr("Modify start point");
        break;

    case mode2Change:
        s = QObject::tr("Modify portamento(Mode2)");
        break;

    case vbrChange:
        s = QObject::tr("Modify vibrato(Mode2)");
        break;

    case envChange:
        s = QObject::tr("Modify envelope");
        break;

    case bpmChange:
        s = QObject::tr("Modify tempo");
        break;

    case mode1Change:
        s = QObject::tr("Modify pitch curves(Mode1)");
        break;

    case labelChange:
        s = QObject::tr("Modify label");
        break;

    case directChange:
        s = QObject::tr("Modify $direct");
        break;

    case patchChange:
        s = QObject::tr("Modify $patch");
        break;

    case regChange:
        s = QObject::tr("Modify region");
        break;

    case regEndChange:
        s = QObject::tr("Modify region");
        break;

    case noteNumChange:
        s = QObject::tr("Modify note pitch");
        break;

    case notePosChange:
        s = QObject::tr("Modify position");
        break;

    case noteAddChange:
        s = QObject::tr("Add note");
        break;
    case noteSubChange:
        s = QObject::tr("Remove note");
        break;

    default:
        break;
    }

    return s;
}

int NoteOperation::undo() {
    NotesArea *w;
    SimpleNote *p;

    int code;

    // 先撤销后放入的
    if (next) {
        code = next->undo();
    } else {
        code = 1;
    }

    if (MainWindow::mainWidget && MainWindow::mainWidget->Interface()) {
        w = MainWindow::mainWidget->Interface();
    } else {
        return 0;
    }

    p = w->findNote(pos[1], noteNum[1]);

    if (!p) {
        qDebug() << "Find note failure.";
        return 0;
    } else {
        qDebug() << "Find note" << p << p->m_note.m_lyric;
    }

    execute(p, qstrs[0], nums[0], reals[0], points[0], count[0]);

    return code;
}

int NoteOperation::redo() {
    NotesArea *w;
    SimpleNote *p;

    int code;

    if (MainWindow::mainWidget && MainWindow::mainWidget->Interface()) {
        w = MainWindow::mainWidget->Interface();
    } else {
        return 0;
    }

    p = w->findNote(pos[0], noteNum[0]);

    if (!p) {
        return 0;
    }

    execute(p, qstrs[1], nums[1], reals[1], points[1], count[1]);

    // 后重做后放入的
    if (next) {
        code = next->redo();
    } else {
        code = 1;
    }

    return code;
}

void NoteOperation::setNext(NoteOperation *p) {
    if (next) {
        next->setNext(p);
    } else {
        next = p;
    }
}

/*--------------------------------------模板函数--------------------------------------*/

// 值覆盖
void NoteOperation::execute(SimpleNote *&p, QString *&qstr, int *&num, double *&real,
                            QCtrlPoint *&point, int &count) {

    switch (type) {
    case lrcChange:
        p->m_note.m_lyric = (*qstr);
        p->changeValues();
        break;

    case flagsChange:
        p->m_note.m_flags = *qstr;
        p->m_note.flagsStatus = count;
        p->changeValues();
        break;

    case lenChange:
        p->m_note.m_length = *num;
        p->changeLength();
        break;

    case intChange:
        p->m_note.m_int = *real;
        p->m_note.intStatus = count;
        p->changeValues();
        break;

    case modChange:
        p->m_note.m_mod = *real;
        p->m_note.modStatus = count;
        p->changeValues();
        break;

    case preChange:
        p->m_note.m_preUttr = *real;
        p->m_note.preUttrStatus = count;
        p->changeValues();
        break;

    case oveChange:
        p->m_note.m_overlap = *real;
        p->m_note.overlapStatus = count;
        p->changeValues();
        break;

    case velChange:
        p->m_note.m_vel = *real;
        p->m_note.velStatus = count;
        p->changeValues();
        break;

    case stpChange:
        p->m_note.m_stp = *real;
        p->m_note.stpStatus = count;
        p->changeValues();
        break;

    case mode2Change:
        p->m_note.Mode2Pitch.clear();
        for (int i = 0; i < count; ++i) {
            p->m_note.Mode2Pitch.push_back(point[i]);
        }
        p->m_note.mode2Sort(); // Mode2 排序
        p->initializeNote(true);
        break;

    case vbrChange:
        p->m_note.m_vbr.clear();
        for (int i = 0; i < count; ++i) {
            p->m_note.m_vbr.push_back(real[i]);
        }
        p->changeVibrato();
        break;

    case envChange:
        p->m_note.m_env.clear();
        for (int i = 0; i < count; ++i) {
            p->m_note.m_env.push_back(point[i]);
        }
        p->m_note.envSort();
        p->changeEnvelope();
        break;

    case bpmChange:
        p->m_note.m_tempo = *real;
        p->m_note.tempoStatus = count;
        p->initializeNote(false);

        p->changeTempo();
        break;

    case mode1Change:
        p->m_note.m_pbtype = *qstr;
        p->m_note.m_pbstart = real[0];
        for (int i = 0; i < count; ++i) {
            p->m_note.m_pitches.push_back(real[i + 1]);
        }
        break;

    case labelChange:
        p->m_note.m_label = *qstr;
        p->changeValues();
        break;

    case directChange:
        p->m_note.m_direct = *qstr;
        p->changeValues();
        break;

    case patchChange:
        p->m_note.m_patch = *qstr;
        p->changeValues();
        break;

    case regChange:
        p->m_note.m_region = *qstr;
        p->changeValues();
        break;

    case regEndChange:
        p->m_note.m_end = *qstr;
        p->changeValues();
        break;

    case noteNumChange:
        p->m_note.m_noteNum = *num;
        p->initializeNote(false);
        break;

    case notePosChange:
        p->setPosition(*num);
        p->initializeNote(false);
        break;

    /*case noteAddChange:
        if (plus) {
            w->addNote(*note);
        } else {
            w->removeNote(p);
        }
        break;
    case noteSubChange:
        if (!plus) {
            w->addNote(*note);
        } else {
            w->removeNote(p);
        }
        break;*/
    default:
        break;
    }
}

// 初始化
void NoteOperation::setValues(SimpleNote *&p, QString *&qstr, int *&num, double *&real,
                              QCtrlPoint *&point, int &count) {
    switch (type) {
    case lrcChange:
        qstr = new QString(p->m_note.m_lyric);
        break;

    case flagsChange:
        qstr = new QString(p->m_note.m_flags);
        count = p->m_note.flagsStatus;
        break;

    case lenChange:
        num = new int(p->m_note.m_length);
        break;

    case intChange:
        real = new double(p->m_note.m_int);
        count = p->m_note.intStatus;
        break;

    case modChange:
        real = new double(p->m_note.m_mod);
        count = p->m_note.modStatus;
        break;

    case preChange:
        real = new double(p->m_note.m_preUttr);
        count = p->m_note.preUttrStatus;
        break;

    case oveChange:
        real = new double(p->m_note.m_overlap);
        count = p->m_note.overlapStatus;
        break;

    case velChange:
        real = new double(p->m_note.m_vel);
        count = p->m_note.velStatus;
        break;

    case stpChange:
        real = new double(p->m_note.m_stp);
        count = p->m_note.stpStatus;
        break;

    case mode2Change:
        count = p->m_note.Mode2Pitch.size();
        point = new QCtrlPoint[count];
        for (int i = 0; i < count; ++i) {
            point[i] = p->m_note.Mode2Pitch[i];
        }
        break;

    case vbrChange:
        count = p->m_note.m_vbr.size();
        real = new double[count];
        for (int i = 0; i < count; ++i) {
            real[i] = p->m_note.m_vbr[i];
        }
        break;

    case envChange:
        count = p->m_note.m_env.size();
        point = new QCtrlPoint[count];
        for (int i = 0; i < count; ++i) {
            point[i] = p->m_note.m_env[i];
        }
        break;

    case bpmChange:
        real = new double(p->m_note.m_tempo);
        count = p->m_note.tempoStatus;
        break;

    case mode1Change:
        qstr = new QString(p->m_note.m_pbtype);

        count = p->m_note.m_pitches.size() + 1;

        real = new double[count];
        real[0] = p->m_note.m_pbstart;

        for (int i = 0; i < count; ++i) {
            real[i + 1] = p->m_note.m_pitches[i];
        }
        break;

    case labelChange:
        qstr = new QString(p->m_note.m_label);
        break;

    case directChange:
        qstr = new QString(p->m_note.m_direct);
        break;

    case patchChange:
        qstr = new QString(p->m_note.m_patch);
        break;

    case regChange:
        qstr = new QString(p->m_note.m_region);
        break;

    case regEndChange:
        qstr = new QString(p->m_note.m_end);
        break;

    case noteNumChange:
        num = new int(p->m_note.m_noteNum);
        break;

    case notePosChange:
        num = new int(p->getPosition());
        break;

    case noteAddChange:
    case noteSubChange:
        note = new AbstractNote(p->m_note);
        break;

    default:
        break;
    }
}


// 删除
void NoteOperation::release(QString *&qstr, int *&num, double *&real, QCtrlPoint *&point,
                            int &count) {

    qDebug() << "Note Operation Released with size : " << count;

    switch (type) {
    case lrcChange:
    case flagsChange:
    case labelChange:
    case directChange:
    case patchChange:
    case regChange:
    case regEndChange:
        delete qstr;
        qstr = nullptr;
        break;

    case intChange:
    case modChange:
    case preChange:
    case oveChange:
    case velChange:
    case stpChange:
    case bpmChange:
        delete real;
        real = nullptr;
        break;

    case mode2Change:
        delete[] point;
        point = nullptr;
        break;

    case vbrChange:
        delete[] real;
        real = nullptr;
        break;

    case envChange:
        delete[] num;
        num = nullptr;
        break;

    case mode1Change:
        delete qstr;
        delete[] real;
        qstr = nullptr;
        real = nullptr;
        break;

    case lenChange:
    case noteNumChange:
    case notePosChange:
        delete num;
        num = nullptr;
        break;

    case noteAddChange:
    case noteSubChange:
        delete note;
        note = nullptr;
        break;

    default:
        break;
    }
}
