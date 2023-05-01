#ifndef ONLINEUPDATER_H
#define ONLINEUPDATER_H

#include <QObject>
#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "QUtils.h"

class OnlineUpdater : public QObject {
    Q_OBJECT
public:
    explicit OnlineUpdater(QWidget *parent);
    ~OnlineUpdater();

    void setVersion(QString oVersion);

    int setFileName(QString oFileName);
    int setOnlineUrl(QString oUrl);

    void executeUpdate();
    void fetchDownloader();

    void setAutoDelete(bool value) {
        m_autoDelete = value;
    }
    bool autoDelete() {
        return m_autoDelete;
    }
    void setShowLatest(bool value) {
        m_showLatest = value;
    }
    bool showLatest() {
        return m_showLatest;
    }

    // 比较版本号
    static bool CompareVersion(const QString &oOrgVersion, const QString &oNewVersion);

    static QString updateWindowTitle;

private:
    QWidget *m_parent;

    QString m_version;
    QString m_content;

    QNetworkAccessManager *m_manager; //定义网络请求对象
    QNetworkReply *m_reply;

    bool m_error;
    bool m_autoDelete;
    bool m_showLatest;

    QString m_downloader;
    QString m_md5;

    int parseUpdateJSON(); // 解析JSON

signals:
    void updateTriggered();

private slots:
    void onReplyFinished(); //网络数据接收完成槽函数的声明
    void onErrorOccured(QNetworkReply::NetworkError error);
};

#endif // ONLINEUPDATER_H
