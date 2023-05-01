#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTranslator>

#include "QUtils.h"
#include "mainwindow.h"

MainWindow *Root = nullptr;
QApplication *App = nullptr;
QSharedMemory *Memory = nullptr;
QString GeneralFontName = "Microsoft YaHei";

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    QSharedMemory s;

    int exitCode;

    // 加载字体
    /*QString fontFamily = loadFontFamilyFromFiles("D:\\NotoSansCJKjp-Regular.otf");
    if (!fontFamily.isEmpty()) {
        GeneralFontName = fontFamily;
    }*/

    // 初始化变量
    Root = &w;
    App = &a;
    Memory = &s;

    a.setFont(normalFont());
    w.applyLanguage();

    // 参数个数判断
    if (argc < 2) {
        QString content = QObject::tr("No parameters. Please run QPitchEditor in UTAU.");
        w.msgboxExclaim(content);
        return 1;

    } else {
        // 防止多开
        s.setKey(appMemoryKey);

        if (s.attach()) {
            QString content = QObject::tr("QPitchEditor is running!");

            w.msgboxExclaim(content);
            return 0;
        }

        // 创建共享内存
        s.create(1);

        // 读取字符串
        QString filename = str2qstr(argv[1]);
        w.loadTempFile(filename);
    }

    exitCode = a.exec();

    // 销毁共享内存
    s.detach();

    return exitCode;
}

/*int main(int argc, char *argv[]) {
    QApplication *a = new QApplication(argc, argv);
    MainWindow *w = new MainWindow();
    QSharedMemory *s = new QSharedMemory();

    int exitCode;

    // 初始化变量
    Root = w;
    App = a;
    Memory = s;

    a->setFont(normalFont());
    w->applyLanguage();

    // 参数个数判断
    if (argc < 2) {
        QString content = QObject::tr("No parameters. Please run QPitchEditor in UTAU.");
        w->msgboxExclaim(content);
        return 1;

    } else {
        // 防止多开
        s->setKey(appMemoryKey);

        if (s->attach()) {
            QString content = QObject::tr("QPitchEditor is running!");

            w->msgboxExclaim(content);
            return 0;
        }

        // 创建共享内存
        s->create(1);

        // 读取字符串
        QString filename = str2qstr(argv[1]);
        w->loadTempFile(filename);
    }
    exitCode = a->exec();

    // 销毁共享内存
    s->detach();

    s->deleteLater();
    w->deleteLater();
    a->deleteLater();
    return exitCode;
}*/
