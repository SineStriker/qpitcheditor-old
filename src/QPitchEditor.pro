QT       += core gui multimedia svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Dialogs/BackstageDialog.cpp \
    Dialogs/EventDialog.cpp \
    Dialogs/InitDialog.cpp \
    Dialogs/NoteDialog.cpp \
    Dialogs/OptionsDialog.cpp \
    Dialogs/PointDialog.cpp \
    Dialogs/RenderDialog.cpp \
    Dialogs/StylizedDialog.cpp \
    Elements/ContinuousCurves.cpp \
    Elements/CustomControls.cpp \
    Elements/CustomObjects.cpp \
    Elements/CustomScrolls.cpp \
    Elements/NoteOperation.cpp \
    Elements/SimpleCurves.cpp \
    Elements/SimpleEnvelope.cpp \
    Elements/SimpleForm.cpp \
    Elements/SimpleNote.cpp \
    Elements/SimplePoint.cpp \
    Elements/SimpleScope.cpp \
    Elements/SimpleVibrato.cpp \
    Elements/VectorButton.cpp \
    Elements/VolumePoint.cpp \
    LibUTAUQt/QGenonSettings.cpp \
    LibUTAUQt/QOtoIni.cpp \
    LibUTAUQt/QPrefixMap.cpp \
    LibUTAUQt/QUtauConstants.cpp \
    LibUTAUQt/QUtauData.cpp \
    LibUTAUQt/QUtauSection.cpp \
    LibUTAUQt/QUtauSectionNote.cpp \
    LibUTAUQt/QUtauSectionSettings.cpp \
    LibUTAUQt/QUtauSectionVersion.cpp \
    LibUTAUQt/QUtauUtils.cpp \
    QUtils.cpp \
    Threads/LoadingThread.cpp \
    Threads/OnlineUpdater.cpp \
    Threads/RenderThread.cpp \
    UtaUtils/UtaPitchCurves.cpp \
    UtaUtils/UtaTranslator.cpp \
    Widgets/BaseWidgets.cpp \
    Widgets/CentralWidget.cpp \
    Widgets/EditorWidget.cpp \
    Widgets/FormLayout.cpp \
    Widgets/ListArea.cpp \
    Widgets/NotesArea.cpp \
    Widgets/PianoArea.cpp \
    Widgets/PropertyWidget.cpp \
    Widgets/ScaleplateArea.cpp \
    Widgets/VolumeArea.cpp \
    initialize.cpp \
    main.cpp \
    mainwindow.cpp \
    preread.cpp \
    render.cpp \
    sound.cpp \
    variables.cpp

HEADERS += \
    Dialogs/BackstageDialog.h \
    Dialogs/EventDialog.h \
    Dialogs/InitDialog.h \
    Dialogs/NoteDialog.h \
    Dialogs/OptionsDialog.h \
    Dialogs/PointDialog.h \
    Dialogs/RenderDialog.h \
    Dialogs/StylizedDialog.h \
    Elements/ContinuousCurves.h \
    Elements/CustomControls.h \
    Elements/CustomObjects.h \
    Elements/CustomScrolls.h \
    Elements/NoteOperation.h \
    Elements/SimpleCurves.h \
    Elements/SimpleEnvelope.h \
    Elements/SimpleForm.h \
    Elements/SimpleNote.h \
    Elements/SimplePoint.h \
    Elements/SimpleScope.h \
    Elements/SimpleVibrato.h \
    Elements/VectorButton.h \
    Elements/VolumePoint.h \
    LibUTAUQt/QGenonSettings.h \
    LibUTAUQt/QOtoIni.h \
    LibUTAUQt/QPrefixMap.h \
    LibUTAUQt/QUtauConstants.h \
    LibUTAUQt/QUtauData.h \
    LibUTAUQt/QUtauGlobal.h \
    LibUTAUQt/QUtauSection.h \
    LibUTAUQt/QUtauSectionNote.h \
    LibUTAUQt/QUtauSectionSettings.h \
    LibUTAUQt/QUtauSectionVersion.h \
    LibUTAUQt/QUtauStrings.h \
    LibUTAUQt/QUtauUtils.h \
    QUtils.h \
    Threads/LoadingThread.h \
    Threads/OnlineUpdater.h \
    Threads/RenderThread.h \
    UtaUtils/UtaPitchCurves.h \
    UtaUtils/UtaTranslator.h \
    Widgets/BaseWidgets.h \
    Widgets/CentralWidget.h \
    Widgets/EditorWidget.h \
    Widgets/FormLayout.h \
    Widgets/ListArea.h \
    Widgets/NotesArea.h \
    Widgets/PianoArea.h \
    Widgets/PropertyWidget.h \
    Widgets/ScaleplateArea.h \
    Widgets/VolumeArea.h \
    mainwindow.h \
    resource.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    language/QPitchEditor_CN.ts \
    language/QPitchEditor_EN.ts \
    language/QPitchEditor_JP.ts

RC_FILE += \
    logo.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    res.qrc
