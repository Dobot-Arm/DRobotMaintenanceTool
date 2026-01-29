#include "CommonData.h"

#include <QTime>
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

    DobotType::CabinetType g_strPropertiesCabinetType;
    DobotType::CabinetType getStrPropertiesCabinetType()
    {
        return g_strPropertiesCabinetType;
    }
    void setStrPropertiesCabinetType(DobotType::CabinetType propertiesCabinetType)
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

    DobotType::StructProtocolVersion g_structProtocolVersion;
    void setStructProtocolVersion(DobotType::StructProtocolVersion structProtocolVersion)
    {
        g_structProtocolVersion = structProtocolVersion;
    }

    DobotType::StructProtocolVersion getStructProtocolVersion()
    {
        return g_structProtocolVersion;
    }

    int upgrade2WidgetSafeIOFOEStatus;
    int getSafeIOFOEStatus()
    {
        return upgrade2WidgetSafeIOFOEStatus;
    }

    void setSafeIOFOEStatus(int status)
    {
        upgrade2WidgetSafeIOFOEStatus = status;
    }

    int uprgade2WidgetSafeIOXMLStatus;
    int getSafeIOXMLStatus()
    {
        return uprgade2WidgetSafeIOXMLStatus;
    }

    void setSafeIOXMLStatus(int status)
    {
        uprgade2WidgetSafeIOXMLStatus = status;
    }

    int upgrade2WidgetFeedbackStatus;
    int getFeedbackStatus()
    {
        return upgrade2WidgetFeedbackStatus;
    }

    void setFeedbackStatus(int status)
    {
        upgrade2WidgetFeedbackStatus = status;
    }

    int upgrade2WidgetUniIOFOEStatus;
    int getUniIOFOEStatus()
    {
        return upgrade2WidgetUniIOFOEStatus;
    }

    void setUniIOFOEStatus(int status)
    {
        upgrade2WidgetUniIOFOEStatus = status;
    }


    int upgrade2WidgetUniIOXMLStatus;
    int getUniIOXMLStatus()
    {
        return upgrade2WidgetUniIOXMLStatus;
    }

    void setUniIOXMLStatus(int status)
    {
        upgrade2WidgetUniIOXMLStatus = status;
    }





    int upgrade2WidgetServoJ1XMLStatus;
    int getServoJ1XMLStatus()
    {
        return upgrade2WidgetServoJ1XMLStatus;
    }


    void setServoJ1XMLStatus(int status)
    {
        upgrade2WidgetServoJ1XMLStatus = status;
    }


    int upgrade2WidgetServoJ1FOEStatus;
    int getServoJ1FOEStatus()
    {
        return upgrade2WidgetServoJ1FOEStatus;
    }

    void setServoJ1FOEStatus(int status)
    {
        upgrade2WidgetServoJ1FOEStatus = status;
    }






    int upgrade2WidgetServoJ2XMLStatus;
    int getServoJ2XMLStatus()
    {
        return upgrade2WidgetServoJ2XMLStatus;
    }


    void setServoJ2XMLStatus(int status)
    {
        upgrade2WidgetServoJ2XMLStatus = status;
    }


    int upgrade2WidgetServoJ2FOEStatus;
    int getServoJ2FOEStatus()
    {
        return upgrade2WidgetServoJ2FOEStatus;
    }

    void setServoJ2FOEStatus(int status)
    {
        upgrade2WidgetServoJ2FOEStatus = status;
    }





    int upgrade2WidgetServoJ3XMLStatus;
    int getServoJ3XMLStatus()
    {
        return upgrade2WidgetServoJ3XMLStatus;
    }


    void setServoJ3XMLStatus(int status)
    {
        upgrade2WidgetServoJ3XMLStatus = status;
    }


    int upgrade2WidgetServoJ3FOEStatus;
    int getServoJ3FOEStatus()
    {
        return upgrade2WidgetServoJ3FOEStatus;
    }

    void setServoJ3FOEStatus(int status)
    {
        upgrade2WidgetServoJ3FOEStatus = status;
    }





    int upgrade2WidgetServoJ4XMLStatus;
    int getServoJ4XMLStatus()
    {
        return upgrade2WidgetServoJ4XMLStatus;
    }


    void setServoJ4XMLStatus(int status)
    {
        upgrade2WidgetServoJ4XMLStatus = status;
    }


    int upgrade2WidgetServoJ4FOEStatus;
    int getServoJ4FOEStatus()
    {
        return upgrade2WidgetServoJ4FOEStatus;
    }

    void setServoJ4FOEStatus(int status)
    {
        upgrade2WidgetServoJ4FOEStatus = status;
    }






    int upgrade2WidgetServoJ5XMLStatus;
    int getServoJ5XMLStatus()
    {
        return upgrade2WidgetServoJ5XMLStatus;
    }


    void setServoJ5XMLStatus(int status)
    {
        upgrade2WidgetServoJ5XMLStatus = status;
    }


    int upgrade2WidgetServoJ5FOEStatus;
    int getServoJ5FOEStatus()
    {
        return upgrade2WidgetServoJ5FOEStatus;
    }

    void setServoJ5FOEStatus(int status)
    {
        upgrade2WidgetServoJ5FOEStatus = status;
    }



    int upgrade2WidgetServoJ6XMLStatus;
    int getServoJ6XMLStatus()
    {
        return upgrade2WidgetServoJ6XMLStatus;
    }


    void setServoJ6XMLStatus(int status)
    {
        upgrade2WidgetServoJ6XMLStatus = status;
    }


    int upgrade2WidgetServoJ6FOEStatus;
    int getServoJ6FOEStatus()
    {
        return upgrade2WidgetServoJ6FOEStatus;
    }

    void setServoJ6FOEStatus(int status)
    {
        upgrade2WidgetServoJ6FOEStatus = status;
    }



    int uprgade2WidgetTerminalXMLStatus;
    int getTerminalXMLStatus()
    {
        return uprgade2WidgetTerminalXMLStatus;
    }

    void setTerminalXMLStatus(int status)
    {
        uprgade2WidgetTerminalXMLStatus = status;
    }

    int upgrade2WidgetTerminalFOEStatus;
    int getTerminalFOEStatus()
    {
        return upgrade2WidgetTerminalFOEStatus;
    }

    void setTerminalFOEStatus(int status)
    {
        upgrade2WidgetTerminalFOEStatus = status;
    }

    int upgrade2WidgetCCBOXFOE;
    int getCCBOXFOEStatus()
    {
        return upgrade2WidgetCCBOXFOE;
    }

    void setCCBOXFOEStatus(int status)
    {
        upgrade2WidgetCCBOXFOE = status;
    }

    int upgrade2MainConrtol;
    void setMainControlStatus(int status)
    {
        upgrade2MainConrtol = status;
    }

    int getMainControlStatus()
    {
        return upgrade2MainConrtol;
    }

    int upgrade2WidgetCCBOXXML;
    int getCCBOXXMLStatus()
    {
        return upgrade2WidgetCCBOXXML;
    }

    void setCCBOXXMLStatus(int status)
    {
        upgrade2WidgetCCBOXXML = status;
    }

    int upgrade2WidgetSafeBFOEStatus;
    int getSafeBFOEStatus()
    {
        return upgrade2WidgetSafeBFOEStatus;
    }

    void setSafeBFOEStatus(int status)
    {
        upgrade2WidgetSafeBFOEStatus = status;
    }

    int getRandom()
    {
        int randTime =  QTime::currentTime().msec();
        if(randTime<10)
        {
            return randTime*1000;
        }
        if(randTime<100)
        {
            return randTime*100;
        }
        if(randTime<1000)
        {
            return randTime*10;
        }
        return randTime;
    }

    DobotType::SettingsProductInfoHardwareInfo g_settingsProductInfoHardwareInfo;
    DobotType::SettingsProductInfoHardwareInfo getSettingsProductInfoHardwareInfo()
    {
        return g_settingsProductInfoHardwareInfo;
    }

    void setSettingsProductInfoHardwareInfo(DobotType::SettingsProductInfoHardwareInfo settingsProductInfoHardwareInfo)
    {
        g_settingsProductInfoHardwareInfo = settingsProductInfoHardwareInfo;
    }

    QHash<int,DobotType::StructErrorCode> g_errorCodes;
    void setErroCodes(QHash<int,DobotType::StructErrorCode> errorCodes)
    {
        g_errorCodes = errorCodes;
    }

    QHash<int,DobotType::StructErrorCode> getErrorCodes()
    {
        return g_errorCodes;
    }

}
