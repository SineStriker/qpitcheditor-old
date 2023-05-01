#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QLayout>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QResizeEvent>
#include <QSound>
#include <QSoundEffect>
#include <QTextStream>
#include <QTranslator>

#include "Dialogs/BackstageDialog.h"
#include "Dialogs/InitDialog.h"
#include "Dialogs/NoteDialog.h"
#include "Dialogs/OptionsDialog.h"
#include "Dialogs/RenderDialog.h"
#include "Dialogs/StylizedDialog.h"
#include "Elements/SimplePoint.h"
#include "QUtils.h"
#include "Threads/LoadingThread.h"
#include "UtaUtils/UtaTranslator.h"
#include "Widgets/CentralWidget.h"
#include "Widgets/PianoArea.h"

#include "LibUTAUQt/QUtauData.h"
#include "Threads/OnlineUpdater.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    // 全局变量
    static QString CurrentVersion;

    static QUtauData aData;
    static QString aTempFile;

    static QString aVoiceDir, aProjectPath, aTool1Path, aTool2Path, aGlobalFlags;

    static int noteUnitWIndex, noteUnitHIndex; // 缩放比例索引

    // 编辑区域滚动条辅助按键
    static Qt::KeyboardModifiers zoomHorizontallyKey;
    static Qt::KeyboardModifiers zoomVerticallyKey;
    static Qt::KeyboardModifiers moveHorizontallyKey;
    static Qt::KeyboardModifiers moveVerticallyKey;

    // 编辑区域拖动对象
    static QSet<SimpleObject *> stageDraggedObjects;

    static QSet<SimplePoint *> stageSelectedPoints;
    static QSet<SimpleNote *> stageSelectedNotes;

    static QVector<NoteOperation *> historyList;
    static int historyIndex;
    static int historySequence;

    static QTabWidget *mainTab;
    static CentralWidget *mainWidget;

    static int debugConstant;

    static bool isLoaded;  // 是否加载成功
    static bool isChanged; // 音符信息是否有改变

    static QSound *sound;
    static QMediaPlayer *player;

    static QVector<QStringList> resamplerArguments;
    static QVector<QStringList> wavtoolArguments;

    static QString renderingDir;
    static QString tempAudioName;
    static QString cacheFolderName;

    static QPoint playingRange;

public:
    static bool bLineMode, bNoteMode, bPlayMode;
    static bool bShowMode1, bShowMode2, bOverlap, bReflect, bShowFlags;
    static bool bShowEnvLines, bShowEnvFill;

    static int snappingUnit;  // 最小长度单位
    static int languageIndex; // 语言
    static int localeIndex;   // 编码

    static int renderMode;  // 渲染模式
    static int stageVolume; // 舞台音量

    static bool doubleClickToAdd; // 双击加点

    static QVector<QString> languageFiles;

public:
    static QColor note_lyricNameColor;
    static QColor note_listed_staticColor;
    static QColor note_listed_selectedColor;
    static QColor note_rest_staticColor;
    static QColor note_rest_selectedColor;
    static QColor note_unlisted_staticColor;
    static QColor note_unlisted_selectedColor;

    static QColor pitchPoint_coreColor;
    static QColor pitchPoint_ringColor;

    static QColor volumePoint_coreColor;
    static QColor volumePoint_ringColor;

    static QColor mode1_solidLineColor_enabled;
    static QColor mode2_solidLineColor_enabled;
    static QColor mode1_solidLineColor_disabled;
    static QColor mode2_solidLineColor_disabled;
    static QColor mode1_dashLineColor_enabled;
    static QColor mode2_dashLineColor_enabled;
    static QColor mode1_dashLineColor_disabled;
    static QColor mode2_dashLineColor_disabled;

    static QColor mode2_invalidLineColor_enabled;
    static QColor mode2_invalidLineColor_disabled;

    static QColor vibrato_buttonBorderColor;
    static QColor vibrato_buttonFillColor;
    static QColor vibrato_frameColor;

    static QColor overlap_lineColor;
    static QColor preUttr_lineColor;
    static QColor reflect_fillColor;

    static QColor parameters_textColor;

    static QColor editor_timeLineColor;
    static QColor editor_sectionLineColor;
    static QColor editor_pitchLineColor;
    static QColor editor_levelLineColor;
    static QColor editor_backLowColor;
    static QColor editor_backHighColor;

    static QColor envelope_solidLineColor_enabled;
    static QColor envelope_solidLineColor_disabled;
    static QColor envelope_dashLineColor_enabled;
    static QColor envelope_dashLineColor_disabled;
    static QColor envelope_fillColor_enabled;
    static QColor envelope_fillColor_disabled;

    static QColor volume_backColor;
    static QColor volume_timeLineColor;
    static QColor volume_sectionLineColor;
    static QColor volume_standardLineColor;

public:
    void setLineMode(bool value);
    void setNoteMode(bool value);

    void setPlayMode(bool value);

    void setMode1Status(bool value);
    void setMode2Status(bool value);
    void setOverlapStatus(bool value);
    void setReflectStatus(bool value);
    void setFlagsStatus(bool value);

    void setEnvLinesStatus(bool value);
    void setEnvFillStatus(bool value);

    void setLengthUnit(int value);

    // 设置程序语言
    void applyLanguage();

private:
    void initPath();
    bool getConfig();
    void getProjectUst();
    void getSettingIni();

    // 第一次加载窗口
    void setFirstWindow();
    void exitWindow();

    // 用于保存一些设定
    void detectLanguage();

    void readSetting();
    void saveSetting();
    void applySetting();

    // 关于初始尺寸
    void setInitGeometry(const QRect &rect);

    // 关于吸附单位
    void modifyAdsorbOptions(int n);
    void cancelAllAdsorbActions();

public:
    // 关于语言
    void setLanguage(const int n);
    void setCodeLocale(const int n);

private:
    // Ui::MainWindow *ui;

    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

public:
    // 静态公共函数
    static void getCharacter(QString &name, QString &image);

    static QString fullWindowTitle();

public:
    // 公共函数
    void loadTempFile(QString &filename); // 主窗口唯一的入口

    void setChanged(bool value);

    void initEditor();

    void reInitializeStrings();

    // 播放相关
    int renderAudio();
    int playAudio();
    int pauseAudio();
    int stopAudio();
    int replayAudio();

    void updatePlayButtons();

    void onSoundStateChanged(QMediaPlayer::State newState);
    void onPlayerStatusChanged(QMediaPlayer::MediaStatus status);

    int playWavFile(QString fileName);

    int RENDER_InitializeProcess();

    // 其他函数
    void showOptions();
    void showNoteProperties();

    int msgboxInfo(QString content, QString title = "");
    int msgboxExclaim(QString content, QString title = "");
    int msgboxError(QString content, QString title = "");
    int msgboxInfoQuestion(QString content, QString title = "", int def = QMessageBox::Yes);

    void checkUpdate(bool showLatest = false);

    // 处理拖动
    void addDraggedObject(SimpleObject *w);
    void removeDraggedObject(SimpleObject *w);
    void removeAllDraggedObjects();

    // 处理选中
    void autoAddSelectedNote(SimpleNote *w);
    void addSelectedNotes(SimpleNote *w);
    void removeSelectedNotes(SimpleNote *w);
    void removeAllSelectedNotes();

    void autoAddSelectedPoint(SimplePoint *w);
    void addSelectedPoints(SimplePoint *w);
    void removeSelectedPoints(SimplePoint *w);
    void removeAllSelectedPoints();

    // 处理历史记录
    void historyAddStep(NoteOperation *n);
    void historyNextStep(bool execute = true);
    void historyPrevStep(bool execute = true);
    bool isLatestStep();
    bool isEarliestStep();

    // 菜单栏更新
    void updateHistoryActions();
    void updateDeleteAction();
    void updateDeselectAction();

private:
    // 菜单项
    static QMenu *fileMenu, *editMenu, *viewMenu, *toolMenu, *helpMenu;
    static QAction *exitFileAction, *saveFileAction, *restoreFileAction, *exportFileAction,
        *exportWaveAction;
    static QAction *undoEditAction, *redoEditAction, *selectAllEditAction, *deselectEditAction,
        *deleteEditAction;
    static QAction *zoomInVerticallyAction, *zoomOutVerticallyAction, *zoomInHorizontallyAction,
        *zoomOutHorizontallyAction, *showPitchAction, *showParaAction;
    static QAction *otherToolAction, *noteToolAction, *settingToolAction;
    static QAction *instructHelpAction, *checkUpdateAction, *aboutThisAction, *aboutQtAction;

    // 子菜单项
    static QMenu *adsorbMidAction;
    static QActionGroup *adsorbActionGroup;
    static QAction *fullAdsorbAction, *halfAdsorbAction, *quarterAdsorbAction, *sixthAdsorbAction,
        *eighthAdsorbAction, *twelfthAdsorbAction, *sixteenthAdsorbAction, *twentyForthAdsorbAction,
        *thirtySecondAdsorbAction, *noAdsorbAction;

    static QMenu *beatMidAction;
    static QActionGroup *beatActionGroup;
    static QAction *fourInFourBeatAction, *threeInFourBeatAction;

    static QMenu *previewMidAction;
    static QAction *playPreviewAction, *replayPreviewAction, *stopPreviewAction, *removeCacheAction,
        *moveToStartPreviewAction, *moveToEndPreviewAction;

    static QTranslator *mainTranslator;

public:
    static QColor normalTextColor;

public:
    void initMenuBar();
    void initMediaComponents();

    void saveTriggered();
    void exportFileTriggered();
    void exportWaveTriggered();
    void restoreFileTriggered();

    void undoTriggered();
    void redoTriggered();

    void deleteTriggered();
    void seleteAllTriggered();
    void deselectTriggered();

    void instructionTriggered();
    void checkUpdateTriggered();
    void aboueThisTriggered();
    void aboutQtTriggered();

    void noteToolTriggered();
    void settingToolTriggered();

    void fourInFourBeatTriggered();
    void threeInFourBeatTriggered();

    void zoomInVerticallyTriggered();
    void zoomOutVerticallyTriggered();
    void zoomInHorizontallyTriggered();
    void zoomOutHorizontallyTriggered();

    void fullAdsorbTriggered();
    void halfAdsorbTriggered();
    void quarterAdsorbTriggered();
    void sixthAdsorbTriggered();
    void eighthAdsorbTriggered();
    void twelfthAdsorbTriggered();
    void sixteenthAdsorbTriggered();
    void twentyForthAdsorbTriggered();
    void thirtySecondAdsorbTriggered();
    void noAdsorbTriggered();

    void playPreviewTriggered();
    void replayPreviewTriggered();
    void stopPreviewTriggered();
    void moveToStartPreviewTriggered();
    void moveToEndPreviewTriggered();
    void removeCacheTriggered();

private:
    bool memoryIsMaximumed;
    QRect memoryWindowGeometry;
    int memoryLengthUnit;
};

#endif // MAINWINDOW_H
