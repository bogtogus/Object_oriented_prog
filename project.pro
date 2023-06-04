QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

subdir = widgets

INCLUDEPATH +=$$PWD/$${subdir}


SOURCES += \
    $${subdir}/achildwin.cpp \
    $${subdir}/bonusprogwindow.cpp \
    $${subdir}/entities.cpp \
    $${subdir}/inputfields.cpp \
    $${subdir}/main.cpp \
    $${subdir}/mainwindow.cpp \
    $${subdir}/medicineswindow.cpp \
    $${subdir}/sellhistwindow.cpp \
    widgets/dbwindow.cpp

HEADERS += \
    $${subdir}/achildwin.h \
    $${subdir}/bonusprogwindow.h \
    $${subdir}/entities.h \
    $${subdir}/inputfields.h \
    $${subdir}/mainwindow.h \
    $${subdir}/medicineswindow.h \
    $${subdir}/sellhistwindow.h \
    widgets/dbwindow.h

FORMS += \
    $${subdir}/bonusprogwindow.ui \
    $${subdir}/inputfields.ui \
    $${subdir}/mainwindow.ui \
    $${subdir}/medicineswindow.ui \
    $${subdir}/sellhistwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
RC_ICONS = "images/medicines.ico"
