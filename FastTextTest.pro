QT       += core gui

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

INCLUDEPATH += \
    src/

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/args.cc \
    src/autotune.cc \
    src/densematrix.cc \
    src/dictionary.cc \
    src/fasttext.cc \
    src/loss.cc \
    src/matrix.cc \
    src/meter.cc \
    src/model.cc \
    src/productquantizer.cc \
    src/quantmatrix.cc \
    src/utils.cc \
    src/vector.cc

HEADERS += \
    mainwindow.h \
    src/args.h \
    src/autotune.h \
    src/densematrix.h \
    src/dictionary.h \
    src/fasttext.h \
    src/fasttext_cmd.h \
    src/fasttext_main.h \
    src/fasttext_util.h \
    src/loss.h \
    src/matrix.h \
    src/meter.h \
    src/model.h \
    src/productquantizer.h \
    src/quantmatrix.h \
    src/real.h \
    src/utils.h \
    src/vector.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
