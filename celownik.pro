#-------------------------------------------------
#
# Project created by QtCreator 2020-05-05T23:21:54
#
#-------------------------------------------------

QT       += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



TARGET = celownik
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    alarmy.cpp \
    tedytor_alarmow_dlg.cpp \
    teksty.cpp \
    tjeden_alarm.cpp \
    togloszenie_alarmu.cpp \
    topcje_dlg.cpp \
    tpreferred_hands.cpp \
    tprzezroczystosc.cpp

HEADERS  += mainwindow.h \
    alarmy.h \
    tedytor_alarmow_dlg.h \
    teksty.h \
    tjeden_alarm.h \
    togloszenie_alarmu.h \
    topcje_dlg.h \
    tpreferred_hands.h \
    tprzezroczystosc.h

FORMS    += mainwindow.ui \
    alarmy.ui \
    tedytor_alarmow_dlg.ui \
    togloszenie_alarmu.ui \
    topcje_dlg.ui \
    tprzezroczystosc.ui


RESOURCES += \
    translations.qrc \
    zegar_elementy.qrc


OTHER_FILES  += \
                content/*.png


DISTFILES += polski2.qm \
    content/jak_robic_wskazowki.txt \

