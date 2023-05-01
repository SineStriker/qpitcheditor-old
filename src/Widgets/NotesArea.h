#ifndef NOTESAREA_H
#define NOTESAREA_H

#include <QPainter>
#include <QWidget>

#include "BaseWidgets.h"
#include "Elements/CustomControls.h"
#include "LibUTAUQt/QUtauData.h"
#include "QUtils.h"

/*--------------------------------------编辑区域--------------------------------------*/
class NotesArea : public EditorAreaWidget {
    Q_OBJECT
public:
    explicit NotesArea(QWidget *parent = nullptr);

    // 调整音符
    void zoomResize(int w, int h, bool Horizontal = true);
    void adjustNotes();
    void adjustEnvelopes();

    void setNotesMovable(bool value);
    void updateAllNotes();

    // 初始化音符
    void addNote(QUtauSectionNote *aNote, int pos, double tempo);
    void initializeNotes();
    void feedbackNotes();

    // 单个处理音符
    void removeNote(SimpleNote *pNote);
    void addNote(AbstractNote bNote);
    SimpleNote *findNote(int pos, int noteNum);

    QWidget *getNoteSegLayer() override {
        return noteTopLayer;
    }

    QWidget *getCurvesSegLayer() override {
        return curvesTopLayer;
    }

    QWidget *getPointSegLayer() override {
        return pointTopLayer;
    }

    QWidget *getVibratoSegLayer() override {
        return vibratoTopLayer;
    }

    PlayHead *getPlayhead() {
        return m_playhead;
    }

    // 框选区域
    bool isCropActive();
    bool isMoving();
    bool isLyricEditing();

    void updateCropArea();
    void removeCropArea();
    void cropSelect();

    // 选择音符
    void selectAllNotes();
    QPoint selectContinuousNotes();

    SimpleNote *getNoteAt(int i);

    void addNoteFromCursor(QPoint cursor); // 从鼠标指针位置确定应该添加曲线的音符

    void updateMode(); // 按钮状态改变

    void onPositionChanged(qint64 position = -1); // 播放头

    // 添加控制点或
    void addFromCursorEvent(QMouseEvent *event);

    // 编辑歌词
    void editNoteLyric(SimpleNote *pNote);
    void onLyricSwitch();
    void onLyricAbandon();

    static NotesArea *Stage;
    static NotesScrollArea *Scroll;

    friend class ContinuousCurves;
    friend class MainWindow;

private:
    void paintEvent(QPaintEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    QPoint limitArea(QPoint origin);

    // 歌词处理
    void onEditLyricFinished();
    void editNextLyric();

    QVector<SimpleNote *> notesList;
    QWidget *noteTopLayer, *curvesTopLayer, *pointTopLayer, *vibratoTopLayer; // 空图层

    CropView *m_view; // 框选对象
    bool m_move;

    PlayHead *m_playhead;
    int m_playToNote;

    ContinuousCurves *m_mode1;

    FixedLineEdit *m_lyricEdit;
    SimpleNote *m_curEditNote;

    QAction *tabLyricAction;
    QAction *escLyricAction;

    double globalTempo;

    QPoint m_startPoint;

    bool isMousePressed;

signals:
    void onMoveEvent();
    void onSumChanged(int n);
    void onCropDrawing();
    void onCanvasMoving(QPoint p = QPoint(0, 0));
    void onPlayheadMoving();
    void onCheckOnStage(QWidget *w, bool right = false);

public slots:
};

#endif // NOTESAREA_H
