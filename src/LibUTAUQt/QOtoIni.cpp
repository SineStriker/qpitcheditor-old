#include "QOtoIni.h"

#include "QUtauConstants.h"
#include "QUtauUtils.h"

#include <QDir>
#include <QFile>

//===========================================================================
QOtoIni::QOtoIni() {
    mPrefixMap = nullptr;
}

QOtoIni::~QOtoIni() {
    if (mPrefixMap) {
        delete mPrefixMap;
        mPrefixMap = nullptr;
    }
}

int QOtoIni::LoadFromDirectory(const QString &oOtoIniPath, bool oRecursive) {
    QStringList aOtoInis;
    QStringList::iterator p;

    mGenonSettings.clear();

    if (oRecursive) {
        find_file(extract_file_path(oOtoIniPath), FILE_NAME_OTO_INI, &aOtoInis);

        if (aOtoInis.isEmpty()) {
            return 0;
        }
    } else {
        aOtoInis.push_back(oOtoIniPath);
    }

    // 检测oto.ini
    mGenonSettings.clear();

    p = aOtoInis.begin();
    while (p != aOtoInis.end()) {
        SetToCore(*p);
        p++;
    }

    if (mGenonSettings.isEmpty()) {
        return 0;
    }

    // 检测prefix.map
    if (mPrefixMap) {
        delete mPrefixMap;
    }
    mPrefixMap = new QPrefixMap();
    QString mapFilePath = extract_file_path(oOtoIniPath) + FILE_NAME_PREFIX_MAP;
    if (!mPrefixMap->SetTo(mapFilePath)) {
        delete mPrefixMap;
        mPrefixMap = nullptr;
    }

    return 1;
}

bool QOtoIni::GetGenon(const QString &oLyric, QGenonSettings *oGenon, const int &oNoteNum) const {
    QMap<QString, QGenonSettings>::const_iterator it;
    QMap<int, QString>::const_iterator it2;

    if (oNoteNum > 0 && mPrefixMap) {
        // 找后缀
        it2 = mPrefixMap->SuffixMap().find(oNoteNum);
        if (it2 != mPrefixMap->SuffixMap().end()) {
            it = mGenonSettings.find(oLyric + it2.value());
            if (it != mGenonSettings.end()) {
                *oGenon = it.value();
                return 1;
            }
        }
        // 找前缀
        it2 = mPrefixMap->PrefixMap().find(oNoteNum);
        if (it2 != mPrefixMap->PrefixMap().end()) {
            it = mGenonSettings.find(it2.value() + oLyric);
            if (it != mGenonSettings.end()) {
                *oGenon = it.value();
                return 1;
            }
        }
    }
    // 直接找
    it = mGenonSettings.find(oLyric);
    if (it != mGenonSettings.end()) {
        *oGenon = it.value();
        return 1;
    }
    return 0;
}

//---------------------------------------------------------------------------
const QMap<QString, QGenonSettings> &QOtoIni::GenonSettings() const {
    return mGenonSettings;
}

//---------------------------------------------------------------------------
int QOtoIni::SetToCore(const QString &oOtoIniPath) {

    QString aAlias;
    QString aFileBody;
    QVector<QString> aNamesStrs;
    QVector<QString> aSettingsStrs;
    QGenonSettings aGenon;

    enum otoRegFlags { ofBlank, ofFileName, ofAlias };

    QMap<QString, otoRegFlags> flags; // 注册顺序标记

    QFile aInFile(oOtoIniPath);
    if (!aInFile.open(QFile::ReadOnly | QIODevice::Text)) {
        return 0;
    }

    QTextStream aText(&aInFile);
    QString aLine;

    while (!aText.atEnd()) {
        aLine = aText.readLine();

        if (!aLine.isEmpty()) {
            aSettingsStrs = qstring_to_qvector_qstring(aLine);

            aNamesStrs = qstring_to_qvector_qstring(aSettingsStrs[0], "=");

            if (aNamesStrs.size() >= 2) {
                aAlias = aNamesStrs[1];
            } else {
                aAlias = "";
            }

            aFileBody = aNamesStrs[0].mid(0, aNamesStrs[0].lastIndexOf("."));

            aSettingsStrs.resize(6);

            aGenon = QGenonSettings();
            aGenon.mFileName = extract_file_path(oOtoIniPath) + aNamesStrs[0]; // 文件名
            aGenon.mAlias = aAlias;                                            // 辅助记号
            aGenon.mOffset = aSettingsStrs[1].toDouble();                      // 偏移
            aGenon.mShiin = aSettingsStrs[2].toDouble();                       // 辅音
            aGenon.mBlank = aSettingsStrs[3].toDouble();                       // 空白
            aGenon.mPreUtterance = aSettingsStrs[4].toDouble();                // 先行声音
            aGenon.mVoiceOverlap = aSettingsStrs[5].toDouble();                // 重叠

            // 优先级：在上面的先被注册
            if (!aFileBody.isEmpty()) {
                if (flags[aFileBody] == ofBlank) {
                    mGenonSettings[aFileBody] = aGenon;
                    flags[aFileBody] = ofFileName;
                }
            }
            if (!aAlias.isEmpty()) {
                if (flags[aAlias] != ofAlias) {
                    mGenonSettings[aAlias] = aGenon;
                    flags[aAlias] = ofAlias;
                }
            }
        }
    }



    aInFile.close();
    return 1;
}

//===========================================================================
