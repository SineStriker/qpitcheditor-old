#ifndef SIMPLENOTE_H
#define SIMPLENOTE_H

#include <QMenu>
#include <QMouseEvent>
#include <QPainterPath>
#include <QWidget>

#include "LibUTAUQt/QUtauSection.h"
#include "NoteOperation.h"
#include "QUtils.h"

class NotesArea;

class SimpleObject;

class SimpleNote;
class SimpleCurves;
class SimplePoint;
class SimpleVibrato;
class SimpleEnvelope;

// 音符状态
enum NoteStatus { NoteUnlistStatus, NoteListStatus, NoteRestStatus };

class SimpleObject : public QWidget {
    Q_OBJECT
public:
    explicit SimpleObject(QWidget *parent) : QWidget(parent) {
        setFocusPolicy(Qt::ClickFocus);
    }
    virtual ~SimpleObject() {
    }
    virtual void linkPressEvent(QMouseEvent *event) {
    }
    virtual void linkMoveEvent(QMouseEvent *event) {
    }
    virtual void linkReleaseEvent(QMouseEvent *event) {
    }
    virtual void autoReleaseEvent(QMouseEvent *event, bool flag) {
    }
    virtual void setSelected(bool b) {
    }

protected:
    virtual QPoint limitArea(QPoint origin) {
        return QPoint();
    }
};

/*--------------------------------------音符元件--------------------------------------*/
class SimpleNote : public SimpleObject {
    Q_OBJECT
public:
    explicit SimpleNote(NotesArea *parent = NULL, int pos = 0);
    explicit SimpleNote(QString lrc = "", NotesArea *parent = NULL, int pos = 0);
    explicit SimpleNote(QUtauSectionNote *aNote, NotesArea *parent = NULL, int pos = 0);

    ~SimpleNote();

    void adjustLayer();

    void initCurves();
    void initEnvelope();

    void resetValues();
    void adjustNoteSize();

    void saveNoteNumFromPosition();
    void saveLyric(QString lyric);

    void updateNoteStatus(bool detectVoice = true);

    void updateCurves();
    void updateEnvelopes(bool rearrange = false);

    // 改动音符
    void initializeNote(bool resetCurves = false);
    void changeValues();   // 隐形更改值
    void changeLength();   // 涉及音长改变
    void changeVibrato();  // 涉及颤音改变
    void changeEnvelope(); // 涉及包络改变
    void changeTempo();

    QPoint getStaticPos();
    int getStaticX();
    int getStaticY();

    void setPosition(int n) {
        pos = n;
    }

    int getPosition() {
        return pos;
    }

    double getDuration() {
        double unit = 120.0 / m_note.m_tempo / 0.96;
        return m_note.m_length * unit;
    }

    double getInPoint();
    double getOutPoint();
    double getInOverlapPoint();
    double getOutOverlapPoint();
    double getPBStart();

    int convertTimeToTick(double time);
    double convertTickToTime(double tick);

    double getTime() {
        if (startTime == 0) {
            if (m_prev != nullptr) {
                startTime = m_prev->getTime() + m_prev->getDuration();
            } else {
                startTime = 0;
            }
        }
        return startTime;
    }

    bool isRest() {
        return m_status == NoteRestStatus;
    }

    bool isListed() {
        return m_status == NoteListStatus;
    }

    bool isUnisted() {
        return m_status == NoteUnlistStatus;
    }

    bool isSelected() {
        return m_selected;
    }
    bool isMovable() {
        return m_movable;
    }

    void setSelected(bool b) override;
    void setMovable(bool b);

    void setTempo(double value) {
        m_note.m_tempo = value;
        update();
    }

    // 先行声音
    double getPreUtterance() {
        if (!isListed()) {
            return 0;
        }
        return m_note.m_preUttr;
    }
    int getPreStatus() {
        return m_note.preUttrStatus;
    }

    // 重叠
    double getVoiceOverlap() {
        if (!isListed()) {
            return 0;
        }
        return m_note.m_overlap;
    }
    int getOveStatus() {
        return m_note.overlapStatus;
    }

    // 曲速
    double getTempo() {
        return m_note.m_tempo;
    }
    int getTempoStatus() {
        return m_note.tempoStatus;
    }

    // 音量
    double getIntensity() {
        return m_note.m_int;
    }
    int getIntStatus() {
        return m_note.intStatus;
    }

    // 移调
    double getModulation() {
        return m_note.m_mod;
    }
    int getModStatus() {
        return m_note.modStatus;
    }

    // Flags
    QString getFlags() {
        return m_note.m_flags;
    }

    SimpleCurves *getMode2Pitch() {
        return m_pitch;
    }

    double getCorrectDuration();
    CorrectGenon getCorrectGenon();

    void linkPressEvent(QMouseEvent *event) override;
    void linkMoveEvent(QMouseEvent *event) override;
    void linkReleaseEvent(QMouseEvent *event) override;
    void autoReleaseEvent(QMouseEvent *event, bool flag = false) override;

public:
    AbstractNote m_note; // 保存音符信息

    SimpleNote *m_prev, *m_next; // 保存前驱与后继
    SimpleCurves *m_pitch;       // 保存属于自己的曲线区域

    SimpleEnvelope *m_envelope;

private:
    NotesArea *m_parent;

    QUtauSectionNote *utauNote;

    int pos;
    double startTime;

    // double tempo;
    // double preUtterance, voiceOverlap;
    // double modulation, intensity;
    // QString flags;

    bool rollOver;
    bool m_movable;

    NoteStatus m_status;

    bool m_move;          // 是否有拖动趋势
    bool m_moving;        // 是否在拖动
    bool m_selected;      // 是否选中
    QPoint m_startPoint;  // 拖动时保存本体起始坐标
    QPoint m_windowPoint; // 拖动时保存鼠标起始坐标

    QMenu *m_menu;

    QAction *portamentoAction;
    QAction *vibratoAction;
    QAction *vbrEditAction;
    QAction *propertyAction;

    QPoint limitArea(QPoint origin) override;

    void initRightMenu();

    void addVibrato(bool showEditor = false);
    void removeVibrato();

private:
    void paintEvent(QPaintEvent *event) override;

    void enterEvent(QEvent *event) override {
        rollOver = true;
        update();
    }
    void leaveEvent(QEvent *event) override {
        rollOver = false;
        update();
    }

    void MyMouseEvent(QMouseEvent *e);

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void triggerAddPitch();
    void triggerAddVibrato();
    void triggerVBREditor();
    void triggerShowProperty();
};

#endif // SIMPLENOTE_H
