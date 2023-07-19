QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TEMPLATE = app

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
TARGET = RobotMaintenanceTools

CONFIG(debug, debug | release) {
    DESTDIR = $$PWD/../bin/debug
    LIBS += -L$$PWD/lib/ -lQSshd
}else{
    DESTDIR = $$PWD/../bin/release
    LIBS += -L$$PWD/lib/ -lQSsh
}
#LIBS += -L$$PWD/src/dobot/ -lDobotDll

SOURCES += \
    CShadowWindow3.cpp \
    CommonData.cpp \
    Define.cpp \
    IsControlLogBusyWidget.cpp \
    MyCheckBox.cpp \
    MyCheckBoxHeader.cpp \
    SFtpClient.cpp \
    BubbleTipsWidget.cpp \
    CopyFolderAFiles.cpp \
    DLogger.cpp \
    DowloadTipWidget.cpp \
    DownloadTool.cpp \
    IPAdressWidget.cpp \
    MainWidget2.cpp \
    ManualIpWidget.cpp \
    MessageWidget.cpp \
    DHttpManager.cpp \
    DobotProtocol.cpp \
    ProgressDialog.cpp \
    RoundProgressBar.cpp \
    ServoParamAllWriteTipWidget.cpp \
    ServoParamsAllUpgradeWidget.cpp \
    ShadowWindow.cpp \
    ShadowWindowPng.cpp \
    SmallControlAutoConnect.cpp \
    TipWidget.cpp \
    Upgrade2Widget.cpp \
    UpgradeIsOldElectronicSkinWidget.cpp \
    Widget2FileRecoveryTip.cpp \
    Widget2ProgressDialog.cpp \
    Widget2ServoParamsWait.cpp \
    Widget2SystemControlBack.cpp \
    WidgetFirmwareCheckItem.cpp \
    WidgetServoParamsPassword.cpp \
    WidgetSingleUpgrade.cpp \
    WidgetSingleUpgrading.cpp \
    WidgetSystemFileCheckTip.cpp \
    WidgetSystemLogExport.cpp \
    WidgetSystemToolTip.cpp \
    WidgetV4SN.cpp \
    ZipDiyManager.cpp \
    baseUI/BasePushButton.cpp \
    baseUI/BaseWidget.cpp \
    main.cpp \
    zips/unzip.cpp \
    zips/zip.cpp \
    DiyPushButton.cpp \
    DTableView.cpp \
    Logger.cpp


HEADERS += \
    BubbleTipsWidget.h \
    CShadowWindow3.h \
    CommonData.h \
    CopyFolderAFiles.h \
    DLogger.h \
    Define.h \
    DowloadTipWidget.h \
    DownloadTool.h \
    IPAdressWidget.h \
    IsControlLogBusyWidget.h \
    MainWidget2.h \
    DHttpManager.h \
    DobotProtocol.h \
    ManualIpWidget.h \
    MessageWidget.h \
    MyCheckBox.h \
    MyCheckBoxHeader.h \
    ProgressDialog.h \
    RoundProgressBar.h \
    ServoParamAllWriteTipWidget.h \
    ServoParamsAllUpgradeWidget.h \
    ShadowWindow.h \
    ShadowWindowPng.h \
    SmallControlAutoConnect.h \
    TableDelegate.h \
    TipWidget.h \
    Upgrade2Widget.h \
    UpgradeIsOldElectronicSkinWidget.h \
    Widget2FileRecoveryTip.h \
    Widget2ProgressDialog.h \
    Widget2ServoParamsWait.h \
    Widget2SystemControlBack.h \
    WidgetFirmwareCheckItem.h \
    WidgetServoParamsPassword.h \
    WidgetSingleUpgrade.h \
    WidgetSingleUpgrading.h \
    WidgetSystemFileCheckTip.h \
    WidgetSystemLogExport.h \
    WidgetSystemToolTip.h \
    WidgetV4SN.h \
    ZipDiyManager.h \
    baseUI/BasePushButton.h \
    baseUI/BaseWidget.h \
    zips/unzip.h \
    zips/zip.h \
    SFtpClient.h \
    DTableView.h \
    DiyPushButton.h \
    TableModel.h \
    Logger.h

FORMS += \
    BubbleTipsWidget.ui \
    DowloadTipWidget.ui \
    IPAdressWidget.ui \
    IsControlLogBusyWidget.ui \
    MainWidget2.ui \
    ManualIpWidget.ui \
    MessageWidget.ui \
    ProgressDialog.ui \
    ServoParamAllWriteTipWidget.ui \
    ServoParamsAllUpgradeWidget.ui \
    ShadowWindow.ui \
    SmallControlAutoConnect.ui \
    TipWidget.ui \
    Upgrade2Widget.ui \
    UpgradeIsOldElectronicSkinWidget.ui \
    Widget2FileRecoveryTip.ui \
    Widget2ProgressDialog.ui \
    Widget2ServoParamsWait.ui \
    Widget2SystemControlBack.ui \
    WidgetFirmwareCheckItem.ui \
    WidgetServoParamsPassword.ui \
    WidgetSingleUpgrade.ui \
    WidgetSingleUpgrading.ui \
    WidgetSystemFileCheckTip.ui \
    WidgetSystemLogExport.ui \
    WidgetSystemToolTip.ui \
    WidgetV4SN.ui \
    baseUI/BaseWidget.ui

TRANSLATIONS += tr_en.ts tr_zh.ts



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
