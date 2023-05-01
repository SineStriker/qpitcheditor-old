#include "QUtauData.h"
#include <cmath>

#include "QUtauSectionNote.h"

//===========================================================================
QUtauSectionNote::QUtauSectionNote() {
    mPrevNoteRef = NULL;
    mNextNoteRef = NULL;

    SetDefaultValue(KEY_NAME_INTENSITY, DEFAULT_VALUE_INTENSITY); // 音量默认值100

    SetDefaultValue(KEY_NAME_MODULATION, DEFAULT_VALUE_MODULATION); // 移调默认值 100
    SetDefaultValue(KEY_NAME_MODURATION, DEFAULT_VALUE_MODULATION);

    SetDefaultValue(KEY_NAME_VELOCITY, DEFAULT_VALUE_VELOCITY); // 辅音速度默认值 100
    SetDefaultValue(KEY_NAME_FLAGS, DEFAULT_VALUE_FLAGS);

    SetDefaultValue(KEY_NAME_TEMPO, DEFAULT_VALUE_TEMPO);

    SetDefaultValue(KEY_NAME_PRE_UTTERANCE, DEFAULT_VALUE_PRE_UTTERANCE); // 默认值 0
    SetDefaultValue(KEY_NAME_VOICE_OVERLAP, DEFAULT_VALUE_VOICE_OVERLAP);
    SetDefaultValue(KEY_NAME_START_POINT, DEFAULT_START_POINT);
}

void QUtauSectionNote::SetNeighborNotes(QUtauSectionNote *oPrev, QUtauSectionNote *oNext) {
    mPrevNoteRef = oPrev;
    mNextNoteRef = oNext;
}

//===========================================================================
// 获取校正后的长度
bool QUtauSectionNote::GetCorrectedDuration(double *oCorrectedDuration) const {
    int aLength;
    double aCorrectedPreUtterance;
    double aNextCorrectedPreUtterance = 0;
    double aNextCorrectedVoiceOverlap = 0;
    double aTempo;

    CorrectGenon aGenon, aNextGenon;

    if (!GetValue(KEY_NAME_TEMPO, &aTempo)) {
        return 0;
    }
    if (!GetValue(KEY_NAME_LENGTH, &aLength)) {
        return 0;
    }
    if (!GetCorrectedGenon(&aGenon)) {
        return 0;
    }

    aCorrectedPreUtterance = aGenon.PreUtterance;

    if (mNextNoteRef != NULL) {
        if (!mNextNoteRef->GetCorrectedGenon(&aNextGenon)) {
            return 0;
        }
        aNextCorrectedPreUtterance = aNextGenon.PreUtterance;
        aNextCorrectedVoiceOverlap = aNextGenon.VoiceOverlap;
    }

    *oCorrectedDuration = tick_to_time(aLength, aTempo) + aCorrectedPreUtterance +
                          aNextCorrectedVoiceOverlap - aNextCorrectedPreUtterance;
    return 1;
}

//===========================================================================
// 获取校正后的先行/重叠/STP
bool QUtauSectionNote::GetCorrectedGenon(CorrectGenon *oCorrectedGenon) const {
    bool aNeedCorrect = false;
    int aPrevLength;
    double aCorrectRatio, aVelocityRatio;
    double aKuikomi;
    double aPreUtteranceTmp, aVoiceOverlapTmp, aStartPointTmp, aVelocity;
    double aThreshold;
    double aPrevTempo;
    QString aPrevLyric;

    if (!GetValue(KEY_NAME_PRE_UTTERANCE, &aPreUtteranceTmp)) {
        return 0;
    }
    if (!GetValue(KEY_NAME_VOICE_OVERLAP, &aVoiceOverlapTmp)) {
        return 0;
    }
    if (!GetValue(KEY_NAME_START_POINT, &aStartPointTmp)) {
        return 0;
    }

    if (!GetValue(KEY_NAME_VELOCITY, &aVelocity)) {
        return 0;
    }
    aVelocityRatio = pow(2, 1 - aVelocity / 100);

    // 加辅音速度
    aPreUtteranceTmp *= aVelocityRatio;
    aVoiceOverlapTmp *= aVelocityRatio;

    if (mPrevNoteRef != NULL) {
        if (!mPrevNoteRef->GetValue(KEY_NAME_LYRIC, &aPrevLyric)) {
            return 0;
        }
        if (!mPrevNoteRef->GetValue(KEY_NAME_LENGTH, &aPrevLength)) {
            return 0;
        }
        if (!mPrevNoteRef->GetValue(KEY_NAME_TEMPO, &aPrevTempo)) {
            return 0;
        }
        if (isRestNoteLyric(aPrevLyric)) {
            aThreshold = tick_to_time(aPrevLength, aPrevTempo); // 最大长度为整个长度
        } else {

            aThreshold = tick_to_time(aPrevLength, aPrevTempo) / 2; // 最大长度为一半长度
        }
        aKuikomi = aPreUtteranceTmp - aVoiceOverlapTmp;
        if (aKuikomi > aThreshold) {
            aNeedCorrect = true;
            aCorrectRatio = aThreshold / aKuikomi;
            (*oCorrectedGenon).VoiceOverlap = aVoiceOverlapTmp * aCorrectRatio;
            (*oCorrectedGenon).PreUtterance = aThreshold + (*oCorrectedGenon).VoiceOverlap;
            (*oCorrectedGenon).StartPoint = aPreUtteranceTmp - (*oCorrectedGenon).PreUtterance;
        }
    }
    if (!aNeedCorrect) {
        (*oCorrectedGenon).PreUtterance = aPreUtteranceTmp;
        (*oCorrectedGenon).VoiceOverlap = aVoiceOverlapTmp;
        (*oCorrectedGenon).StartPoint = 0;
    }

    (*oCorrectedGenon).StartPoint += aStartPointTmp;

    return 1;
}

//===========================================================================
// 获取音量包络的绝对坐标
bool QUtauSectionNote::GetCorrectedEnvelope(QVector<QCtrlPoint> *oCorrectedEnvelope) const {
    double aCorrectedDuration;
    QString aEnvelope;
    QVector<QCtrlPoint> aEnvelopeVector;

    oCorrectedEnvelope->clear();

    if (!GetCorrectedDuration(&aCorrectedDuration)) {
        return 0;
    }
    if (!GetValue(KEY_NAME_ENVELOPE, &aEnvelope)) {
        return 0;
    }
    aEnvelopeVector = StringToEnvelope(aEnvelope);

    for (QVector<QCtrlPoint>::size_type i = 0; i < aEnvelopeVector.size(); i++) {
        if (aEnvelopeVector[i].mX == NODEF_DOUBLE) {
            aEnvelopeVector[i].mX = 0.0;
        }
        if (aEnvelopeVector[i].mY == NODEF_DOUBLE) {
            aEnvelopeVector[i].mY = 0.0;
        }
    }

    oCorrectedEnvelope->push_back(aEnvelopeVector[0]);

    oCorrectedEnvelope->push_back(QCtrlPoint((*oCorrectedEnvelope)[0].mX, 0) + aEnvelopeVector[1]);

    if (aEnvelopeVector.size() == NUM_ENVELOPE_POINTS) {
        oCorrectedEnvelope->push_back(QCtrlPoint((*oCorrectedEnvelope)[1].mX, 0) +
                                      aEnvelopeVector[2]);
    }

    oCorrectedEnvelope->push_back(QCtrlPoint(aCorrectedDuration - 1 -
                                                 aEnvelopeVector[aEnvelopeVector.size() - 1].mX -
                                                 aEnvelopeVector[aEnvelopeVector.size() - 2].mX,
                                             aEnvelopeVector[aEnvelopeVector.size() - 2].mY));

    oCorrectedEnvelope->push_back(
        QCtrlPoint(aCorrectedDuration - 1 - aEnvelopeVector[aEnvelopeVector.size() - 1].mX,
                   aEnvelopeVector[aEnvelopeVector.size() - 1].mY));
    return 1;
}

//---------------------------------------------------------------------------
bool QUtauSectionNote::SetCorrectedEnvelope(const QVector<QCtrlPoint> &oCorrectedEnvelope) {
    bool aOffset;
    double aCorrectedDuration;
    QVector<QCtrlPoint> aEnvelope;

    if (oCorrectedEnvelope.size() != NUM_ENVELOPE_POINTS &&
        oCorrectedEnvelope.size() != NUM_ENVELOPE_POINTS - 1) {
        return 0;
    }

    if (!GetCorrectedDuration(&aCorrectedDuration)) {
        return 0;
    }

    aOffset = (oCorrectedEnvelope.size() == NUM_ENVELOPE_POINTS);
    aEnvelope.push_back(QCtrlPoint(oCorrectedEnvelope[0].mX, oCorrectedEnvelope[0].mY));
    aEnvelope.push_back(
        QCtrlPoint(oCorrectedEnvelope[1].mX - oCorrectedEnvelope[0].mX, oCorrectedEnvelope[1].mY));
    if (oCorrectedEnvelope.size() == NUM_ENVELOPE_POINTS) {
        aEnvelope.push_back(QCtrlPoint(oCorrectedEnvelope[2].mX - oCorrectedEnvelope[1].mX,
                                       oCorrectedEnvelope[2].mY));
    }
    aEnvelope.push_back(
        QCtrlPoint(oCorrectedEnvelope[3 + aOffset].mX - oCorrectedEnvelope[2 + aOffset].mX,
                   oCorrectedEnvelope[2 + aOffset].mY));
    aEnvelope.push_back(QCtrlPoint(aCorrectedDuration - 1 - oCorrectedEnvelope[3 + aOffset].mX,
                                   oCorrectedEnvelope[3 + aOffset].mY));

    SetValue(KEY_NAME_ENVELOPE, EnvelopeToString(aEnvelope));
    return 1;
}

//===========================================================================
// 获取绝对 Mode2 音高曲线
bool QUtauSectionNote::GetCorrectedPortamento(QVector<QCtrlPoint> *oCorrectedPitch) const {
    int aNoteNum, aPrevNoteNum;
    QString aPBS, aPBW, aPBY, aPBM;
    QVector<double> aPBSXY, aPBWs, aPBYs;
    QVector<QString> aPBMs;
    QCtrlPoint aPoint;

    QString aPrevLyric;

    oCorrectedPitch->clear();

    if (!GetValue(KEY_NAME_PBS, &aPBS)) {
        return 0;
    }
    aPBSXY = qstring_to_qvector_double(aPBS, ";");
    if (!aPBSXY.isEmpty()) {
        aPoint.mX = aPBSXY[0];

        if (mPrevNoteRef != NULL) {
            mPrevNoteRef->GetValue(KEY_NAME_LYRIC, &aPrevLyric);
        }
        if (mPrevNoteRef != NULL && isRestNoteLyric(aPrevLyric)) {
            if (aPBSXY.size() >= 2) {
                aPoint.mY = aPBSXY[1];
            } else {
                aPoint.mY = 0;
            }
        } else {
            if (mPrevNoteRef != NULL) {
                GetValue(KEY_NAME_NOTE_NUM, &aNoteNum);
                mPrevNoteRef->GetValue(KEY_NAME_NOTE_NUM, &aPrevNoteNum);
                aPoint.mY = (aPrevNoteNum - aNoteNum) * 10;
            }
        }
    }
    oCorrectedPitch->push_back(aPoint);

    if (GetValue(KEY_NAME_PBW, &aPBW)) {
        aPBWs = qstring_to_qvector_double(aPBW);
        if (GetValue(KEY_NAME_PBY, &aPBY)) {
            aPBYs = qstring_to_qvector_double(aPBY);
        }
        if (GetValue(KEY_NAME_PBM, &aPBM)) {
            aPBMs = qstring_to_qvector_qstring(aPBM);
        }
        for (QVector<double>::size_type i = 0; i < qMax(aPBWs.size(), aPBYs.size()); i++) {
            aPoint = QCtrlPoint();
            if (aPBWs.size() > i) {
                aPoint.mX = aPBWs[i] >= 0 ? aPBWs[i] : 0;
            }
            if (aPBYs.size() > i) {
                aPoint.mY = (aPBYs[i] == NODEF_DOUBLE) ? 0 : aPBYs[i];
            }
            if (aPBMs.size() > i) {
                aPoint.mP = (aPBMs[i] == NODEF_STRING) ? sType : stringToPointType(aPBMs[i]);
            }
            aPoint.mX += oCorrectedPitch->back().mX;
            oCorrectedPitch->push_back(aPoint);
        }
    }
    return 1;
}

//---------------------------------------------------------------------------
// 设置绝对 Mode2 音高曲线
void QUtauSectionNote::SetCorrectedPortamento(const QVector<QCtrlPoint> &oCorrectedPitch) {
    int aNoteNum, aPrevNoteNum;
    QVector<double> aVectorDouble;
    QVector<QString> aVectorString;

    aVectorDouble.clear();
    aVectorDouble.push_back(oCorrectedPitch[0].mX);

    if (mPrevNoteRef == NULL) {
        aVectorDouble.push_back(oCorrectedPitch[0].mY);
    } else {
        GetValue(KEY_NAME_NOTE_NUM, &aNoteNum);
        mPrevNoteRef->GetValue(KEY_NAME_NOTE_NUM, &aPrevNoteNum);
        if (oCorrectedPitch[0].mY != (aPrevNoteNum - aNoteNum) * 10) {
            aVectorDouble.push_back(oCorrectedPitch[0].mY);
        }
    }
    SetValue(KEY_NAME_PBS, qvector_double_to_qstring(aVectorDouble, ";"));

    aVectorDouble.clear();
    for (QVector<QCtrlPoint>::size_type i = 1; i < oCorrectedPitch.size(); i++) {
        aVectorDouble.push_back(oCorrectedPitch[i].mX - oCorrectedPitch[i - 1].mX);
    }
    SetValue(KEY_NAME_PBW, qvector_double_to_qstring(aVectorDouble));

    aVectorDouble.clear();
    for (QVector<QCtrlPoint>::size_type i = 1; i < oCorrectedPitch.size(); i++) {
        aVectorDouble.push_back(oCorrectedPitch[i].mY);
    }
    SetValue(KEY_NAME_PBY, qvector_double_to_qstring(aVectorDouble));

    aVectorString.clear();
    for (QVector<QCtrlPoint>::size_type i = 1; i < oCorrectedPitch.size(); i++) {
        aVectorString.push_back(pointTypeToString(oCorrectedPitch[i].mP));
    }

    SetValue(KEY_NAME_PBM, qvector_qstring_to_qstring(aVectorString));
}

bool QUtauSectionNote::GetEnvelope(QVector<QCtrlPoint> *oCorrectedEnvelope) const {
    QString aEnvelope;
    QVector<QCtrlPoint> aEnvelopeVector;

    if (!GetValue(KEY_NAME_ENVELOPE, &aEnvelope)) {
        return 0;
    }
    aEnvelopeVector = StringToEnvelope(aEnvelope);

    for (QVector<QCtrlPoint>::size_type i = 0; i < aEnvelopeVector.size(); i++) {
        if (aEnvelopeVector[i].mX == NODEF_DOUBLE) {
            aEnvelopeVector[i].mX = 0.0;
        }
        if (aEnvelopeVector[i].mY == NODEF_DOUBLE) {
            aEnvelopeVector[i].mY = 0.0;
        }
    }
    *oCorrectedEnvelope = aEnvelopeVector;

    return 1;
}

bool QUtauSectionNote::SetEnvelope(const QVector<QCtrlPoint> &oCorrectedEnvelope) {
    QString aEnvelope = EnvelopeToString(oCorrectedEnvelope);
    SetValue(KEY_NAME_ENVELOPE, aEnvelope);
    return 1;
}

//===========================================================================
// 获取 UTAU 矫正的值
bool QUtauSectionNote::GetSuperiorPreUtterance(double *oCorrectedPreUtterance) const {
    return GetValue(KEY_NAME_PRE_UTTERANCE_READONLY, oCorrectedPreUtterance);
}

bool QUtauSectionNote::GetSuperiorVoiceOverlap(double *oCorrectedVoiceOverlap) const {
    return GetValue(KEY_NAME_VOICE_OVERLAP_READONLY, oCorrectedVoiceOverlap);
}

bool QUtauSectionNote::GetSuperiorStartPoint(double *oCorrectedStartPoint) const {
    return GetValue(KEY_NAME_START_POINT_READONLY, oCorrectedStartPoint);
}

//===========================================================================
// 获取文件名、辅助记号、缓存
bool QUtauSectionNote::GetSuperiorFileName(QString *oCorrectedFileName) const {
    return GetValue(KEY_NAME_FILENAME_READONLY, oCorrectedFileName);
}

bool QUtauSectionNote::GetSuperiorAlias(QString *oCorrectedAlias) const {
    return GetValue(KEY_NAME_ALIAS_READONLY, oCorrectedAlias);
}

bool QUtauSectionNote::GetSuperiorCache(QString *oCorrectedCache) const {
    return GetValue(KEY_NAME_CACHE_READONLY, oCorrectedCache);
}

//===========================================================================
bool QUtauSectionNote::IsValidSectionName(const QString &oSectionName) const {
    return !oSectionName.isEmpty() &&
           (oSectionName == SECTION_NAME_PREV || oSectionName == SECTION_NAME_NEXT ||
            (oSectionName[0] >= '0' && oSectionName <= '9'));
}

bool QUtauSectionNote::GetValueCore(const QString &oKey, QString *oValue, int *oStatus) const {
    int aStatus;
    bool aValueResult, aGenonResult;
    QGenonSettings aGenonSettings;

    aValueResult = QUtauSection::GetValueCore(oKey, oValue, &aStatus);

    if ((aValueResult && aStatus == MFM_OMITTED) || !aValueResult) {
        if (oKey == KEY_NAME_PRE_UTTERANCE || oKey == KEY_NAME_VOICE_OVERLAP) {
            aGenonResult = GetGenonSettings(&aGenonSettings);
            if (aGenonResult) {
                if (oKey == KEY_NAME_PRE_UTTERANCE) {
                    *oValue = num_to_qstring(aGenonSettings.mPreUtterance);
                } else if (oKey == KEY_NAME_VOICE_OVERLAP) {
                    *oValue = num_to_qstring(aGenonSettings.mVoiceOverlap);
                }
                aStatus = MFM_OMITTED;
                aValueResult = true;
            }
        }
    }
    if (oStatus != NULL) {
        *oStatus = aStatus;
    }

    return aValueResult;
}

//===========================================================================
// 获取原音设定
bool QUtauSectionNote::GetGenonSettings(QGenonSettings *oGenonSettings) const {
    QString aLyric;
    int aNoteNum;

    if (Parent() == NULL) {
        return 0;
    }

    if (!GetValue(KEY_NAME_LYRIC, &aLyric)) {
        return 0;
    }
    if (!GetValue(KEY_NAME_LYRIC, &aNoteNum)) {
        return 0;
    }

    return Parent()->SectionSettings()->GenonSettingsFind(aLyric, oGenonSettings, aNoteNum);
}

//===========================================================================
// 字符串转包络
QVector<QCtrlPoint> QUtauSectionNote::StringToEnvelope(const QString &oStr) {
    QVector<QString> aVectorString = qstring_to_qvector_qstring(oStr);
    QVector<double> aVectorDouble;
    QVector<QCtrlPoint> aEnvelope;

    if (aVectorString.size() >= 8) {
        aVectorString.erase(aVectorString.begin() + 7);
    }
    aVectorDouble = qvector_qstring_to_qvector_double(aVectorString);
    if (aVectorDouble.size() % 2 != 0) {
        aVectorDouble.push_back(NODEF_DOUBLE);
    }

    aEnvelope.push_back(QCtrlPoint(aVectorDouble[0], aVectorDouble[3]));
    aEnvelope.push_back(QCtrlPoint(aVectorDouble[1], aVectorDouble[4]));
    if (aVectorDouble.size() == NUM_ENVELOPE_POINTS * 2) {
        aEnvelope.push_back(QCtrlPoint(aVectorDouble[8], aVectorDouble[9]));
    }
    aEnvelope.push_back(QCtrlPoint(aVectorDouble[2], aVectorDouble[5]));
    aEnvelope.push_back(QCtrlPoint(aVectorDouble[7], aVectorDouble[6]));
    return aEnvelope;
}

//---------------------------------------------------------------------------
// 包络转字符串
QString QUtauSectionNote::EnvelopeToString(const QVector<QCtrlPoint> &oEnvelope) {
    int aOffset;
    QVector<QString> aVectorString;
    QVector<double> aVectorDouble;

    if (oEnvelope.size() < 4) {
        return "";
    }

    aOffset = (oEnvelope.size() == NUM_ENVELOPE_POINTS);
    aVectorDouble.push_back(oEnvelope[0].mX);
    aVectorDouble.push_back(oEnvelope[1].mX);
    aVectorDouble.push_back(oEnvelope[2 + aOffset].mX);
    aVectorDouble.push_back(oEnvelope[0].mY);
    aVectorDouble.push_back(oEnvelope[1].mY);
    aVectorDouble.push_back(oEnvelope[2 + aOffset].mY);
    aVectorDouble.push_back(oEnvelope[3 + aOffset].mY);
    aVectorDouble.push_back(oEnvelope[3 + aOffset].mX);
    if (oEnvelope.size() == NUM_ENVELOPE_POINTS) {
        aVectorDouble.push_back(oEnvelope[2].mX);
        aVectorDouble.push_back(oEnvelope[2].mY);
    }

    if (aVectorDouble.size() == 8 && aVectorDouble[7] == 0.0) {
        aVectorDouble.pop_back();
    }
    aVectorString = qvector_double_to_qvector_qstring(aVectorDouble);

    if (aVectorString.size() > 7) {
        aVectorString.insert(aVectorString.begin() + 7, "%");
    }
    return qvector_qstring_to_qstring(aVectorString);
}

//===========================================================================
