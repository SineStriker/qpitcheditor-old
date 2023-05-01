#include "QUtauUtils.h"

#include "QUtauConstants.h"

#include <QDir>
#include <QDirIterator>

//===========================================================================
// UTAU 控制点类
QCtrlPoint::QCtrlPoint() {
    mX = 0.0;
    mY = 0.0;
    mP = sType;
}

QCtrlPoint::QCtrlPoint(const QCtrlPoint &oRHS) {
    mX = oRHS.mX;
    mY = oRHS.mY;
    mP = oRHS.mP;
}

QCtrlPoint::QCtrlPoint(double oX, double oY) {
    mX = oX;
    mY = oY;
    mP = sType;
}

QCtrlPoint::QCtrlPoint(double oX, double oY, ControlPointType oP) {
    mX = oX;
    mY = oY;
    mP = oP;
}

QCtrlPoint &QCtrlPoint::operator=(const QCtrlPoint &oRHS) {
    mX = oRHS.mX;
    mY = oRHS.mY;
    mP = oRHS.mP;

    return *this;
}

QCtrlPoint QCtrlPoint::operator+(const QCtrlPoint &oRHS) const {
    QCtrlPoint aResult;
    aResult.mX = mX + oRHS.mX;
    aResult.mY = mY + oRHS.mY;

    return aResult;
}

QCtrlPoint &QCtrlPoint::operator+=(const QCtrlPoint &oRHS) {
    mX += oRHS.mX;
    mY += oRHS.mY;

    return *this;
}

QCtrlPoint QCtrlPoint::operator-() const {
    QCtrlPoint aResult(-mX, -mY, mP);
    return aResult;
}

QCtrlPoint QCtrlPoint::operator-(const QCtrlPoint &oRHS) const {
    QCtrlPoint aResult;
    aResult.mX = mX - oRHS.mX;
    aResult.mY = mY - oRHS.mY;

    return aResult;
}

QCtrlPoint &QCtrlPoint::operator-=(const QCtrlPoint &oRHS) {
    mX -= oRHS.mX;
    mY -= oRHS.mY;

    return *this;
}

QDebug operator<<(QDebug debug, const QCtrlPoint &oRHS) {
    QString info =
        QString("QCtrlPoint(mX:%1; mY:%2; mP:%3)").arg(oRHS.mX).arg(oRHS.mY).arg(oRHS.mP);
    debug << info;
    return debug;
}

//===========================================================================
// 字符串 转 字符串数组
QVector<QString> qstring_to_qvector_qstring(const QString &oStr, const QString &oDelim) {
    QString::size_type aCurPos = 0, aNextPos;
    QVector<QString> aVectorString;

    aVectorString.clear();
    while (aCurPos < oStr.length()) {
        aNextPos = oStr.indexOf(oDelim, aCurPos);
        if (aNextPos < 0) {
            aNextPos = oStr.length();
        }
        aVectorString.push_back(oStr.mid(aCurPos, aNextPos - aCurPos));
        aCurPos = aNextPos + 1;
    }
    if (aVectorString.empty()) {
        aVectorString.push_back(NODEF_STRING);
    }
    return aVectorString;
}

//---------------------------------------------------------------------------
// 字符串 转 整数数组
QVector<int> qstring_to_qvector_int(const QString &oStr, const QString &oDelim) {
    QVector<QString> aVectorString = qstring_to_qvector_qstring(oStr, oDelim);
    QVector<int> aVectorInt32;

    for (QVector<QString>::size_type i = 0; i < aVectorString.size(); i++) {
        if (!aVectorString[i].isEmpty()) {
            aVectorInt32.push_back(aVectorString[i].toInt());
        } else {
            aVectorInt32.push_back(NODEF_INT);
        }
    }
    return aVectorInt32;
}

//---------------------------------------------------------------------------
// 字符串 转 浮点数组
QVector<double> qstring_to_qvector_double(const QString &oStr, const QString &oDelim) {
    QVector<QString> aVectorString = qstring_to_qvector_qstring(oStr, oDelim);

    return qvector_qstring_to_qvector_double(aVectorString);
}

//---------------------------------------------------------------------------
// 整数数组 转 字符串数组
QVector<QString> qvector_double_to_qvector_qstring(const QVector<double> &oVectorDouble) {
    QVector<QString> aVectorString;

    for (QVector<QString>::size_type i = 0; i < oVectorDouble.size(); i++) {
        if (oVectorDouble[i] != NODEF_DOUBLE) {
            aVectorString.push_back(num_to_qstring(oVectorDouble[i]));
        } else {
            aVectorString.push_back(NODEF_STRING);
        }
    }
    return aVectorString;
}

//---------------------------------------------------------------------------
// 浮点数组 转 字符串数组
QVector<double> qvector_qstring_to_qvector_double(const QVector<QString> &oVectorString) {
    QVector<double> aVectorDouble;

    for (QVector<QString>::size_type i = 0; i < oVectorString.size(); i++) {
        if (!oVectorString[i].isEmpty()) {
            aVectorDouble.push_back(oVectorString[i].toDouble());
        } else {
            aVectorDouble.push_back(NODEF_DOUBLE);
        }
    }
    return aVectorDouble;
}

//---------------------------------------------------------------------------
// 字符串数组 转 字符串
QString qvector_qstring_to_qstring(const QVector<QString> &oVectorString, const QString &oDelim) {
    QString aStr;

    for (QVector<QString>::size_type i = 0; i < oVectorString.size(); i++) {
        if (oVectorString[i] != NODEF_STRING) {
            aStr += oVectorString[i];
        }
        if (i < oVectorString.size() - 1) {
            aStr += oDelim;
        }
    }
    return aStr;
}

//---------------------------------------------------------------------------
// 整数数组 转 字符串
QString qvector_int_to_qstring(const QVector<int> &oVectorInt32, const QString &oDelim) {
    QVector<QString> aVectorString;

    for (QVector<int>::size_type i = 0; i < oVectorInt32.size(); i++) {
        if (oVectorInt32[i] != NODEF_INT) {
            aVectorString.push_back(num_to_qstring(oVectorInt32[i]));
        } else {
            aVectorString.push_back(NODEF_STRING);
        }
    }
    return qvector_qstring_to_qstring(aVectorString, oDelim);
}

//---------------------------------------------------------------------------
// 浮点数组 转 字符串
QString qvector_double_to_qstring(const QVector<double> &oVectorDouble, const QString &oDelim) {
    QVector<QString> aVectorString;

    aVectorString = qvector_double_to_qvector_qstring(oVectorDouble);
    return qvector_qstring_to_qstring(aVectorString, oDelim);
}

//===========================================================================
// Ticks 转 毫秒
double tick_to_time(int oTick, double oTempo) {
    return (static_cast<unsigned long>(60000) * oTick) / (oTempo * TIME_BASE);
}

//---------------------------------------------------------------------------
// 毫秒 转 Ticks
int time_to_tick(double oTime, double oTempo) {
    return static_cast<int>(oTime * oTempo * TIME_BASE / 60000);
}

//---------------------------------------------------------------------------
// 音阶 转 NoteNum
int tone_name_to_tone_number(const QString &oToneName) {
    int aOctave;
    QString::size_type aToneNameIndex;

    if (oToneName.length() < 2) {
        return TONE_NUMBER_BASE;
    }

    aToneNameIndex = TONE_NAMES.indexOf(oToneName[0]);
    if (aToneNameIndex < 0) {
        aToneNameIndex = 0;
    }

    aOctave = oToneName.rightRef(1).toInt();

    if (aOctave < TONE_OCTAVE_MIN) {
        aOctave = TONE_OCTAVE_MIN;
    } else if (aOctave > TONE_OCTAVE_MAX) {
        aOctave = TONE_OCTAVE_MAX;
    }

    return TONE_NUMBER_BASE + (aOctave - 1) * TONE_OCTAVE_STEPS + aToneNameIndex +
           static_cast<int>(oToneName[1] == TONE_NAME_SHARP);
}

//---------------------------------------------------------------------------
// 音阶 转 音阶
QString tone_number_to_tone_name(int oToneNumber) {
    return tone_number_to_tone_name(oToneNumber % TONE_OCTAVE_STEPS,
                                    oToneNumber / TONE_OCTAVE_STEPS - 2);
}

//---------------------------------------------------------------------------
// NoteNum 转 音阶
QString tone_number_to_tone_name(int oNameIndex, int oOctaveIndex) {
    QString aToneName;

    aToneName = TONE_NAMES[oNameIndex];
    if (oNameIndex > 0 && TONE_NAMES[oNameIndex] == TONE_NAMES[oNameIndex - 1]) {
        aToneName += TONE_NAME_SHARP;
    }
    aToneName += num_to_qstring(oOctaveIndex + 1);
    return aToneName;
}

//===========================================================================
// 递归找文件
int find_file(const QString &oBaseFolder, const QString &oLeaf, QStringList *oFiles) {
    if (oBaseFolder.isEmpty() || oLeaf.isEmpty()) {
        return 0;
    }

    QDir aDir;
    QStringList aFilters;
    QString aPath;

    aFilters << oLeaf;
    aDir.setPath(oBaseFolder);
    aDir.setNameFilters(aFilters);

    QDirIterator iter(aDir, QDirIterator::Subdirectories);

    while (iter.hasNext()) {
        iter.next();
        QFileInfo aInfo = iter.fileInfo();
        if (aInfo.isFile()) {
            aPath = aInfo.absoluteFilePath();
            aPath = QDir::toNativeSeparators(aPath); // 适配分隔符
            (*oFiles).append(aPath);
        }
    }

    return oFiles->size();
}

//---------------------------------------------------------------------------
// 文件大小
long long file_size(const QString &oPath) {
    long long size = 0;

    QFile aFile(oPath);
    if (aFile.open(QIODevice::ReadOnly)) {
        size = aFile.size();
        aFile.close();
    }
    return size;
}

//===========================================================================
// 取文件名或文件夹名
QString extract_file_folder_path_kernel(const QString &oFullPath, int oAddOffset) {
    QString::size_type aDemiPos;
    QString aPath;

    aDemiPos = oFullPath.lastIndexOf('\\');
    if (aDemiPos < 0) {
        // Do nothing
    } else {
        aPath = oFullPath.mid(0, aDemiPos + oAddOffset);
    }
    return aPath;
}

//===========================================================================
// 判断是否为休止符
bool isRestNoteLyric(const QString &oLyric) {
    QString aTrimmedLyric = oLyric.trimmed();

    return (aTrimmedLyric.isEmpty() || aTrimmedLyric == "R" || aTrimmedLyric == "r");
}

QString pointTypeToString(const ControlPointType &oType) {
    QString aResult;
    switch (oType) {
    case sType:
        aResult = "";
        break;
    case lineType:
        aResult = "s";
    case rType:
        aResult = "r";
        break;
    case jType:
        aResult = "j";
        break;
    }

    return aResult;
}

ControlPointType stringToPointType(const QString &oString) {
    ControlPointType aResult;

    if (oString == "s") {
        aResult = lineType;
    } else if (oString == "r") {
        aResult = rType;
    } else if (oString == "j") {
        aResult = jType;
    } else {
        aResult = sType;
    }

    return aResult;
}

//===========================================================================
