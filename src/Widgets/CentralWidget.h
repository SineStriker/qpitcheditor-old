#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "EditorWidget.h"
#include "PropertyWidget.h"

class CentralWidget : public QWidget {
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr);

    PropertyArea *Property() {
        if (!propertyPart) {
            return nullptr;
        } else {
            return propertyPart->getArea();
        }
    }

    EditorOuterArea *Editor() {
        if (!editorPart) {
            return nullptr;
        } else {
            return editorPart->getArea();
        }
    }

    NotesArea *Interface() {
        if (!Editor()) {
            return nullptr;
        } else {
            return Editor()->MainArea();
        }
    }

    NotesScrollArea *Container() {
        if (!Editor()) {
            return nullptr;
        } else {
            return Editor()->MainScroll();
        }
    }

    EditorWidget *EditorPanel() {
        return editorPart;
    }

    PropertyWidget *PropertyPanel() {
        return propertyPart;
    }

    void reInitializeStrings();

private:
    PropertyWidget *propertyPart;
    EditorWidget *editorPart;

    FormSplitter *mainSplitter;
    QGridLayout *mainLayout;

    // 当其中一个部分出现折叠时调用
    void adjustStretchFactors();

signals:
};
#endif // CENTRALWIDGET_H
