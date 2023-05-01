#ifndef LISTAREA_H
#define LISTAREA_H

#include <QListWidget>
#include <QPushButton>
#include <QScrollBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "BaseWidgets.h"
#include "Elements/CustomControls.h"

// 功能：填词，插入休止符，编组/解散，查找/替换，合并音符，填充休止符
// 单独音符属性：歌词（功能），音阶（功能），音长（功能），音量（功能），移调（功能）
// 组合音符属性：音量/移调/先行/重叠/辅音速度/Flags
// 滑音
// 颤音

class ListArea : public EditorAreaWidget {
    Q_OBJECT
public:
    explicit ListArea(QWidget *parent = nullptr);

    void reInitializeStrings();

    static ListArea *Stage;
    static BasicScrollArea *Scroll;

private:
    QVBoxLayout *mainLayout;

    ToolMultiButton *btnHead1, *btnHead2, *btnHead3, *btnHead4;

    ToolMultiButton *btnFillLyrics, *btnInsertRest, *btnAssemble, *btnFind, *btnFillRest;

    ToolMultiButton *btnLyric, *btnTone, *btnLength, *btnIntensity, *btnModulation, *btnTempo,
        *btnEnvelope;

    ToolMultiButton *btnPortamento, *btnVibrato;

    ToolMultiButton *btnProperties, *btnVoiceBank;

    bool bVisible1, bVisible2, bVisible3, bVisible4;

    void initList();

    void resizeEvent(QResizeEvent *event) override;

signals:
    void onReizeEvent();

private:
    void onHead1Click();
    void onHead2Click();
    void onHead3Click();
    void onHead4Click();
};

#endif // LISTAREA_H
