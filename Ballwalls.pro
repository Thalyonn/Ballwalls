QT       += core gui\
        network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ball.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    sendworker.cpp \
    sprite.cpp \
    wall.cpp \
    scenewindow.cpp \
    worker.cpp

HEADERS += \
    ball.h \
    mainwindow.h \
    networkmanager.h \
    particleinfo.h \
    sendworker.h \
    sprite.h \
    wall.h \
    scenewindow.h \
    worker.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
