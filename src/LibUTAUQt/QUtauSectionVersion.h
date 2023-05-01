#ifndef QUTAUSECTIONVERSION_H
#define QUTAUSECTIONVERSION_H

/*===========================================================================
    UTAU Sequence Text - 版本节
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include "QUtauSection.h"

class QUtauSectionVersion : public QUtauSection {
public:
    QUtauSectionVersion();

    virtual bool SetSectionName(const QString &oSectionName) override;

    bool GetUstVersion(QString *oVersion) const;

    QString CompatibilityKey(const QString &oKey) const;

private:
    virtual bool IsValidSectionName(const QString &oSectionName) const override;

    virtual bool SetValueCore(const QString &oKey, const QString &oValue) override;
};

#endif // QUTAUSECTIONVERSION_H
