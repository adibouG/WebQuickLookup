QT       += core gui webenginewidgets xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    jsontodomconverter.cpp \
    main.cpp \
    settings.cpp \
    settingsdialog.cpp \
    systemtraywrapper.cpp \
    webcontentdisplaywidget.cpp \
    weblookupdialog.cpp \
    webpageview.cpp \
    webresponseformatter.cpp

HEADERS += \
    jsontodomconverter.h \
    settings.h \
    settingsdialog.h \
    systemtraywrapper.h \
    webcontentdisplaywidget.h \
    weblookupdialog.h \
    weblookuperror.h \
    webpageview.h \
    webresponseformatter.h

FORMS += \
    settingsdialog.ui \
    settingsdialog.ui \
    webcontentdisplaywidget.ui \
    weblookupdialog.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md \
    img/

RESOURCES += \
    systrayicon.qrc
