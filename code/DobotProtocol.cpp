#include "DobotProtocol.h"
#include <QTimer>
DobotProtocol::DobotProtocol()
{
    m_httpManager = new DHttpManager();
}
ConnectState DobotProtocol::getConnectionState(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/connection/state",2000);
    if(getReply.isEmpty())
    {
        return ConnectState::unconnected;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    if(jsonObject.contains("value")){
      QString connectState = jsonObject.value("value").toString();
      if(connectState == "connected"){
          return ConnectState::connected;
      }else if(connectState == "occupied"){
          qDebug()<<ip << " occupied  ";
          return ConnectState::occupied;
      }else{
          return ConnectState::unconnected;
      }
    }
    return ConnectState::unconnected;
}

DobotType::SettingsVersion DobotProtocol::getSettingsVersion(QString ip)
{
    QByteArray postReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/settings/version");
    qDebug()<<"postReply  " <<postReply;
    DobotType::SettingsVersion settingsVersion;
    if(postReply.isEmpty())
    {
        return settingsVersion;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    settingsVersion.algs = jsonObject.value("algs").toString();
    settingsVersion.codesys = jsonObject.value("codesys").toString();
    //当前控制器版本
    settingsVersion.control = jsonObject.value("control").toString();
    settingsVersion.feedback = jsonObject.value("feedback").toString();
    //安全IO
    settingsVersion.safeio = jsonObject.value("safeio").toString();
    //伺服
    settingsVersion.servo1 = jsonObject.value("servo1").toString();
    settingsVersion.servo2 = jsonObject.value("servo2").toString();
    settingsVersion.servo3 = jsonObject.value("servo3").toString();
    settingsVersion.servo4 = jsonObject.value("servo4").toString();
    settingsVersion.servo5 = jsonObject.value("servo5").toString();
    settingsVersion.servo6 = jsonObject.value("servo6").toString();
    //当前系统版本
    settingsVersion.system = jsonObject.value("system").toString();
    //当前末端IO
    settingsVersion.terminal = jsonObject.value("terminal").toString();
    //当前通用IO
    settingsVersion.unio = jsonObject.value("unio").toString();
    /***********/

    qDebug()<< "settingsVersion  ";
    settingsVersion.toString();

    return settingsVersion;
}



DobotType::SettingsVersion DobotProtocol::postSettingsVersion(QString ip, QString postData)
{
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22000"+"/settings/version",postData);
    qDebug()<<"postReply  " <<postReply;
    DobotType::SettingsVersion settingsVersion;
    if(postReply.isEmpty())
    {
        return settingsVersion;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    settingsVersion.algs = jsonObject.value("algs").toString();
    settingsVersion.codesys = jsonObject.value("codesys").toString();
    //当前控制器版本
    settingsVersion.control = jsonObject.value("control").toString();
    settingsVersion.feedback = jsonObject.value("feedback").toString();
    //安全IO
    settingsVersion.safeio = jsonObject.value("safeio").toString();
    //伺服
    settingsVersion.servo1 = jsonObject.value("servo1").toString();
    settingsVersion.servo2 = jsonObject.value("servo2").toString();
    settingsVersion.servo3 = jsonObject.value("servo3").toString();
    settingsVersion.servo4 = jsonObject.value("servo4").toString();
    settingsVersion.servo5 = jsonObject.value("servo5").toString();
    settingsVersion.servo6 = jsonObject.value("servo6").toString();
    //当前系统版本
    settingsVersion.system = jsonObject.value("system").toString();
    //当前末端IO
    settingsVersion.terminal = jsonObject.value("terminal").toString();
    //当前通用IO
    settingsVersion.unio = jsonObject.value("unio").toString();
    /***********/

    qDebug()<< "settingsVersion  ";
    settingsVersion.toString();

    return settingsVersion;
}

DobotType::ControllerType DobotProtocol::getControllerType(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/properties/controllerType");
    qDebug()<<"/properties/controllerType getReply  " <<getReply;
    DobotType::ControllerType controllerType;
    if(getReply.isEmpty())
    {
        return controllerType;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    controllerType.name = jsonObject.value("name").toString();
    controllerType.version = jsonObject.value("version").toInt();

    return controllerType;
}



PropertiesCabinetType DobotProtocol::getPropertiesCabinetType(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/properties/cabinetType");
    if(getReply.isEmpty())
    {
        return PropertiesCabinetType::CC162;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    if(jsonObject.contains("name")){
      QString connectState = jsonObject.value("name").toString();
      if(connectState == "CCBOX"){
          return PropertiesCabinetType::CCBOX;
      }else if(connectState == "CC162"){
          return PropertiesCabinetType::CC162;
      }else if(connectState == "CC262"){
          return PropertiesCabinetType::CC262;
      }else if(connectState == "MG400"){
          return PropertiesCabinetType::MG400;
      }else if(connectState == "M1Pro"){
          return PropertiesCabinetType::M1Pro;
      }else if(connectState == "MG6"){
          return PropertiesCabinetType::MG6;
      }
    }
    return PropertiesCabinetType::CC162;
}

QString DobotProtocol::getPropertiesCabinetTypeStr(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/properties/cabinetType");
    if(getReply.isEmpty())
    {
        return "CC162";
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    if(jsonObject.contains("name")){
      QString cabinetType = jsonObject.value("name").toString();
      return cabinetType;
    }
    return "CC162";
}

DobotType::ProtocolExchangeResult DobotProtocol::getProtocolExchange(QString ip)
{
    DobotType::ProtocolExchangeResult protocolExchange;
    protocolExchange.isAlarm = false;
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/protocol/exchange");
    if(getReply.isEmpty())
    {
        return protocolExchange;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();

    protocolExchange.prjState = jsonObject["prjState"].toString();

    QJsonArray array = jsonObject["alarms"].toArray();
    for (int i = 0; i < array.size(); i++)
    {
        QJsonArray arrayChild = array.at(i).toArray();
        for (int j = 0; j < arrayChild.size(); j++)
        {
            if(arrayChild.at(j).isNull()){
                continue;
            }
            if(arrayChild.at(j).toInt() != 0){
                protocolExchange.isAlarm = true;
            }
        }
    }

    return protocolExchange;
}

QString DobotProtocol::postFaultCheck(QString ip, QString postData)
{
    QJsonObject jsonFunc;
    jsonFunc.insert("operationType", postData);
    postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<" postFaultCheck "<<postData;
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/fault/check",postData);
    qDebug()<<"  postReply "<<postReply;
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();
}

QStringList DobotProtocol::getXmlVersion(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/update/xmlVersion");
    if(getReply.isEmpty())
    {
        return QStringList();
    }
    QStringList strList;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    if(jsonObject.contains("version")){
      QJsonArray versionArray = jsonObject.value("version").toArray();
      for(QJsonValue jsonValue:versionArray)
      {
          strList.append(jsonValue.toString());
      }
      return strList;
    }
    return strList;
}

QString DobotProtocol::postFaultRepair(QString ip, QString postData,QString macAddress,QString date,QString time)
{
    QJsonObject jsonFunc;
//    macAddress.insert(0,"000");
//    macAddress.insert(2,':');
//    macAddress.insert(5,':');
//    macAddress.insert(8,':');
//    macAddress.insert(11,':');
//    macAddress.insert(14,':');
//    qDebug()<<"macAddress ---"<<macAddress;

//    return "success";

    if(!macAddress.isEmpty()){
        macAddress.insert(0,"000EC69");
        macAddress.insert(2,':');
        macAddress.insert(5,':');
        macAddress.insert(8,':');
        macAddress.insert(11,':');
        macAddress.insert(14,':');
        qDebug()<<"macAddress ---"<<macAddress;
        jsonFunc.insert("macAddress", macAddress);
    }

    if(!date.isEmpty())
    {
        jsonFunc.insert("date", date);
    }

    if(!time.isEmpty())
    {
        jsonFunc.insert("time", time);
    }
    jsonFunc.insert("operationType", postData);
    postData = QString(QJsonDocument(jsonFunc).toJson());
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/fault/repair",postData);
    qDebug()<<"postFaultRepair postData "<<postData <<"  result-->"<<postReply;
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();
}



void DobotProtocol::setCurrentIp(QString ip)
{

}

QString DobotProtocol::getCurrentIp()
{
    return "";
}

bool DobotProtocol::getIsOnline()
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://www.baidu.com/",500);
    if(!getReply.isEmpty())
    {
        return true;
    }
    return false;
}

QString DobotProtocol::postUpdateDiskCheck(QString ip)
{
    QJsonObject jsonFunc;
    jsonFunc.insert("operationType", "checkP5");
    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/diskCheck",postData);
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();
}


QString DobotProtocol::postUpdateCRSingleFwAndXml(QString ip,DobotType::StructCRSingleFwAndXml singleCRFwAndXml)
{
    QJsonObject jsonFunc;
    if(singleCRFwAndXml.seqId != -1 )
    {
        jsonFunc.insert("seqId", singleCRFwAndXml.seqId);
    }
    else
    {
        jsonFunc.insert("operationType", singleCRFwAndXml.operationType);
        jsonFunc.insert("cabType", singleCRFwAndXml.cabType);
        jsonFunc.insert("cabVersion",singleCRFwAndXml.cabVersion);
        jsonFunc.insert("updateType",singleCRFwAndXml.updateType);
        jsonFunc.insert("updateFile",singleCRFwAndXml.updateFile);
        jsonFunc.insert("updateFile2",singleCRFwAndXml.updateFile2);
        jsonFunc.insert("slaveId", singleCRFwAndXml.slaveId);
    }

    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"http://"+ip+":22002"+"/update/CR/singleFwAndXml";
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/CR/singleFwAndXml",postData);
    qDebug().noquote()<<"/update/CR/singleFwAndXml  "<<postData.toUtf8();
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();
}

QString DobotProtocol::postUpdateM1ProSingleFwAndXml(QString ip, DobotType::StructM1ProSingleFwAndXml singleM1ProFwAndXml)
{
    QJsonObject jsonFunc;
    jsonFunc.insert("operationType", singleM1ProFwAndXml.operationType);
    jsonFunc.insert("updateType",singleM1ProFwAndXml.updateType);
    jsonFunc.insert("updateFile",singleM1ProFwAndXml.updateFile);
    jsonFunc.insert("slaveId", singleM1ProFwAndXml.slaveId);


    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"http://"+ip+":22002"+"/update/M1Pro/singleFwAndXml";
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/M1Pro/singleFwAndXml",postData);
    qDebug()<<"/update/M1Pro/singleFwAndXml  "<<postData.toUtf8();
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();

}

QString DobotProtocol::postUpdateMG400SingleFwAndXml(QString ip, DobotType::StructMG400SingleFwAndXml singleMG400FwAndXml)
{
    QJsonObject jsonFunc;
    jsonFunc.insert("operationType", singleMG400FwAndXml.operationType);
    jsonFunc.insert("updateType",singleMG400FwAndXml.updateType);
    jsonFunc.insert("updateFile",singleMG400FwAndXml.updateFile);
    jsonFunc.insert("slaveId", 1);


    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"http://"+ip+":22002"+"/update/MG400/singleFwAndXml";
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/MG400/singleFwAndXml",postData);
    qDebug()<<"/update/MG400/singleFwAndXml  "<<postData.toUtf8();
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();
}

bool DobotProtocol::postSettingsModifyServoParams(QString ip, DobotType::StructSettingsServoParams settingsServoParams)
{
    QJsonObject jsonFunc;
    jsonFunc.insert("src", settingsServoParams.src);

    QJsonArray jsonArray;
    for(DobotType::StructServoParam servoParam: settingsServoParams.servoParams){
        QJsonObject jsonServoParam;
        jsonServoParam.insert("servoNum",servoParam.servoNum);
        jsonServoParam.insert("key",servoParam.key);
        jsonServoParam.insert("value",servoParam.value);
        jsonArray.append(jsonServoParam);
    }

    jsonFunc.insert("body",jsonArray);



    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"http://"+ip+":22000"+"/settings/modifyServoParams";

    qDebug().noquote()<<"/settings/modifyServoParams  "<<postData.toUtf8();

    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22000"+"/settings/modifyServoParams",postData);
    if(postReply.isEmpty())
    {
        return false;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("status").toBool();
}

DobotType::StructSettingsServoParamsResult DobotProtocol::getSettingsModifyServoParams(QString ip)
{
    DobotType::StructSettingsServoParamsResult result;

    QByteArray postReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/settings/modifyServoParams");
    if(postReply.isEmpty())
    {
        return result;
    }
    qDebug().noquote()<<" getSettingsModifyServoParams reply "<<postReply;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray ServoParamResultJsonArray = jsonObject.value("body").toArray();
    for(int i = 0; i < ServoParamResultJsonArray.size();i++){
        DobotType::StructServoParam servoParam;
        servoParam.key = ServoParamResultJsonArray[i].toObject().value("key").toString();
        servoParam.servoNum = ServoParamResultJsonArray[i].toObject().value("servoNum").toString();
        servoParam.status = ServoParamResultJsonArray[i].toObject().value("status").toBool();
        result.servoParams.append(servoParam);
    }
    result.status  = jsonObject.value("status").toBool();

    /*** data **/
    return result;
}

bool DobotProtocol::postSettingsReadServoParams(QString ip, DobotType::StructSettingsServoParams settingsServoParams)
{

    QJsonObject jsonFunc;

    QJsonArray jsonArray;
    for(DobotType::StructServoParam servoParam: settingsServoParams.servoParams){
        QJsonObject jsonServoParam;
        jsonServoParam.insert("servoNum",servoParam.servoNum);
        jsonServoParam.insert("key",servoParam.key);
        jsonArray.append(jsonServoParam);
    }

    jsonFunc.insert("body",jsonArray);

    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"http://"+ip+":22000"+"/settings/readServoParams";
    qDebug().noquote()<<"postSettingsReadServoParams post -->  "<<postData;
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22000"+"/settings/readServoParams",postData);
    if(postReply.isEmpty())
    {
        return false;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();



    return jsonObject.value("status").toBool();

}

DobotType::StructSettingsServoParamsResult DobotProtocol::getSettingsReadServoParams(QString ip)
{
    DobotType::StructSettingsServoParamsResult result;

    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/settings/readServoParams");
    if(getReply.isEmpty())
    {
        return result;
    }
    qDebug().noquote()<<"getSettingsReadServoParams getReply"<<getReply;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray ServoParamResultJsonArray = jsonObject.value("body").toArray();
    for(int i = 0; i < ServoParamResultJsonArray.size();i++){
        DobotType::StructServoParam servoParam;
        servoParam.key = ServoParamResultJsonArray[i].toObject().value("key").toString();
        servoParam.value = ServoParamResultJsonArray[i].toObject().value("value").toDouble();
        servoParam.servoNum = ServoParamResultJsonArray[i].toObject().value("servoNum").toString();
        servoParam.status = ServoParamResultJsonArray[i].toObject().value("status").toBool();
        result.servoParams.append(servoParam);
    }
    result.status = jsonObject.value("status").toBool();

    /*** data **/
    return result;
}
//http://192.168.5.1:22002/update/checkKernelVersion
QString DobotProtocol::postUpdateCheckKernelVersion(QString ip,QString cabVersion)
{



    QJsonObject jsonFunc;
    jsonFunc.insert("cabVersion",cabVersion);

    QString postData = QString(QJsonDocument(jsonFunc).toJson());

    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/checkKernelVersion",postData);
    qDebug()<<"postReply  postUpdateCheckKernelVersion " <<postReply;




    if(postReply.isEmpty())
    {
        return "";
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();



    return jsonObject.value("result").toString();



}
//http://192.168.5.1:22002/update/checkPartitionOccupancy
QString DobotProtocol::getUpdateCheckPartitionOccupancy(QString ip)
{

    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/update/checkPartitionOccupancy");
    qDebug()<<"getReply  /update/checkPartitionOccupancy " <<getReply;

    if(getReply.isEmpty())
    {
        return "";
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();



    return jsonObject.value("result").toString();
}

QString DobotProtocol::postUpdateCRFwAndXml(QString ip,DobotType::UpdateFirmware updateFirmware)
{
    QJsonObject jsonFunc;
    jsonFunc.insert("operationType", updateFirmware.operation);
    jsonFunc.insert("cabType", updateFirmware.cabType);
    jsonFunc.insert("cabVersion", updateFirmware.cabVersion);

    QJsonObject jsonFuncCurrentVersion;
    jsonFuncCurrentVersion.insert("unio",updateFirmware.currentVersion.unio);
    jsonFuncCurrentVersion.insert("safeio",updateFirmware.currentVersion.safeio);
    jsonFuncCurrentVersion.insert("servo1",updateFirmware.currentVersion.servo1);
    jsonFuncCurrentVersion.insert("servo2",updateFirmware.currentVersion.servo2);
    jsonFuncCurrentVersion.insert("servo3",updateFirmware.currentVersion.servo3);
    jsonFuncCurrentVersion.insert("servo4",updateFirmware.currentVersion.servo4);
    jsonFuncCurrentVersion.insert("servo5",updateFirmware.currentVersion.servo5);
    jsonFuncCurrentVersion.insert("servo6",updateFirmware.currentVersion.servo6);
    jsonFuncCurrentVersion.insert("terminal",updateFirmware.currentVersion.terminal);
    jsonFuncCurrentVersion.insert("feedback",updateFirmware.currentVersion.feedback);
    jsonFuncCurrentVersion.insert("control",updateFirmware.currentVersion.control);

    jsonFunc.insert("currentVersion",QJsonValue(jsonFuncCurrentVersion));

    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"http://"+ip+":22002"+"/update/CR/fwAndXml";
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/CR/fwAndXml",postData);
    qDebug().noquote()<<"/update/CR/fwAndXml "<<postData.toUtf8();
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();
}

int DobotProtocol::getUpdateSingleStatus(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/update/singleStatus");
    if(getReply.isEmpty())
    {
        qDebug()<<"getUpdateSingleStatus get no data 如果 CCBOX 会断电 所以默认成功 ";
        return 2;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    int status = jsonObject.value("result").toInt();
    return status;
}

DobotType::UpdateStatus DobotProtocol::getUpdateCRStatus(QString ip)
{
    DobotType::UpdateStatus updateStatus;
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/update/CR/status");
    if(getReply.isEmpty())
    {
        updateStatus.bIsOutage = true;
        return updateStatus;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray FWUpdateResultJsonArray = jsonObject.value("fwUpdateResult").toArray();
    for(int i = 0; i < FWUpdateResultJsonArray.size();i++){
        updateStatus.FWUpdateResult.append(FWUpdateResultJsonArray[i].toInt());
    }
    QJsonArray XMLUpdateResultJsonArray = jsonObject.value("xmlUpdateResult").toArray();
    for(int i = 0; i < XMLUpdateResultJsonArray.size();i++){
        updateStatus.XMLUpdateResult.append(XMLUpdateResultJsonArray[i].toInt());
    }
    return updateStatus;
}

QString DobotProtocol::getProtocolVersion(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/protocol/version");
    qDebug()<<"DobotProtocol::getProtocolVersion-->  "<<getReply;
    if(getReply.isEmpty())
    {
        return "";
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    return jsonObject.value("toolVersion").toString();
}




void DobotProtocol::sleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

