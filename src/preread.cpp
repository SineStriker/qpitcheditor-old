#include "mainwindow.h"

/*--------------------------------------预检测临时文件--------------------------------------*/
void MainWindow::initPath() {

    // 初始化
    aVoiceDir = aProjectPath = aTool1Path = aTool2Path = aGlobalFlags = "";

    // 预先测试临时文件
    QFile fs(aTempFile);

    if (isDirExist(aTempFile)) {
        QString title = childWindowTitle;
        QString content = tr("\"%1\" is a directory other than a file.").arg(aTempFile);

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    if (!fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString title = childWindowTitle;
        QString content = tr("Cannot find \"%1\".").arg(aTempFile);

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_NOT_FOUND);
    }

    QTextStream txtInput(&fs);
    QString line;

    bool isSetting = false;
    bool isInvalid = false;
    int eq;
    int cnt = 0;

    // 读取数据
    while (!txtInput.atEnd()) {
        line = txtInput.readLine();

        if (line.startsWith("[")) {
            if (isSetting) {
                break;
            } else if (line == "[#SETTING]") {
                isSetting = true;
            }
        }

        if (!isSetting) {

            // 十行内没有找到设定
            if (cnt > 10) {
                isInvalid = true;
                break;
            } else {
                cnt++;
            }

            continue;
        }

        // 在[#SETTING]中查找

        if (line.startsWith("Project=")) {
            eq = line.indexOf("=") + 1;
            aProjectPath = line.mid(eq, line.size() - eq);
        }

        else if (line.startsWith("VoiceDir=")) {
            eq = line.indexOf("=") + 1;
            aVoiceDir = line.mid(eq, line.size() - eq);
        }

        // 理论上以下条件都不可能满足，但如果在此之前添加也能检测到

        else if (line.startsWith("Tool1=")) {
            eq = line.indexOf("=") + 1;
            aTool1Path = line.mid(eq, line.size() - eq);
        }

        else if (line.startsWith("Tool2=")) {
            eq = line.indexOf("=") + 1;
            aTool2Path = line.mid(eq, line.size() - eq);
        }

        else if (line.startsWith("Flags=")) {
            eq = line.indexOf("=") + 1;
            aGlobalFlags = line.mid(eq, line.size() - eq);
        }
    }

    fs.close();

    if (isInvalid || !isSetting) {
        QString title = childWindowTitle;
        QString content = tr("Invalid file format!");

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    if (aVoiceDir == "") {

        QString title = childWindowTitle;
        QString content = tr("No singer.");

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);

    } else if (!isDirExist(aVoiceDir)) {

        QString title = childWindowTitle;
        QString content = tr("Voice directory \"%1\" not found.").arg(aVoiceDir);

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    // 没有找到Project键
    if (aProjectPath == "") {

        if (getConfig()) {
            // 一般情况可忽略，这是PlayInPlugin专属的默认配置，可自行编写
            // 如果不存在，则去setting.ini中寻找
        } else {
            aTool1Path = aTool2Path = aGlobalFlags = "";

            getSettingIni();
        }
    } else {
        getProjectUst();
    }

    qDebug() << "VoiceDir : " << aVoiceDir;
    qDebug() << "Wavtool : " << aTool1Path;
    qDebug() << "Resampler : " << aTool2Path;
}

/*--------------------------------------检测配置文件--------------------------------------*/
bool MainWindow::getConfig() {
    // 一旦失败则从setting.ini中获取
    QFile fs("PlayInPlugin.ini");

    if (!fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream txtInput(&fs);
    QString line;

    int eq;

    // 从PlayInPlugin.ini中取得两个工具的路径
    while (!txtInput.atEnd()) {

        line = txtInput.readLine();

        if (line.startsWith("Tool1=")) {
            eq = line.indexOf("=") + 1;
            aTool1Path = line.mid(eq, line.size() - eq);
        }

        else if (line.startsWith("Tool2=")) {
            eq = line.indexOf("=") + 1;
            aTool2Path = line.mid(eq, line.size() - eq);

        } else if (line.startsWith("Flags=")) {
            eq = line.indexOf("=") + 1;
            aGlobalFlags = line.mid(eq, line.size() - eq);
        }
    }

    fs.close();

    // 检查两个工具是否记录在PlayInPlugin.ini中
    if (aTool1Path == "" || aTool2Path == "") {
        return false;
    }

    // 获取当前绝对路径与setting.ini所在路径
    QString absolutePath = getWorkPathUp2();
    QString thisPath = getWorkPath();

    // 将引擎相对路径转为绝对路径
    QString tool1, tool2;

    // 如果以?开头说明是当前路径的相对路径
    if (aTool1Path.startsWith("?")) {
        aTool1Path = aTool1Path.mid(1, aTool1Path.size() - 1);
        tool1 = aTool1Path;
        tool1 = thisPath + "\\" + tool1;
    } else {
        tool1 = aTool1Path;
        if (isPathRelative(tool1)) {
            tool1 = absolutePath + "\\" + tool1;
        }
    }

    if (aTool2Path.startsWith("?")) {
        aTool2Path = aTool2Path.mid(1, aTool2Path.size() - 1);
        tool2 = aTool2Path;
        tool2 = thisPath + "\\" + tool2;
    } else {
        tool2 = aTool2Path;
        if (isPathRelative(tool2)) {
            tool2 = absolutePath + "\\" + tool2;
        }
    }

    // 检查两个工具是否存在
    if (!isFileExist(tool1) || !isFileExist(tool2)) {
        return false;
    }

    aTool1Path = tool1;
    aTool2Path = tool2;

    return true;
}

/*--------------------------------------检测UST文件--------------------------------------*/
void MainWindow::getProjectUst() {
    QFile fs(aProjectPath);

    if (!fs.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QString title = childWindowTitle;
        QString content = tr("Project file \"%1\" not found.").arg(aProjectPath);

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    QTextStream txtInput(&fs);
    QString line;

    bool isSetting = false;
    int eq;

    // 从setting.ini中取得两个工具的路径
    while (!txtInput.atEnd()) {

        line = txtInput.readLine();

        if (line.startsWith("[")) {
            if (isSetting) {
                break;
            } else if (line == "[#SETTING]") {
                isSetting = true;
            }
        }
        if (!isSetting) {
            continue;
        }

        // 在[#SETTING]中查找

        if (line.startsWith("Tool1=")) {
            eq = line.indexOf("=") + 1;
            aTool1Path = line.mid(eq, line.size() - eq);
        }

        else if (line.startsWith("Tool2=")) {
            eq = line.indexOf("=") + 1;
            aTool2Path = line.mid(eq, line.size() - eq);
        } else if (line.startsWith("Flags=")) {
            eq = line.indexOf("=") + 1;
            aGlobalFlags = line.mid(eq, line.size() - eq);
        }
    }

    fs.close();

    // 检查两个工具是否记录在project中
    if (aTool1Path == "" || aTool2Path == "") {

        QString title = childWindowTitle;
        QString content =
            tr("No tool information is found in \"%1\".").arg(aProjectPath);

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    // 获取当前绝对路径与setting.ini所在路径
    QString absolutePath = getWorkPathUp2();
    QString thisPath = getWorkPath();

    qDebug() << "This Dir : " << thisPath;
    qDebug() << "UTAU Dir : " << absolutePath;

    // 将引擎相对路径转为绝对路径
    QString tool1, tool2;
    tool1 = aTool1Path;
    tool2 = aTool2Path;

    if (isPathRelative(tool1)) {
        tool1 = absolutePath + "\\" + tool1;
    }
    if (isPathRelative(tool2)) {
        tool2 = absolutePath + "\\" + tool2;
    }

    // 检查两个工具是否存在
    if (!isFileExist(tool1) || !isFileExist(tool2)) {

        QString title = childWindowTitle;
        QString content =
            tr("Tool information found in \"%1\" is invalid.").arg(aProjectPath);

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    aTool1Path = tool1;
    aTool2Path = tool2;
}

/*--------------------------------------检测 Setting.ini--------------------------------------*/
void MainWindow::getSettingIni() {
    QFile fs("..\\..\\setting.ini");

    if (!fs.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QString title = childWindowTitle;
        QString content = tr("\"%1\" not found.").arg("setting.ini");

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    QTextStream txtInput(&fs);
    QString line;

    int eq;

    // 从setting.ini中取得两个工具的路径
    while (!txtInput.atEnd()) {
        line = txtInput.readLine();

        if (line.startsWith("Tool1Path=")) {
            eq = line.indexOf("=") + 1;
            aTool1Path = line.mid(eq, line.size() - eq);
        }

        else if (line.startsWith("Tool2Path=")) {
            eq = line.indexOf("=") + 1;
            aTool2Path = line.mid(eq, line.size() - eq);

        } else if (line.startsWith("Flags=")) {
            eq = line.indexOf("=") + 1;
            aGlobalFlags = line.mid(eq, line.size() - eq);
        }
    }

    fs.close();

    // 检查两个工具是否记录在setting.ini中
    if (aTool1Path == "" || aTool2Path == "") {

        QString title = childWindowTitle;
        QString content = tr("No tool information is found in \"%1\".").arg("setting.ini");

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    // 获取当前绝对路径与setting.ini所在路径
    QString absolutePath = getWorkPathUp2();
    QString thisPath = getWorkPath();

    // 将引擎相对路径转为绝对路径
    QString tool1, tool2;
    tool1 = aTool1Path;
    tool2 = aTool2Path;

    if (isPathRelative(tool1)) {
        tool1 = absolutePath + "\\" + tool1;
    }
    if (isPathRelative(tool2)) {
        tool2 = absolutePath + "\\" + tool2;
    }

    // 检查两个工具是否存在
    if (!isFileExist(tool1) || !isFileExist(tool2)) {
        QString title = childWindowTitle;
        QString content = tr("Tool information found in \"%1\" is invalid.").arg("setting.ini");

        QMessageBox::warning(NULL, title, content);
        exit(ERROR_FILE_INVALID);
    }

    aTool1Path = tool1;
    aTool2Path = tool2;
}
