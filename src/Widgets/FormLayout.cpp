#include "FormLayout.h"

FormLayout::FormLayout(QWidget *parent) : QWidget(parent) {
}

void FormLayout::addForm(SimpleForm *form) {
    forms.append(form);
}

void FormLayout::insertForm(int index, SimpleForm *form) {
    forms.insert(index, form);
}

void FormLayout::removeForm(SimpleForm *form) {
    forms.removeOne(form);
}

void FormLayout::setStretchFactor(SimpleForm *form, int stretch) {
}

void FormLayout::setStretchFactor(int index, int stretch) {
}

void FormLayout::resizeEvent(QResizeEvent *event) {
}
