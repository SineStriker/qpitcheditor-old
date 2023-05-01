#include "CentralWidget.h"

/*--------------------------------------整个窗口--------------------------------------*/

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent) {

    mainSplitter = new FormSplitter(this);

    propertyPart = new PropertyWidget(this);
    editorPart = new EditorWidget(this);

    // mainSplitter->setTopWidget(new QWidget());
    mainSplitter->addForm(propertyPart);
    mainSplitter->addForm(editorPart);

    mainSplitter->setStretchFactor(propertyPart, 1);
    mainSplitter->setStretchFactor(editorPart, 1);
    mainSplitter->setSizes({1000, 3000}); // 强行设置比例

    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(mainSplitter);
    this->setLayout(mainLayout);
}

void CentralWidget::reInitializeStrings() {
    propertyPart->reInitializeStrings();
    editorPart->reInitializeStrings();
}
