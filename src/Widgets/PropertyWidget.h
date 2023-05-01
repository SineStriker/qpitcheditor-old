#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include <QBrush>
#include <QDockWidget>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QTextBrowser>
#include <QWidget>

#include "EditorWidget.h"
#include "Elements/CustomControls.h"

class PropertyArea;
class PropertyWidget;

/*--------------------------------------上面板中心区域--------------------------------------*/
class PropertyArea : public QWidget {
    Q_OBJECT
public:
    explicit PropertyArea(QWidget *parent = nullptr);

    void onGlobalFlagsChanged();

    int getAvatarAreaHeight() {
        return leftWidget->height() + mainLayout->margin();
    }

    friend class PropertyWidget;
    void reInitializeStrings();

private:
    void paintEvent(QPaintEvent *);

    TextControl *textTool, *textResamp, *textOutput, *textVoiceDir;

    QLabel *lbAvatar;
    QLabel *lbVoiceName;

    //QTextBrowser *browser;

    QWidget *leftWidget;
    MoveWidget *rightWidget;

    NoneScrollArea *leftScroll;
    BasicScrollArea *rightScroll;

    QVBoxLayout *voiceLayout;

    // 放左右Scroll Area
    QHBoxLayout *mainLayout;

signals:

public slots:
    void onRightAreaMoved();
    void onEditingFinished();
};

/*--------------------------------------上面板整体--------------------------------------*/
class PropertyWidget : public SimpleForm {
    Q_OBJECT
public:
    explicit PropertyWidget(QWidget *parent = nullptr);

    PropertyArea *getArea() {
        return properties;
    }

    void reInitializeStrings();

private:
    PropertyArea *properties;

signals:
};

#endif // PROPERTYWIDGET_H
