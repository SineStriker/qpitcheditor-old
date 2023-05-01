#ifndef FORMLAYOUT_H
#define FORMLAYOUT_H

#include <QLayout>
#include <QWidget>

#include <Elements/SimpleForm.h>

class FormLayout : public QWidget {
    Q_OBJECT
public:
    explicit FormLayout(QWidget *parent = nullptr);

    void addForm(SimpleForm *form);
    void insertForm(int index, SimpleForm *form);
    void removeForm(SimpleForm *form);

    void setStretchFactor(SimpleForm *form, int stretch);
    void setStretchFactor(int index, int stretch);

private:
    void resizeEvent(QResizeEvent *event) override;

private:
    QVector<SimpleForm *> forms;

signals:
};

#endif // FORMLAYOUT_H
