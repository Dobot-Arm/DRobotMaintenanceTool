#ifndef DEFINE_H
#define DEFINE_H
#include <QString>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QJsonObject>

//特殊临时版本只需要用一次，所以屏蔽掉，但是不删除，防止后面又要修改
/*
#ifndef V136_NOVA_UPDATE
#define V136_NOVA_UPDATE "V1.3.6-NovaUpdate"
#endif
*/

enum SFtpFileAction{
    SystemLogExport_OPT_LISTDIR=201
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

extern QString g_controlModeDisable;
extern QString g_controlModeEnable;

extern QString g_strCurrentIP;
extern bool g_isEnglish;
extern bool g_isRestartServer;
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

extern QString g_strAddr200118;
extern QString g_strAddr200101;
extern QString g_strAddr200102;

extern QString g_strHttpDobotPrefixDebug;
extern QString g_strHttpDobotPrefixRealse;
extern QString g_strHttpDobotPrefix;
extern QString g_strAllVersion;
const int colPort = 1;
const int colUrl = 2;
const int colRequestType = 3;
const int colBody = 4;
const int colHttpTimeOut = 5;
const int colExpectResult = 6;
const int colActualResult = 7;


const int colReadAWirte = 0;
const int colIsCheck = 1;
const int colGroupCatogary = 2;
const int colSaftyLevel = 3;
const int colParaIndex = 4;
const int colValue = 5;
const int colParaName = 10;
const int colParaMapIndex = 11;
const int colDotDividedBits = 12;
const int colIsReadOnly = 13;


//初始化表格最大行鼠标 最大列数
const int modelRowMax = 300;
const int modelColMax = 30;

#define COLUMN_SINGLEWRITE           "单独写入"
#define COLUMN_SINGLEREAD           "单独读取"

/*比较版本号
版本号格式为 xxx.xxx.xxx.xxx
返回值：>0表示strV1高于strV2,==0表示版本号相等，<0表示strV1低于strV2
*/
extern int CompareVersion(const QString& strV1, const QString& strV2);
extern bool isCurrentFWLargerTargetFWVersion(QString currentFW,QString targertFW);
extern bool isCurrentControlLagerTargetControl(QString currentControl,QString targetControl);

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
        QString originName; //没有被替换前的name
        QString nameExt;
    };

    struct CabinetType{
        QString name;
        QString power;
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
        QString updateControlVersion;
    };

    struct StructCRSingleFwAndXml{
        QString operationType;
        QString cabType;
        QString cabVersion;
        QString updateType;
        QStringList allUpdateFile;//动态扩展,至少2个,依次对应字段 updateFile,updateFile2,updateFile3,updateFile4....
        QString updateControlVersion;
        int seqId = -1;
        int slaveId;
        void toString(){
            qDebug()<<" operationType "<<operationType<<"\n cabType "<<cabType<<"\n cabVersion "<<cabVersion
                   <<"\n updateType "<<updateType<<"\n allUpdateFile "<<allUpdateFile <<"\n slaveId "<<slaveId;
        }
    };

    struct StructGetCRSingleStatus{
        int result;
        int errorCode;
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

    struct StrutctBackupInfo
    {
        bool status; //当数值为true时，证明此时需要把备份包从控制器传输到本地，false时就不用。
        QString path; //备份包传输全路径
    };

    struct UpdateStatus{
        bool bIsOutage = false;
        /*长度为12，依次分别是：
通用io，
主控板，
馈能板(仅大控制柜)，
安全io(二代柜安全控制器b板)(仅大控制柜)，
安全io(二代柜安全控制器的a板)(仅大控制柜)，
1轴，2轴，3轴，4轴，5轴，6轴，
末端io
*/
        QList<int> FWUpdateResult;
        /*长度固定，依次是：通用io、安全io、1轴，2轴，3轴，4轴，5轴，6轴，末端io；*/
        QList<int> XMLUpdateResult;
        int errorCode = 0;
        StrutctBackupInfo backupInfo;

    };

    struct StructPathInfo
    {
        QString slave;
        QString control;
        QString kernel;
        QString v4restoreBackup;
    };

    struct StructProtocolVersion{
        QString toolVersion;
        StructPathInfo pathInfo;

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
        bool status = false;
        QList<StructServoParam> servoParams;
    };

    struct StructServoValue{
        int jointId; //关机序号1~6
        int dianji; //电机型号值
        int changjia; //厂家值
    };

    struct StructFaultCheck
    {
        QString result;
        QStringList missFiles;
    };

    struct ProtocolExchangeResult{
         QString controlMode;
         QString dragMode;
         bool dragPlayback;
         QString powerState;
         QString coordinate;
         int isSafeSuspend;
         QList<QList<int>> alarms;

         QString jogMode;
         QString prjState;
         bool isAlarm;
    };

    struct SettingsProductInfoHardwareInfo
    {
        QString RobotArmSNCode;  // 当前控制柜匹配的本体SN序列号
        QString ControllCabinetSNCode; // 控制柜SN序列号
        QString RealArmSNCode;  // 当前连接的真实本体SN序列号
    };


    struct StructErrorCode
    {
        int errorCode;
        QString zhDescription;
        QString zhSolution;
        QString enDescription;
        QString enSolution;
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


#define SET_STYLE_SHEET(className)                \
do{                                               \
    QString strQss;                               \
    QFile file(":/qss/myqss/"#className".qss");     \
    if (file.open(QIODevice::ReadOnly)){          \
        strQss = QLatin1String(file.readAll());   \
        file.close();                             \
        this->setStyleSheet(strQss);              \
    }                                             \
}while(false)

extern bool g_bReStartApp;
class MainWidget2;
extern void PublicSetMainWindow(MainWidget2 *);
extern MainWidget2 *PublicgGetMainWindow();

extern void PublicSleep(int milliseconds);
//解析csv文件
extern QList<QStringList> parseCSVFile(const QString &strFile);
//检查文件的编码格式
extern const char* checkFileTextCode(QFile* pFile);

//获取updateconfig.json配置文件本地路径
extern QString getUpdateConfigJsonFile();
//获取updateconfig.json配置文件的下载url
extern QString getUpdateConfigJsonFileUrl();
//获取updatefirmware.lua文件的本地路径
extern QString getUpdateFirmwareLuaFile();
//获取startServer.zip工具文件本地路径
extern QString getStartServerZipFile();
//后去startServer.zip工具本地存放的目录
extern QString getStartServerZipFileDir();
//伺服模板、关节电机类型对照表文件
extern QString getServoParamTemplateDir();
extern QString getServoJointTableFile();
extern QString getServoTemplateJointFileZIP();
extern QString getServoTemplateJointFileUrl();
//获取KeyParameter.csv文件的完整路径
extern QString getKeyParameterCSVFile();
//获取当前机型的伺服参数模板param.csv文件完整路径
extern QString getServoTemplateFile();
//获取升级包中伺服关节需要文件的系列文件夹名称，也就是servoCSV的下一级目录名称
extern QString getServoCSVSerialTypeName();

//获取控制器删除脚本的文件目录
extern QString getControllerScriptDir();
extern QString getControllerScriptFileZIP();
//获取控制器删除脚本的下载地址
extern QString getControllerScriptFileUrl();

//获取恢复出厂设置的文件目录
extern QString getFactoryRecoveryDir();
extern QString getFactoryRecoveryFile();

struct ControllerSnData
{
    QString ControllCabinetSNCode; //控制器SN码
    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};
struct V3ControllerSnData
{
    QString SNcode; //V3控制器SN码
    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};
struct RobotArmSnData
{
    QString RobotArmSNCode; //分别是这些值dns/mac/files，表示不同功能
    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

struct FaultCheckData
{
    QString operationType; //分别是这些值dns/mac/files，表示不同功能
    QString control; //控制器版本，dns/files需要control
    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};
struct FaultCheckResult
{
    QString result; // success/other reasons of failure 成功/其余失败
    QStringList missingFiles; //file 文件检测缺失时，列举缺失的文件
    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString &strJson);
    void fromJson(const QJsonObject& obj);
};

struct CRUpdateInfo
{
    bool valid;
    int index;
    int length;
};
extern CRUpdateInfo getMatchCRUpdateInfo(const QString strCRUpdateName);

#endif // DEFINE_H
