#ifndef COMMONDATA_H
#define COMMONDATA_H
#include "Define.h"
namespace CommonData {
    void setCurrentSettingsVersion(const DobotType::SettingsVersion& settingsVersion);
    DobotType::SettingsVersion getCurrentSettingsVersion();

    void setUpgradeSettingsVersion(const DobotType::SettingsVersion& settingsVersion);
    DobotType::SettingsVersion getUpgradeSettingsVersion();

    QString getStrPropertiesCabinetType();
    void setStrPropertiesCabinetType(QString propertiesCabinetType);

    DobotType::ControllerType getControllerType();
    void setControllerType(DobotType::ControllerType controllerType);

    void setAllUpgradeToSingleUpgradeCount(int count);
    int getAllUpgradeToSingleUpgradeCount();


    int getUpgrade2WidgetSafeIOFOEStatus();
    void setUpgrade2WidgetSafeIOFOEStatus(int status);



    int getUpgrade2WidgetSafeBFOEStatus();
    void setUpgrade2WidgetSafeBFOEStatus(int status);

    int getUpgrade2WidgetSafeIOXMLStatus();
    void setUpgrade2WidgetSafeIOXMLStatus(int status);

    int getUpgrade2WidgetFeedbackStatus();
    void setUpgrade2WidgetFeedbackStatus(int status);

    int getUpgrade2WidgetUniIOFOEStatus();
    void setUpgrade2WidgetUniIOFOEStatus(int status);

    int getUpgrade2WidgetUniIOXMLStatus();
    void setUpgrade2WidgetUniIOXMLStatus(int status);



    int getUpgrade2WidgetServoJ1XMLStatus();
    void setUpgrade2WidgetServoJ1XMLStatus(int status);

    int getUpgrade2WidgetServoJ1FOEStatus();
    void setUpgrade2WidgetServoJ1FOEStatus(int status);



    int getUpgrade2WidgetServoJ2XMLStatus();
    void setUpgrade2WidgetServoJ2XMLStatus(int status);

    int getUpgrade2WidgetServoJ2FOEStatus();
    void setUpgrade2WidgetServoJ2FOEStatus(int status);



    int getUpgrade2WidgetServoJ3XMLStatus();
    void setUpgrade2WidgetServoJ3XMLStatus(int status);

    int getUpgrade2WidgetServoJ3FOEStatus();
    void setUpgrade2WidgetServoJ3FOEStatus(int status);




    int getUpgrade2WidgetServoJ4XMLStatus();
    void setUpgrade2WidgetServoJ4XMLStatus(int status);

    int getUpgrade2WidgetServoJ4FOEStatus();
    void setUpgrade2WidgetServoJ4FOEStatus(int status);




    int getUpgrade2WidgetServoJ5XMLStatus();
    void setUpgrade2WidgetServoJ5XMLStatus(int status);

    int getUpgrade2WidgetServoJ5FOEStatus();
    void setUpgrade2WidgetServoJ5FOEStatus(int status);


    int getUpgrade2WidgetServoJ6XMLStatus();
    void setUpgrade2WidgetServoJ6XMLStatus(int status);

    int getUpgrade2WidgetServoJ6FOEStatus();
    void setUpgrade2WidgetServoJ6FOEStatus(int status);




    int getUpgrade2WidgetTerminalXMLStatus();
    void setUpgrade2WidgetTerminalXMLStatus(int status);

    int getUpgrade2WidgetTerminalFOEStatus();
    void setUpgrade2WidgetTerminalFOEStatus(int status);

    int getUpgrade2WidgetCCBOXFOEStatus();
    void setUpgrade2WidgetCCBOXFOEStatus(int status);

    int getUpgrade2WidgetCCBOXXMLStatus();
    void setUpgrade2WidgetCCBOXXMLStatus(int status);


}

#endif // COMMONDATA_H
