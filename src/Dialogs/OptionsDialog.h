#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QCloseEvent>
#include <QEvent>
#include <QEventLoop>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "Elements/CustomControls.h"
#include "EventDialog.h"
#include "QUtils.h"

class OptionsDialog : public EventDialog {
    Q_OBJECT
public:
    explicit OptionsDialog(QWidget *parent = nullptr);

    void getCurrentValues();
    void saveCurrentValues();

protected:
    QPushButton *btnCancel, *btnOK;
    QLabel *lbCaption;

    ComboControl *ccLanguage, *ccDoubleClick, *ccThreads;
    SlideControl *scVolume;

private:
    void onCancelClicked();
    void onOKClicked();

signals:
};

#endif // OPTIONSDIALOG_H
