#include "QGenonSettings.h"

//===========================================================================
QGenonSettings::QGenonSettings() {
    mFileName = "";
    mAlias = "";
    mOffset = 0.0;
    mShiin = 0.0;
    mBlank = 0.0;
    mPreUtterance = 0.0;
    mVoiceOverlap = 0.0;
}

QGenonSettings::~QGenonSettings() {
}

QDebug operator<<(QDebug debug, const QGenonSettings &oGenon) {
    QString info = QString("QGenon(file:%1; alias:%2;)").arg(oGenon.mFileName, oGenon.mAlias);
    debug << info;
    return debug;
}
