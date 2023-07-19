#ifndef DEFINE_H
#define DEFINE_H
#include <QString>
#include <QDebug>
enum SFtpFileAction{
    SystemLogExport_OPT_LISTDIR=201,
    FileBackupStart_OPT_LISTDIR=301
};

enum ExecuteStatus{
    XML_FAIL = -2,
    FAIL_OR_FIRMWARE_FAIL= -1,
    INIT = 0,
    DOING = 1,
    SUCCESS = 2
};

enum SystemFileCheckTipStatus
{
    FileCheck_VEVRSION_FAIL = -2,
    FileCheck_FAIL = -1,
    FileCheck_INIT = 0,
    FileCheck_DOING = 1,
    FileCheck_SUCCESS = 2

};

extern QString g_strCurrentIP;
extern bool g_isEnglish;
extern bool g_isRestartServer;
extern bool g_isOnline;
extern QString g_strPropertiesCabinetTypeCCBOX;
extern QString g_strPropertiesCabinetTypeCC162;
extern QString g_strPropertiesCabinetTypeCC262;
extern QString g_strPropertiesCabinetTypeCC26X;
extern QString g_strPropertiesCabinetTypeMG400;
extern QString g_strPropertiesCabinetTypeMagicianE6;
extern QString g_strPropertiesCabinetTypeCC263;


extern QString g_strSpinServoParamsFailStyleSheet;
extern QString g_strSpinServoParamsSuccessStyleSheet;

extern QString g_strSuccess;

extern QString g_strFontFailedStyleSheet;
extern QString g_strFontSuccessStyleSheet;

extern QString g_strServoParamsPassword;

extern QString g_strServoParamsColValueTrue;

extern QString g_strServoParamsColValueFalse;

extern QString g_strUpgradeVersion;
extern QString g_strUpgradeParentVersion;

extern QString g_strAddr200118;

const int colIsY = 0;
const int colDescription = 1;
const int colType = 2;
const int colStartId = 3;
const int colQueryId = 4;
const int colFunctionId = 5;
const int colJudgement = 6;
const int colMsgBox = 7;
const int colDownExcept = 8;
const int colUpExcept = 9;
const int colTimeOut = 10;
const int colBackData = 11;
const int colResult = 12;
const int colSingleTest = 13;
const int colTestProgress = 14;

const int colPort = 1;
const int colUrl = 2;
const int colRequestType = 3;
const int colBody = 4;
const int colHttpTimeOut = 5;
const int colExpectResult = 6;
const int colActualResult = 7;

const int colIsCheck = 2;
const int colWirte = 0;
const int colRead = 1;
const int colParaIndex = 5;
const int colValue = 6;
const int colDotDividedBits = 13;
const int colIsReadOnly = 14;






//初始化表格最大行鼠标 最大列数
const int modelRowMax = 300;
const int modelColMax = 30;

#define COLUMN_SINGLEWRITE           "单独写入"
#define COLUMN_SINGLEREAD           "单独读取"



namespace DobotType {
    typedef struct
    {
        QString controlMode;
        QString dragMode;
        bool dragPlayback;
        QString powerState;
        QString coordinate;
        int isSafeSuspend;
    }protocolExchange;

    typedef struct
    {
        int alarmsID[7][64];

        int industrialType;
    }Alarms;

    struct SettingsVersion
    {

        QString algs;
        QString codesys;
        QString control;
        QString feedback;
        QString safeio;
        QString servo1;
        QString servo2;
        QString servo3;
        QString servo4;
        QString servo5;
        QString servo6;
        QString system;
        QString terminal;
        QString unio;
        void toString(){
            qDebug()<<" algs "<<algs<<"\n codesys "<<codesys<<"\n control "<<control
                   <<"\n feedback "<<feedback<<"\n safeio "<<safeio<<"\n servo1 "
                    <<servo1 <<"\n servo2 "<<servo2<<"\n servo3 "<<servo3<<"\n servo4 "<<servo4
                   <<"\n servo5 "<<servo5<< "\n servo6 "<<servo6<<"\n system "<<system
                  <<"\n terminal "<<terminal<<"\n unio "<<unio;
        }
    };

    struct ControllerType{
        int version;
        QString name;
    };

    struct CurrentVersion{
        QString unio;
        QString safeio;
        QString servo1;
        QString servo2;
        QString servo3;
        QString servo4;
        QString servo5;
        QString servo6;
        QString system;
        QString terminal;
        QString feedback;
        QString control;
    };
    struct UpdateFirmware{
        CurrentVersion currentVersion;
        QString operation;
        QString cabType;
        QString cabVersion;
    };

    struct StructCRSingleFwAndXml{
        QString operationType;
        QString cabType;
        QString cabVersion;
        QString updateType;
        QString updateFile;
        QString updateFile2;
        int seqId = -1;
        int slaveId;

        void toString(){
            qDebug()<<" operationType "<<operationType<<"\n cabType "<<cabType<<"\n cabVersion "<<cabVersion
                   <<"\n updateType "<<updateType<<"\n updateFile "<<updateFile<<"\n updateFile2 "
                    <<updateFile2 <<"\n slaveId "<<slaveId;
        }
    };

    struct StructM1ProSingleFwAndXml{
        QString operationType;
        QString updateType;
        QString updateFile;
        int slaveId;
    };


    struct StructMG400SingleFwAndXml{
        QString operationType;
        QString updateType;
        QString updateFile;
        int slaveId;
    };


    struct UpdateStatus{
        bool bIsOutage = false;
        QList<int> FWUpdateResult;
        QList<int> XMLUpdateResult;
    };

    struct StructServoParam{
        QString servoNum;
        QString key;
        double value;
        bool status;
    };
    struct StructSettingsServoParams{
        QString src;
        QList<StructServoParam> servoParams;
    };

    struct StructSettingsServoParamsResult{
        bool status;
        QList<StructServoParam> servoParams;
    };



    struct ProtocolExchangeResult{
//         QString controlMode;
//         QString dragMode;
//         bool dragPlayback;
//         QString powerState;
//         QString coordinate;
//         QString jogMode;
         QString prjState;
         bool isAlarm;
//         int toolCoordinate;
//         int userCoordinate;
//         QString autoManual;

    };
}
enum PropertiesCabinetType{
    CCBOX,
    CC162,
    CC262,
    MG400,
    M1Pro,
    MG6
};

enum Firmware{
    TerminalIO,
    Servo,
    UniversalIO,
    SafeIO,
    Controller
};

enum AllUpgradeStatus{
    AllUpgradeStatus_SUCCESS=2,
    AllUpgradeStatus_FWFAIILED=-1,
    AllUpgradeStatus_FAIILED = -1,
    AllUpgradeStatus_XMLFAIILED=-2,
    AllUpgradeStatus_DOING=1,
    AllUpgradeStatus_INIT=0
};

enum ConnectState{
    unconnected=-1,
    connected,
    occupied=1
};




#endif // DEFINE_H
