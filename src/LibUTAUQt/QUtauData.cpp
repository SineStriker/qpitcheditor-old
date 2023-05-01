#include "QUtauData.h"

//===========================================================================
QUtauData::QUtauData() {
    GenonAnalyzed = false;
}

QUtauData::~QUtauData() {
}

int QUtauData::LoadFromFile(const QString &oPath, bool loadOto) {
    GenonAnalyzed = loadOto;

    int aResult = ERROR_INVALID_FORMAT; // 假设格式不正确
    bool aBeginNote = false;
    // 打开文件
    QFile aInFile(oPath);
    if (!aInFile.open(QFile::ReadOnly | QIODevice::Text)) {
        return ERROR_INVALID_INPUT_FILE_PATH;
    }

    QTextStream aText(&aInFile);

    QStringList aCurrentSection;
    QString aSectionHead;

    QUtauSectionNote aNote;
    double aCurrentTempo;

    try {
        mSectionVersion = QUtauSectionVersion();
        mSectionVersion.SetParent(this);
        mSectionSettings = QUtauSectionSettings();
        mSectionSettings.SetParent(this);
        mSectionNotes.clear();

        // 读取第一行
        aSectionHead = aText.readLine();
        aCurrentSection.push_back(aSectionHead);

        while (!aText.atEnd()) {
            aSectionHead = aText.readLine();

            // 没有结束或没有遇到下一节的开始就继续附加
            if (!aSectionHead.startsWith(SECTION_BEGIN_MARK) && !aText.atEnd()) {
                aCurrentSection.push_back(aSectionHead);
                continue;
            }

            if (mSectionVersion.LoadFromStringList(aCurrentSection)) {
                // 读取版本信息
            } else {
                int tempResult = mSectionSettings.LoadFromStringList(aCurrentSection);

                if (tempResult > MINOR_ERROR) {
                    aResult = tempResult;
                    aBeginNote = true;
                }
            }
            aCurrentSection.clear();
            aCurrentSection.push_back(aSectionHead);

            if (aBeginNote) {
                break;
            }
        }

        if (aResult < MINOR_ERROR) {
            throw aResult;
        }

        // 保存全局曲速
        if (!mSectionSettings.GetValue(KEY_NAME_TEMPO, &aCurrentTempo)) {
            aCurrentTempo = DEFAULT_VALUE_TEMPO.toDouble();
        }

        while (!aText.atEnd()) {
            aSectionHead = aText.readLine();

            // 没有结束或没有遇到下一节的开始就继续附加
            if (!aSectionHead.startsWith(SECTION_BEGIN_MARK) && !aText.atEnd()) {
                aCurrentSection.push_back(aSectionHead);
                continue;
            }

            aNote = QUtauSectionNote();
            if (aNote.LoadFromStringList(aCurrentSection)) {
                aNote.SetParent(this);

                // 设置全局影响的曲速
                aNote.SetDefaultValue(KEY_NAME_TEMPO, num_to_qstring(aCurrentTempo));
                aNote.GetValue(KEY_NAME_TEMPO, &aCurrentTempo);

                mSectionNotes.push_back(aNote);
            } else {
                // 跳过
            }

            aCurrentSection.clear();
            aCurrentSection.push_back(aSectionHead);
        }
        SetNeighborNotes();

        // aResult = NO_ERROR;
    } catch (...) {
        aResult = ERROR_INVALID_FORMAT;
    }

    return aResult;
}

bool QUtauData::Export(const QString &oPath) const {
    QStringList aStringList;
    QStringList::iterator p;

    QFile aOutFile(oPath);

    if (!aOutFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return 0;
    }

    QTextStream txtOutput(&aOutFile);

    mSectionSettings.Export(&aStringList);
    for (QVector<QUtauSectionNote>::size_type i = 0; i < mSectionNotes.size(); i++) {
        mSectionNotes[i].Export(&aStringList);
    }

    p = aStringList.begin();
    while (p != aStringList.end()) {
        txtOutput << *p << endl;
        p++;
    }

    aOutFile.close();
    return 1;
}

bool QUtauData::ExportTotal(const QString &oPath) const {
    QStringList aStringList;
    QStringList::iterator p;

    QFile aOutFile(oPath);

    if (!aOutFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return 0;
    }

    QTextStream txtOutput(&aOutFile);

    mSectionSettings.Export(&aStringList, true);
    for (QVector<QUtauSectionNote>::size_type i = 0; i < mSectionNotes.size(); i++) {
        if (i < SectionNotesNormalBegin() || i >= SectionNotesNormalEnd()) {
            mSectionNotes[i].Export(&aStringList, true);
        } else {
            // 先删再加
            aStringList.push_back(mSectionNotes[i].strSectionName(SECTION_NAME_DELETE));
            aStringList.push_back(mSectionNotes[i].strSectionName(SECTION_NAME_INSERT));
            mSectionNotes[i].ExportKeysAndValues(&aStringList, true);
        }
    }

    p = aStringList.begin();
    while (p != aStringList.end()) {
        txtOutput << *p << endl;
        p++;
    }

    aOutFile.close();
    return 1;
}

bool QUtauData::ExportToUst(const QString &oPath) {
    QStringList aStringList;
    QStringList::iterator p;

    QFile aOutFile(oPath);

    if (!aOutFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return 0;
    }

    QTextStream txtOutput(&aOutFile);

    mSectionSettings.RemoveKey(KEY_NAME_PROJECT); // 临时删除

    mSectionSettings.Export(&aStringList, true);
    for (QVector<QUtauSectionNote>::size_type i = SectionNotesNormalBegin();
         i < SectionNotesNormalEnd(); i++) {
        mSectionNotes[i].Export(&aStringList, true);
    }

    mSectionSettings.RestoreKey(KEY_NAME_PROJECT); // 取回

    p = aStringList.begin();
    while (p != aStringList.end()) {
        txtOutput << *p << endl;
        p++;
    }

    txtOutput << mSectionSettings.strSectionName(SECTION_NAME_TRACKEND) << endl;

    aOutFile.close();
    return 1;
}

int QUtauData::CountSectionNotes() const {
    return mSectionNotes.size();
}

int QUtauData::SectionNotesNormalBegin() const {
    if (!mSectionNotes.empty() && mSectionNotes[0].SectionName() == SECTION_NAME_PREV) {
        return 1;
    }
    return 0;
}

int QUtauData::SectionNotesNormalEnd() const {
    if (!mSectionNotes.empty() &&
        mSectionNotes[mSectionNotes.size() - 1].SectionName() == SECTION_NAME_NEXT) {
        return mSectionNotes.size() - 1;
    }
    return mSectionNotes.size();
}

bool QUtauData::hasPrevSpecialNote() const {
    return mSectionNotes[0].SectionName() == SECTION_NAME_PREV;
}

bool QUtauData::hasNextSpecialNote() const {
    return mSectionNotes[mSectionNotes.size() - 1].SectionName() == SECTION_NAME_NEXT;
}

const QUtauSectionVersion *QUtauData::SectionVersion() const {
    return &mSectionVersion;
}

const QUtauSectionSettings *QUtauData::SectionSettings() const {
    return &mSectionSettings;
}

const QUtauSectionNote *QUtauData::SectionNoteAt(int oIndex) const {
    if (oIndex < 0 || oIndex >= mSectionNotes.size()) {
        return NULL;
    }
    return &mSectionNotes[oIndex];
}

QUtauSectionVersion *QUtauData::SectionVersion() {
    return &mSectionVersion;
}

QUtauSectionSettings *QUtauData::SectionSettings() {
    return &mSectionSettings;
}

QUtauSectionNote *QUtauData::SectionNoteAt(int oIndex) {
    if (oIndex < 0 || oIndex >= mSectionNotes.size()) {
        return NULL;
    }
    return &mSectionNotes[oIndex];
}

bool QUtauData::AddSectionNote(const QUtauSectionNote &oSectionNote) {
    return InsertSectionNote(oSectionNote, CountSectionNotes());
}

bool QUtauData::InsertSectionNote(const QUtauSectionNote &oSectionNote, int oIndex) {
    QVector<QUtauSectionNote>::iterator p;

    if (oIndex < 0 || oIndex > mSectionNotes.size()) {
        return 0;
    }
    InvalidateNeighborNotes();

    p = mSectionNotes.insert(mSectionNotes.begin() + oIndex, oSectionNote);
    p->SetParent(this);
    SetNeighborNotes();

    return 1;
}

bool QUtauData::ReplaceSectionNote(const QUtauSectionNote &oSectionNote, int oIndex) {
    if (oIndex < 0 || oIndex >= mSectionNotes.size()) {
        return 0;
    }
    InvalidateNeighborNotes();
    mSectionNotes[oIndex] = oSectionNote;
    mSectionNotes[oIndex].SetParent(this);
    SetNeighborNotes();
    return 1;
}

bool QUtauData::RemoveSectionNote(int oIndex) {
    if (oIndex < 0 || oIndex >= mSectionNotes.size()) {
        return 0;
    }
    InvalidateNeighborNotes();
    mSectionNotes.erase(mSectionNotes.begin() + oIndex);
    SetNeighborNotes();
    return 1;
}

bool QUtauData::isGenonAnalyzed() {
    return GenonAnalyzed;
}

bool QUtauData::AnalyzeGenonSettings() {
    GenonAnalyzed = true;

    return mSectionSettings.AnalyzeGenonSettings();
}

bool QUtauData::InvalidateNeighborNotes() {
    for (QVector<QUtauSectionNote>::size_type i = 0; i < mSectionNotes.size(); i++) {
        mSectionNotes[i].SetNeighborNotes(NULL, NULL);
    }
    return 1;
}

bool QUtauData::SetNeighborNotes() {
    if (mSectionNotes.size() <= 1) {
        return 1;
    }

    mSectionNotes[0].SetNeighborNotes(NULL, &mSectionNotes[1]);

    for (QVector<QUtauSectionNote>::size_type i = 1; i < mSectionNotes.size() - 1; i++) {
        mSectionNotes[i].SetNeighborNotes(&mSectionNotes[i - 1], &mSectionNotes[i + 1]);
    }

    mSectionNotes[mSectionNotes.size() - 1].SetNeighborNotes(
        &mSectionNotes[mSectionNotes.size() - 2], NULL);

    return 1;
}
