#ifndef QGENONSETTINGS_H
#define QGENONSETTINGS_H

/*===========================================================================
    UTAU Sequence Text - 原音设定类
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include <QDebug>
#include <QObject>

class QGenonSettings {
public:
    QGenonSettings();
    ~QGenonSettings();

    QString mFileName;
    QString mAlias;
    double mOffset;
    double mShiin;
    double mBlank;
    double mPreUtterance;
    double mVoiceOverlap;

    friend QDebug operator<<(QDebug debug, const QGenonSettings &oGenon);
};

#endif // QGENONSETTINGS_H
