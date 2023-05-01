#ifndef QUTAUSECTIONSETTINGS_H
#define QUTAUSECTIONSETTINGS_H

/*===========================================================================
    UTAU Sequence Text - 设定节
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include "QUtauSection.h"

class QUtauSectionSettings : public QUtauSection {
public:
    QUtauSectionSettings();

    virtual int LoadFromStringList(const QStringList &oStringList) override;

    virtual bool SetSectionName(const QString &oSectionName) override;

    const QMap<QString, QGenonSettings> &GenonSettings() const;

    bool GenonSettingsFind(const QString &oLyric, QGenonSettings *oGenon,
                           const int &oNoteNum = 0) const;

    friend class QUtauData; // 将父级设为友元类

private:
    virtual bool IsValidSectionName(const QString &oSectionName) const override;

    virtual bool SetValueCore(const QString &oKey, const QString &oValue) override;

    bool AnalyzeGenonSettings();

    QOtoIni mOtoIni;
};

#endif // QUTAUSECTIONSETTINGS_H
