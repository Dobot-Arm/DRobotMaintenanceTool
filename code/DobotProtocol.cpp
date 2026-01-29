#include "DobotProtocol.h"
#include <QTimer>
DobotProtocol::DobotProtocol()
{
    m_httpManager = new DHttpCurlManager();
    m_getCRStatusHttpManager = new DHttpCurlManager();//new DHttpManager();
    m_getConnectionStateHttpManager = new DHttpCurlManager();//
}
ConnectState DobotProtocol::getConnectionState(QString ip)
{
    QByteArray getReply = m_getConnectionStateHttpManager->httpManagerGet("http://"+ip+":22000"+"/connection/state",2000);
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
    qDebug()<<"/settings/version  getReply  " <<postReply;
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
    qDebug()<<"/settings/version  postReply  " <<postReply;
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
    controllerType.nameExt = jsonObject.value("controllerTypeExt").toString();
    controllerType.version = jsonObject.value("version").toInt();
    controllerType.originName = controllerType.name;
    if (!controllerType.nameExt.isEmpty()
        && controllerType.nameExt.startsWith("CR")
        && controllerType.nameExt.endsWith("A-IS"))
    {
        qDebug()<<"controllerTypeExt不是空,且是CRxxxA-IS系列的，所以name要被替换为controllerTypeExt";
        controllerType.name = controllerType.nameExt;
    }
    return controllerType;
}

DobotType::CabinetType DobotProtocol::getPropertiesCabinetType(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/properties/cabinetType");
    qDebug()<<"/properties/cabinetType getReply  " <<getReply;
    DobotType::CabinetType cabinetType;
    cabinetType.name = "CC162";
    if(getReply.isEmpty())
    {
        return cabinetType;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    cabinetType.name = jsonObject.value("name").toString("CC162");
    cabinetType.power = jsonObject.value("power").toString();
    return cabinetType;
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
    protocolExchange.controlMode = jsonObject["controlMode"].toString();
    protocolExchange.dragMode = jsonObject.value("dragMode").toString();
    protocolExchange.dragPlayback = jsonObject.value("dragPlayback").toBool();
    protocolExchange.powerState = jsonObject.value("powerState").toString();
    protocolExchange.coordinate = jsonObject.value("coordinate").toString();
    protocolExchange.isSafeSuspend = jsonObject.value("isSafeSuspend").toInt();

    QJsonArray array = jsonObject["alarms"].toArray();
    for (int i = 0; i < array.size(); i++)
    {
        QList<int> subAlarms;
        QJsonArray arrayChild = array.at(i).toArray();
        for (int j = 0; j < arrayChild.size(); j++)
        {
            if(arrayChild.at(j).isNull()){
                continue;
            }
            subAlarms.append(arrayChild.at(j).toInt());
            if(arrayChild.at(j).toInt() != 0){
                protocolExchange.isAlarm = true;
            }
        }
        protocolExchange.alarms.append(subAlarms);
    }
    return protocolExchange;
}

QList<QPair<QString, QString> > DobotProtocol::getNetworkCardMac(QString ip)
{
    QList<QPair<QString, QString>> ret;
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/control/networkCardMAC");
    qDebug()<<"getReply /control/networkCardMAC: "<<getReply;
    if(getReply.isEmpty())
    {
        return ret;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonArray arr = jsonDoc.array();
    for (int i=0; i<arr.size(); ++i)
    {
        auto obj = arr.at(i).toObject();
        QString strName = obj["Name"].toString();
        QString strMac = obj["MAC"].toString();
        if (!strName.isEmpty() || !strMac.isEmpty())
        {
            ret.append({strName,strMac});
        }
    }
    return ret;
}

DobotType::StructFaultCheck DobotProtocol::postFaultCheck(QString ip, QString postData)
{
    DobotType::StructFaultCheck faultCheck;
    QJsonObject jsonFunc;
    jsonFunc.insert("operationType", postData);
    if(postData == "files"||postData == "dns")
    {
        jsonFunc.insert("control", CommonData::getCurrentSettingsVersion().control);
    }
    postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"/fault/check postFaultCheck "<<postData;
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/fault/check",postData);
    qDebug()<<"/fault/check  postReply "<<postReply;
    if(postReply.isEmpty())
    {
        return faultCheck;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    faultCheck.result = jsonObject.value("result").toString();
    QStringList strListMissFiles;
    QJsonArray jsonArray = jsonObject.value("missingFiles").toArray();
    for(int i = 0; i < jsonArray.count();i++)
    {
        strListMissFiles.append(jsonArray[i].toString());
    }
    faultCheck.missFiles = strListMissFiles;
    /*** data **/
    return faultCheck;
}

FaultCheckResult DobotProtocol::postFaultCheck(const QString &strIP, const FaultCheckData &data, int iTimeoutMilliseconds)
{
    FaultCheckResult result;
    QString strUrl = QString("http://%1:22002/fault/check").arg(strIP);
    QString strPostData=data.toJsonString();
    QByteArray postReply = m_httpManager->httpManagerPost(strUrl, strPostData,iTimeoutMilliseconds);
    qDebug()<<"postReply  postFaultCheck " <<postReply;
    if (!postReply.isEmpty())
    {
        result.fromJson(QString(postReply));
    }
    return result;
}

QStringList DobotProtocol::getXmlVersion(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/update/xmlVersion");
    qDebug()<<"getReply  /update/xmlVersion " <<getReply;
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

QString DobotProtocol::postFaultRepair(QString ip, QString postData,QString macAddress,QString date,QString time,QString control)
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

    if(!control.isEmpty())
    {
        jsonFunc.insert("control",control);
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
    qDebug()<<"postReply  /update/diskCheck " <<postReply;
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
        while (singleCRFwAndXml.allUpdateFile.size()<2){
            singleCRFwAndXml.allUpdateFile.append(QString(""));
        }
        for (int i=0; i<singleCRFwAndXml.allUpdateFile.size(); ++i)
        {
            if (i==0){
                jsonFunc.insert("updateFile",singleCRFwAndXml.allUpdateFile.at(i));
            }else{
                jsonFunc.insert(QString("updateFile%1").arg(i+1),singleCRFwAndXml.allUpdateFile.at(i));
            }
        }
        jsonFunc.insert("slaveId", singleCRFwAndXml.slaveId);
    }

    if(singleCRFwAndXml.slaveId == 0)
    {
        jsonFunc.insert("updateControlVersion", singleCRFwAndXml.updateControlVersion);
    }

    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"/update/CR/singleFwAndXml    "<<postData.toUtf8();
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/CR/singleFwAndXml",postData,15000);
    qDebug().noquote()<<"/update/CR/singleFwAndXml  postReply   "<<postReply;
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
    qDebug()<<"/update/M1Pro/singleFwAndXml  "<<postData.toUtf8();
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/M1Pro/singleFwAndXml",postData);
    qDebug()<<"/update/M1Pro/singleFwAndXml  postReply "<<postReply;
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
    qDebug()<<"/update/MG400/singleFwAndXml   "<<postData.toUtf8();;
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/MG400/singleFwAndXml",postData);
    qDebug()<<"/update/MG400/singleFwAndXml  postReply  "<<postReply;
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
    qDebug().noquote()<<"/settings/modifyServoParams  "<<postData.toUtf8();
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22000"+"/settings/modifyServoParams",postData);
    qDebug()<<"/settings/modifyServoParams  postReply  "<<postReply;
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
    QByteArray getReply;
    for (int i=0;i<5;++i)
    {
         getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/settings/modifyServoParams");
        qDebug().noquote()<<"/settings/modifyServoParams getReply"<<getReply;
        if(getReply.isEmpty())
        {
            sleep(5000);
        }
        else
        {
            break;
        }
    }
    if(getReply.isEmpty())
    {
        return result;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
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
    qDebug()<<"/settings/readServoParams  postReply   "<<postReply;
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

    QByteArray getReply;
    for (int i=0;i<5;++i)
    {
         getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/settings/readServoParams");
        qDebug().noquote()<<"/settings/readServoParams getReply"<<getReply;
        if(getReply.isEmpty())
        {
            sleep(5000);
        }
        else
        {
            break;
        }
    }
    if(getReply.isEmpty())
    {
        return result;
    }
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

QHash<QString, DobotType::StructServoValue> DobotProtocol::getReadServoValueDianjixinghao(QString ip)
{
    QHash<QString, //关节序号J1~J6
            DobotType::StructServoValue
         > lstResult;
    const QString strUrl = QString("http://%1:22000/settings/readServoParams").arg(ip);
    const QString strChangjia = "addr200101"; //H1.00   厂家
    const QString strDianji = "addr200102"; //H1.01     电机型号
    QJsonArray jsonArray;
    for (int i=1;i<=6; ++i){
        QJsonObject obj1;
        obj1["key"] = strChangjia;
        obj1["servoNum"] = QString("J%1").arg(i);
        jsonArray.append(obj1);
        QJsonObject obj2;
        obj2["key"] = strDianji;
        obj2["servoNum"] = QString("J%1").arg(i);
        jsonArray.append(obj2);
    }
    QJsonObject jsObject;
    jsObject["body"] = jsonArray;
    QString postData = QString(QJsonDocument(jsObject).toJson());
    qDebug().noquote()<<"getReadServoValueDianjixinghao send -->  "<<postData;
    QByteArray postReply = m_httpManager->httpManagerPost(strUrl,postData);
    qDebug()<<"getReadServoValueDianjixinghao  recv   "<<postReply;
    if(postReply.isEmpty()) return lstResult;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    jsObject = jsonDoc.object();
    if (!jsObject.value("status").toBool()) return lstResult;

    sleep(6000);

    QByteArray getReply;
    for (int i=0;i<5;++i)
    {
         getReply = m_httpManager->httpManagerGet(strUrl);
        qDebug().noquote()<<"getReadServoValueDianjixinghao getReply"<<getReply;
        if(getReply.isEmpty()){
            sleep(5000);
        }else{
            break;
        }
    }
    if(getReply.isEmpty()) return lstResult;

    jsonDoc = QJsonDocument::fromJson(getReply);
    jsObject = jsonDoc.object();
    if (!jsObject.value("status").toBool()) return lstResult;

    jsonArray = jsObject.value("body").toArray();
    for(int i = 0; i < jsonArray.size();i++){
        auto jo = jsonArray[i].toObject();
        bool status = jo.value("status").toBool();
        int v = jo.value("value").toInt();
        QString joint = jo.value("servoNum").toString();
        QString addr = jo.value("key").toString();
        if (strChangjia == addr){
            for (int jt=1; jt<=6; ++jt){
                if (joint != QString::asprintf("J%d",jt)){
                    continue;
                }
                auto itr = lstResult.find(joint);
                if (itr==lstResult.end()){
                    DobotType::StructServoValue data;
                    data.jointId = jt;
                    data.changjia = v;
                    lstResult.insert(joint, data);
                }else{
                    itr->changjia = v;
                }
            }
        }else if (strDianji == addr){
            for (int jt=1; jt<=6; ++jt){
                if (joint != QString::asprintf("J%d",jt)){
                    continue;
                }
                auto itr = lstResult.find(joint);
                if (itr==lstResult.end()){
                    DobotType::StructServoValue data;
                    data.jointId = jt;
                    data.dianji = v;
                    lstResult.insert(joint, data);
                }else{
                    itr->dianji = v;
                }
            }
        }
    }
    return lstResult;
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

    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/update/checkPartitionOccupancy",10000);
    qDebug()<<"getReply  /update/checkPartitionOccupancy " <<getReply;

    if(getReply.isEmpty())
    {
        return "";
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    return jsonObject.value("result").toString();
}

bool DobotProtocol::postSettingsControlMode(QString ip, QString controlMode)
{
    QJsonObject jsonFunc;
    jsonFunc.insert("controlMode",controlMode);

    QString postData = QString(QJsonDocument(jsonFunc).toJson());

    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22000"+"/settings/controlMode",postData);
    qDebug()<<"postReply  postSettingsControlMode " <<postReply;

    if(postReply.isEmpty())
    {
        return false;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();
    return jsonObject.value("status").toBool();
}

QString DobotProtocol::getSettingsControlMode(QString ip)
{
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/settings/controlMode");
    qDebug()<<"getReply  /settings/controlMode " <<getReply;
    if(getReply.isEmpty())
    {
        return "";
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    if(jsonObject.contains("controlMode")){
      return jsonObject.value("controlMode").toString();
    }
    return "";
}

QString DobotProtocol::postUpdateKernel(QString ip, QString cabVersion)
{

    QJsonObject jsonFunc;
    jsonFunc.insert("cabVersion",cabVersion);

    QString postData = QString(QJsonDocument(jsonFunc).toJson());

    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/kernel",postData,10000);
    qDebug()<<"postReply  postUpdateKernel " <<postReply;

    if(postReply.isEmpty())
    {
        return "";
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    return jsonObject.value("result").toString();
}

//用于当V4主控板升级后，备份从控制器传输到本地后，发送这个通知请求，通知嵌入式继续升级下一个板卡。
QString DobotProtocol::postUpdateContinue(QString ip)
{
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/continue","");
    qDebug()<<"postReply  postUpdateContinue " <<postReply;

    if(postReply.isEmpty())
    {
        return "";
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
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

    jsonFunc.insert("updateControlVersion",updateFirmware.updateControlVersion);


    QString postData = QString(QJsonDocument(jsonFunc).toJson());
    qDebug()<<"/update/CR/fwAndXml   "<<postData.toUtf8();
    QByteArray postReply = m_httpManager->httpManagerPost("http://"+ip+":22002"+"/update/CR/fwAndXml",postData);
    qDebug().noquote()<<"/update/CR/fwAndXml postReply  "<<postReply;
    if(postReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(postReply);
    QJsonObject jsonObject = jsonDoc.object();

    /*** data **/
    return jsonObject.value("result").toString();
}

DobotType::StructGetCRSingleStatus DobotProtocol::getUpdateSingleStatus(QString ip)
{
    DobotType::StructGetCRSingleStatus singleStatus;
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/update/singleStatus");
    qDebug().noquote()<<"/update/singleStatus getReply  "<<getReply;
    if(getReply.isEmpty())
    {
        qDebug()<<"getUpdateSingleStatus get no data 如果 CCBOX MG6UniIO CC26X UniIO 会断电 所以默认成功 ";
        singleStatus.result = 2;
        return singleStatus;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    int status = jsonObject.value("result").toInt();

    singleStatus.result = status;
    singleStatus.errorCode = jsonObject.value("errorCode").toInt();
    return singleStatus;
}

DobotType::UpdateStatus DobotProtocol::getUpdateCRStatus(QString ip)
{
    m_id++;
    DobotType::UpdateStatus updateStatus;
    QByteArray getReply = m_getCRStatusHttpManager->httpManagerGet("http://"+ip+":22002"+"/update/CR/status",30000);
    qDebug().noquote()<<"/update/CR/status getReply  "<<getReply;
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

    int errorCode = jsonObject.value("errorCode").toInt();
    updateStatus.errorCode = errorCode;

    updateStatus.backupInfo.path = jsonObject.value("backupInfo").toObject().value("path").toString();
    updateStatus.backupInfo.status = jsonObject.value("backupInfo").toObject().value("status").toBool();


    return updateStatus;
}

DobotType::StructProtocolVersion DobotProtocol::getProtocolVersion(QString ip)
{
    DobotType::StructProtocolVersion structProtocolVersion;
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22002"+"/protocol/version");
    qDebug()<<"DobotProtocol::getProtocolVersion-->  "<<getReply;
    if(getReply.isEmpty())
    {
        return structProtocolVersion;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    structProtocolVersion.toolVersion = jsonObject.value("toolVersion").toString();

    structProtocolVersion.pathInfo.slave = jsonObject.value("pathInfo").toObject().value("slave").toString();
    structProtocolVersion.pathInfo.control = jsonObject.value("pathInfo").toObject().value("control").toString();
    structProtocolVersion.pathInfo.kernel = jsonObject.value("pathInfo").toObject().value("kernel").toString();
    structProtocolVersion.pathInfo.v4restoreBackup = jsonObject.value("pathInfo").toObject().value("v4restoreBackup").toString();
    return structProtocolVersion;
}


DobotType::SettingsProductInfoHardwareInfo DobotProtocol::getSettingsProductInfoHardwareInfo(QString ip)
{
    DobotType::SettingsProductInfoHardwareInfo productInfoHardwareInfo;
    QByteArray getReply = m_httpManager->httpManagerGet("http://"+ip+":22000"+"/settings/productInfo/hardwareInfo");
    qDebug()<<"DobotProtocol::getSettingsProductInfoHardwareInfo-->  "<<getReply;
    if(getReply.isEmpty())
    {
        return productInfoHardwareInfo;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    productInfoHardwareInfo.ControllCabinetSNCode = jsonObject.value("ControllCabinetSNCode").toString();
    productInfoHardwareInfo.RobotArmSNCode = jsonObject.value("RobotArmSNCode").toString();
    productInfoHardwareInfo.RealArmSNCode = jsonObject.value("RealArmSNCode").toString();

    return productInfoHardwareInfo;
}


QString DobotProtocol::getSettingsProductInfoControllerSn(const QString &strIP, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/productInfo/controllerSn").arg(strIP);
    QByteArray getReply = m_httpManager->httpManagerGet(strUrl,iTimeoutMilliseconds);
    qDebug()<<"DobotProtocol::getSettingsProductInfoControllerSn-->  "<<getReply;
    if (getReply.isEmpty())
    {
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(getReply);
    QJsonObject jsonObject = jsonDoc.object();
    return jsonObject.value("SNcode").toString();
}

void DobotProtocol::sleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

bool DobotProtocol::postSettingsProductInfoControllerSn(const QString &strIP,const ControllerSnData& controlSNData, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/productInfo/controllerSn").arg(strIP);
    QByteArray postReply = m_httpManager->httpManagerPost(strUrl,controlSNData.toJsonString(),iTimeoutMilliseconds);
    qDebug()<<"postReply  postSettingsProductInfoControllerSn " <<postReply;
    if(postReply.isEmpty())
    {
        return false;
    }
    return QJsonDocument::fromJson(postReply).object().value("status").toBool();
}
bool DobotProtocol::postV3SettingsProductInfoControllerSn(const QString &strIP, const V3ControllerSnData &controlSNData, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/productInfo/controllerSn").arg(strIP);
    QByteArray postReply = m_httpManager->httpManagerPost(strUrl, controlSNData.toJsonString(),iTimeoutMilliseconds);
    qDebug()<<"postReply  postV3SettingsProductInfoControllerSn " <<postReply;
    if (postReply.isEmpty())
    {
        return false;
    }
    return QJsonDocument::fromJson(postReply).object().value("status").toBool();
}

bool DobotProtocol::postSettingsProductInfoRobotArmSn(const QString &strIP,const RobotArmSnData& robotArmSNData, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/productInfo/robotArmSn").arg(strIP);
    QByteArray postReply = m_httpManager->httpManagerPost(strUrl, robotArmSNData.toJsonString(),iTimeoutMilliseconds);
    qDebug()<<"postReply  postSettingsProductInfoRobotArmSn " <<postReply;
    if (postReply.isEmpty())
    {
        return false;
    }
    return QJsonDocument::fromJson(postReply).object().value("status").toBool();
}

bool DobotProtocol::postInterfaceClearAlarms(const QString &strIP, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/interface/clearAlarms").arg(strIP);
    QByteArray postReply = m_httpManager->httpManagerPost(strUrl, QString(),iTimeoutMilliseconds);
    qDebug()<<"postReply  postInterfaceClearAlarms " <<postReply;
    if (postReply.isEmpty())
    {
        return false;
    }
    return QJsonDocument::fromJson(postReply).object().value("status").toBool();
}
