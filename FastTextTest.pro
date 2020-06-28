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
    fasttext/

SOURCES += \
    fasttext/fasttextutil.cpp \
    main.cpp \
    mainwindow.cpp \
    fasttext/args.cc \
    fasttext/autotune.cc \
    fasttext/densematrix.cc \
    fasttext/dictionary.cc \
    fasttext/fasttext.cc \
    fasttext/loss.cc \
    fasttext/matrix.cc \
    fasttext/meter.cc \
    fasttext/model.cc \
    fasttext/productquantizer.cc \
    fasttext/quantmatrix.cc \
    fasttext/utils.cc \
    fasttext/vector.cc

HEADERS += \
    fasttext/fasttext_if.h \
    fasttext/fasttextutil.h \
    mainwindow.h \
    fasttext/args.h \
    fasttext/autotune.h \
    fasttext/densematrix.h \
    fasttext/dictionary.h \
    fasttext/fasttext.h \
    fasttext/fasttext_cmd.h \
    fasttext/fasttext_main.h \
    fasttext/loss.h \
    fasttext/matrix.h \
    fasttext/meter.h \
    fasttext/model.h \
    fasttext/productquantizer.h \
    fasttext/quantmatrix.h \
    fasttext/real.h \
    fasttext/utils.h \
    fasttext/vector.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md
