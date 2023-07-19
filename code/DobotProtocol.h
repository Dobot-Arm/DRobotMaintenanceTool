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

class DobotProtocol : public QObject
{
    Q_OBJECT
public:
    DobotProtocol();
    ConnectState getConnectionState(QString ip);
    DobotType::SettingsVersion postSettingsVersion(QString ip,QString postData);
    DobotType::ControllerType getControllerType(QString ip);
    PropertiesCabinetType getPropertiesCabinetType(QString ip);
    QString getPropertiesCabinetTypeStr(QString ip);
    DobotType::ProtocolExchangeResult getProtocolExchange(QString ip);
    QString postFaultCheck(QString ip,QString postData);
    QStringList getXmlVersion(QString ip);
    QString postFaultRepair(QString ip,QString postData,QString macAddress = QString(),QString date = QString(),QString time = QString());
    void setCurrentIp(QString ip);
    QString getCurrentIp();
    bool getIsOnline();
    DobotType::SettingsVersion getSettingsVersion(QString ip);
    QString postUpdateDiskCheck(QString ip);
    QString postUpdateCRFwAndXml(QString ip,DobotType::UpdateFirmware updateFirmware);
    QString postUpdateCRSingleFwAndXml(QString ip,DobotType::StructCRSingleFwAndXml singleCRFwAndXml);
    QString postUpdateM1ProSingleFwAndXml(QString ip,DobotType::StructM1ProSingleFwAndXml singleM1ProFwAndXml);
    QString postUpdateMG400SingleFwAndXml(QString ip,DobotType::StructMG400SingleFwAndXml singleMG400FwAndXml);
    int getUpdateSingleStatus(QString ip);
    DobotType::UpdateStatus getUpdateCRStatus(QString ip);
    QString getProtocolVersion(QString ip);
    bool postSettingsModifyServoParams(QString ip,DobotType::StructSettingsServoParams settingsServoParams);
    DobotType::StructSettingsServoParamsResult getSettingsModifyServoParams(QString ip);
    bool postSettingsReadServoParams(QString ip,DobotType::StructSettingsServoParams settingsServoParams);
    DobotType::StructSettingsServoParamsResult getSettingsReadServoParams(QString ip);
    QString postUpdateCheckKernelVersion(QString ip,QString cabVersion);
    //http://192.168.5.1:22002/update/checkPartitionOccupancy
    QString getUpdateCheckPartitionOccupancy(QString ip);
private:
    void sleep(int milliseconds);
    DHttpManager* m_httpManager;
};

#endif // DOBOTPROTOCOL_H
