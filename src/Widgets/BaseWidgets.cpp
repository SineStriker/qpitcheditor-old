#include "BaseWidgets.h"
#include "EditorWidget.h"
#include "QSizePolicy"


/*--------------------------------------下面板元素--------------------------------------*/
EditorAreaWidget::EditorAreaWidget(QWidget *parent) : QWidget(parent) {
    parentArea = nullptr;
}

/*--------------------------------------自定义布局管理器--------------------------------------*/

FormSplitter::FormSplitter(QWidget *parent) : QSplitter(parent) {
    forms.clear();
    topWidget = nullptr;

    setHandleWidth(3);
    setFrameStyle(QFrame::NoFrame);

    setOrientation(Qt::Vertical);
    setChildrenCollapsible(false);
}

void FormSplitter::addForm(SimpleForm *form, int stretch) {
    forms.append(form);
    QSplitter::addWidget(form);
    // setStretchFactor(form, stretch);

    connect(form, &SimpleForm::visibilityChanged, this, &FormSplitter::onFormVisibilityChanged);
    modifyFeatures();
}

void FormSplitter::insertForm(int index, SimpleForm *form, int stretch) {
    forms.insert(index, form);
    QSplitter::insertWidget(index, form);
    // setStretchFactor(form, stretch);

    connect(form, &SimpleForm::visibilityChanged, this, &FormSplitter::onFormVisibilityChanged);

    modifyFeatures();
}

void FormSplitter::removeForm(SimpleForm *form) {
    forms.removeOne(form);
    // QSplitter::(form);

    disconnect(form, &SimpleForm::visibilityChanged, this, &FormSplitter::onFormVisibilityChanged);
    modifyFeatures();
}

void FormSplitter::setTopWidget(QWidget *w, int stretch) {
    if (!topWidget) {
        insertWidget(0, w);
        topWidget = w;
        modifyFeatures();
    }
}

void FormSplitter::removeTopWidget() {
    if (topWidget) {
        // removeWidget(topWidget);
        topWidget = nullptr;
        modifyFeatures();
    }
}

void FormSplitter::setStretchFactor(QWidget *w, int stretch) {
    int index = indexOf(w);
    if (index < 0) {
        return;
    }
    setStretchFactor(index, stretch);
}

void FormSplitter::setStretchFactor(int index, int stretch) {
    QSplitter::setStretchFactor(index, stretch);
}

void FormSplitter::onFormVisibilityChanged(bool b) {
    SimpleForm *form = qobject_cast<SimpleForm *>(sender());

    int i;
    bool flag = false;
    int index = forms.indexOf(form);

    if (index < 0 || topWidget) {
        modifyFeatures();
        return;
    }

    for (i = 0; i < forms.size(); ++i) {
        if (forms[i]->formUnfolded()) {
            flag = true;
            break;
        }
    }

    // 所有面板都关掉了
    if (!flag) {
        if (index == 0) {
            forms[index + 1]->reverseFold();
        } else {
            forms[0]->reverseFold();
        }
    }
    modifyFeatures();
}

void FormSplitter::modifyFeatures() {
    int i;

    for (i = 0; i < forms.size(); ++i) {
        if ((i == 0 && !topWidget) || (i > 0 && !forms[i - 1]->formUnfolded())) {
            forms[i]->getTitleBar()->setTopVisible(false);
            handle(indexOf(forms[i]))->setEnabled(false);
        } else {
            forms[i]->getTitleBar()->setTopVisible(true);
            handle(indexOf(forms[i]))->setEnabled(true);
        }

        if (i == forms.size() - 1 && !forms[i]->formUnfolded()) {
            handle(indexOf(forms[i]))->setEnabled(false);
        }
    }

    modifyStretches();
}

void FormSplitter::modifyStretches() {
    int i;

    for (i = 0; i < forms.size(); ++i) {
        if (!forms[i]->formUnfolded()) {
            setStretchFactor(i, 1);
        }
    }
}

QSplitterHandle *FormSplitter::createHandle() {
    FormSplitterHandle *handle = new FormSplitterHandle(this);
    return handle;
}
