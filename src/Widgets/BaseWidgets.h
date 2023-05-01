#ifndef BASEWIDGETS_H
#define BASEWIDGETS_H

#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include <Elements/SimpleForm.h>

class EditorArea : public QWidget {
    Q_OBJECT
public:
    explicit EditorArea(QWidget *parent) : QWidget(parent) {
    }
};

/*--------------------------------------下面板元素--------------------------------------*/
class EditorAreaWidget : public QWidget {
    Q_OBJECT
public:
    explicit EditorAreaWidget(QWidget *parent);

    void setParent(EditorArea *w) {
        parentArea = w;
    }

    EditorArea *Parent() {
        return parentArea;
    }

    virtual QWidget *getNoteSegLayer() {
        return nullptr;
    }

    virtual QWidget *getCurvesSegLayer() {
        return nullptr;
    }

    virtual QWidget *getPointSegLayer() {
        return nullptr;
    }

    virtual QWidget *getVibratoSegLayer() {
        return nullptr;
    }

protected:
    EditorArea *parentArea;
};

/*--------------------------------------自定义布局管理器--------------------------------------*/
class FormSplitter : public QSplitter {
    Q_OBJECT
public:
    explicit FormSplitter(QWidget *parent = nullptr);

    void addForm(SimpleForm *form, int stretch = 0);
    void insertForm(int index, SimpleForm *form, int stretch = 0);
    void removeForm(SimpleForm *form);

    void setTopWidget(QWidget *w, int stretch = 0);
    void removeTopWidget();

    void setStretchFactor(QWidget *w, int stretch);
    void setStretchFactor(int index, int stretch);

private:
    QVector<SimpleForm *> forms;
    QWidget *topWidget;

    void onFormVisibilityChanged(bool b);

    void modifyFeatures();
    void modifyStretches();

    QSplitterHandle *createHandle() override;
};

/*--------------------------------------自定义拉伸助手--------------------------------------*/
class FormSplitterHandle : public QSplitterHandle {
    Q_OBJECT
public:
    explicit FormSplitterHandle(QSplitter *parent) : QSplitterHandle(Qt::Vertical, parent) {
        QPalette pa;
        QColor color = Qt::transparent;

        // color = Qt::white;
        pa.setColor(QPalette::Background, color);
        setPalette(pa);
    }

private:
};

#endif // BASEWIDGETS_H
