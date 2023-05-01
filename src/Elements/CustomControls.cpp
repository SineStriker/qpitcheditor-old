#include "CustomControls.h"
#include "mainwindow.h"

/*--------------------------------------滑块控制--------------------------------------*/

SlideControl::SlideControl(QWidget *parent) : QWidget(parent) {
    InitSlideControl("", 0);
}

SlideControl::SlideControl(QString text, QWidget *parent) : QWidget(parent) {

    InitSlideControl(text, 0);
}

SlideControl::SlideControl(QString text, double value, QWidget *parent) : QWidget(parent) {

    InitSlideControl(text, value);
}

void SlideControl::setMaximum(double value) {
    maximum = value;
    pSpinBox->setMaximum(maximum);
    pSlider->setMaximum(maximum * pow(10, pSpinBox->decimals()));
}
void SlideControl::setMinimum(double value) {
    minimum = value;
    pSpinBox->setMinimum(minimum);
    pSlider->setMinimum(minimum * pow(10, pSpinBox->decimals()));
}
void SlideControl::setSingleStep(double value) {
    singleStep = value;
    pSpinBox->setDecimals(-log10(value));
}
void SlideControl::setValue(double value) {
    this->value = value;
    pSpinBox->setValue(value);
    pSlider->setValue(value * pow(10, pSpinBox->decimals()));
}

double SlideControl::getValue() {
    return pSlider->value() / pow(10, pSpinBox->decimals());
}

void SlideControl::InitSlideControl(QString text, double value) {
    QFont font(normalFont());

    // 标签
    pLabel = new QLabel(text, this);
    pLabel->setFont(font);
    pLabel->adjustSize();

    // 微调框
    pSpinBox = new QDoubleSpinBox(this);
    pSpinBox->setDecimals(2);
    pSpinBox->setFont(font);
    pSpinBox->adjustSize();

    // 滑动条
    pSlider = new QSlider(this);
    pSlider->setOrientation(Qt::Horizontal); // 水平方向
    pSlider->adjustSize();

    setMinimum(0);    // 最小值
    setMaximum(100);  // 最大值
    setSingleStep(1); // 步长
    setValue(value);

    // pLabel->setGeometry(25, 10, 100, 18);
    // pSpinBox->setGeometry(150, 10, 100, 18);
    // pSlider->setGeometry(25, 32, 225, 18);
    // setFixedSize(280, 60);

    pSlider->setFixedHeight(18);

    pLayout = new QGridLayout(this);

    pLayout->addWidget(pLabel, 0, 0);
    pLayout->addWidget(pSpinBox, 0, 1);
    pLayout->addWidget(pSlider, 1, 0, 1, 2);

    setLayout(pLayout);

    connect(pSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onTextChanged(double)));
    connect(pSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
    bUnmodified = false;

    /*setStyleSheet("QLabel{"
                  "color:white;"
                  "}"
                  "QSlider::groove:horizontal {"
                  "border:0px solid #bbb;"
                  "}"
                  "QSlider::sub-page:horizontal {"
                  "background:#979797;"
                  "border-radius:0px;"
                  "margin-top:7px;"
                  "margin-bottom:7px;"
                  "}"
                  "QSlider::add-page:horizontal {"
                  "background:#2B2B2B;"
                  "border:0px solid #777;"
                  "border-radius:0px;"
                  "margin-top:7px;"
                  "margin-bottom:7px;"
                  "}"
                  "QSlider::handle:horizontal {"
                  "background:#C6C6C6;"
                  "border:2px solid #363636;"
                  "width:14px;"
                  "height:14px;"
                  "border-radius:7px;"
                  //"margin-top:2px;"
                  //"margin-bottom:2px;"
                  "}"
                  "QSlider::handle:horizontal:hover {"
                  "background:white;"
                  "}"
                  "QSlider::sub-page:horizontal:disabled {"
                  "background:#bbb;"
                  "border-color:#999;"
                  "}"
                  "QSlider::add-page:horizontal:disabled {"
                  "background:#eee;"
                  "border-color:#999;"
                  "}"
                  "QSlider::handle:horizontal:disabled {"
                  "background:#eee;"
                  "border:1px solid #aaa;"
                  "border-radius:4px;"
                  "}");*/
}

void SlideControl::onModifyAction() {
    if (bUnmodified) {
        setUnmodified(false);
    }
}

void SlideControl::onSliderChanged(int n) {
    onModifyAction();
    pSpinBox->setValue(double(n) / pow(10, pSpinBox->decimals()));
}

void SlideControl::onTextChanged(double n) {
    onModifyAction();
    pSlider->setValue(n * pow(10, pSpinBox->decimals()));
}

void SlideControl::setProportion(int a, int b) {
    pLayout->setColumnStretch(0, a);
    pLayout->setColumnStretch(1, b);
}

void SlideControl::setMargin(int n) {
    pLayout->setMargin(n);
}

void SlideControl::setUnmodified(bool value) {
    if (value) {
        pSpinBox->setStyleSheet("background-color:#E1E1E1;");
    } else {
        pSpinBox->setStyleSheet("background-color:white;");
    }

    bUnmodified = value;
}

bool SlideControl::isUnmodified() {
    return bUnmodified;
}


/*--------------------------------------文本控制--------------------------------------*/

TextControl::TextControl(QWidget *parent) : QWidget(parent) {
    InitTextControl("", "");
}

TextControl::TextControl(QString text, QWidget *parent) : QWidget(parent) {
    InitTextControl(text, "");
}

TextControl::TextControl(QString text, QString value, QWidget *parent) : QWidget(parent) {
    InitTextControl(text, value);
}

void TextControl::setProportion(int a, int b) {
    pLayout->setStretchFactor(pLabel, a);
    pLayout->setStretchFactor(pText, b);
}

void TextControl::setMargin(int n) {
    pLayout->setMargin(n);
}

void TextControl::setUnmodified(bool value) {
    if (value) {
        pText->setStyleSheet("background-color:#E1E1E1;");
        pText->setPlaceholderText(placeholderUnmodified);
    } else {
        pText->setStyleSheet("background-color:white;");
        pText->setPlaceholderText(placeholderModified);
    }
    bUnmodified = value;
}

bool TextControl::isUnmodified() {
    return bUnmodified;
}

void TextControl::setPlaceholder(QString modi, QString unmo) {
    placeholderModified = modi;
    placeholderUnmodified = unmo;
    if (bUnmodified) {
        pText->setPlaceholderText(unmo);
    } else {
        pText->setPlaceholderText(modi);
    }
}

void TextControl::setValue(QString value) {
    pText->setText(value);
}

QString TextControl::getValue() {
    return pText->text();
}

void TextControl::InitTextControl(QString text, QString value) {

    QFont font(normalFont());

    pLabel = new QLabel(text, this);
    pText = new FixedLineEdit(value, this);

    pValidator = nullptr;

    pLabel->setFont(font);
    pText->setFont(font);

    pLabel->adjustSize();
    pText->adjustSize();

    setBlankValue(0);

    pLayout = new QHBoxLayout(this);

    // pLayout->setMargin(0);

    pLayout->addWidget(pLabel, 1);
    pLayout->addWidget(pText, 10);
    // pLayout->addWidget(pLabel);
    // pLayout->addWidget(pText);

    setLayout(pLayout);

    connect(pText, &FixedLineEdit::textChanged, this, &TextControl::onTextChanged);

    pText->setPlaceholderText(placeholderModified);
    bUnmodified = false;
}

void TextControl::onModifyAction() {
    if (bUnmodified) {
        bool status = pText->isClearButtonEnabled();
        pText->setClearButtonEnabled(false);
        pText->clear();
        pText->setClearButtonEnabled(status);

        setUnmodified(false);
    }
}

void TextControl::onTextChanged(QString s) {
    onModifyAction();

    if (pValidator) {
        double n = (s.isEmpty() || !isNumber(s, true, true)) ? blankValue : s.toDouble();

        if (n > pValidator->top()) {
            n = pValidator->top();
            s = QString::number(n);
            pText->setText(s);
        }
    }
}

void TextControl::setValidator(QDoubleValidator *v) {
    pValidator = v;
    pText->setValidator(v);
}

/*--------------------------------------列表框控制--------------------------------------*/

ComboControl::ComboControl(QWidget *parent) : QWidget(parent) {
    QStringList a{};
    InitComboControl("", a);
}

ComboControl::ComboControl(QString text, QWidget *parent) : QWidget(parent) {
    QStringList a{};
    InitComboControl(text, a);
}

ComboControl::ComboControl(QString text, const QStringList &values, QWidget *parent)
    : QWidget(parent) {
    InitComboControl(text, values);
}

void ComboControl::setProportion(int a, int b) {
    pLayout->setStretchFactor(pLabel, a);
    pLayout->setStretchFactor(pCombo, b);
}

void ComboControl::setMargin(int n) {
    pLayout->setMargin(n);
}

void ComboControl::setUnmodified(bool value) {
    if (value) {
        pCombo->setStyleSheet("background-color:#E1E1E1;");
    } else {
        pCombo->setStyleSheet("background-color:white;");
    }
    bUnmodified = value;
}

bool ComboControl::isUnmodified() {
    return bUnmodified;
}

void ComboControl::InitComboControl(QString text, const QStringList &values) {

    QFont font(normalFont());

    pLabel = new QLabel(text, this);
    pCombo = new QComboBox(this);

    pLabel->setFont(font);
    pCombo->setFont(font);

    pLabel->adjustSize();
    pCombo->adjustSize();

    pLayout = new QHBoxLayout(this);

    pLayout->addWidget(pLabel);
    pLayout->addWidget(pCombo);

    if (!values.empty()) {
        pCombo->addItems(values);
    }

    setLayout(pLayout);

    connect(pCombo, &QComboBox::currentTextChanged, this, &ComboControl::onModifyAction);
    bUnmodified = false;
}

void ComboControl::onModifyAction() {
    if (bUnmodified) {
        setUnmodified(false);
    }
}

/*--------------------------------------滑块文本控制--------------------------------------*/
SlideTextControl::SlideTextControl(QWidget *parent) : QWidget(parent) {
    InitSlideControl("", 0);
}

SlideTextControl::SlideTextControl(QString text, QWidget *parent) : QWidget(parent) {

    InitSlideControl(text, 0);
}

SlideTextControl::SlideTextControl(QString text, double value, QWidget *parent) : QWidget(parent) {

    InitSlideControl(text, value);
}

void SlideTextControl::setBlankValue(double value) {
    blankValue = value;
}

void SlideTextControl::setMaximum(double value) {
    maximum = value;
    pValidator->setTop(maximum);
    pSlider->setMaximum(maximum * pow(10, pValidator->decimals()));
}
void SlideTextControl::setMinimum(double value) {
    minimum = value;
    pValidator->setBottom(minimum);
    pSlider->setMinimum(minimum * pow(10, pValidator->decimals()));
}
void SlideTextControl::setSingleStep(double value) {
    singleStep = value;
    pValidator->setDecimals(-log10(value));
}
void SlideTextControl::setValue(double value) {
    this->value = value;
    pText->setText(QString::number(value));
    pSlider->setValue(value * pow(10, pValidator->decimals()));
}

double SlideTextControl::getValue() {
    return pSlider->value() / pow(10, pValidator->decimals());
}

void SlideTextControl::setText(QString value)
{
    pText->setText(value);
}

QString SlideTextControl::getText() {
    return pText->text();
}

void SlideTextControl::InitSlideControl(QString text, double value) {

    isSliderMoving = isTextEditing = false;

    QFont font(normalFont());

    // 标签
    pLabel = new QLabel(text, this);
    pLabel->setFont(font);
    pLabel->adjustSize();

    // 微调框
    pText = new FixedLineEdit(this);
    pValidator = new QDoubleValidator();

    pValidator->setDecimals(2);
    pValidator->setNotation(QDoubleValidator::StandardNotation);

    pText->setFont(font);
    pText->setValidator(pValidator);

    pText->adjustSize();

    // 滑动条
    pSlider = new QSlider(this);
    pSlider->setOrientation(Qt::Horizontal); // 水平方向
    pSlider->adjustSize();

    setMinimum(0);    // 最小值
    setMaximum(100);  // 最大值
    setSingleStep(1); // 步长
    setValue(value);
    setBlankValue(0);

    // pLabel->setGeometry(25, 10, 100, 18);
    // pSpinBox->setGeometry(150, 10, 100, 18);
    // pSlider->setGeometry(25, 32, 225, 18);
    // setFixedSize(280, 60);

    pSlider->setFixedHeight(18);

    pLayout = new QGridLayout(this);

    pLayout->addWidget(pLabel, 0, 0);
    pLayout->addWidget(pText, 0, 1);
    pLayout->addWidget(pSlider, 1, 0, 1, 2);

    setLayout(pLayout);

    connect(pText, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(pSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));

    pText->setPlaceholderText(placeholderModified);
    bUnmodified = false;
}

void SlideTextControl::onModifyAction() {
    if (bUnmodified) {
        bool status = pText->isClearButtonEnabled();
        pText->setClearButtonEnabled(false);
        pText->clear();
        pText->setClearButtonEnabled(status);

        setUnmodified(false);
    }
}

void SlideTextControl::setPlaceholder(QString modi, QString unmo) {
    placeholderModified = modi;
    placeholderUnmodified = unmo;

    if (bUnmodified) {
        pText->setPlaceholderText(unmo);
    } else {
        pText->setPlaceholderText(modi);
    }
}

void SlideTextControl::onSliderChanged(int n) {
    onModifyAction();

    if (isTextEditing) {
        return;
    }
    isSliderMoving = true;
    pText->setText(QString::number(double(n) / pow(10, pValidator->decimals())));
    isSliderMoving = false;
}

void SlideTextControl::onTextChanged(QString s) {
    onModifyAction();

    if (isSliderMoving) {
        return;
    }
    double n = (s.isEmpty() || !isNumber(s, true, true)) ? blankValue : s.toDouble();

    if (n > maximum) {
        n = maximum;
        s = QString::number(n);
        pText->setText(s);
    }

    isTextEditing = true;
    pSlider->setValue(n * pow(10, pValidator->decimals()));
    isTextEditing = false;
}

void SlideTextControl::setProportion(int a, int b) {
    pLayout->setColumnStretch(0, a);
    pLayout->setColumnStretch(1, b);
}

void SlideTextControl::setMargin(int n) {
    pLayout->setMargin(n);
}

void SlideTextControl::setUnmodified(bool value) {
    if (value) {
        pText->setStyleSheet("background-color:#E1E1E1;");
        pText->setPlaceholderText(placeholderUnmodified);
    } else {
        pText->setStyleSheet("background-color:white;");
        pText->setPlaceholderText(placeholderModified);
    }

    bUnmodified = value;
}

bool SlideTextControl::isUnmodified() {
    return bUnmodified;
}
