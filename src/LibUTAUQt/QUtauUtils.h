#ifndef QUTAUUTILS_H
#define QUTAUUTILS_H

/*===========================================================================
    UTAU Sequence Text - 实用函数
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

#include <QDebug>
#include <QObject>

#include "QUtauGlobal.h"

//===========================================================================

// 音阶名
const QString TONE_NAMES = "CCDDEFFGGAAB";
enum ControlPointType { sType, lineType, rType, jType };

// UTAU 控制点类
class QCtrlPoint {
public:
    QCtrlPoint();
    QCtrlPoint(const QCtrlPoint &oRHS);

    QCtrlPoint(double oX, double oY);
    QCtrlPoint(double oX, double oY, ControlPointType oP);

    QCtrlPoint &operator=(const QCtrlPoint &oRHS);

    QCtrlPoint operator+(const QCtrlPoint &oRHS) const;
    QCtrlPoint &operator+=(const QCtrlPoint &oRHS);

    QCtrlPoint operator-() const;

    QCtrlPoint operator-(const QCtrlPoint &oRHS) const;
    QCtrlPoint &operator-=(const QCtrlPoint &oRHS);

    friend QDebug operator<<(QDebug debug, const QCtrlPoint &oRHS);

    double mX;
    double mY;
    ControlPointType mP;
};

//===========================================================================
// 包含先行声音，重叠，STP的组
struct CorrectGenon {
    double PreUtterance;
    double VoiceOverlap;
    double StartPoint;

    CorrectGenon() {
        PreUtterance = VoiceOverlap = StartPoint = 0;
    }
    CorrectGenon(double pre, double ove, double stp) {
        PreUtterance = pre;
        VoiceOverlap = ove;
        StartPoint = stp;
    }
};

//===========================================================================
// 字符串 转 字符串数组、整数数组、浮点数组
QVector<QString> qstring_to_qvector_qstring(const QString &oStr, const QString &oDelim = ",");
QVector<int> qstring_to_qvector_int(const QString &oStr, const QString &oDelim = ",");
QVector<double> qstring_to_qvector_double(const QString &oStr, const QString &oDelim = ",");

//---------------------------------------------------------------------------
// 整数数组、浮点数组 转 字符串数组
QVector<QString> qvector_double_to_qvector_qstring(const QVector<double> &oVectorDouble);
QVector<double> qvector_qstring_to_qvector_double(const QVector<QString> &oVectorString);

//---------------------------------------------------------------------------
// 字符串数组、整数数组、浮点数组 转 字符串
QString qvector_qstring_to_qstring(const QVector<QString> &oVectorString,
                                   const QString &oDelim = ",");
QString qvector_int_to_qstring(const QVector<int> &oVectorInt32, const QString &oDelim = ",");
QString qvector_double_to_qstring(const QVector<double> &oVectorDouble,
                                  const QString &oDelim = ",");

//===========================================================================
// Ticks 与 毫秒 互转
double tick_to_time(int oTick, double oTempo);
int time_to_tick(double oTime, double oTempo);

//---------------------------------------------------------------------------
// NoteNum 与 音阶 互转
int tone_name_to_tone_number(const QString &oToneName);
QString tone_number_to_tone_name(int oToneNumber);
QString tone_number_to_tone_name(int oNameIndex, int oOctaveIndex);

//---------------------------------------------------------------------------
// 数字 转 字符串
template <class T> QString num_to_qstring(T oVal) {
    return QString::number(oVal);
}

//===========================================================================
// 文件大小
long long file_size(const QString &oPath);

//---------------------------------------------------------------------------
// 递归找文件
int find_file(const QString &oBaseFolder, const QString &oLeaf, QStringList *oFiles);

//===========================================================================
// 取文件名或文件夹名
QString extract_file_folder_path_kernel(const QString &oFullPath, int oAddOffset);

//---------------------------------------------------------------------------
// 取文件夹名
inline QString extract_file_folder(const QString &oFullPath) {
    return extract_file_folder_path_kernel(oFullPath, 0);
}

//---------------------------------------------------------------------------
// 取文件名
inline QString extract_file_path(const QString &oFullPath) {
    return extract_file_folder_path_kernel(oFullPath, 1);
}

//===========================================================================
// 判断休止符
bool isRestNoteLyric(const QString &oLyric);

// 控制点类型转换
QString pointTypeToString(const ControlPointType &oType);
ControlPointType stringToPointType(const QString &oString);

//===========================================================================

#endif // QUTAUUTILS_H
