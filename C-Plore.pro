#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T14:59:10
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = C-Plore
TEMPLATE = app

INCLUDEPATH += $$PWD/ScrollLabelEx
INCLUDEPATH += C:/Users\yci2/Desktop/Projects/BindCertificate/SysCertificateLib/SysCertification

SOURCES += main.cpp\
    mainpane.cpp \
    commonutils.cpp \
    deltaxbinder.cpp \
    listwidget.cpp \
    resourcemanager.cpp \
    touchpane.cpp \
    uianimationmanager.cpp \
    variantsetspane.cpp \
    widgetitem.cpp \
    covercotrolpane.cpp \
    UdpSender.cpp \
    settinghelpdiag.cpp \
    addjobdiag.cpp \
    pagemanager.cpp \
    panebase.cpp \
    modelselectionpane.cpp \
    logmanager.cpp \
    itembase.cpp \
    itemmodel.cpp \
    modelselectionpaneex.cpp \
    modelselection.cpp \
    externalprocesscontroller.cpp \
    loginpane.cpp \
    deletedialog.cpp \
    itemmodelex.cpp \
    modelselectionex.cpp \
    scrolllabelex.cpp \
    scrolltext.cpp \
    UdpReceiver.cpp \
    deletedialogex.cpp \
    joblistdiag.cpp \
    jobtaskitem.cpp \
    renderonscreen.cpp \
    watermarkwidget.cpp

HEADERS  += \
    mainpane.h \
    commonutils.h \
    deltaxbinder.h \
    listwidget.h \
    resourcemanager.h \
    touchpane.h \
    uianimationmanager.h \
    variantsetspane.h \
    widgetitem.h \
    covercotrolpane.h \
    UdpSender.h \
    UdpReceiver.h \
    settinghelpdiag.h \
    addjobdiag.h \
    pagemanager.h \
    panebase.h \
    modelselectionpane.h \
    logmanager.h \
    itembase.h \
    itemmodel.h \
    modelselectionpaneex.h \
    modelselection.h \
    externalprocesscontroller.h \
    loginpane.h \
    deletedialog.h \
    itemmodelex.h \
    modelselectionex.h \
    scrolllabelex.h \
    scrolltext.h \
    deletedialogex.h \
    joblistdiag.h \
    jobtaskitem.h \
    renderonscreen.h \
    watermarkwidget.h

FORMS    += \
    mainpane.ui \
    listwidget.ui \
    touchpane.ui \
    widgetitem.ui \
    covercotrolpane.ui \
    settinghelpdiag.ui \
    addjobdiag.ui \
    modelselectionpane.ui \
    itemmodel.ui \
    modelselectionpaneex.ui \
    modelselection.ui \
    loginpane.ui \
    deletedialog.ui \
    itemmodelex.ui \
    modelselectionex.ui \
    scrolllabelex.ui \
    deletedialogex.ui \
    joblistdiag.ui \
    jobtaskitem.ui \
    renderonscreen.ui \
    watermarkwidget.ui

RESOURCES += \
    resource.qrc \

LIBS += C:/Users/yci2/Desktop/Projects/BindCertificate/Release/SysCertification/SysCertification.dll


RC_FILE = app.rc
