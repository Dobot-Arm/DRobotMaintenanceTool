QT       += core gui network concurrent gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TEMPLATE = app

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
TARGET = "DOBOT Maintenance tool"

CONFIG(debug, debug | release) {
    DESTDIR = $$PWD/../bin/debug
    LIBS += -L$$PWD/lib/ -lQSshd
}else{
    DESTDIR = $$PWD/../bin/release
    LIBS += -L$$PWD/lib/ -lQSsh
}
#LIBS += -L$$PWD/src/dobot/ -lDobotDll

SOURCES += \
    CMSExceptionHandler.cpp \
    CShadowWindow3.cpp \
    CommonData.cpp \
    DHttpCurlManager.cpp \
    Define.cpp \
    EnvDebugConfig.cpp \
    FileBackupWidget.cpp \
    FileRecoveryWidget.cpp \
    IsControlLogBusyWidget.cpp \
    IsDisableControlWidget.cpp \
    LoadingUI.cpp \
    LuaApi.cpp \
    LuaDebugForm.cpp \
    LuaRunner.cpp \
    MainWidget2Extent.cpp \
    MaskDialogContainer.cpp \
    MyCheckBox.cpp \
    MyCheckBoxHeader.cpp \
    PlinkCmd.cpp \
    RobotStudio/ControllerWarn.cpp \
    RobotStudio/DlgMsgBox.cpp \
    RobotStudio/FormAbout.cpp \
    RobotStudio/FormLookXmlVersion.cpp \
    RobotStudio/FormMacDetail.cpp \
    RobotStudio/FormMenuButton.cpp \
    RobotStudio/FormMsgBox.cpp \
    RobotStudio/FormPackageVersionDetail.cpp \
    RobotStudio/FormRestartApp.cpp \
    RobotStudio/FormServoParamButton.cpp \
    RobotStudio/FormServoTips.cpp \
    RobotStudio/FormServoUpdateComfirm.cpp \
    RobotStudio/FormServoWriteProgress.cpp \
    RobotStudio/QMenuIconStyle.cpp \
    RobotStudio/WidgetToast.cpp \
    SFtpClient.cpp \
    BubbleTipsWidget.cpp \
    CopyFolderAFiles.cpp \
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
    SNAlarmBubbleTipWidget.cpp \
    SNMatchFailedWidget.cpp \
    SNWriteConfirmWidget.cpp \
    SNWriteDoingWidget.cpp \
    SNWriteStatusWidget.cpp \
    ServoParamBubbleTipWidget.cpp \
    ShadowWindow.cpp \
    ShadowWindowPng.cpp \
    SystemToolSNRepairWidget.cpp \
    SystemToolSNWriteWidget.cpp \
    TipWidget.cpp \
    UpdateConfigBean.cpp \
    Upgrade2Widget.cpp \
    UpgradeIsOldElectronicSkinWidget.cpp \
    Widget2FileRecoveryTip.cpp \
    Widget2ProgressDialog.cpp \
    Widget2ServoParamsProgress.cpp \
    WidgetFirmwareCheckItem.cpp \
    WidgetServoParamsPassword.cpp \
    WidgetSingleUpgrade.cpp \
    WidgetSingleUpgradeMainControl.cpp \
    WidgetSingleUpgrading.cpp \
    WidgetSystemFileCheckTip.cpp \
    WidgetSystemLogExport.cpp \
    WidgetSystemToolTip.cpp \
    ZipDiyManager.cpp \
    baseUI/BasePushButton.cpp \
    baseUI/BaseWidget.cpp \
    baseUI/UIBaseWidget.cpp \
    lua/lapi.c \
    lua/lauxlib.c \
    lua/lbaselib.c \
    lua/lcode.c \
    lua/lcorolib.c \
    lua/lctype.c \
    lua/ldblib.c \
    lua/ldebug.c \
    lua/ldo.c \
    lua/ldump.c \
    lua/lfunc.c \
    lua/lgc.c \
    lua/linit.c \
    lua/liolib.c \
    lua/llex.c \
    lua/lmathlib.c \
    lua/lmem.c \
    lua/loadlib.c \
    lua/lobject.c \
    lua/lopcodes.c \
    lua/loslib.c \
    lua/lparser.c \
    lua/lstate.c \
    lua/lstring.c \
    lua/lstrlib.c \
    lua/ltable.c \
    lua/ltablib.c \
    lua/ltm.c \
    lua/lundump.c \
    lua/lutf8lib.c \
    lua/lvm.c \
    lua/lzio.c \
    main.cpp \
    md5.cpp \
    zips/unzip.cpp \
    zips/zip.cpp \
    DiyPushButton.cpp \
    DTableView.cpp \
    Logger.cpp


HEADERS += \
    BubbleTipsWidget.h \
    CMSExceptionHandler.h \
    CShadowWindow3.h \
    CommonData.h \
    CopyFolderAFiles.h \
    DHttpCurlManager.h \
    Define.h \
    DoStatus.h \
    DowloadTipWidget.h \
    DownloadTool.h \
    EnvDebugConfig.h \
    FileBackupWidget.h \
    FileRecoveryWidget.h \
    IPAdressWidget.h \
    IsControlLogBusyWidget.h \
    IsDisableControlWidget.h \
    LoadingUI.h \
    LuaApi.h \
    LuaDebugForm.h \
    MainWidget2.h \
    DHttpManager.h \
    DobotProtocol.h \
    ManualIpWidget.h \
    MaskDialogContainer.h \
    MessageWidget.h \
    MyCheckBox.h \
    MyCheckBoxHeader.h \
    PlinkCmd.h \
    ProgressDialog.h \
    RobotStudio/ControllerWarn.h \
    RobotStudio/DlgMsgBox.h \
    RobotStudio/FormAbout.h \
    RobotStudio/FormLookXmlVersion.h \
    RobotStudio/FormMacDetail.h \
    RobotStudio/FormMenuButton.h \
    RobotStudio/FormMsgBox.h \
    RobotStudio/FormPackageVersionDetail.h \
    RobotStudio/FormRestartApp.h \
    RobotStudio/FormServoParamButton.h \
    RobotStudio/FormServoTips.h \
    RobotStudio/FormServoUpdateComfirm.h \
    RobotStudio/FormServoWriteProgress.h \
    RobotStudio/QMenuIconStyle.h \
    RobotStudio/WidgetToast.h \
    RoundProgressBar.h \
    SNAlarmBubbleTipWidget.h \
    SNMatchFailedWidget.h \
    SNWriteConfirmWidget.h \
    SNWriteDoingWidget.h \
    SNWriteStatusWidget.h \
    ServoParamBubbleTipWidget.h \
    ShadowWindow.h \
    ShadowWindowPng.h \
    SystemToolSNRepairWidget.h \
    SystemToolSNWriteWidget.h \
    TableDelegate.h \
    TipWidget.h \
    UpdateConfigBean.h \
    Upgrade2Widget.h \
    UpgradeIsOldElectronicSkinWidget.h \
    Widget2FileRecoveryTip.h \
    Widget2ProgressDialog.h \
    Widget2ServoParamsProgress.h \
    WidgetFirmwareCheckItem.h \
    WidgetServoParamsPassword.h \
    WidgetSingleUpgrade.h \
    WidgetSingleUpgradeMainControl.h \
    WidgetSingleUpgrading.h \
    WidgetSystemFileCheckTip.h \
    WidgetSystemLogExport.h \
    WidgetSystemToolTip.h \
    ZipDiyManager.h \
    baseUI/BasePushButton.h \
    baseUI/BaseWidget.h \
    baseUI/UIBaseWidget.h \
    lua/lapi.h \
    lua/lauxlib.h \
    lua/lcode.h \
    lua/lctype.h \
    lua/ldebug.h \
    lua/ldo.h \
    lua/lfunc.h \
    lua/lgc.h \
    lua/ljumptab.h \
    lua/llex.h \
    lua/llimits.h \
    lua/lmem.h \
    lua/lobject.h \
    lua/lopcodes.h \
    lua/lopnames.h \
    lua/lparser.h \
    lua/lprefix.h \
    lua/lstate.h \
    lua/lstring.h \
    lua/ltable.h \
    lua/ltm.h \
    lua/lua.h \
    lua/lua.hpp \
    lua/luaconf.h \
    lua/lualib.h \
    lua/lundump.h \
    lua/lvm.h \
    lua/lzio.h \
    md5.h \
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
    FileBackupWidget.ui \
    FileRecoveryWidget.ui \
    IPAdressWidget.ui \
    IsControlLogBusyWidget.ui \
    IsDisableControlWidget.ui \
    LoadingUI.ui \
    LuaDebugForm.ui \
    MainWidget2.ui \
    ManualIpWidget.ui \
    MessageWidget.ui \
    ProgressDialog.ui \
    RobotStudio/ControllerWarn.ui \
    RobotStudio/DlgMsgBox.ui \
    RobotStudio/FormAbout.ui \
    RobotStudio/FormLookXmlVersion.ui \
    RobotStudio/FormMenuButton.ui \
    RobotStudio/FormMsgBox.ui \
    RobotStudio/FormPackageVersionDetail.ui \
    RobotStudio/FormRestartApp.ui \
    RobotStudio/FormServoParamButton.ui \
    RobotStudio/FormServoTips.ui \
    RobotStudio/FormServoUpdateComfirm.ui \
    RobotStudio/FormServoWriteProgress.ui \
    RobotStudio/WidgetToast.ui \
    SNAlarmBubbleTipWidget.ui \
    SNMatchFailedWidget.ui \
    SNWriteConfirmWidget.ui \
    SNWriteDoingWidget.ui \
    SNWriteStatusWidget.ui \
    ServoParamBubbleTipWidget.ui \
    ShadowWindow.ui \
    SystemToolSNRepairWidget.ui \
    SystemToolSNWriteWidget.ui \
    TipWidget.ui \
    Upgrade2Widget.ui \
    UpgradeIsOldElectronicSkinWidget.ui \
    Widget2FileRecoveryTip.ui \
    Widget2ProgressDialog.ui \
    Widget2ServoParamsProgress.ui \
    WidgetFirmwareCheckItem.ui \
    WidgetServoParamsPassword.ui \
    WidgetSingleUpgrade.ui \
    WidgetSingleUpgradeMainControl.ui \
    WidgetSingleUpgrading.ui \
    WidgetSystemFileCheckTip.ui \
    WidgetSystemLogExport.ui \
    WidgetSystemToolTip.ui \
    baseUI/BaseWidget.ui

TRANSLATIONS += tr_en.ts tr_zh.ts

include(./CurlHttpClient/CurlHttpClient.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

RC_FILE = images/myapp.rc

LIBS += -lDbgHelp
#表示在release下可以生成调试信息
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
