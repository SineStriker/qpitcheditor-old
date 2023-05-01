#include "QUtauSection.h"

#include <QTime>

//===========================================================================
QUtauSection::QUtauSection() {
    mParent = NULL;
}

QUtauSection::~QUtauSection() {
}

//===========================================================================
// 设置或返回父级
void QUtauSection::SetParent(QUtauData *oParent) {
    mParent = oParent;
}

const QUtauData *QUtauSection::Parent() const {
    return mParent;
}

QUtauData *QUtauSection::Parent() {
    return mParent;
}

//===========================================================================
// 初始化
int QUtauSection::LoadFromStringList(const QStringList &oStringList) {
    QString aKey, aValue;
    QString aSectionName;

    if (oStringList.isEmpty()) {
        return 0;
    }
    if (!IsSectionLine(oStringList[0], &aSectionName)) {
        return 0; // 第一行不符合节名的语法（[#?]）
    }
    if (!IsValidSectionName(aSectionName)) {
        return 0; // 第一行不符合节名的格式（子类定标准）
    }

    mSectionName = aSectionName;
    for (QStringList::size_type i = 1; i < oStringList.size(); ++i) {
        if (GetKeyAndValue(oStringList[i], &aKey, &aValue)) {
            mOriginalValueMap[aKey] = aValue; // 获取键值
            if (aValue.isEmpty()) {
                mLatestKeyMap[aKey] = MFM_OMITTED; // 默认值
            } else {
                mLatestKeyMap[aKey] = MFM_ORIGINAL; // 原始值
            }
        }
    }
    return 1;
}

//===========================================================================
// 设置节名
bool QUtauSection::SetSectionName(const QString &oSectionName) {
    mSectionName = oSectionName;
    return 1;
}

// 返回节名
QString QUtauSection::SectionName() const {
    return mSectionName;
}

//===========================================================================
// 获取键值
bool QUtauSection::GetValue(const QString &oKey, QString *oValue, int *oStatus) const {
    return GetValueCore(oKey, oValue, oStatus);
}

//---------------------------------------------------------------------------
bool QUtauSection::GetValue(const QString &oKey, int *oValue, int *oStatus) const {
    bool aResult;
    QString aValue;

    aResult = GetValueCore(oKey, &aValue, oStatus);
    if (!aResult) {
        return 0;
    }
    *oValue = aValue.toInt();
    return 1;
}

//---------------------------------------------------------------------------
bool QUtauSection::GetValue(const QString &oKey, double *oValue, int *oStatus) const {
    bool aResult;
    QString aValue;

    aResult = GetValueCore(oKey, &aValue, oStatus);
    if (!aResult) {
        return 0;
    }
    *oValue = aValue.toDouble();
    return 1;
}

//---------------------------------------------------------------------------
// 获取键的默认值
bool QUtauSection::GetDefaultValue(const QString &oKey, QString *oValue) const {
    QMap<QString, QString>::const_iterator aDVItr;

    aDVItr = mDefaultValueMap.find(oKey);

    if (aDVItr == mDefaultValueMap.end()) {
        return 0; // 本体中不存在这样的键
    }

    *oValue = aDVItr.value();
    return 1;
}

//---------------------------------------------------------------------------
// 获取键的原始值
bool QUtauSection::GetOriginalValue(const QString &oKey, QString *oValue) const {
    QMap<QString, QString>::const_iterator aDVItr;

    aDVItr = mOriginalValueMap.find(oKey);

    if (aDVItr == mOriginalValueMap.end()) {
        return 0; // 原始值中不存在这样的键
    }

    *oValue = aDVItr.value();
    return 1;
}

//===========================================================================
// 设定键值
bool QUtauSection::SetValue(const QString &oKey, const QString &oValue) {
    return SetValueCore(oKey, oValue);
}

bool QUtauSection::SetValue(const QString &oKey, int oValue) {
    return SetValueCore(oKey, num_to_qstring(oValue));
}

bool QUtauSection::SetValue(const QString &oKey, double oValue) {
    return SetValueCore(oKey, num_to_qstring(oValue));
}

//---------------------------------------------------------------------------
// 删除键
bool QUtauSection::RemoveKey(const QString &oKey) {
    if (!mLatestKeyMap.contains(oKey)) {
        return 0;
    }
    if (mLatestKeyMap[oKey] == MFM_ORIGIN_REMOVED || mLatestKeyMap[oKey] == MFM_MODIFY_REMOVED ||
        mLatestKeyMap[oKey] == MFM_NEW_REMOVED) {
        return 0;
    }
    if (mLatestKeyMap[oKey] == MFM_NEW) {
        mLatestKeyMap[oKey] = MFM_NEW_REMOVED;
    } else if (mLatestKeyMap[oKey] == MFM_ORIGINAL) {
        mLatestKeyMap[oKey] = MFM_ORIGIN_REMOVED;
    } else {
        mLatestKeyMap[oKey] = MFM_MODIFY_REMOVED;
    }
    return 1;
}

//---------------------------------------------------------------------------
// 恢复键
bool QUtauSection::RestoreKey(const QString &oKey) {
    if (!mLatestKeyMap.contains(oKey)) {
        return 0;
    }
    if (!(mLatestKeyMap[oKey] == MFM_ORIGIN_REMOVED || mLatestKeyMap[oKey] == MFM_MODIFY_REMOVED ||
          mLatestKeyMap[oKey] == MFM_NEW_REMOVED)) {
        return 0;
    }
    if (mLatestKeyMap[oKey] == MFM_NEW_REMOVED) {
        mLatestKeyMap[oKey] = MFM_NEW;
    } else if (mLatestKeyMap[oKey] == MFM_ORIGIN_REMOVED) {
        mLatestKeyMap[oKey] = MFM_ORIGINAL;
    } else {
        mLatestKeyMap[oKey] = MFM_MODIFIED;
    }
    return 1;
}

//---------------------------------------------------------------------------
bool QUtauSection::isKeyExist(const QString &oKey) const {
    if (!mLatestKeyMap.contains(oKey)) {
        return 0;
    }
    if (mLatestKeyMap[oKey] == MFM_ORIGIN_REMOVED || mLatestKeyMap[oKey] == MFM_MODIFY_REMOVED ||
        mLatestKeyMap[oKey] == MFM_NEW_REMOVED) {
        return 0;
    } else {
        return 1;
    }
}

//---------------------------------------------------------------------------
// 默认化
bool QUtauSection::MakeDefault(const QString &oKey) {
    if (!isKeyExist(oKey)) {
        return 0;
    }

    QString aDefaultValue;
    if (!GetDefaultValue(oKey, &aDefaultValue)) {
        return 0;
    }

    SetValue(oKey, aDefaultValue);
    return 1;
}

bool QUtauSection::MakeOmitted(const QString &oKey) {
    if (!isKeyExist(oKey)) {
        return 0;
    }
    mLatestKeyMap[oKey] = MFM_OMITTED;

    return 1;
}

//===========================================================================
// 设置某个键的状态为没有改过
bool QUtauSection::Revert(const QString &oKey) {
    QMap<QString, int>::iterator it;

    it = mLatestKeyMap.find(oKey);
    if (it == mLatestKeyMap.end() || it.value() == MFM_ORIGINAL) {
        return 0; // 说明这个键本来就没改过
    }

    if (it.value() == MFM_NEW || it.value() == MFM_NEW_REMOVED) {
        // 新建出来的
        mLatestKeyMap.remove(oKey);
    } else {
        // 被改动的
        mLatestKeyMap[oKey] = MFM_ORIGINAL;
    }

    mUserValueMap.remove(oKey);
    return 1;
}

void QUtauSection::removeKeysWithDefaultValue() {
    QString aDefaultValue, aValue;
    bool aResult;
    QString aKey;

    QMap<QString, int>::iterator it;

    for (it = mLatestKeyMap.begin(); it != mLatestKeyMap.end(); it++) {
        aKey = it.key();

        aResult = GetDefaultValue(aKey, &aDefaultValue);
        if (!aResult) {
            continue;
        }

        GetValue(aKey, &aValue);
        if (aValue == aDefaultValue) {
            RemoveKey(aKey);
        }
    }
}

QString QUtauSection::strSectionName(const QString &oName) const {
    if (oName == "") {
        return SECTION_BEGIN_MARK + SectionName() + SECTION_END_MARK;
    } else {
        return SECTION_BEGIN_MARK + oName + SECTION_END_MARK;
    }
}

//---------------------------------------------------------------------------
// 导出
void QUtauSection::Export(QStringList *oStringList, bool exportAll) const {
    oStringList->push_back(strSectionName());
    ExportKeysAndValues(oStringList, exportAll);
}

// 导出下面的键
void QUtauSection::ExportKeysAndValues(QStringList *oStringList, bool exportAll) const {
    QMap<QString, int>::const_iterator it;
    QString aValue;
    int aStatus;

    if (exportAll) {
        for (it = mLatestKeyMap.begin(); it != mLatestKeyMap.end(); it++) {
            if (it.value() == MFM_ORIGIN_REMOVED || it.value() == MFM_MODIFY_REMOVED ||
                it.value() == MFM_NEW_REMOVED) {
                continue;
            }

            aStatus = MFM_OMITTED;
            GetValue(it.key(), &aValue, &aStatus);
            if (aStatus != MFM_OMITTED) {
                oStringList->push_back(it.key() + "=" + aValue);
            }
        }
    } else {
        for (it = mLatestKeyMap.begin(); it != mLatestKeyMap.end(); it++) {
            if (it.value() != MFM_MODIFIED && it.value() != MFM_NEW) {
                continue;
            }

            aStatus = MFM_OMITTED;
            GetValue(it.key(), &aValue, &aStatus);
            if (aStatus != MFM_OMITTED) {
                oStringList->push_back(it.key() + "=" + aValue);
            } else {
                oStringList->push_back(it.key() + "=");
            }
        }
    }
}

//===========================================================================
// 检查是否符合格式
bool QUtauSection::IsValidSectionName(const QString &oSectionName) const {
    return true; // 抽象类不具有判断标准
}

// 返回保存默认键值的 Map
const QMap<QString, QString> &QUtauSection::OriginalValueMap() const {
    return mOriginalValueMap;
}

//===========================================================================
// 获取键值（外部获取的桥梁）
bool QUtauSection::GetValueCore(const QString &oKey, QString *oValue, int *oStatus) const {
    int aResult = MFM_OMITTED; // 假设没有出现
    bool aIsDefault = false;

    // 先检查这个键是否出现过
    QMap<QString, int>::const_iterator it = mLatestKeyMap.find(oKey);

    if (it != mLatestKeyMap.end()) {
        aResult = it.value();
        // 被更改了
        switch (aResult) {
        case MFM_ORIGINAL: // 原始值
            *oValue = mOriginalValueMap.find(oKey).value();
            break;
        case MFM_MODIFIED: // 修改后的值
        case MFM_NEW:      // 新增的值
            *oValue = mUserValueMap.find(oKey).value();
            break;
        default:
            break;
        }
    }

    // 找到了直接返回
    if (aResult != MFM_OMITTED) {
        if (oStatus != NULL) {
            *oStatus = aResult;
        }
        return 1;
    }

    // 否则进入 默认 Map 中查找，找不到说明不存在
    aIsDefault = GetDefaultValue(oKey, oValue);

    if (aIsDefault) {
        if (oStatus != NULL) {
            *oStatus = aResult;
        }
        return 1;
    }

    return 0;
}

//---------------------------------------------------------------------------
// 设定键值（外部调用的桥梁）
bool QUtauSection::SetValueCore(const QString &oKey, const QString &oValue) {
    if (mLatestKeyMap.find(oKey) != mLatestKeyMap.end()) {
        // 存在这个键
        if (mLatestKeyMap[oKey] == MFM_ORIGINAL || mLatestKeyMap[oKey] == MFM_ORIGIN_REMOVED ||
            mLatestKeyMap[oKey] == MFM_MODIFY_REMOVED) {
            mLatestKeyMap[oKey] = MFM_MODIFIED;
        }
    } else {
        // 新建这个键
        mLatestKeyMap[oKey] = MFM_NEW;
    }
    mUserValueMap[oKey] = oValue;

    return 1;
}

//---------------------------------------------------------------------------
// 设置默认键值
void QUtauSection::SetDefaultValue(const QString &oKey, const QString &oValue) {
    mDefaultValueMap[oKey] = oValue;
}

//---------------------------------------------------------------------------
// 检查是否是节头
bool QUtauSection::IsSectionLine(const QString &oLine, QString *oSectionName) {
    if (oLine.indexOf(SECTION_BEGIN_MARK) != 0) {
        return 0;
    }
    *oSectionName =
        oLine.mid(SECTION_BEGIN_MARK.length(),
                  oLine.length() - (SECTION_BEGIN_MARK.length() + SECTION_END_MARK.length()));
    return 1;
}

//---------------------------------------------------------------------------
// 将一行保存为键名和键值
bool QUtauSection::GetKeyAndValue(const QString &oLine, QString *oKey, QString *oValue) const {
    QString::size_type aEqPos;
    if (oLine.isEmpty()) {
        return 0;
    }
    aEqPos = oLine.indexOf('=');
    if (aEqPos < 0) {
        *oKey = oLine;
        *oValue = "";
    } else {
        *oKey = oLine.mid(0, aEqPos);
        *oValue = oLine.mid(aEqPos + 1);
    }
    return 1;
}
//===========================================================================
