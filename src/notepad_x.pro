QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = notepad_x
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    editorwidget.cpp \
    linenumberarea.cpp

HEADERS += \
    mainwindow.h \
    editorwidget.h \
    linenumberarea.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
