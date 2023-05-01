//
// Created by Functioner on 2020/12/9.
//

#ifndef MODE2PITCH_UTAPITCHCURVES_H
#define MODE2PITCH_UTAPITCHCURVES_H

#include "LibUTAUQt/QUtauData.h"

using namespace std;

#define PI 3.1415926

static const char Base64EncodeMap[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

class UtaPitchCurves {
    UtaPitchCurves();
    ~UtaPitchCurves();

public:
    static double f_x(const double &x1, const double &y1, const double &x2, const double &y2,
                      const double &x);

    static double f_r(const double &x1, const double &y1, const double &x2, const double &y2,
                      const double &x);

    static double f_j(const double &x1, const double &y1, const double &x2, const double &y2,
                      const double &x);

    static double f_s(const double &x1, const double &y1, const double &x2, const double &y2,
                      const double &x);

    static double f_type(const ControlPointType &ptype, const double &x1, const double &y1,
                         const double &x2, const double &y2, const double &x);

    static QVector<int> convert_from_vector_point(
        const double &tempo1, const QVector<QCtrlPoint> &curNote, const QVector<double> &curVBR,
        const double &curPre, const double &curStp, const int &curLength,

        const QVector<QCtrlPoint> &nextNote = {}, const QVector<double> &nextVBR = {},
        const double &nextPre = 0, const double &nextOve = 0, const int &nextLength = 0,

        const QVector<QCtrlPoint> &prevNote = {}, const QVector<double> &prevVBR = {},
        const int &prevLength = 0);

    static QString encode_from_vector(const QVector<int> &pitchBend);

    static QString encode_single_num(int n);

    static int findImpact(const QVector<QCtrlPoint> &portamento,
                          QVector<QCtrlPoint>::size_type &startIndex, int curTick,
                          const double &PositiveTempo, double NegativeTempo = 0,
                          const QVector<double> &vibrato = {}, const int &length = 0);
};

#endif // MODE2PITCH_UTAPITCHCURVES_H
