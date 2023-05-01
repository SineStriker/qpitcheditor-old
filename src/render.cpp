#include "mainwindow.h"

// 音符颜色
QColor MainWindow::note_lyricNameColor = Qt::black;

// 曲线颜色
// static int cc = 0xCC;
static int cc = 0xFF;

QColor MainWindow::mode1_solidLineColor_enabled = Qt::white;
QColor MainWindow::mode2_solidLineColor_enabled = Qt::white;

QColor MainWindow::mode1_solidLineColor_disabled = QColor(cc, cc, cc, 192);
QColor MainWindow::mode2_solidLineColor_disabled = QColor(cc, cc, cc, 192);

QColor MainWindow::mode1_dashLineColor_enabled = QColor(255, 255, 255, 192);
QColor MainWindow::mode2_dashLineColor_enabled = QColor(255, 255, 255, 192);

QColor MainWindow::mode1_dashLineColor_disabled = QColor(cc, cc, cc, 192);
QColor MainWindow::mode2_dashLineColor_disabled = QColor(cc, cc, cc, 192);

QColor MainWindow::mode2_invalidLineColor_enabled = QColor(255, 0, 0, 72);
QColor MainWindow::mode2_invalidLineColor_disabled = QColor(255, 0, 0, 30);

QColor MainWindow::vibrato_buttonBorderColor = QColor(255, 140, 0);
QColor MainWindow::vibrato_buttonFillColor = QColor(0x232323);
QColor MainWindow::vibrato_frameColor = vibrato_buttonBorderColor;

QColor MainWindow::overlap_lineColor = QColor(0, 255, 0, 128);
QColor MainWindow::preUttr_lineColor = QColor(255, 0, 0, 128);
QColor MainWindow::reflect_fillColor = QColor(255, 255, 255, 50);

QColor MainWindow::parameters_textColor = QColor(255, 255, 255, 120);

// 控制点颜色
QColor MainWindow::pitchPoint_coreColor = QColor(Qt::white);
QColor MainWindow::pitchPoint_ringColor = QColor(Qt::white);

QColor MainWindow::volumePoint_coreColor = Qt::white;
QColor MainWindow::volumePoint_ringColor = Qt::white;

QColor MainWindow::editor_timeLineColor = QColor(0x1C1C1C);
QColor MainWindow::editor_sectionLineColor = QColor(0x101010);

QColor MainWindow::editor_pitchLineColor = QColor(0x101010);
QColor MainWindow::editor_levelLineColor = QColor(0x101010);
QColor MainWindow::editor_backLowColor = QColor(0x232323);
QColor MainWindow::editor_backHighColor = QColor(0x2D2D2D);

QColor MainWindow::envelope_solidLineColor_enabled = Qt::white;
QColor MainWindow::envelope_solidLineColor_disabled = QColor(255, 255, 255, 192);

QColor MainWindow::envelope_dashLineColor_enabled = Qt::white;
QColor MainWindow::envelope_dashLineColor_disabled = QColor(255, 255, 255, 192);

QColor MainWindow::envelope_fillColor_enabled = QColor(255, 255, 255, 96);
QColor MainWindow::envelope_fillColor_disabled = QColor(255, 255, 255, 48);

QColor MainWindow::volume_backColor = QColor(0x2E2E2E);
QColor MainWindow::volume_timeLineColor = editor_timeLineColor;
QColor MainWindow::volume_sectionLineColor = editor_sectionLineColor;
QColor MainWindow::volume_standardLineColor = QColor(0x111111);

int MainWindow::RENDER_InitializeProcess() {
    const QString defaultParams = "AA#5#";

    const QUtauSectionSettings *aSettings = aData.SectionSettings();

    int rangeBegin = playingRange.x();
    int rangeOver = playingRange.y();
    int rangeLength = rangeOver - rangeBegin + 1;
    double aFirstTempo = mainWidget->Interface()->getNoteAt(rangeBegin)->getTempo();

    QString aMode2;

    // 获取 Mode 2
    aMode2 = "False";
    if (aSettings->GetValue("Mode2", &aMode2)) {
    }

    aMode2 = !Root->bLineMode ? "True" : "False"; // 直接读取面板

    // 获取 前缀后缀表
    QString aLyric, aFileName, aFlag, aToneName;

    // 音高线用
    double aCurTempo;
    double aPreUttr, aOverlap, aStartPoint;
    double aNextPreUttr, aNextOverlap;

    int aPrevLength, aNextLength;

    QString aPrevLyric;
    int aPrevNoteNum, aNextNoteNum;

    // 原音设定
    double aBlank, aConst, aOffset;

    // 计算长度
    double aDurationFix, aRealLength;
    double aDuration; // 削长度
    QString aFullCacheName;

    // 属性
    int aMod, aVel, aInt, aLength;

    int aNoteNum;
    QString aCacheName;

    QStringList aEnvelope;

    QVector<QCtrlPoint> aPitch, aNextPitch, aPrevPitch;
    QVector<double> aVibrato, aNextVibrato, aPrevVibrato;

    QVector<QCtrlPoint> aEnvelopeArr;
    QVector<int> aPitchArrMode1, aPitchArrMode2;

    QString setParams;

    QString strVibrato;
    QString strMode1Pitch;

    QString aSuperiorFile;
    QString aSuperiorAlias;

    // 写入temp.bat
    SimpleNote *CurrentNote, *PreviousNote, *NextNote;

    QString tempBatPath = renderingDir + "\\temp.bat";
    QString helperBatPath = renderingDir + "\\temp_helper.bat";
    QString cacheDirPath = renderingDir + "\\" + cacheFolderName;

    QTextStream fs;
    QFile tempFile(tempBatPath);

    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return -2;
    }

    fs.setDevice(&tempFile);

    // temp.bat初始配置
    fs << "@set loadmodule=" << endl;
    fs << "@set tempo=" << aFirstTempo << endl;
    fs << "@set samples=" << 44100 << endl;
    fs << "@set oto=" << aVoiceDir << endl;
    fs << "@set tool=" << aTool1Path << endl;
    fs << "@set resamp=" << aTool2Path << endl;
    fs << "@set output=" << tempAudioName << endl;
    fs << "@set helper="
       << "temp_helper.bat" << endl;
    fs << "@set cachedir=" << cacheDirPath << endl;
    fs << "@set flag="
       << "\"" << aGlobalFlags << "\"" << endl;
    fs << "@set env="
       << "0 5 35 0 100 100 0" << endl;
    fs << "@set stp=" << 0 << endl;

    fs << endl;
    fs << "@del \"%output%\" 2>nul" << endl;
    fs << "@mkdir \"%cachedir%\" 2>nul" << endl;
    fs << endl;
    fs << endl;

    // 顺序遍历音符

    for (int i = rangeBegin; i <= rangeOver; i++) {
        // 临时变量
        strVibrato = strMode1Pitch = "";
        aSuperiorFile = aSuperiorAlias = "";

        // 始终存在的条目
        aLyric = "";
        aNoteNum = 60;
        aLength = 480;

        // 音高线用
        aPreUttr = aOverlap = aStartPoint = 0;
        aNextPreUttr = aNextOverlap = 0;
        aPrevLength = aNextLength = 480;

        aPrevNoteNum = aNextNoteNum = 0;
        aPrevLyric = "";

        aPitch = aNextPitch = aPrevPitch = {};
        aVibrato = aNextVibrato = aPrevVibrato = {};
        aPitchArrMode1 = aPitchArrMode2 = {};

        // 原音设定
        aFileName = "";
        aBlank = aConst = aOffset = 0;

        // 其他属性
        aFlag = "";
        aMod = 0;
        aVel = 100;
        aInt = 100;

        setParams = "";
        aEnvelopeArr = {};

        CurrentNote = mainWidget->Interface()->getNoteAt(i);
        PreviousNote = CurrentNote->m_prev;
        NextNote = CurrentNote->m_next;

        // 曲速
        aCurTempo = CurrentNote->getTempo();

        // 上一个音符
        if (PreviousNote) {
            aPrevLyric = PreviousNote->m_note.Lyric(); // 歌词

            aPrevNoteNum = PreviousNote->m_note.m_noteNum;
            aPrevLength = PreviousNote->m_note.m_length;

            aPrevPitch = PreviousNote->m_note.Mode2Pitch; // Mode2 音高控制点
            aPrevVibrato = PreviousNote->m_note.m_vbr;    // Mode2 颤音

            if (!aPrevPitch.isEmpty()) {
                // 层层套娃
                SimpleNote *NoteBeforePrev = PreviousNote->m_prev;
                if (NoteBeforePrev) {
                    QString tmpLyric = NoteBeforePrev->m_note.m_lyric;
                    int tmpNoteNum = NoteBeforePrev->m_note.m_noteNum;

                    double y1 = UtaTranslator::getCorrectPBSY(tmpNoteNum, tmpLyric, aPrevNoteNum);
                    aPrevPitch[0].mY = y1;
                }
            }
        }

        // 当前音符
        {
            CorrectGenon curGenon = CurrentNote->getCorrectGenon();

            aLyric = CurrentNote->m_note.Lyric();     // 歌词
            aNoteNum = CurrentNote->m_note.m_noteNum; // 音阶
            aLength = CurrentNote->m_note.m_length;   // 长度

            aPreUttr = curGenon.PreUtterance;  // 先行声音
            aOverlap = curGenon.VoiceOverlap;  // 重叠
            aStartPoint = curGenon.StartPoint; // STP

            aPitch = CurrentNote->m_note.Mode2Pitch; // Mode2 音高控制点
            aVibrato = CurrentNote->m_note.m_vbr;    // Mode2 颤音

            // 校正第一个点的纵坐标
            if (!aPitch.isEmpty()) {
                double y1 = UtaTranslator::getCorrectPBSY(aPrevNoteNum, aPrevLyric, aNoteNum);
                aPitch[0].mY = y1;
            }
        }

        // 下一个音符
        if (NextNote) {
            CorrectGenon nextGenon = NextNote->getCorrectGenon();

            aNextNoteNum = NextNote->m_note.m_noteNum; // 音阶
            aNextLength = NextNote->m_note.m_length;   // 长度

            aNextPreUttr = nextGenon.PreUtterance; // 先行声音
            aNextOverlap = nextGenon.VoiceOverlap; // 重叠

            aNextPitch = NextNote->m_note.Mode2Pitch; // Mode2 音高控制点
            aNextVibrato = NextNote->m_note.m_vbr;    // Mode2 颤音

            // 校正第一个点的纵坐标
            if (!aNextPitch.isEmpty()) {
                double y1 = UtaTranslator::getCorrectPBSY(aNoteNum, aLyric, aNextNoteNum);
                aNextPitch[0].mY = y1;
            }
        }

        // 当前音符
        aVibrato = CurrentNote->m_note.m_vbr;

        aMod = CurrentNote->getModulation(); // 移调
        aInt = CurrentNote->getIntensity();  // 音量
        aVel = CurrentNote->m_note.m_vel;    // 辅音速度
        aFlag = CurrentNote->m_note.m_flags; // Flags

        // 音高曲线
        if (aMode2 == "True") {
            // 没有音高线就添加默认音高线
            if (aPitch.isEmpty()) {
                aPitch = UtaTranslator::defaultPitch(aPrevNoteNum, aPrevLyric, aNoteNum);
            }

            // 复杂的 Mode2 转 Mode1 过程
            aPitchArrMode2 = UtaPitchCurves::convert_from_vector_point(
                aCurTempo, aPitch, aVibrato, aPreUttr, aStartPoint, aLength, aNextPitch,
                aNextVibrato, aNextPreUttr, aNextOverlap, aNextLength, aPrevPitch, aPrevVibrato,
                aPrevLength);

            qDebug() << aLyric << aPitchArrMode2;

            setParams = UtaPitchCurves::encode_from_vector(aPitchArrMode2);
        } else {
            setParams = defaultParams;
        }

        // OTO
        if (!aLyric.isEmpty()) {
            QGenonSettings aGenon;
            if (aSettings->GenonSettingsFind(aLyric, &aGenon, aNoteNum)) {
                aFileName = aGenon.mFileName;
                aBlank = aGenon.mBlank;
                aConst = aGenon.mShiin;
                aOffset = aGenon.mOffset;
            } else {
                aFileName = aVoiceDir + "\\" + aLyric + ".wav";
            }
        }

        // 整合全局Flags与音符Flags
        aFlag = aFlag + aGlobalFlags;
        aFlag = UtaTranslator::fixFlags(aFlag);

        // 音阶名
        aToneName = tone_number_to_tone_name(aNoteNum);

        // 包络
        aEnvelope = UtaTranslator::convertEnvelopeFromVector(CurrentNote->m_note.m_env, aOverlap);

        // 长度校正
        aDuration = (double(aLength) / 480 * 60 / aCurTempo * 1000); // 由 ticks 换算长度
        aDurationFix = aPreUttr - aNextPreUttr + aNextOverlap;

        aRealLength = aDuration + aDurationFix + aStartPoint + 50;
        aRealLength = (aRealLength < aConst) ? aConst : aRealLength;
        aRealLength = int((aRealLength + 25) / 50) * 50;

        // 缓存名
        aCacheName = QString::number(i + 1) + "_" + UtaTranslator::compatibleFileName(aLyric) +
                     "_" + aToneName + "_" + QString::number(aLength) + ".wav";
        aFullCacheName = cacheDirPath + "\\" + aCacheName;

        if (isRestNoteLyric(aLyric)) {
            fs << "@\"%tool%\" \"%output%\" \"%oto%\\R.wav\" 0";
            fs << " " << aLength << "@" << aCurTempo << (aDurationFix >= 0 ? "+" : "")
               << aDurationFix; // 静止长度
            fs << " 0 0";
            fs << endl;

            QStringList list;
            // 重采样数据清空
            if (i >= resamplerArguments.size()) {
                resamplerArguments.resize(i + 1);
                resamplerArguments[i].clear();
            } else {
                if (!resamplerArguments[i].isEmpty()) {
                    deleteFilesWithPrefix(cacheDirPath, QString::number(i + 1));
                    resamplerArguments[i].clear();
                }
            }
            // 写入合成数据
            list.append(tempAudioName);
            list.append(aVoiceDir + "\\R.wav");
            list.append("0");
            list.append(QString::number(aLength) + "@" + QString::number(aCurTempo) +
                        (aDurationFix >= 0 ? "+" : "") + QString::number(aDurationFix));
            list.append("0");
            list.append("0");

            if (i >= wavtoolArguments.size()) {
                wavtoolArguments.resize(i + 1);
            }
            wavtoolArguments[i] = list;

        } else {
            fs << "@echo " << UtaTranslator::LoadingBar(i - rangeBegin + 1, rangeLength) << endl;

            fs << "@set params=" << aInt << " " << aMod << " "
               << "!" << aCurTempo << " " << setParams << endl;
            fs << "@set flag="
               << "\"" << aFlag << "\"" << endl;
            fs << "@set env=" << aEnvelope.join(" ") << endl;

            fs << "@set stp=" << aStartPoint << endl;
            fs << "@set vel=" << aVel << endl;
            fs << "@set temp="
               << "\"%cachedir%\\" << aCacheName << "\"" << endl;

            // 传递参数
            fs << "@call"
               << " "
               << "\"%helper%\"";
            fs << " "
               << "\"%oto%\\" << PathFindFileName(aFileName, aVoiceDir) << "\""; // 原始声音文件名
            fs << " " << aToneName;                                              // 音阶名称
            fs << " " << aLength << "@" << aCurTempo << (aDurationFix >= 0 ? "+" : "")
               << aDurationFix;    // 静止长度
            fs << " " << aPreUttr; // 先行发声
            fs << " " << aOffset;  // 偏移（右侧蓝色区域）
            fs << " " << aRealLength;
            fs << " " << aConst;
            fs << " " << aBlank;  // 空白（右侧蓝色区域）
            fs << " " << (i + 1); // 序号

            fs << endl;

            QStringList list;
            // 将重采样参数写入系列字符串
            list.append(aFileName);                    // 第一个参数：文件名音源
            list.append(aFullCacheName);               // 第二个参数：输出文件名
            list.append(aToneName);                    // 第三个参数：音阶
            list.append(QString::number(aVel));        // 第四个参数：辅音速度
            list.append(aFlag);                        // 第五个参数：Flags
            list.append(QString::number(aOffset));     // 第六个参数：偏移
            list.append(QString::number(aRealLength)); // 第七个参数：实际长度
            list.append(QString::number(aConst));      // 第八个参数：固定长度
            list.append(QString::number(aBlank));      // 第九个参数：右侧空白
            list.append(QString::number(aInt));        // 第十个参数：音量
            list.append(QString::number(aMod));        // 第十一个参数：移调
            list.append("!" + QString::number(aCurTempo)); // 第十二个参数：音高曲线字符串组合
            list.append(setParams);

            if (i >= resamplerArguments.size()) {
                resamplerArguments.resize(i + 1);
                resamplerArguments[i] = list;
            } else {
                if (resamplerArguments[i] != list) {
                    // 如果不一样就删了
                    deleteFilesWithPrefix(cacheDirPath, QString::number(i + 1));
                    resamplerArguments[i] = list;
                }
            }

            // 将合成参数写入字符串
            list.clear();

            list.append(tempAudioName);                // 第一个参数：输出文件
            list.append(aFullCacheName);               // 第二个参数：输入文件
            list.append(QString::number(aStartPoint)); // 第三个参数：STP
            // 第四个参数：校正长度
            list.append(QString::number(aLength) + "@" + QString::number(aCurTempo) +
                        (aDurationFix >= 0 ? "+" : "") + QString::number(aDurationFix));
            list.append(aEnvelope); // 包络线

            if (i >= wavtoolArguments.size()) {
                wavtoolArguments.resize(i + 1);
            }
            wavtoolArguments[i] = list;
        }
        // 传递参数结束

        fs << endl;
    }

    fs << endl;

    fs << "@if not exist \"%output%.whd\" goto E" << endl;
    fs << "@if not exist \"%output%.dat\" goto E" << endl;
    fs << "copy /Y \"%output%.whd\" /B + \"%output%.dat\" /B \"%output%\"" << endl;
    fs << "del \"%output%.whd\"" << endl;
    fs << "del \"%output%.dat\"" << endl;
    fs << ":E" << endl;

    tempFile.close();

    // 写入temp_helper.bat
    QFile helperFile(helperBatPath);

    if (!helperFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return -2;
    }

    fs.setDevice(&helperFile);

    fs << "@if exist %temp% goto A" << endl;
    fs << "@if exist \"%cachedir%\\%9_*.wav\" del \"%cachedir%\\%9_*.wav\"" << endl;
    fs << "@\"%resamp%\" %1 %temp% %2 %vel% %flag% %5 %6 %7 %8 %params%" << endl;
    fs << ":A" << endl;
    fs << "@\"%tool%\" \"%output%\" %temp% %stp% %3 %env%" << endl;

    helperFile.close();

    return 0;
}
