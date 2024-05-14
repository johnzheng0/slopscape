QT += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console
CONFIG += warn_on

INCLUDEPATH += /usr/include/opencv4
LIBS += \
    -lsqlite3 \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_imgcodecs \
    -lopencv_videoio \
    -lopencv_highgui \
    -lopencv_ml

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    alert.cpp \
    autotagmanager.cpp \
    database.cpp \
    dialogconfirm.cpp \
    dialogedit.cpp \
    flowlayout.cpp \
    imagecard.cpp \
    imagelist.cpp \
    inputlinetags.cpp \
    inputtags.cpp \
    main.cpp \
    mainwindow.cpp \
    scandialog.cpp \
    scanmanager.cpp \
    settings.cpp \
    tagchip.cpp \
    viewerimage.cpp \
    viewermain.cpp \
    viewervideo.cpp

HEADERS += \
    alert.h \
    autotagmanager.h \
    colors.h \
    configs.h \
    database.h \
    dialogconfirm.h \
    dialogedit.h \
    flowlayout.h \
    imagecard.h \
    imagelist.h \
    inputlinetags.h \
    inputtags.h \
    lineedit.h \
    mainwindow.h \
    scandialog.h \
    scanmanager.h \
    settings.h \
    slider.h \
    tagchip.h \
    videowidget.h \
    viewerimage.h \
    viewermain.h \
    viewervideo.h

FORMS += \
    alert.ui \
    autotagmanager.ui \
    dialogconfirm.ui \
    dialogedit.ui \
    imagecard.ui \
    imagelist.ui \
    inputlinetags.ui \
    inputtags.ui \
    mainwindow.ui \
    scandialog.ui \
    scanmanager.ui \
    settings.ui \
    tagchip.ui \
    viewerimage.ui \
    viewermain.ui \
    viewervideo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
