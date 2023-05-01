#include "UtaTranslator.h"

QString UtaTranslator::LoadingBar(int n, int sum) {
    int i, x;
    QString s = "";

    x = n * 40 / sum;

    for (i = 1; i <= 40; ++i) {

        if (i <= x) {
            s += '#';
        } else {
            s += '-';
        }
    }

    s += '(' + QString::number(n) + '/' + QString::number(sum) + ')';

    return s;
}

QString UtaTranslator::convertEnvelope(const QString &s, const double &overlap) {
    int i;
    QString env;
    QVector<QString> vals;

    QString strOverlap = QString::number(overlap);

    if (s.isEmpty()) {
        return ("0 5 35 0 100 100 0 " + strOverlap);
    }

    vals = qstring_to_qvector_qstring(s);

    for (i = 0; i < vals.size(); ++i) {
        if (vals[i] == NODEF_STRING) {
            if (i == 8) {
                vals[i] = "0"; // v4
            } else if (i == 9) {
                vals[i] = vals[1]; // p5 = p2
            } else if (i == 10) {
                vals[i] = vals[4]; // v5 = v2
            }
        }
    }

    // 有第五个点则补全
    if (vals.size() == 10) {
        vals.push_back("100");
    }

    // 删掉母音结合符号
    if (vals.size() >= 8) {
        vals.erase(vals.begin() + 7);
    }

    env = vals[0];

    for (i = 1; i < vals.size(); ++i) {
        env += " " + vals[i];

        // 重叠
        if (i == 6) {
            env += " " + strOverlap;
        }
    }

    return env;
}

QStringList UtaTranslator::convertEnvelopeFromVector(const QVector<QCtrlPoint> &tpoints,
                                                     const double &overlap) {
    QString strOverlap = QString::number(overlap);
    QStringList listEnv;

    if (tpoints.size() < 4) {
        listEnv << "0"
                << "5"
                << "35"
                << "0"
                << "100"
                << "100"
                << "0" << strOverlap;
        return listEnv;
    }

    listEnv << QString::number(tpoints[0].mX);
    listEnv << QString::number(tpoints[1].mX);
    listEnv << QString::number(tpoints[tpoints.size() - 2].mX);
    listEnv << QString::number(tpoints[0].mY);
    listEnv << QString::number(tpoints[1].mY);
    listEnv << QString::number(tpoints[tpoints.size() - 2].mY);
    listEnv << QString::number(tpoints[tpoints.size() - 1].mY);
    listEnv << strOverlap;
    if (tpoints.size() == 5) {
        listEnv << QString::number(tpoints[tpoints.size() - 1].mX);
        listEnv << QString::number(tpoints[2].mX);
        listEnv << QString::number(tpoints[2].mY);
    } else if (tpoints[tpoints.size() - 1].mX != 0) {
        listEnv << QString::number(tpoints[tpoints.size() - 1].mX);
    }
    return listEnv;
}

QString UtaTranslator::fixFlags(const QString &s) {

    QString s2 = "";

    for (QString::size_type i = 0; i < s.size(); ++i) {
        if (s[i] == '\"') {
            continue;
        }
        if (s[i] == 'e' || s[i] == 'E') {
            s2 += '/';
        }
        s2 += s[i];
    }

    return s2;
}

double UtaTranslator::getCorrectPBSY(const int &prevNoteNum, const QString &prevLyric,
                                     const int &curNoteNum) {
    double y1 = (prevNoteNum <= 0) ? 0 : double((prevNoteNum - curNoteNum) * 10);

    if (isRestNoteLyric(prevLyric)) {
        y1 = 0;
    }

    return y1;
}

QVector<QCtrlPoint> UtaTranslator::defaultPitch(const int &prevNoteNum, const QString &prevLyric,
                                                const int &curNoteNum) {

    double y1 = getCorrectPBSY(prevNoteNum, prevLyric, curNoteNum);

    QCtrlPoint first(0, y1);
    QCtrlPoint second(0, 0);

    second.mP = sType;

    QVector<QCtrlPoint> pitch = {first, second};

    return pitch;
}

QString UtaTranslator::compatibleFileName(const QString &filename) {
    QString s;

    for (QString::size_type i = 0; i < filename.size(); ++i) {
        switch (filename[i].toLatin1()) {
        case ' ': // 空格
            s += '+';
            break;

        case '\\': // 反斜杠
        case '/':  // 斜杠
            s += '_';
            break;

        case '*': // 星号
            s += '$';
            break;

        case '?': // 问号
            s += '=';
            break;

        case ':': // 冒号
        case '|': // 竖线
        case '>': // 大于号
        case '<': // 小于号
            break;

        default:
            s += filename[i];
        }
    }

    return s;
}
