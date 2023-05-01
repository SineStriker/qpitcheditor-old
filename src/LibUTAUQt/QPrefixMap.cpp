#include "QPrefixMap.h"

#include "QUtauUtils.h"

QPrefixMap::QPrefixMap() {
}

QPrefixMap::~QPrefixMap() {
}

QString QPrefixMap::PrefixedLyric(int oNoteNum, const QString &oLyric) const {
    QMap<int, QString>::const_iterator p;
    QString aPrefixedLyric = oLyric;

    p = mPrefixMap.find(oNoteNum);
    if (p != mPrefixMap.end()) {
        aPrefixedLyric = p.value() + aPrefixedLyric;
    }
    p = mSuffixMap.find(oNoteNum);
    if (p != mSuffixMap.end()) {
        aPrefixedLyric += p.value();
    }
    return aPrefixedLyric;
}

const QMap<int, QString> &QPrefixMap::PrefixMap() const {
    return mPrefixMap;
}

bool QPrefixMap::SetTo(const QString &oPrefixMapPath) {
    QFile aInFile;
    QTextStream txtInput;
    QString aLine;
    QVector<QString> aPrefixStrs;

    mPrefixMap.clear();
    mSuffixMap.clear();

    aInFile.setFileName(oPrefixMapPath);
    if (!aInFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 0;
    }

    txtInput.setDevice(&aInFile);

    while (!txtInput.atEnd()) {
        aLine = txtInput.readLine();
        if (!aLine.isEmpty()) {
            aPrefixStrs = qstring_to_qvector_qstring(aLine, "\t");
            if (aPrefixStrs.size() >= 3) {
                mPrefixMap[tone_name_to_tone_number(aPrefixStrs[0])] = aPrefixStrs[1];
                mSuffixMap[tone_name_to_tone_number(aPrefixStrs[0])] = aPrefixStrs[2];
            }
        }
    }
    aInFile.close();
    return 1;
}

const QMap<int, QString> &QPrefixMap::SuffixMap() const {
    return mSuffixMap;
}
