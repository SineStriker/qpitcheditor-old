//
// Created by Functioner on 2020/12/9.
//
#include "UtaPitchCurves.h"
#include <cmath>

double UtaPitchCurves::f_x(const double &x1, const double &y1, const double &x2, const double &y2,
                           const double &x) {
    if (x1 == x2) {
        return y1;
    }

    return (y1 - y2) / 2 * cos(PI * (x - x1) / (x2 - x1)) + (y1 + y2) / 2;
}


double UtaPitchCurves::f_r(const double &x1, const double &y1, const double &x2, const double &y2,
                           const double &x) {
    if (x1 == x2) {
        return y1;
    }
    return (y2 - y1) * cos(PI / 2 / (x2 - x1) * (x - x2)) + y1;
}


double UtaPitchCurves::f_j(const double &x1, const double &y1, const double &x2, const double &y2,
                           const double &x) {
    if (x1 == x2) {
        return y1;
    }
    return (y1 - y2) * cos(PI / 2 / (x2 - x1) * (x - x1)) + y2;
}


double UtaPitchCurves::f_s(const double &x1, const double &y1, const double &x2, const double &y2,
                           const double &x) {
    if (x1 == x2) {
        return y1;
    }
    return (y2 - y1) / (x2 - x1) * (x - x1) + y1;
}

double UtaPitchCurves::f_type(const ControlPointType &ptype, const double &x1, const double &y1,
                              const double &x2, const double &y2, const double &x) {
    int impact;

    switch (ptype) {
    case lineType:
        impact = int(f_s(x1, y1, x2, y2, x));
        break;
    case jType:
        impact = int(f_j(x1, y1, x2, y2, x));
        break;
    case rType:
        impact = int(f_r(x1, y1, x2, y2, x));
        break;
    default:
        impact = int(f_x(x1, y1, x2, y2, x));
        break;
    }

    return impact;
}

UtaPitchCurves::UtaPitchCurves() {
}

UtaPitchCurves::~UtaPitchCurves() {
}

QVector<int> UtaPitchCurves::convert_from_vector_point(
    const double &tempo1, const QVector<QCtrlPoint> &curNote, const QVector<double> &curVBR,
    const double &curPre, const double &curStp, const int &curLength,
    const QVector<QCtrlPoint> &nextNote, const QVector<double> &nextVBR, const double &nextPre,
    const double &nextOve, const int &nextLength, const QVector<QCtrlPoint> &prevNote,
    const QVector<double> &prevVBR, const int &prevLength) {

    // Mode 2 转 Mode 1 原则
    // 1. 先行声音部分，用前一个音符曲速（伪命题）
    // 2. 其余所有部分，用自己的曲速
    // 3. 先行声音部分，可能受前一个音符的音高线影响
    // 4. 其余所有部分，可能受后一个音符的音高线影响

    // curNote 存自己的控制点，nextNote 存后一个音符的控制点，prevNote 存前一个音符的控制点
    // tempo 存自己的曲速，tempo2 存前一个音符的曲速

    QVector<int> PitchBend;
    double duration, nextStart, pbstart;
    int basePitch, prevImpact, nextImpact;
    QVector<QCtrlPoint>::size_type i, j, k; // 自身，后继，前驱的计数器

    double tick;

    double curTempo = tempo1;
    double prevTempo = tempo1;

    // 不算先行声音的长度
    duration = double(curLength) + (-nextPre + nextOve) * curTempo / 60 * 480 / 1000;
    // 后一个音符开始影响
    nextStart =
        (!nextNote.empty()) ? (curLength + nextNote[0].mX * curTempo / 60 * 480 / 1000) : INFINITY;

    i = 0;
    j = 0;
    k = 0;

    // 按照前一个音符的曲速，把起始点设在前一个音上（PBStart 以 stp + pre 为起始）
    pbstart = -(curPre + curStp) * prevTempo / 60 * 480 / 1000;
    tick = pbstart;

    while (tick < duration) {
        prevImpact = 0;
        nextImpact = 0;

        basePitch = findImpact(curNote, i, tick, curTempo, prevTempo, curVBR, curLength);

        // 后一个音符影响的部分
        if (tick >= nextStart) {
            if (j < nextNote.size() - 1) {
                nextImpact = findImpact(nextNote, j, tick - curLength, curTempo, curTempo, nextVBR,
                                        nextLength);
            }
            nextImpact += -nextNote[0].mY * 10;
        }

        // 前一个音符影响的部分
        if (tick <= 0) {
            prevImpact = findImpact(prevNote, k, tick + prevLength, prevTempo, prevTempo, prevVBR,
                                    prevLength);
        }

        // 附加前后音符音高线的影响
        PitchBend.push_back(basePitch + prevImpact + nextImpact);
        tick = tick + 5;
    }

    // 删除后面多余的 0
    while (PitchBend.size() >= 7 && PitchBend.back() == 0) {
        PitchBend.pop_back();
    }

    return PitchBend;
}

QString UtaPitchCurves::encode_single_num(int n) {
    QChar x, y;
    QString result;

    //若为负则以12位二进制取反
    if (n < 0) {
        n += 4096;
    }

    //转化为64进制，十位为x，个位为y，并映射为Base64字符
    x = QChar(Base64EncodeMap[int(n / 64)]);
    y = QChar(Base64EncodeMap[n % 64]);

    result.append(x);
    result.append(y);

    return result;
}

int UtaPitchCurves::findImpact(const QVector<QCtrlPoint> &portamento,
                               QVector<QCtrlPoint>::size_type &startIndex, int curTick,
                               const double &PositiveTempo, double NegativeTempo,
                               const QVector<double> &vibrato, const int &length) {

    // portamento: Mode2 音高曲线点列
    // startIndex: 从第几个点开始查找
    // curTick: 当前 tick 数
    // PositiveTempo: ticks 为正时使用的曲速
    // NegativeTempo: ticks 为负时使用的曲速（作为前一个音符的曲速，常速曲与前者一致）
    // vibrato: 颤音序列，可以没有

    if (startIndex < 0) {
        return 0;
    }

    int basePitch;
    QVector<QCtrlPoint>::size_type &i = startIndex; // 自身计数器
    double x1, y1, x2, y2;
    ControlPointType ptype;
    bool tooLeft;

    basePitch = 0;
    // 查找滑音
    if (!portamento.empty()) {
        tooLeft = false;

        while (i < portamento.size() - 1) {
            // 上一个控制点
            x1 = portamento[i].mX * ((portamento[i].mX < 0) ? NegativeTempo : PositiveTempo) / 60 *
                 480 / 1000;

            if (curTick < x1) {
                if (i > 0) {
                    i--; // startIndex 不正确，需要向前找
                    continue;
                } else if (i == 0) {
                    tooLeft = true;
                    break;
                }
            }

            y1 = portamento[i].mY * 10;

            // 下一个控制点
            x2 = portamento[i + 1].mX *
                 ((portamento[i + 1].mX < 0) ? NegativeTempo : PositiveTempo) / 60 * 480 / 1000;

            y2 = portamento[i + 1].mY * 10;

            ptype = portamento[i + 1].mP;

            if (curTick > x2) {
                i++;
                continue;
            }
            basePitch = f_type(ptype, x1, y1, x2, y2, curTick);
            break;
        }
        if (tooLeft) {
            basePitch = int(portamento[0].mY * 10); // 还没到第一个点
        }
    }

    // 查找颤音
    if (vibrato.size() >= 8 && length > 0) {
        double proportion = vibrato[0];
        double period = vibrato[1];
        double amplitude = vibrato[2];
        double easeIn = vibrato[3];
        double easeOut = vibrato[4];
        double phase = vibrato[5];
        double offset = vibrato[6];

        proportion /= 100.0;

        double tick_time = period * PositiveTempo / 60 * 480 / 1000; // ticks 周期
        double tick_length = proportion * length;                    // 颤音 ticks 时长
        double tick_start = (1 - proportion) * length; // 颤音相对开头 ticks 时间

        double ratio;
        double x, k, p, y;

        k = 1 / tick_time * 2 * PI; // 圆频率
        p = phase / 100.0 * 2 * PI; // 初相位

        easeIn = easeIn / 100.0 * tick_length;         // 淡入时间
        easeOut = (1 - easeOut / 100.0) * tick_length; // 淡出时间
        offset /= 100.0;

        x = curTick - tick_start;       // tick 所在横坐标
        y = amplitude * sin(k * x - p); // 原始颤音

        if (x > 0 && x < tick_length) {
            ratio = 1;
            // 套上偏移
            y += offset * amplitude;
            // 计算包络
            if (x < easeIn) {
                ratio *= x / easeIn;
            }
            if (x > easeOut) {
                ratio *= 1 - (x - easeOut) / (tick_length - easeOut);
            }
            // 套上包络
            y = ratio * y;
            // 作用
            basePitch += y;
        }
    }

    return basePitch;
}

QString UtaPitchCurves::encode_from_vector(const QVector<int> &pitchBend) {
    QVector<int>::size_type pos = 0;
    int count = 0;
    int curInt;
    int prevInt = INT32_MIN;
    QString result;

    while (pos < pitchBend.size()) {
        pos++;
        curInt = (pitchBend[pos - 1] == NODEF_INT) ? 0 : pitchBend[pos - 1];

        if (curInt == prevInt) {
            ++count;
            //最后一次处理
            if (pos == pitchBend.size()) {
                if (count >= 2) {
                    result += "#" + QString::number(count) + "#";
                } else {
                    result += encode_single_num(prevInt);
                }
            }
        } else {
            if (count != 0) {
                //连续出现n次（n>=3）则后面n-1次用#n-1代替
                if (count >= 2) {
                    result += "#" + QString::number(count) + "#";
                } else {
                    result += encode_single_num(prevInt);
                }
                count = 0;
            }
            result += encode_single_num(curInt);
        }

        prevInt = curInt;
    }

    return result;
}
