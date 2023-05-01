#ifndef STYLIZEDDIALOG_H
#define STYLIZEDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QObject>

class StylizedDialog : public QMessageBox {
    Q_OBJECT
public:
    explicit StylizedDialog(QWidget *parent = nullptr) : QMessageBox(parent) {
        setStyleSheet("QWidget{"
                      "background:#373737;"
                      "}");
    }


};

#endif // STYLIZEDDIALOG_H
