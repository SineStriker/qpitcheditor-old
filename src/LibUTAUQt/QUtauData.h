#ifndef QUTAUDATA_H
#define QUTAUDATA_H

/*===========================================================================
    UTAU Sequence Text - 记录类型
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QTextStream>

#include "QUtauSection.h"
#include "QUtauSectionNote.h"
#include "QUtauSectionSettings.h"
#include "QUtauSectionVersion.h"

class QUtauData {
public:
    QUtauData();
    ~QUtauData();

    // 如果将 loadOto 指定为 false，那么不加载原音设定，用来减少启动时间
    int LoadFromFile(const QString &oPath, bool loadOto = true);

    bool Export(const QString &oPath) const;
    bool ExportTotal(const QString &oPath) const;
    bool ExportToUst(const QString &oPath);

    int CountSectionNotes() const;
    int SectionNotesNormalBegin() const;
    int SectionNotesNormalEnd() const;

    bool hasPrevSpecialNote() const;
    bool hasNextSpecialNote() const;

    const QUtauSectionVersion *SectionVersion() const;
    const QUtauSectionSettings *SectionSettings() const;
    const QUtauSectionNote *SectionNoteAt(int oIndex) const;

    QUtauSectionVersion *SectionVersion();
    QUtauSectionSettings *SectionSettings();
    QUtauSectionNote *SectionNoteAt(int oIndex);

    bool AddSectionNote(const QUtauSectionNote &oSectionNote);
    bool InsertSectionNote(const QUtauSectionNote &oSectionNote, int oIndex);
    bool ReplaceSectionNote(const QUtauSectionNote &oSectionNote, int oIndex);
    bool RemoveSectionNote(int oIndex);

    bool isGenonAnalyzed();
    bool AnalyzeGenonSettings();

private:
    bool InvalidateNeighborNotes();
    bool SetNeighborNotes();

    QUtauSectionVersion mSectionVersion;
    QUtauSectionSettings mSectionSettings;
    QVector<QUtauSectionNote> mSectionNotes;

    bool GenonAnalyzed;
};

#endif // QUTAUDATA_H
