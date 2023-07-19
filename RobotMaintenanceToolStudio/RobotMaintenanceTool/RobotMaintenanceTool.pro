QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = RobotMaintenanceTool

INCLUDEPATH += $$PWD/qssh/include
DEPENDPATH += $$PWD/qssh/include

contains(QT_ARCH, i386) {
    TARGET_OUTPUT_DIR=x86_output
    LIB_DIR_PRE=x86
} else {
    TARGET_OUTPUT_DIR=x64_output
    LIB_DIR_PRE=x64
}
CONFIG(release, debug|release): {
    DESTDIR = $$PWD/../$${TARGET_OUTPUT_DIR}/Release
    LIBS += -L$$PWD/qssh/lib/$${LIB_DIR_PRE} -lQSsh
}
else:CONFIG(debug, debug|release): {
    DESTDIR = $$PWD/../$${TARGET_OUTPUT_DIR}/Debug
    LIBS += -L$$PWD/qssh/lib/$${LIB_DIR_PRE} -lQSshd
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Bean/CRFirmwareAndXmlData.cpp \
    Bean/CRFirmwareAndXmlResult.cpp \
    Bean/CRSingleFirmwareXmlData.cpp \
    Bean/FaultRepairData.cpp \
    Bean/ServoPrmModifyData.cpp \
    Bean/ServoPrmModifyResult.cpp \
    Bean/ServoPrmReadData.cpp \
    Bean/ServoPrmReadResult.cpp \
    Common/HttpUserAPI.cpp \
    Common/Logger.cpp \
    Common/SFtpFileClient.cpp \
    Common/ShellCmd.cpp \
    MainWnd/AdvanceFunctionWidget.cpp \
    MainWnd/ControlPannelWidget.cpp \
    MainWnd/FileBackupWidget.cpp \
    MainWnd/FileRecoveryWidget.cpp \
    MainWnd/FirmwareUpdateWidget.cpp \
    MainWnd/ServoParameterWidget.cpp \
    MainWnd/SystemToolWidget.cpp \
    MainWnd/TitleBarWidget.cpp \
    Public.cpp \
    main.cpp \
    MainWidget.cpp

HEADERS += \
    Bean/CRFirmwareAndXmlData.h \
    Bean/CRFirmwareAndXmlResult.h \
    Bean/CRSingleFirmwareXmlData.h \
    Bean/FaultRepairData.h \
    Bean/ServoPrmModifyData.h \
    Bean/ServoPrmModifyResult.h \
    Bean/ServoPrmReadData.h \
    Bean/ServoPrmReadResult.h \
    Bean/UpdateState.h \
    Common/HttpUserAPI.h \
    Common/Logger.h \
    Common/SFtpConnectParam.h \
    Common/SFtpFileClient.h \
    Common/ShellCmd.h \
    MainWnd/AdvanceFunctionWidget.h \
    MainWnd/ControlPannelWidget.h \
    MainWnd/FileBackupWidget.h \
    MainWnd/FileRecoveryWidget.h \
    MainWnd/FirmwareUpdateWidget.h \
    MainWnd/ServoParameterWidget.h \
    MainWnd/SystemToolWidget.h \
    MainWnd/TitleBarWidget.h \
    MainWidget.h \
    Public.h

FORMS += \
    MainWnd/AdvanceFunctionWidget.ui \
    MainWnd/ControlPannelWidget.ui \
    MainWnd/FileBackupWidget.ui \
    MainWnd/FileRecoveryWidget.ui \
    MainWnd/FirmwareUpdateWidget.ui \
    MainWnd/ServoParameterWidget.ui \
    MainWnd/SystemToolWidget.ui \
    MainWnd/TitleBarWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

linux {
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\':\'\$$ORIGIN/lib\',--enable-new-dtags"
}

RESOURCES += \
    res/main.qrc

TRANSLATIONS += \
    res/lang/chinese.ts \
    res/lang/english.ts
