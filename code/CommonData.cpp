#include "CommonData.h"
namespace CommonData {
    static DobotType::SettingsVersion g_currentSettingVersion;
    void setCurrentSettingsVersion(const DobotType::SettingsVersion &settingsVersion)
    {
        g_currentSettingVersion = settingsVersion;
    }

    DobotType::SettingsVersion getCurrentSettingsVersion()
    {
        return g_currentSettingVersion;
    }

    static DobotType::SettingsVersion g_upgradeSettingVersion;
    void setUpgradeSettingsVersion(const DobotType::SettingsVersion &settingsVersion)
    {
        g_upgradeSettingVersion = settingsVersion;
    }

    DobotType::SettingsVersion getUpgradeSettingsVersion()
    {
        return g_upgradeSettingVersion;
    }

    QString g_strPropertiesCabinetType;
    QString getStrPropertiesCabinetType()
    {
        return g_strPropertiesCabinetType;
    }
    void setStrPropertiesCabinetType(QString propertiesCabinetType)
    {
        g_strPropertiesCabinetType = propertiesCabinetType;
    }

    DobotType::ControllerType g_controllerType;
    DobotType::ControllerType getControllerType()
    {
        return g_controllerType;
    }

    void setControllerType(DobotType::ControllerType controllerType)
    {
        g_controllerType = controllerType;
    }

    int g_upgradeCount = 0;
    void setAllUpgradeToSingleUpgradeCount(int count)
    {
        g_upgradeCount = count;
    }
    int getAllUpgradeToSingleUpgradeCount()
    {
        return g_upgradeCount;
    }

    int upgrade2WidgetSafeIOFOEStatus;
    int getUpgrade2WidgetSafeIOFOEStatus()
    {
        return upgrade2WidgetSafeIOFOEStatus;
    }

    void setUpgrade2WidgetSafeIOFOEStatus(int status)
    {
        upgrade2WidgetSafeIOFOEStatus = status;
    }

    int uprgade2WidgetSafeIOXMLStatus;
    int getUpgrade2WidgetSafeIOXMLStatus()
    {
        return upgrade2WidgetSafeIOFOEStatus;
    }

    void setUpgrade2WidgetSafeIOXMLStatus(int status)
    {
        uprgade2WidgetSafeIOXMLStatus = status;
    }

    int upgrade2WidgetFeedbackStatus;
    int getUpgrade2WidgetFeedbackStatus()
    {
        return upgrade2WidgetFeedbackStatus;
    }

    void setUpgrade2WidgetFeedbackStatus(int status)
    {
        upgrade2WidgetFeedbackStatus = status;
    }

    int upgrade2WidgetUniIOFOEStatus;
    int getUpgrade2WidgetUniIOFOEStatus()
    {
        return upgrade2WidgetUniIOFOEStatus;
    }

    void setUpgrade2WidgetUniIOFOEStatus(int status)
    {
        upgrade2WidgetUniIOFOEStatus = status;
    }


    int upgrade2WidgetUniIOXMLStatus;
    int getUpgrade2WidgetUniIOXMLStatus()
    {
        return upgrade2WidgetUniIOXMLStatus;
    }

    void setUpgrade2WidgetUniIOXMLStatus(int status)
    {
        upgrade2WidgetUniIOXMLStatus = status;
    }





    int upgrade2WidgetServoJ1XMLStatus;
    int getUpgrade2WidgetServoJ1XMLStatus()
    {
        return upgrade2WidgetServoJ1XMLStatus;
    }


    void setUpgrade2WidgetServoJ1XMLStatus(int status)
    {
        upgrade2WidgetServoJ1XMLStatus = status;
    }


    int upgrade2WidgetServoJ1FOEStatus;
    int getUpgrade2WidgetServoJ1FOEStatus()
    {
        return upgrade2WidgetServoJ1FOEStatus;
    }

    void setUpgrade2WidgetServoJ1FOEStatus(int status)
    {
        upgrade2WidgetServoJ1FOEStatus = status;
    }






    int upgrade2WidgetServoJ2XMLStatus;
    int getUpgrade2WidgetServoJ2XMLStatus()
    {
        return upgrade2WidgetServoJ2XMLStatus;
    }


    void setUpgrade2WidgetServoJ2XMLStatus(int status)
    {
        upgrade2WidgetServoJ2XMLStatus = status;
    }


    int upgrade2WidgetServoJ2FOEStatus;
    int getUpgrade2WidgetServoJ2FOEStatus()
    {
        return upgrade2WidgetServoJ2FOEStatus;
    }

    void setUpgrade2WidgetServoJ2FOEStatus(int status)
    {
        upgrade2WidgetServoJ2FOEStatus = status;
    }





    int upgrade2WidgetServoJ3XMLStatus;
    int getUpgrade2WidgetServoJ3XMLStatus()
    {
        return upgrade2WidgetServoJ3XMLStatus;
    }


    void setUpgrade2WidgetServoJ3XMLStatus(int status)
    {
        upgrade2WidgetServoJ3XMLStatus = status;
    }


    int upgrade2WidgetServoJ3FOEStatus;
    int getUpgrade2WidgetServoJ3FOEStatus()
    {
        return upgrade2WidgetServoJ3FOEStatus;
    }

    void setUpgrade2WidgetServoJ3FOEStatus(int status)
    {
        upgrade2WidgetServoJ3FOEStatus = status;
    }





    int upgrade2WidgetServoJ4XMLStatus;
    int getUpgrade2WidgetServoJ4XMLStatus()
    {
        return upgrade2WidgetServoJ4XMLStatus;
    }


    void setUpgrade2WidgetServoJ4XMLStatus(int status)
    {
        upgrade2WidgetServoJ4XMLStatus = status;
    }


    int upgrade2WidgetServoJ4FOEStatus;
    int getUpgrade2WidgetServoJ4FOEStatus()
    {
        return upgrade2WidgetServoJ4FOEStatus;
    }

    void setUpgrade2WidgetServoJ4FOEStatus(int status)
    {
        upgrade2WidgetServoJ4FOEStatus = status;
    }






    int upgrade2WidgetServoJ5XMLStatus;
    int getUpgrade2WidgetServoJ5XMLStatus()
    {
        return upgrade2WidgetServoJ5XMLStatus;
    }


    void setUpgrade2WidgetServoJ5XMLStatus(int status)
    {
        upgrade2WidgetServoJ5XMLStatus = status;
    }


    int upgrade2WidgetServoJ5FOEStatus;
    int getUpgrade2WidgetServoJ5FOEStatus()
    {
        return upgrade2WidgetServoJ5FOEStatus;
    }

    void setUpgrade2WidgetServoJ5FOEStatus(int status)
    {
        upgrade2WidgetServoJ5FOEStatus = status;
    }



    int upgrade2WidgetServoJ6XMLStatus;
    int getUpgrade2WidgetServoJ6XMLStatus()
    {
        return upgrade2WidgetServoJ6XMLStatus;
    }


    void setUpgrade2WidgetServoJ6XMLStatus(int status)
    {
        upgrade2WidgetServoJ6XMLStatus = status;
    }


    int upgrade2WidgetServoJ6FOEStatus;
    int getUpgrade2WidgetServoJ6FOEStatus()
    {
        return upgrade2WidgetServoJ6FOEStatus;
    }

    void setUpgrade2WidgetServoJ6FOEStatus(int status)
    {
        upgrade2WidgetServoJ6FOEStatus = status;
    }



    int uprgade2WidgetTerminalXMLStatus;
    int getUpgrade2WidgetTerminalXMLStatus()
    {
        return uprgade2WidgetTerminalXMLStatus;
    }

    void setUpgrade2WidgetTerminalXMLStatus(int status)
    {
        uprgade2WidgetTerminalXMLStatus = status;
    }

    int upgrade2WidgetTerminalFOEStatus;
    int getUpgrade2WidgetTerminalFOEStatus()
    {
        return upgrade2WidgetTerminalFOEStatus;
    }

    void setUpgrade2WidgetTerminalFOEStatus(int status)
    {
        upgrade2WidgetTerminalFOEStatus = status;
    }

    int upgrade2WidgetCCBOXFOE;
    int getUpgrade2WidgetCCBOXFOEStatus()
    {
        return upgrade2WidgetCCBOXFOE;
    }

    void setUpgrade2WidgetCCBOXFOEStatus(int status)
    {
        upgrade2WidgetCCBOXFOE = status;
    }

    int upgrade2WidgetCCBOXXML;
    int getUpgrade2WidgetCCBOXXMLStatus()
    {
        return upgrade2WidgetCCBOXXML;
    }

    void setUpgrade2WidgetCCBOXXMLStatus(int status)
    {
        upgrade2WidgetCCBOXXML = status;
    }

    int upgrade2WidgetSafeBFOEStatus;
    int getUpgrade2WidgetSafeBFOEStatus()
    {
        return upgrade2WidgetSafeBFOEStatus;
    }

    void setUpgrade2WidgetSafeBFOEStatus(int status)
    {
        upgrade2WidgetSafeBFOEStatus = status;
    }






}
