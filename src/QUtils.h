#ifndef QUTILS_H
#define QUTILS_H

#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QSharedMemory>
#include <QString>
#include <QSvgRenderer>
#include <QWidget>

#include <cmath>

class MainWindow;

using namespace std;

static const QSize titleBarSizeHint = QSize(180, 30); // 默认标题栏尺寸

static const int minimumSections = 40;

static const QVector<string> pointType = {"x", "s", "r", "j"};

static const QString noteNames[12] = {"B", "C",  "C#", "D",  "D#", "E",
                                      "F", "F#", "G",  "G#", "A",  "A#"}; // 音符名前缀

static const QString mainWindowTitle = QObject::tr("QPitchEditor for UTAU");
static const QString childWindowTitle = QObject::tr("QPitchEditor");

static const QString appMemoryKey = QString("QPitchEditor_Running_Lock_2020_12_16");

static const QRect noRect = QRect(0, 0, 0, 0);

// 控制点特殊位置
enum specialPointPositions { pointLeftmost, pointBetween, pointRightmost };

// 控制点移动事件
enum pointMoveEvent {
    pointPress,
    pointMoving,
    pointDelete,
    pointRelease,
    pointStable,
    pointAbandon
};

// 缩放宽度序列
static const QVector<unsigned> noteUnitWidthArr = {
    80,  96,  112, 128,  144,  176,  208,  240,  288,  336,  384,  448,  512,  576,  640,  720,
    800, 800, 960, 1080, 1200, 1320, 1440, 1560, 1680, 1800, 2000, 2200, 2400, 2600, 2800, 3000};
// 80
//+16+16+16+16=144
//+32+32+32=240
//+48+48+48+48+48=384
//+64+64+64+64=640
//+80+80+80+80+80=1080
//+120+120+120+120+120+120=1800
//+200+200+200+200+200+200=3000

// 缩放高度序列
static const QVector<unsigned> noteUnitHeightArr = {12, 18, 24, 30, 36, 42, 48,
                                                    54, 60, 66, 72, 78, 84, 90};

// 默认值
static const double blankIntensity = 100;
static const double blankModulation = 100;
static const double blankVelocity = 100;
static const double blankStartPoint = 0;

static const int stageMaxVolume = 100;

// 采用值
extern MainWindow *Root;
extern QApplication *App;
extern QSharedMemory *Memory;
extern QString GeneralFontName;

void swapInt(int &a, int &b);

QString str2qstr(const string &str); // 把 std::string 转为 QString

string qstr2str(const QString &qstr); // 把 QString 转为 std::string

int moveToCenter(QWidget *ptr); // 窗口移动到中心

bool isDirExist(QString fullPath); // 文件夹是否存在

void violentMakeDir(QString fullPath); // 创建文件夹

void violentDelDir(QString fullPath); // 删除文件夹

bool isFileExist(QString fullFileName); // 文件是否存在

bool combineFile(QString oFile1, QString oFile2, QString oFile3);

void deleteFile(QString fileName);

void deleteFilesWithPrefix(QString strDir, QString prefix);

bool isPathRelative(QString fullPath); // 是否是相对路径

QString getWorkPath(); // 获取工作路径

QString getWorkPathUp2(); // 获取工作路径上两级

QString PathFindFileName(QString &s); // 根据路径获取文件名

QString PathFindFileName(QString s, QString folder);

QPixmap *createBitmapFromSVG(QString fullpath, QSize size); // 加载 SVG 文件到位图

void pathCosineTo(QPainterPath &path, QPointF prev, QPointF curr);

void pathCosineJTo(QPainterPath &path, QPointF prev, QPointF curr);

void pathCosineRTo(QPainterPath &path, QPointF prev, QPointF curr);

double Hypotenuse(double x1, double x2, double y1, double y2); // 两点间距离

// int pointTypeNum(string s);

bool isNumber(QString s, bool considerDot = false, bool considerNeg = false);

QString strToMD5(QString text);

QString fileToMD5(QString fileName);

QString loadFontFamilyFromFiles(const QString &fontFileName);

// 样式表

static QString menuStyleSheet = QString("QMenuBar{"
                                        "background:#373737;"
                                        "color:white;"
                                        "font-weight:regular;"
                                        "font-size:15px;"
                                        "}"
                                        "QMenuBar::item{"
                                        "padding:8px 10px;"
                                        "}"
                                        "QMenuBar::item:selected{"
                                        "background:#555555;"
                                        "}"
                                        "QMenu{"
                                        "padding:5px 5px;"
                                        "background:#323232;"
                                        "font-size:15px;"
                                        "}"
                                        "QMenu::item {"
                                        "padding :6px 20px;"
                                        "background: transparent;"
                                        "color:white;"
                                        //"border: 1px solid transparent;"
                                        //"font-family:'Microsoft YaHei';"
                                        //"font-weight:regular;"
                                        "}"
                                        "QMenu::item:selected {"
                                        "background: rgba(255, 255, 255, 25%);"
                                        "}"
                                        "QMenu::item:disabled {"
                                        "color: rgba(255, 255, 255, 50%);"
                                        "}"
                                        "QMenu::separator"
                                        "{"
                                        "height:3px;"
                                        "background-color:#2D2D2D;"
                                        "margin:6px 6px 6px 6px;"
                                        "}"
                                        "QMenu::indicator{"
                                        "width:18px;"
                                        "height:18px;"
                                        "padding-left:5px;"
                                        "padding-right:0px;"
                                        "}"
                                        "QMenu::indicator:checked{"
                                        "image:url(:/images/checkedHook.png);"
                                        "}");

// 字体
QFont normalFont();

QFont normalHFont();

QFont uiFont(int pixel = 12);

void setLabelColor(QLabel *label, QColor c = Qt::transparent);

void fillBackground(QWidget *w, QColor c = Qt::transparent);

// 编码
QString systemToJis(QString s);

QString JisToSystem(QString s);

#endif // QUTILS_H
