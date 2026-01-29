#ifndef COMMONDATA_H
#define COMMONDATA_H
#include "Define.h"
namespace CommonData {
    void setCurrentSettingsVersion(const DobotType::SettingsVersion& settingsVersion);
    DobotType::SettingsVersion getCurrentSettingsVersion();

    void setUpgradeSettingsVersion(const DobotType::SettingsVersion& settingsVersion);
    DobotType::SettingsVersion getUpgradeSettingsVersion();

    DobotType::CabinetType getStrPropertiesCabinetType();
    void setStrPropertiesCabinetType(DobotType::CabinetType propertiesCabinetType);

    DobotType::ControllerType getControllerType();
    void setControllerType(DobotType::ControllerType controllerType);

    DobotType::StructProtocolVersion getStructProtocolVersion();
    void setStructProtocolVersion(DobotType::StructProtocolVersion);


    DobotType::SettingsProductInfoHardwareInfo getSettingsProductInfoHardwareInfo();
    void setSettingsProductInfoHardwareInfo(DobotType::SettingsProductInfoHardwareInfo settingsProductInfoHardwareInfo);

    void setAllUpgradeToSingleUpgradeCount(int count);

    int getSafeIOFOEStatus();
    void setSafeIOFOEStatus(int status);



    int getSafeBFOEStatus();
    void setSafeBFOEStatus(int status);

    int getSafeIOXMLStatus();
    void setSafeIOXMLStatus(int status);

    int getFeedbackStatus();
    void setFeedbackStatus(int status);

    int getUniIOFOEStatus();
    void setUniIOFOEStatus(int status);

    int getUniIOXMLStatus();
    void setUniIOXMLStatus(int status);



    int getServoJ1XMLStatus();
    void setServoJ1XMLStatus(int status);

    int getServoJ1FOEStatus();
    void setServoJ1FOEStatus(int status);



    int getServoJ2XMLStatus();
    void setServoJ2XMLStatus(int status);

    int getServoJ2FOEStatus();
    void setServoJ2FOEStatus(int status);



    int getServoJ3XMLStatus();
    void setServoJ3XMLStatus(int status);

    int getServoJ3FOEStatus();
    void setServoJ3FOEStatus(int status);




    int getServoJ4XMLStatus();
    void setServoJ4XMLStatus(int status);

    int getServoJ4FOEStatus();
    void setServoJ4FOEStatus(int status);




    int getServoJ5XMLStatus();
    void setServoJ5XMLStatus(int status);

    int getServoJ5FOEStatus();
    void setServoJ5FOEStatus(int status);


    int getServoJ6XMLStatus();
    void setServoJ6XMLStatus(int status);

    int getServoJ6FOEStatus();
    void setServoJ6FOEStatus(int status);




    int getTerminalXMLStatus();
    void setTerminalXMLStatus(int status);

    int getTerminalFOEStatus();
    void setTerminalFOEStatus(int status);

    int getCCBOXFOEStatus();
    void setCCBOXFOEStatus(int status);

    void setMainControlStatus(int status);
    int getMainControlStatus();

    int getCCBOXXMLStatus();
    void setCCBOXXMLStatus(int status);

    int getRandom();


    void setErroCodes(QHash<int,DobotType::StructErrorCode>);
    QHash<int,DobotType::StructErrorCode> getErrorCodes();


}

#endif // COMMONDATA_H
