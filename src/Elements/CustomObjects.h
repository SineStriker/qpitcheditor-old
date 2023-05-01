#ifndef CUSTOMOBJECTS_H
#define CUSTOMOBJECTS_H

#include <QDebug>
#include <QLineEdit>

class FixedLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit FixedLineEdit(QWidget *parent = nullptr);
    explicit FixedLineEdit(const QString &text, QWidget *parent = nullptr);

    void setText(QString text);

private:
    QString recentText;

    void focusOutEvent(QFocusEvent *event) override;

    void initText();
    void onEditingFinished();

signals:
    void editingFinished2();
    void loseFocus();
};

#endif // CUSTOMOBJECTS_H
