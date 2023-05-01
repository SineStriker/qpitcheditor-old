#include "ListArea.h"

ListArea *ListArea::Stage = nullptr;
BasicScrollArea *ListArea::Scroll = nullptr;

ListArea::ListArea(QWidget *parent) : EditorAreaWidget(parent) {
    initList();
}

void ListArea::reInitializeStrings() {
    btnHead1->setCaption(tr("Operations"));
    btnHead2->setCaption(tr("Properties"));
    btnHead3->setCaption(tr("Pitch"));
    btnHead4->setCaption(tr("General"));

    btnFillLyrics->setCaption(tr("Insert Lyrics"));
    btnInsertRest->setCaption(tr("Insert Rest"));
    btnAssemble->setCaption(tr("Group/Ungroup"));
    btnFind->setCaption(tr("Find/Replace"));
    btnFillRest->setCaption(tr("Fill Rests"));

    btnLyric->setCaption(tr("Lyric"));
    btnTone->setCaption(tr("Tone"));
    btnLength->setCaption(tr("Length"));
    btnIntensity->setCaption(tr("Intensity"));
    btnModulation->setCaption(tr("Modulation"));

    btnTempo->setCaption(tr("Tempo"));
    btnEnvelope->setCaption(tr("Envelope"));

    btnPortamento->setCaption(tr("Portamento"));
    btnVibrato->setCaption(tr("Vibrato"));

    btnProperties->setCaption(tr("Note Properties"));
    btnVoiceBank->setCaption(tr("Voice Config"));
}

void ListArea::initList() {
    int itemHeight = 25;

    bVisible1 = bVisible2 = bVisible3 = bVisible4 = true;

    mainLayout = new QVBoxLayout(this);

    // 新建按钮
    btnHead1 = new ToolMultiButton(tr("Operations"), this);
    btnHead2 = new ToolMultiButton(tr("Properties"), this);
    btnHead3 = new ToolMultiButton(tr("Pitch"), this);
    btnHead4 = new ToolMultiButton(tr("General"), this);

    btnFillLyrics = new ToolMultiButton(tr("Insert Lyrics"), this);
    btnInsertRest = new ToolMultiButton(tr("Insert Rest"), this);
    btnAssemble = new ToolMultiButton(tr("Group/Ungroup"), this);
    btnFind = new ToolMultiButton(tr("Find/Replace"), this);
    btnFillRest = new ToolMultiButton(tr("Fill Rests"), this);

    btnLyric = new ToolMultiButton(tr("Lyric"), this);
    btnTone = new ToolMultiButton(tr("Tone"), this);
    btnLength = new ToolMultiButton(tr("Length"), this);
    btnIntensity = new ToolMultiButton(tr("Intensity"), this);
    btnModulation = new ToolMultiButton(tr("Modulation"), this);

    btnTempo = new ToolMultiButton(tr("Tempo"), this);
    btnEnvelope = new ToolMultiButton(tr("Envelope"), this);

    btnPortamento = new ToolMultiButton(tr("Portamento"), this);
    btnVibrato = new ToolMultiButton(tr("Vibrato"), this);

    btnProperties = new ToolMultiButton(tr("Note Properties"), this);
    btnVoiceBank = new ToolMultiButton(tr("Voice Config"), this);

    btnHead1->setHead(true);
    btnHead2->setHead(true);
    btnHead3->setHead(true);
    btnHead4->setHead(true);

    // 确定高度
    btnHead1->setFixedHeight(itemHeight);
    btnHead2->setFixedHeight(itemHeight);
    btnHead3->setFixedHeight(itemHeight);
    btnHead4->setFixedHeight(itemHeight);

    btnFillLyrics->setFixedHeight(itemHeight);
    btnInsertRest->setFixedHeight(itemHeight);
    btnAssemble->setFixedHeight(itemHeight);
    btnFind->setFixedHeight(itemHeight);
    btnFillRest->setFixedHeight(itemHeight);

    btnLyric->setFixedHeight(itemHeight);
    btnTone->setFixedHeight(itemHeight);
    btnLength->setFixedHeight(itemHeight);
    btnIntensity->setFixedHeight(itemHeight);
    btnModulation->setFixedHeight(itemHeight);

    btnTempo->setFixedHeight(itemHeight);
    btnEnvelope->setFixedHeight(itemHeight);

    btnPortamento->setFixedHeight(itemHeight);
    btnVibrato->setFixedHeight(itemHeight);

    btnProperties->setFixedHeight(itemHeight);
    btnVoiceBank->setFixedHeight(itemHeight);

    // 加入排列
    mainLayout->addWidget(btnHead1);
    mainLayout->addWidget(btnFillLyrics);
    mainLayout->addWidget(btnInsertRest);
    mainLayout->addWidget(btnAssemble);
    mainLayout->addWidget(btnFind);
    mainLayout->addWidget(btnFillRest);

    mainLayout->addWidget(btnHead2);
    mainLayout->addWidget(btnLyric);
    mainLayout->addWidget(btnTone);
    mainLayout->addWidget(btnLength);
    mainLayout->addWidget(btnIntensity);
    mainLayout->addWidget(btnModulation);

    mainLayout->addWidget(btnTempo);
    mainLayout->addWidget(btnEnvelope);

    mainLayout->addWidget(btnHead3);
    mainLayout->addWidget(btnPortamento);
    mainLayout->addWidget(btnVibrato);

    mainLayout->addWidget(btnHead4);
    mainLayout->addWidget(btnProperties);
    mainLayout->addWidget(btnVoiceBank);

    this->setLayout(mainLayout);
    fillBackground(this);

    adjustSize();

    // 连接事件
    connect(btnHead1, &ToolMultiButton::clicked, this, &ListArea::onHead1Click);
    connect(btnHead2, &ToolMultiButton::clicked, this, &ListArea::onHead2Click);
    connect(btnHead3, &ToolMultiButton::clicked, this, &ListArea::onHead3Click);
    connect(btnHead4, &ToolMultiButton::clicked, this, &ListArea::onHead4Click);
}

void ListArea::resizeEvent(QResizeEvent *event) {
    emit onReizeEvent();
}

void ListArea::onHead1Click() {
    bVisible1 = !bVisible1;

    btnFillLyrics->setVisible(bVisible1);
    btnInsertRest->setVisible(bVisible1);
    btnAssemble->setVisible(bVisible1);
    btnFind->setVisible(bVisible1);
    btnFillRest->setVisible(bVisible1);

    adjustSize();
}

void ListArea::onHead2Click() {
    bVisible2 = !bVisible2;

    btnLyric->setVisible(bVisible2);
    btnTone->setVisible(bVisible2);
    btnLength->setVisible(bVisible2);
    btnIntensity->setVisible(bVisible2);
    btnModulation->setVisible(bVisible2);

    btnTempo->setVisible(bVisible2);
    btnEnvelope->setVisible(bVisible2);

    adjustSize();
}

void ListArea::onHead3Click() {
    bVisible3 = !bVisible3;

    btnPortamento->setVisible(bVisible3);
    btnVibrato->setVisible(bVisible3);

    adjustSize();
}

void ListArea::onHead4Click() {
    bVisible4 = !bVisible4;

    btnProperties->setVisible(bVisible4);
    btnVoiceBank->setVisible(bVisible4);

    adjustSize();
}
