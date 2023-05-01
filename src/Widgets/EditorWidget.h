#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QDockWidget>
#include <QEnterEvent>
#include <QLabel>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyle>
#include <QSvgRenderer>
#include <QWidget>

#include "Elements/CustomControls.h"
#include "Elements/SimpleForm.h"
#include "ListArea.h"
#include "NotesArea.h"
#include "PianoArea.h"
#include "QUtils.h"
#include "ScaleplateArea.h"
#include "VolumeArea.h"

class EditorArea;
class EditorWidget;

/*--------------------------------------下面板中心区域--------------------------------------*/
class EditorOuterArea : public EditorArea {
    Q_OBJECT
public:
    explicit EditorOuterArea(QWidget *parent = nullptr);

    PianoArea *Piano() {
        return piano;
    }

    ScaleplateArea *Scaleplate() {
        return scaleplate;
    }

    NotesArea *MainArea() {
        return mainArea;
    }

    NotesScrollArea *MainScroll() {
        return mainScroll;
    }

    VolumeForm *VolumePart() {
        return volumeArea;
    }

    VolumeArea *Volume(){
        return volumeArea->getWidget()->Volume();
    }

    void linkZoomInHorizontally();
    void linkZoomOutHorizontally();
    void linkZoomInVertically();
    void linkZoomOutVertically();
    void linkMoveLeft();
    void linkMoveRight();
    void linkMoveUp();
    void linkMoveDown();

    void checkShowOnStage(QWidget *w, bool right = false);

    friend class EditorWidget;
    void reInitializeStrings();

    bool getShowFunctions() const;
    void setShowFunctions(bool value);

private:
    void paintEvent(QPaintEvent *) override;
    void wheelEvent(QWheelEvent *event) override;

    void posLinkChange();
    void sizeLinkChange();
    void sumLinkChange(int n);
    void cropLinkChange();
    void cropTimerEvent();
    void canvasLinkEvent(QPoint p = QPoint(0, 0));

    void playheadLinkChange();

    void updateSingleStep();

    void adjustHorizontalView(); // 调整标尺尺寸

    QGridLayout *mainLayout;

    PianoArea *piano;
    NoneScrollArea *pianoScroll;

    ScaleplateArea *scaleplate;
    NoneScrollArea *scaleplateScroll;

    NotesArea *mainArea;
    NotesScrollArea *mainScroll;

    ListArea *functions;
    BasicScrollArea *funcScroll;

    VolumeForm *volumeArea;
    FormSplitter *mainSplitter;

    QWidget *emptyRow;
    QWidget *emptyRect;
    QWidget *emptyRect2;

    QTimer *barTimer;

    bool showFunctions;

signals:
    void notesAreaMoved();
};

/*--------------------------------------下面板整体--------------------------------------*/
class EditorWidget : public SimpleForm {
    Q_OBJECT
public:
    explicit EditorWidget(QWidget *parent = nullptr);

    EditorOuterArea *getArea() {
        return editor;
    }

    void updateButtons();

    void onPlayBtnClick();
    void onStopBtnClick();
    void onMoveToStartBtnClick();
    void onMoveToEndBtnClick();

    void reInitializeStrings();

private:
    EditorOuterArea *editor;

    // 添加在标题栏的按钮
    VectorDblButton *btnLine, *btnNote;
    VectorDblButton *btnPlay, *btnStop, *btnMoveToStart, *btnMoveToEnd;

    VectorDblButton *btnMode1, *btnMode2, *btnOverlap, *btnReflect, *btnFlags;
    VectorDblButton *btnLocaleMenu;

    // 右侧菜单
    QMenu *m_menu;
    QAction *systemLocaleAction;
    QAction *shiftJISLocaleAction;
    QActionGroup *localeActionGroup;

    void onLineBtnClick();
    void onNoteBtnClick();

    void onMode1BtnClick();
    void onMode2BtnClick();
    void onOverlapBtnClick();
    void onReflectBtnClick();
    void onFlagsBtnClick();

    void onLocaleMenuBtnClick();

    void initLocaleMenu();

    void onSystemLocaleTriggered();
    void onShiftJISLocaleTriggered();

signals:
};

#endif // EDITORWIDGET_H
