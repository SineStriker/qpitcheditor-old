#ifndef QOTOINI_H
#define QOTOINI_H

/*===========================================================================
    UTAU Sequence Text - 读取原音设定
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include <QDebug>
#include <QMap>
#include <QObject>

#include "QGenonSettings.h"
#include "QPrefixMap.h"

class QOtoIni {
public:
    QOtoIni();
    ~QOtoIni();

    // 初始化
    int LoadFromDirectory(const QString &oOtoIniPath, bool oRecursive = true);

    // 查找采样
    bool GetGenon(const QString &oLyric, QGenonSettings *oGenon, const int &oNoteNum = 0) const;

    // 返回原音设定集
    const QMap<QString, QGenonSettings> &GenonSettings() const;

private:
    // 内部初始化函数
    int SetToCore(const QString &oOtoIniPath);

    // 存储原音设定集
    QMap<QString, QGenonSettings> mGenonSettings;
    QPrefixMap *mPrefixMap;
};

#endif // QOTOINI_H
