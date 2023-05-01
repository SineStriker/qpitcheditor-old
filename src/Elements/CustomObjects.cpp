#include "CustomObjects.h"

FixedLineEdit::FixedLineEdit(QWidget *parent) : QLineEdit(parent) {
    initText();
}

FixedLineEdit::FixedLineEdit(const QString &text, QWidget *parent) : QLineEdit(text, parent) {
    initText();
}

void FixedLineEdit::setText(QString text) {
    recentText = text;
    QLineEdit::setText(text);
}

void FixedLineEdit::focusOutEvent(QFocusEvent *event) {
    QLineEdit::focusOutEvent(event);
    qDebug() << "Lose Focus";
    emit loseFocus();
}

void FixedLineEdit::initText() {
    recentText = text();
    connect(this, &QLineEdit::editingFinished, this, &FixedLineEdit::onEditingFinished);
}

void FixedLineEdit::onEditingFinished() {
    if (hasFocus()) {
        clearFocus();
    }
    if (text() == recentText) {
        return;
    }
    recentText = text();
    emit editingFinished2();
}
