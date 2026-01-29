#ifndef DOBOTPROTOCOL_H
#define DOBOTPROTOCOL_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QEventLoop>
#include <QList>
#include "DHttpManager.h"
#include "Define.h"
#include "DHttpCurlManager.h"
#include "CommonData.h"
#include "Logger.h"
class DobotProtocol : public QObject
{
    Q_OBJECT
public:
    DobotProtocol();
    ConnectState getConnectionState(QString ip);
    DobotType::SettingsVersion postSettingsVersion(QString ip,QString postData);
    DobotType::ControllerType getControllerType(QString ip);
    DobotType::CabinetType getPropertiesCabinetType(QString ip);
    DobotType::ProtocolExchangeResult getProtocolExchange(QString ip);
    QList<QPair<QString,QString>> getNetworkCardMac(QString ip);
    DobotType::StructFaultCheck postFaultCheck(QString ip,QString postData);
    /*
     *功  能：故障检查
     *参  数：strIP-ip地址，
              data-故障检查参数，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：故障检查返回值，具体看实体类
     */
    FaultCheckResult postFaultCheck(const QString& strIP,
                 const FaultCheckData& data,
                 int iTimeoutMilliseconds=3000);
    QStringList getXmlVersion(QString ip);
    QString postFaultRepair(QString ip,QString postData,QString macAddress = QString(),QString date = QString(),QString time = QString(),QString control = QString());
    void setCurrentIp(QString ip);
    QString getCurrentIp();
    bool getIsOnline();
    DobotType::SettingsVersion getSettingsVersion(QString ip);
    QString postUpdateDiskCheck(QString ip);
    QString postUpdateCRFwAndXml(QString ip,DobotType::UpdateFirmware updateFirmware);
    QString postUpdateCRSingleFwAndXml(QString ip,DobotType::StructCRSingleFwAndXml singleCRFwAndXml);
    QString postUpdateM1ProSingleFwAndXml(QString ip,DobotType::StructM1ProSingleFwAndXml singleM1ProFwAndXml);
    QString postUpdateMG400SingleFwAndXml(QString ip,DobotType::StructMG400SingleFwAndXml singleMG400FwAndXml);
    DobotType::StructGetCRSingleStatus getUpdateSingleStatus(QString ip);
    DobotType::UpdateStatus getUpdateCRStatus(QString ip);
    DobotType::StructProtocolVersion getProtocolVersion(QString ip);
    bool postSettingsModifyServoParams(QString ip,DobotType::StructSettingsServoParams settingsServoParams);
    DobotType::StructSettingsServoParamsResult getSettingsModifyServoParams(QString ip);
    bool postSettingsReadServoParams(QString ip,DobotType::StructSettingsServoParams settingsServoParams);
    DobotType::StructSettingsServoParamsResult getSettingsReadServoParams(QString ip);
    QHash<QString, DobotType::StructServoValue> getReadServoValueDianjixinghao(QString ip);//获取6个伺服电机型号
    QString postUpdateCheckKernelVersion(QString ip,QString cabVersion);
    QString getUpdateCheckPartitionOccupancy(QString ip);
    bool postSettingsControlMode(QString ip,QString controlMode);
    QString getSettingsControlMode(QString ip);
    QString postUpdateKernel(QString ip,QString cabVersion);
    QString postUpdateContinue(QString ip);
    DobotType::SettingsProductInfoHardwareInfo getSettingsProductInfoHardwareInfo(QString ip);
    /*
     *功  能：设置控制器的SN的http接口（厂家功能）
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true表示适合升级，false表示不适合升级
     */
    //settings/productInfo/controllerSn
    QString getSettingsProductInfoControllerSn(const QString& strIP,
                               int iTimeoutMilliseconds=5000);
    /*
     *功  能：设置控制器的SN的http接口（厂家功能）
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true表示适合升级，false表示不适合升级
     */
    //settings/productInfo/controllerSn
    bool postSettingsProductInfoControllerSn(const QString& strIP,const ControllerSnData& controlSNData,
                               int iTimeoutMilliseconds=5000);
    /*
     *功  能：设置V3控制器的SN的http接口（厂家功能）
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true表示适合升级，false表示不适合升级
     */
    //settings/productInfo/controllerSn
    bool postV3SettingsProductInfoControllerSn(const QString& strIP,const V3ControllerSnData& controlSNData,
                               int iTimeoutMilliseconds=5000);
    /*
     *功  能：设置本体的SN的http接口（厂家功能）
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
             请求时间起码有6s
     *返回值：true表示适合升级，false表示不适合升级
     */
    bool postSettingsProductInfoRobotArmSn(const QString& strIP,const RobotArmSnData& robotArmSNData,
                               int iTimeoutMilliseconds=10000);

    /*
     *功  能：清楚报警
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true表示适合升级，false表示不适合升级
     */
    bool postInterfaceClearAlarms(const QString& strIP,
                               int iTimeoutMilliseconds=5000);
private:
    void sleep(int milliseconds);
//    DHttpManager* m_httpManager;
//    DHttpManager* m_getCRStatusHttpManager;
//    DHttpManager* m_getConnectionStateHttpManager;
    int m_id = 0;
    DHttpCurlManager* m_httpManager;
    DHttpCurlManager* m_getCRStatusHttpManager;
    DHttpCurlManager* m_getConnectionStateHttpManager;

};

#endif // DOBOTPROTOCOL_H
