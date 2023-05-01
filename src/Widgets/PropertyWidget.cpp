#include "PropertyWidget.h"
#include "mainwindow.h"

/*--------------------------------------上面板整体--------------------------------------*/
PropertyWidget::PropertyWidget(QWidget *parent) : SimpleForm(parent) {
    setWindowTitle(tr("Property Panel"));

    properties = new PropertyArea(this);

    setDefaultTitleBar();
    setWidget(properties);
    setTitle(tr("Property Panel"));

    getLayout()->setMargin(0);
    // getTitleBar()->setTop(0);

    setFocusPolicy(Qt::ClickFocus);
}

void PropertyWidget::reInitializeStrings() {
    setWindowTitle(tr("Property Panel"));
    setTitle(tr("Property Panel"));

    properties->reInitializeStrings();
}

/*--------------------------------------上面板中心区域--------------------------------------*/
PropertyArea::PropertyArea(QWidget *parent) : QWidget(parent) {

    leftScroll = new NoneScrollArea(this);
    leftWidget = new QWidget(leftScroll);

    rightScroll = new BasicScrollArea(this);
    rightWidget = new MoveWidget(rightScroll);

    rightScroll->setStyleSheet("QWidget{"
                               "background-color:transparent;"
                               "}"
                               "QLabel{"
                               "color:white;"
                               "}"
                               "QLineEdit{"
                               "background:#323232;"
                               "border:1px solid transparent;"
                               "border-radius:3px;"
                               "color:#909090;"
                               "}");

    rightScroll->setFrameShape(QFrame::NoFrame);

    int textHeight = 40;

    // 右侧：全局属性
    textVoiceDir = new TextControl(tr("Voice Dir"), "", rightWidget);
    textTool = new TextControl(tr("Wavtool"), "", rightWidget);
    textResamp = new TextControl(tr("Resampler"), "", rightWidget);
    textOutput = new TextControl(tr("Global Flags"), "", rightWidget);

    textVoiceDir->setMargin(0);
    textTool->setMargin(0);
    textResamp->setMargin(0);
    textOutput->setMargin(0);

    textVoiceDir->setGeometry(0, 0, 1000, textHeight);
    textTool->setGeometry(0, textVoiceDir->y() + textVoiceDir->height(), 1000, textHeight);
    textResamp->setGeometry(0, textTool->y() + textTool->height(), 1000, textHeight);
    textOutput->setGeometry(0, textResamp->y() + textResamp->height(), 1000, textHeight);

    rightWidget->adjustSize();
    rightWidget->setFixedSize(rightWidget->width(), rightWidget->height());

    rightScroll->setWidget(rightWidget);

    // 左侧：头像与名称

    // ---- 获取 ----
    QString imageName = "";
    QString voiceName = "";
    QString imagePath;

    MainWindow::getCharacter(voiceName, imageName);

    qDebug() << "Voice Name : " << voiceName;
    qDebug() << "Image : " << imageName;

    if (voiceName == "") {
        voiceName = PathFindFileName(MainWindow::aVoiceDir); // 找不到名字就用文件夹名代替
    }

    imagePath = MainWindow::aVoiceDir + "\\" + imageName;

    // ---- 获取结束 ----

    lbAvatar = new QLabel(leftWidget);
    lbVoiceName = new QLabel(leftWidget);

    lbAvatar->setFixedSize(100, 100);
    lbAvatar->setStyleSheet("border:2px solid rgba(0,0,0,30%);");

    lbVoiceName->setFixedWidth(150);
    lbVoiceName->setWordWrap(true);
    lbVoiceName->setAlignment(Qt::AlignHCenter);
    lbVoiceName->setEnabled(false);

    lbVoiceName->setFont(normalFont());
    lbVoiceName->setStyleSheet("color:white;");

    lbVoiceName->setText(voiceName);
    lbVoiceName->adjustSize();

    voiceLayout = new QVBoxLayout(leftWidget);
    voiceLayout->addWidget(lbAvatar);
    voiceLayout->addWidget(lbVoiceName);
    voiceLayout->setAlignment(lbAvatar, Qt::AlignHCenter);

    leftWidget->adjustSize();
    leftScroll->setWidget(leftWidget);
    leftScroll->setFixedWidth(180);

    connect(rightWidget, &MoveWidget::onMoveEvent, this, &PropertyArea::onRightAreaMoved);
    connect(rightScroll->verticalScrollBar(), &QScrollBar::rangeChanged, this,
            &PropertyArea::onRightAreaMoved);
    connect(rightScroll->verticalScrollBar(), &QScrollBar::valueChanged, this,
            &PropertyArea::onRightAreaMoved);

    // 左右空几个像素
    QWidget *leftMargin = new QWidget(this);
    QWidget *rightMargin = new QWidget(this);

    leftMargin->setFixedWidth(5);
    rightMargin->setFixedWidth(5);

    // 整体
    mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    mainLayout->addWidget(leftMargin);

    mainLayout->addWidget(leftScroll);
    mainLayout->addWidget(rightScroll);

    // mainLayout->addWidget(rightMargin);

    setLayout(mainLayout);

    // 设置各元件的属性
    textVoiceDir->setValue(MainWindow::aVoiceDir);
    textTool->setValue(MainWindow::aTool1Path);
    textResamp->setValue(MainWindow::aTool2Path);
    textOutput->setValue(MainWindow::aGlobalFlags);

    textVoiceDir->Text()->setEnabled(false);
    textTool->Text()->setEnabled(false);
    textResamp->Text()->setEnabled(false);

    // 设置头像
    if (!isFileExist(imagePath)) {
        return;
    }

    QPixmap pixs;
    pixs.load(imagePath);

    if (!pixs.isNull()) {
        qDebug() << "This is image.";
        lbAvatar->setPixmap(pixs);
        lbAvatar->setScaledContents(true);
    }

    // connect(textOutput->Text(), &QLineEdit::textChanged, this,
    // &PropertyArea::onGlobalFlagsChanged);
    connect(textOutput->Text(), &FixedLineEdit::editingFinished2, this,
            &PropertyArea::onEditingFinished);
}

void PropertyArea::onGlobalFlagsChanged() {
    MainWindow::aGlobalFlags = textOutput->getValue();

    // 以下是测试内容，马上删
    QStringList stringList = textOutput->getValue().split(",");

    if (stringList.size() == 3) {
        MainWindow::vibrato_buttonBorderColor =
            QColor(stringList[0].toInt(), stringList[1].toInt(), stringList[2].toInt());

        MainWindow::vibrato_frameColor = MainWindow::vibrato_buttonBorderColor;
    }
}

void PropertyArea::reInitializeStrings() {
    textVoiceDir->setCaption(tr("Voice Dir"));
    textTool->setCaption(tr("Wavtool"));
    textResamp->setCaption(tr("Resampler"));
    textOutput->setCaption(tr("Global Flags"));
}

void PropertyArea::paintEvent(QPaintEvent *) {
}

void PropertyArea::onRightAreaMoved() {

    QScrollBar *bar = rightScroll->verticalScrollBar();

    if (bar->maximum() == 0) {
        leftScroll->setPercentageY(0);
        return;
    }

    leftScroll->setPercentageY(double(bar->value()) / bar->maximum() * 100.0);
}

void PropertyArea::onEditingFinished() {
    onGlobalFlagsChanged();
}
