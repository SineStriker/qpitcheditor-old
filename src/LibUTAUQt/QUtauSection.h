#ifndef QUTAUSECTION_H
#define QUTAUSECTION_H

/*===========================================================================
    UTAU Sequence Text - 划分节抽象类
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

//---------------------------------------------------------------------------

#include <QDebug>
#include <QObject>
#include <QTextStream>

#include "QOtoIni.h"
#include "QUtauConstants.h"
#include "QUtauUtils.h"

#include "QUtauStrings.h"

class QUtauData;
class QUtauSection {
public:
    // 构造
    QUtauSection();

    // 析构
    virtual ~QUtauSection();

    // 设置或返回父级
    void SetParent(QUtauData *oParent);
    const QUtauData *Parent() const;
    QUtauData *Parent();

    // 初始化
    virtual int LoadFromStringList(const QStringList &oStringList);

    // 设置或返回节名
    virtual bool SetSectionName(const QString &oSectionName);
    QString SectionName() const;

    virtual QString strSectionName(const QString &oName = "") const;

    // 获取节中的键值
    bool GetValue(const QString &oKey, QString *oValue, int *oStatus = NULL) const;
    bool GetValue(const QString &oKey, int *oValue, int *oStatus = NULL) const;
    bool GetValue(const QString &oKey, double *oValue, int *oStatus = NULL) const;

    // 获取键的默认值
    bool GetDefaultValue(const QString &oKey, QString *oValue) const;

    // 获取键的原始值
    bool GetOriginalValue(const QString &oKey, QString *oValue) const;

    // 设定节中的键值
    bool SetValue(const QString &oKey, const QString &oValue);
    bool SetValue(const QString &oKey, int oValue);
    bool SetValue(const QString &oKey, double oValue);

    // 设定键的默认值
    void SetDefaultValue(const QString &oKey, const QString &oValue);

    // 设置某个键不导出/撤销
    bool RemoveKey(const QString &oKey);
    bool RestoreKey(const QString &oKey);

    // 判断某个键是否正常
    bool isKeyExist(const QString &oKey) const;

    // 设定某个键为默认值
    bool MakeDefault(const QString &oKey);

    // 设定某个键为清空状态
    bool MakeOmitted(const QString &oKey);

    // 设置某个键的状态为没有改过
    bool Revert(const QString &oKey);

    // 将键值为默认值的键排除
    virtual void removeKeysWithDefaultValue();

    // 导出为字符串序列
    virtual void Export(QStringList *oStringList, bool exportAll = false) const;
    virtual void ExportKeysAndValues(QStringList *oStringList, bool exportAll = false) const;

protected:
    // 检查节名是否符合语法
    virtual bool IsValidSectionName(const QString &oSectionName) const;

    // 返回保存默认键值的 Map
    const QMap<QString, QString> &OriginalValueMap() const;

    // 获取键值（外部调用的桥梁）
    virtual bool GetValueCore(const QString &oKey, QString *oValue, int *oStatus) const;

    // 设定键值（外部调用的桥梁）
    virtual bool SetValueCore(const QString &oKey, const QString &oValue);

private:
    // 检查是否是节头
    static bool IsSectionLine(const QString &oLine, QString *oSectionName);

    // 将一行保存为键名和键值
    bool GetKeyAndValue(const QString &oLine, QString *oKey, QString *oValue) const;

    QUtauData *mParent;                       // 父级
    QMap<QString, QString> mOriginalValueMap; // 保存原始键值
    QMap<QString, QString> mDefaultValueMap;  // 保存默认键值
    QMap<QString, QString> mUserValueMap;     // 保存更改的键值
    QMap<QString, int> mLatestKeyMap;         // 保存键值的状态（是否更改）
    QString mSectionName;                     // 节名
};

#endif // QUTAUSECTION_H
