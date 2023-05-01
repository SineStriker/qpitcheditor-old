#ifndef POINTDIALOG_H
#define POINTDIALOG_H

#include <QObject>
#include <QWidget>

#include "EventDialog.h"

class PointDialog : public EventDialog
{
    Q_OBJECT
public:
    explicit PointDialog(QWidget *parent);

private:
};

#endif // POINTDIALOG_H
