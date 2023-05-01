#ifndef QUTAUSECTIONNOTE_H
#define QUTAUSECTIONNOTE_H

/*===========================================================================
    UTAU Sequence Text - 音符节
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include "QUtauSection.h"

class QUtauSectionNote : public QUtauSection {
public:
    // 构造
    QUtauSectionNote();

    // 设置左右音符
    void SetNeighborNotes(QUtauSectionNote *oPrev, QUtauSectionNote *oNext);

    // 获取校正后的时长
    bool GetCorrectedDuration(double *oCorrectedDuration) const;

    // 获取校正后的先行声音
    bool GetCorrectedGenon(CorrectGenon *oCorrectedGenon) const;

    // 获取绝对包络坐标
    bool GetCorrectedEnvelope(QVector<QCtrlPoint> *oCorrectedEnvelope) const;

    // 设置绝对包络坐标
    bool SetCorrectedEnvelope(const QVector<QCtrlPoint> &oCorrectedEnvelope);

    // 获取绝对 Mode2 音高曲线
    bool GetCorrectedPortamento(QVector<QCtrlPoint> *oCorrectedPitch) const;

    // 设置绝对 Mode2 音高曲线
    void SetCorrectedPortamento(const QVector<QCtrlPoint> &oCorrectedPitch);

    // 获取包络数组
    bool GetEnvelope(QVector<QCtrlPoint> *oCorrectedEnvelope) const;
    // 获取包络数组
    bool SetEnvelope(const QVector<QCtrlPoint> &oCorrectedEnvelope);

    // 获取 UTAU 校正的值
    bool GetSuperiorPreUtterance(double *oCorrectedPreUtterance) const;
    bool GetSuperiorVoiceOverlap(double *oCorrectedVoiceOverlap) const;
    bool GetSuperiorStartPoint(double *oCorrectedStartPoint) const;

    // 获取文件名、辅助记号、缓存
    bool GetSuperiorFileName(QString *oCorrectedFileName) const;
    bool GetSuperiorAlias(QString *oCorrectedAlias) const;
    bool GetSuperiorCache(QString *oCorrectedCache) const;

    // 字符串转包络
    static QVector<QCtrlPoint> StringToEnvelope(const QString &oStr);

    // 包络转字符串
    static QString EnvelopeToString(const QVector<QCtrlPoint> &oEnvelope);

    friend class QUtauData;

private:
    // 音符节名格式
    virtual bool IsValidSectionName(const QString &oSectionName) const override;

    virtual bool GetValueCore(const QString &oKey, QString *oValue, int *oStatus) const override;

    // 获取原音设定
    bool GetGenonSettings(QGenonSettings *oGenonSettings) const;

    // 邻接音符
    QUtauSectionNote *mPrevNoteRef;
    QUtauSectionNote *mNextNoteRef;
};

#endif // QUTAUSECTIONNOTE_H
