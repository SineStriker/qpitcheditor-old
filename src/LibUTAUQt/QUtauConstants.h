#ifndef QUTAUCONSTANTS_H
#define QUTAUCONSTANTS_H

/*===========================================================================
    UTAU Sequence Text - 常量
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include <QDebug>
#include <QObject>


const QString DELETE_STRING = "%DELETE%";
const QString NODEF_STRING = "%NODEF%";
const int NODEF_INT = INT_MIN;
const double NODEF_DOUBLE = -99999999.9;

const wchar_t TONE_NAME_SHARP = L'#';

const int TIME_BASE = 480;
const int TONE_NUMBER_BASE = 24;
const int TONE_OCTAVE_MAX = 7;
const int TONE_OCTAVE_MIN = 1;
const int TONE_OCTAVE_STEPS = 12;

const QString LYRIC_R = "R";
const QString FILE_NAME_OTO_INI = "oto.ini";
const QString FILE_NAME_PREFIX_MAP = "prefix.map"; // 新增


#endif // QUTAUCONSTANTS_H
