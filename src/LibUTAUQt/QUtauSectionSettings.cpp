#include "QUtauSectionSettings.h"
#include "QUtauData.h"

//===========================================================================
QUtauSectionSettings::QUtauSectionSettings() {
    SetDefaultValue(KEY_NAME_TEMPO, DEFAULT_VALUE_TEMPO); // 设曲速为默认曲速
}

int QUtauSectionSettings::LoadFromStringList(const QStringList &oStringList) {
    bool aResult;

    aResult = QUtauSection::LoadFromStringList(oStringList);
    if (!aResult) {
        return ERROR_INVALID_FORMAT;
    }

    if (Parent() && !Parent()->isGenonAnalyzed()) {
        // 不加载原音设定
    } else if (!AnalyzeGenonSettings()) {
        return ERROR_INVALID_VOICE_DIR_PATH;
    }

    return NONE_ERROR;
}

bool QUtauSectionSettings::SetSectionName(const QString &oSectionName) {
    return 0; // 不可改节名
}

const QMap<QString, QGenonSettings> &QUtauSectionSettings::GenonSettings() const {
    return mOtoIni.GenonSettings();
}

bool QUtauSectionSettings::GenonSettingsFind(const QString &oLyric, QGenonSettings *oGenon,
                                             const int &oNoteNum) const {
    return mOtoIni.GetGenon(oLyric, oGenon, oNoteNum);
}

//===========================================================================
// 判断节名是否合法
bool QUtauSectionSettings::IsValidSectionName(const QString &oSectionName) const {
    return oSectionName == SECTION_NAME_SETTING;
}

bool QUtauSectionSettings::SetValueCore(const QString &oKey, const QString &oValue) {
    return QUtauSection::SetValueCore(oKey, oValue);
}

//===========================================================================
// 加载原音设定
bool QUtauSectionSettings::AnalyzeGenonSettings() {
    QString aVoiceDir;

    GetValue(KEY_NAME_VOICE_DIR, &aVoiceDir);
    if (aVoiceDir.isEmpty()) {
        return 0;
    }
    if (aVoiceDir.rightRef(1) != '\\') {
        aVoiceDir += '\\';
    }
    return mOtoIni.LoadFromDirectory(aVoiceDir + FILE_NAME_OTO_INI);
}
//===========================================================================
