#include "QUtauSectionVersion.h"

//===========================================================================
QUtauSectionVersion::QUtauSectionVersion() {
}

bool QUtauSectionVersion::SetSectionName(const QString &oSectionName) {
    return 0; // 不可改节名
}

//===========================================================================
// 获取 UST 版本
bool QUtauSectionVersion::GetUstVersion(QString *oVersion) const {
    QMap<QString, QString>::const_iterator p;

    p = OriginalValueMap().begin();
    while (p != OriginalValueMap().end()) {
        if (p.key().indexOf(UST_VERSION_PREFIX) == 0) {
            *oVersion = p.key().mid(UST_VERSION_PREFIX.size());
            return 1;
        }
        p++;
    }
    *oVersion = "";
    return 0;
}

//---------------------------------------------------------------------------
// 获取兼容的键名
QString QUtauSectionVersion::CompatibilityKey(const QString &oKey) const {
    QString aCompatibilityKey = oKey;
    QString aUstVersion;

    GetUstVersion(&aUstVersion);
    if (oKey == KEY_NAME_MODULATION) {
        if (aUstVersion < UST_VERSION_1_2) {
            aCompatibilityKey = KEY_NAME_MODURATION;
        }
    }
    return aCompatibilityKey;
}
//===========================================================================
bool QUtauSectionVersion::IsValidSectionName(const QString &oSectionName) const {
    return oSectionName == SECTION_NAME_VERSION;
}

bool QUtauSectionVersion::SetValueCore(const QString &oKey, const QString &oValue) {
    return QUtauSection::SetValueCore(oKey, oValue);
}

//===========================================================================
