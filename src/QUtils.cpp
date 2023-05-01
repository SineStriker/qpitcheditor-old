#include "QUtils.h"
#include <QFontDatabase>
#include <QTextCodec>

#include "mainwindow.h"

void swapInt(int &a, int &b) {
    int t;
    t = a;
    a = b;
    b = t;
}

QString str2qstr(const string &str) {
    return QString::fromLocal8Bit(str.data());
}

string qstr2str(const QString &qstr) {
    QByteArray cdata = qstr.toLocal8Bit();
    return string(cdata);
}

int moveToCenter(QWidget *ptr) {
    QDesktopWidget *desktop = QApplication::desktop();
    ptr->move((desktop->width() - ptr->width()) / 2, (desktop->height() - ptr->height()) / 2);

    return 1;
}

bool isDirExist(QString fullPath) {
    QFileInfo fileInfo(fullPath);

    return fileInfo.isDir();
}

bool isFileExist(QString fullFileName) {
    QFileInfo fileInfo(fullFileName);

    return fileInfo.isFile();
}

void deleteFile(QString fileName) {
    QFileInfo FileInfo(fileName);

    if (FileInfo.isFile()) {
        QFile::remove(fileName);
    }
}

void deleteFilesWithPrefix(QString strDir, QString prefix) {
    QDir dir;
    QFileInfoList fileList;
    QFileInfo curFile;

    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setPath(strDir);

    if (!dir.exists()) {
        return;
    }

    fileList = dir.entryInfoList();

    for (int i = fileList.size() - 1; i >= 0; i--) {
        curFile = fileList[i];
        if (curFile.fileName().startsWith(prefix)) {
            QFile fileTemp(curFile.filePath());
            fileTemp.remove();
        }
    }
}

bool isPathRelative(QString fullPath) {
    QFileInfo fileInfo(fullPath);

    return fileInfo.isRelative();
}

QString getWorkPath() {
    return QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
}

QString getWorkPathUp2() {
    QString absolutePath;

    absolutePath = getWorkPath();
    for (int i = 0; i < 2; ++i) {
        absolutePath = absolutePath.mid(0, absolutePath.lastIndexOf('\\'));
    }

    return absolutePath;
}

QString PathFindFileName(QString &s) {
    return s.mid(s.lastIndexOf("\\") + 1);
}

QString PathFindFileName(QString s, QString folder) {
    if (folder.rightRef(1) != "\\") {
        folder += "\\";
    }
    if (s.size() < folder.size()) {
        return "";
    } else {
        return s.mid(folder.size());
    }
}

void violentMakeDir(QString fullPath) {
    QDir dir(fullPath);

    if (dir.exists()) {
        return;
    } else {
        dir.mkpath(fullPath);
    }
}

void violentDelDir(QString fullPath) {
    QDir dir(fullPath);

    if (dir.exists()) {
        dir.removeRecursively();
    }
}

QPixmap *createBitmapFromSVG(QString fullpath, QSize size) {
    QSvgRenderer svgRender(fullpath);

    QPixmap *bmp;

    // qDebug() << "SVG Default Size" << svgRender.defaultSize();

    bmp = new QPixmap(size);

    bmp->fill(Qt::transparent); //设置背景透明

    QPainter painter(bmp);
    painter.setRenderHint(QPainter::Antialiasing, true);

    svgRender.render(&painter);

    return bmp;
}

void pathCosineTo(QPainterPath &path, QPointF prev, QPointF curr) {
    double ratio = 0.364212423249; // 贝塞尔趋近正弦比例系数

    QPointF assist1 = QPointF(prev.x() * (1 - ratio) + curr.x() * ratio, prev.y());
    QPointF assist2 = QPointF(prev.x() * ratio + curr.x() * (1 - ratio), curr.y());

    path.cubicTo(assist1, assist2, curr);
}

void pathCosineJTo(QPainterPath &path, QPointF prev, QPointF curr) {
    double ratioX = 342.0 / 942.0;

    double ratioA = 305.0 / 942.0;
    double ratioB = 305.0 / 600.0;

    QPointF assist1 = QPointF(prev.x() * (1 - ratioX) + curr.x() * ratioX, prev.y());

    QPointF assist2 = QPointF(prev.x() * ratioA + curr.x() * (1 - ratioA),
                              prev.y() * ratioB + curr.y() * (1 - ratioB));

    path.cubicTo(assist1, assist2, curr);
}

void pathCosineRTo(QPainterPath &path, QPointF prev, QPointF curr) {

    double ratioX = 342.0 / 942.0;

    double ratioA = 305.0 / 942.0;
    double ratioB = 305.0 / 600.0;

    QPointF assist1 = QPointF(curr.x() * ratioA + prev.x() * (1 - ratioA),
                              curr.y() * ratioB + prev.y() * (1 - ratioB));

    QPointF assist2 = QPointF(curr.x() * (1 - ratioX) + prev.x() * ratioX, curr.y());

    path.cubicTo(assist1, assist2, curr);
}

double Hypotenuse(double x1, double x2, double y1, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

/*int pointTypeNum(string s) {
    if (s == "x" || s.empty()) {
        return 0;
    } else if (s == "s") {
        return 1;
    } else if (s == "r") {
        return 2;
    } else {
        return 3;
    }
}*/

bool isNumber(QString s, bool considerDot, bool considerNeg) {
    bool flag = true;

    for (QString::size_type i = 0; i < s.size(); ++i) {
        if ((s[i] >= '0' && s[i] <= '9') || (considerDot && s[i] == '.') ||
            (considerNeg && s[i] == '-')) {
            // is Number
        } else {
            flag = false;
            break;
        }
    }

    return flag;
}

QString strToMD5(QString text) {
    QByteArray aBytes;
    aBytes = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Md5);
    QString tempFilenameMD5(aBytes.toHex());

    return tempFilenameMD5;
}


QString fileToMD5(QString fileName) {
    QFile aFile(fileName);
    if (!aFile.open(QIODevice::ReadOnly)) {
        return "";
    }

    QByteArray aBytes = QCryptographicHash::hash(aFile.readAll(), QCryptographicHash::Md5);
    aFile.close();
    QString tempContentMD5(aBytes.toHex());

    return tempContentMD5;
}

bool combineFile(QString oFile1, QString oFile2, QString oFile3) {
    QFile aFile1(oFile1);
    QFile aFile2(oFile2);
    QFile aFile3(oFile3);

    if (aFile1.open(QIODevice::ReadOnly) && aFile2.open(QIODevice::ReadOnly) &&
        aFile3.open(QIODevice::WriteOnly)) {

    } else {
        return 0;
    }

    QByteArray bytes1, bytes2;
    bytes1 = aFile1.readAll();
    bytes2 = aFile2.readAll();

    aFile3.write(bytes1 + bytes2);

    aFile1.close();
    aFile2.close();
    aFile3.close();

    return 1;
}

QFont normalFont() {
    return uiFont(15);
}

QFont normalHFont() {
    return uiFont(18);
}

QFont uiFont(int pixel) {
    QFont font(GeneralFontName);
    font.setPixelSize(pixel);

    // font.setPointSize(pixel * 0.6);

    // QFont::PreferAntialias	0x0080	antialias if possible.
    // font.setStyleStrategy(QFont::PreferBitmap);
    // font.setWeight(63);

    return font;
}

QString loadFontFamilyFromFiles(const QString &fontFileName) {
    static QHash<QString, QString> tmd;
    if (tmd.contains(fontFileName)) {
        return tmd.value(fontFileName);
    }
    QString font = "";
    QFile fontFile(fontFileName);

    if (!fontFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Open font file error";
        return "";
    }

    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);

    qDebug() << loadedFontFamilies;



    if (!loadedFontFamilies.empty()) {
        font = loadedFontFamilies.at(0);
    }
    fontFile.close();

    if (!(font.isEmpty()))
        tmd.insert(fontFileName, font);
    return font;
}

void setLabelColor(QLabel *label, QColor c) {

    if (c == Qt::transparent) {
        c = MainWindow::normalTextColor;
    }

    QPalette palette;
    palette.setColor(QPalette::WindowText, c);

    label->setPalette(palette);
}

void fillBackground(QWidget *w, QColor c) {
    QPalette palette;
    palette.setColor(QPalette::Background, c);

    w->setPalette(palette);
}

QString systemToJis(QString s) {
    QTextCodec *code = QTextCodec::codecForName("Shift-JIS");
    QByteArray encodedString = code->fromUnicode(s);
    QString str = QString::fromLocal8Bit(encodedString);

    return str;
}

QString JisToSystem(QString s) {
    QTextCodec *code = QTextCodec::codecForName("Shift-JIS");
    QByteArray encodedString = s.toLocal8Bit();
    QString str = code->toUnicode(encodedString);

    return str;
}
