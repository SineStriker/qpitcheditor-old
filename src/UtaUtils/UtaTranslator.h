#include "LibUTAUQt/QUtauData.h"

#include "QUtils.h"
#include "UtaPitchCurves.h"

using namespace std;

class UtaTranslator {

public:
    static QString LoadingBar(int n, int sum);

    static QString convertEnvelope(const QString &s, const double &overlap);
    static QStringList convertEnvelopeFromVector(const QVector<QCtrlPoint> &tpoints,
                                                 const double &overlap);
    static QString fixFlags(const QString &s);
    static double getCorrectPBSY(const int &prevNoteNum, const QString &prevLyric,
                                 const int &curNoteNum);
    static QVector<QCtrlPoint> defaultPitch(const int &prevNoteNum, const QString &prevLyric,
                                            const int &curNoteNum);

    static QString compatibleFileName(const QString &filename);
};
