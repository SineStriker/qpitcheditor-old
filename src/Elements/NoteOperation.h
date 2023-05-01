#ifndef NOTEOPERATION_H
#define NOTEOPERATION_H

#include <QObject>
#include <QWidget>

#include "LibUTAUQt/QUtauData.h"
#include "QUtils.h"

class SimpleNote;

/*--------------------------------------保存一个独立音符的信息--------------------------------------*/
struct AbstractNote {
    QString m_lyric, m_flags; // 歌词，Flags

    int m_noteNum; // 音高
    int m_length;  // 音长

    double m_int, m_mod, m_preUttr, m_overlap, m_vel, m_stp;
    // 音量，移调，先行声音，重叠，辅音速度，StartPoint

    int flagsStatus, intStatus, modStatus, preUttrStatus, overlapStatus, velStatus, stpStatus,
        tempoStatus;

    QVector<double> m_vbr; // 颤音

    QVector<QCtrlPoint> m_env; // 音量包络
    // QString m_strEnvelope;
    double m_tempo; // 曲速

    double m_pbstart;          // Mode1音符起始
    QVector<double> m_pitches; // Mode1音高采样点坐标
    QString m_pbtype;          // Mode1曲线类型

    QString m_label, m_direct, m_patch, m_region, m_end;
    // 标签，直接输出，wav直接输出，区域开始，区域结束

    QVector<QCtrlPoint> Mode2Pitch;

    AbstractNote() {
        resetValues();
    }

    void resetValues() {

        m_lyric = "a";
        m_noteNum = 60;
        m_length = 480;

        m_mod = m_preUttr = m_overlap = m_stp = 0.0;
        m_int = m_vel = 100.0;

        m_flags = "";

        m_vbr = {};
        m_env = {};
        // m_strEnvelope = "";

        m_tempo = 120.0;

        m_pbstart = NODEF_DOUBLE;
        m_pitches = {};
        m_pbtype = NODEF_STRING;

        m_label = m_direct = m_patch = m_region = m_end = NODEF_STRING;

        Mode2Pitch = {};

        // 标记preUttr是否为音源库里读到的
        preUttrStatus = MFM_OMITTED;
        overlapStatus = MFM_OMITTED;
        stpStatus = MFM_OMITTED;
        flagsStatus = MFM_OMITTED;
        intStatus = MFM_OMITTED;
        modStatus = MFM_OMITTED;
        velStatus = MFM_OMITTED;
        tempoStatus = MFM_OMITTED;
    }

    void setValuesToNote(QUtauSectionNote *aNote);
    void setValuesFromNote(QUtauSectionNote *aNote);

    bool updateGenons(QGenonSettings *g = nullptr);

    QString Lyric();

    // 假的赋值重载
    AbstractNote &operator=(const AbstractNote &note2) {
        AbstractNote *a = new AbstractNote(note2);
        return *a;
    }

    void mode2Sort() {
        arrSort(Mode2Pitch);
    }

    void envSort() {
        return;
        arrSort(m_env);
    }

    static void arrSort(QVector<QCtrlPoint> &array) {
        int i, j;
        QCtrlPoint key;

        for (j = 1; j < array.size(); ++j) {
            key = array[j];
            i = j - 1;
            while (i >= 0 && array[i].mX > key.mX) {
                array[i + 1] = array[i];
                i = i - 1;
            }
            array[i + 1] = key;
        }
    }

    static QVector<QCtrlPoint> defaultEnvelope() {
        QCtrlPoint p1(0, 0);
        QCtrlPoint p2(5, 100);
        QCtrlPoint p3(35, 100);
        QCtrlPoint p4(0, 0);

        return {p1, p2, p3, p4};
    }
};

enum NoteChangeTypes {

    unknownChange, // ？

    lrcChange,    // 歌词改变
    flagsChange,  // Flags改变
    lenChange,    // 长度改变
    intChange,    // 音量改变
    modChange,    // 移调改变
    preChange,    // 先行声音改变
    oveChange,    // 重叠改变
    velChange,    // 辅音速度改变
    stpChange,    // STP改变
    mode2Change,  // Mode2音高曲线改变
    vbrChange,    // 颤音改变
    envChange,    // 音量包络改变
    bpmChange,    // 曲速改变
    mode1Change,  // Mode1音高曲线改变
    labelChange,  // 标签改变
    directChange, // $direct改变
    patchChange,  // $patch改变
    regChange,    // 区域改变
    regEndChange, // 区域尾部改变

    noteNumChange, // 音高改变
    notePosChange, // 音符移动
    noteSubChange, // 音符删除
    noteAddChange  // 音符增加
};

/*--------------------------------------保存一次操作信息--------------------------------------*/

// 占用空间越小越好
class NoteOperation {
public:
    explicit NoteOperation(NoteChangeTypes t = unknownChange);
    ~NoteOperation();

    QString description(); // 步骤描述
    void release();        // 释放步骤占用的存储空间

    void setOrg(SimpleNote *p);
    void setNew(SimpleNote *p);
    bool differ(); // 判断改动是否有效

    int undo();
    int redo();

    NoteOperation *getNext() {
        return next;
    }

    void setNext(NoteOperation *p);

    void resetValues() {
        type = unknownChange;
        next = nullptr;

        pos[0] = pos[1] = 0;
        noteNum[0] = noteNum[1] = 0;

        count[0] = count[1] = 0;

        qstrs[0] = qstrs[1] = nullptr;
        reals[0] = reals[1] = nullptr;
        nums[0] = nums[1] = nullptr;

        points[0] = points[1] = nullptr;
        note = nullptr;
    }

    NoteChangeTypes getType() {
        return type;
    }

private:
    NoteChangeTypes type; // 类型
    NoteOperation *next;  // 序号

    int pos[2], noteNum[2];

    int count[2];      // 保存操作对象的个数，或者保存对象的状态
    QString *qstrs[2]; // 字符串类型
    double *reals[2];  // 实数类型
    int *nums[2];      // 整数类型

    QCtrlPoint *points[2]; // 控制点类型

    AbstractNote *note; // 需要时使用以节省空间

    void setValues(SimpleNote *&p, QString *&qstr, int *&num, double *&real, QCtrlPoint *&point,
                   int &count);
    void execute(SimpleNote *&p, QString *&qstr, int *&num, double *&real, QCtrlPoint *&point,
                 int &count);
    void release(QString *&qstr, int *&num, double *&real, QCtrlPoint *&point, int &count);
};

#endif // NOTEOPERATION_H
