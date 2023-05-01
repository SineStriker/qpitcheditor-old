#include <QApplication>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QProcess>

#include "OnlineUpdater.h"
#include "mainwindow.h"

QString OnlineUpdater::updateWindowTitle;

OnlineUpdater::OnlineUpdater(QWidget *parent) : m_parent(parent) {
    updateWindowTitle = tr("Check updates");

    m_reply = nullptr;
    m_manager = nullptr;

    m_autoDelete = false;
    m_showLatest = false;
}

OnlineUpdater::~OnlineUpdater() {
    qDebug() << "To remove updater.";
    if (m_reply) {
        m_reply->deleteLater();
    }
    if (m_manager) {
        m_manager->deleteLater();
    }
    m_reply = nullptr;
    m_manager = nullptr;
}

void OnlineUpdater::setVersion(QString oVersion) {
    m_version = oVersion;
}

// 从文件读取
int OnlineUpdater::setFileName(QString oFileName) {
    QFile aJsonFile;

    aJsonFile.setFileName(oFileName);
    if (!aJsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Root->msgboxError(tr("Cannot open file."), tr("Error"));
        return 0;
    }

    m_content = aJsonFile.readAll();
    aJsonFile.close();

    parseUpdateJSON();

    if (m_autoDelete) {
        this->deleteLater();
    }
    return 1;
}

// 从url读取
int OnlineUpdater::setOnlineUrl(QString oUrl) {
    m_error = false;
    m_manager = new QNetworkAccessManager(0);
    m_reply = m_manager->get(QNetworkRequest(QUrl(oUrl)));

    qDebug() << "Start to fetch data.";

    connect(m_reply, &QNetworkReply::finished, this, &OnlineUpdater::onReplyFinished);
    connect(m_reply, &QNetworkReply::errorOccurred, this, &OnlineUpdater::onErrorOccured);

    return 1;
}

void OnlineUpdater::executeUpdate() {
    QString workDir = getWorkPath();
    QString exePath = workDir + "\\" + "Downloader.exe";

    QString curMD5;
    bool needReload = false;

    qDebug() << "Latest Downloader:" << m_downloader << m_md5;

    if (!isFileExist(exePath)) {
        qDebug() << "The update module is missing!";
        needReload = true;
    } else {
        curMD5 = fileToMD5(exePath);
        qDebug() << "Current Downloader:" << exePath << curMD5;

        if (curMD5 != m_md5) {
            qDebug() << "Need to delete.";
            needReload = true;
        }
    }

    if (needReload) {
        fetchDownloader();
    }

    return;

    Root->msgboxInfo(
        tr("QPitchEditor will quit and you can restart it after the update process finishes."));

    QProcess *p = new QProcess(0);

    QFileInfo file(exePath);
    qDebug() << file.isFile();

    p->setProgram(exePath);
    p->setWorkingDirectory(workDir);
    p->start();

    // 此进程不删
    exit(0);
}

void OnlineUpdater::fetchDownloader() {
}

// 解析 JSON
int OnlineUpdater::parseUpdateJSON() {
    QJsonParseError aError;
    QByteArray aBytes = m_content.toUtf8();
    QJsonDocument aRootDoc = QJsonDocument::fromJson(aBytes, &aError);

    if (aError.error != QJsonParseError::NoError) {
        // 提示解析错误
        if (m_showLatest) {
            Root->msgboxError(tr("Server address error or JSON format error!"),
                                    updateWindowTitle);
        }
        return -1;
    }

    if (aRootDoc.isObject()) {
        QJsonObject aRootObj = aRootDoc.object(); //创建JSON对象，不是字符串

        QJsonObject ClientValue = aRootObj.value("Windows").toObject();

        QString NextVersion = ClientValue.value("LatestVersion").toString(); // 新版本号
        QString UpdateTime = ClientValue.value("UpdateTime").toString();     // 更新日期
        QString ReleaseNote = ClientValue.value("ReleaseNote").toString();   // 更新说明

        QString DownloaderMD5 = ClientValue.value("MD5").toString(); // 下载器MD5
        QString DownloaderUrl = ClientValue.value("Url").toString(); // 下载器链接

        if (CompareVersion(m_version, NextVersion)) {
            // 显示更新信息
            QString strDetected = tr("New version detected!");
            QString strVersion = tr("Latest version: ");
            QString strTime = tr("Updated date: ");
            QString strNote = tr("Release notes: ");

            QString content = strDetected + "\n" + strVersion + NextVersion + "\n" + strTime +
                              UpdateTime + "\n" + strNote + "\n" + ReleaseNote;

            int result = Root->msgboxInfoQuestion(content, updateWindowTitle);

            if (result == QMessageBox::Yes) //点击更新
            {
                // QDesktopServices::openUrl(QUrl(Url));
                m_downloader = DownloaderUrl;
                m_md5 = DownloaderMD5;
                executeUpdate();
            }
        } else {
            // 提示已是最新版本
            if (m_showLatest) {
                Root->msgboxInfo(tr("It is currently the latest version!"),
                                       updateWindowTitle);
            }
        }
    }
    return 0;
}

void OnlineUpdater::onReplyFinished() {

    m_content = m_reply->readAll();

    m_reply->deleteLater();
    m_manager->deleteLater();

    m_reply = nullptr;
    m_manager = nullptr;

    if (!m_error) {
        parseUpdateJSON();
    }
    if (m_autoDelete) {
        this->deleteLater();
    }
}

void OnlineUpdater::onErrorOccured(QNetworkReply::NetworkError error) {
    m_error = true;

    QString strError;
    qDebug() << error;

    // 提示访问网络错误
    if (m_showLatest) {
        switch (error) {
        case QNetworkReply::ContentNotFoundError:
            strError = tr("Update file not found!");
            break;
        default:
            strError = tr("Connection Error!");
            break;
        }

        Root->msgboxError(strError, updateWindowTitle);
    }
}

bool OnlineUpdater::CompareVersion(const QString &oOrgVersion, const QString &oNewVersion) {
    QStringList orgList = oOrgVersion.split(".");
    QStringList newList = oNewVersion.split(".");

    int i = 0;
    bool needUpdate = false;

    while (i < orgList.size() || i < newList.size()) {
        if (i < orgList.size() && i == newList.size()) {
            break;
        } else if (i == orgList.size() && i < newList.size()) {
            needUpdate = true;
            break;
        }
        if (orgList[i].toInt() < newList[i].toInt()) {
            needUpdate = true;
            break;
        }
        ++i;
    }
    return needUpdate;
}
