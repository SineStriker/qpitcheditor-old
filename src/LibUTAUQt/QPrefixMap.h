#ifndef QPREFIXMAP_H
#define QPREFIXMAP_H

#include <QMap>
#include <QObject>
#include <QFile>

#include "QUtauGlobal.h"

class QPrefixMap {
public:
    QPrefixMap();
    ~QPrefixMap();

    bool SetTo(const QString &oPrefixMapPath);

    QString PrefixedLyric(int oNoteNum, const QString &oLyric) const;

    const QMap<int, QString> &PrefixMap() const;
    const QMap<int, QString> &SuffixMap() const;

private:
    QMap<int, QString> mPrefixMap;
    QMap<int, QString> mSuffixMap;
};

#endif // QPREFIXMAP_H
