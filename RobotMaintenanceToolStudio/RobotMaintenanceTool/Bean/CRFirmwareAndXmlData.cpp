#include "CRFirmwareAndXmlData.h"
#include <QJsonDocument>

QJsonObject CRFirmwareAndXmlVersion::toJsonObject() const
{
    QJsonObject obj;
    obj["unio"] = unio;
    obj["safeio"] = safeio;
    obj["servo1"] = servo1;
    obj["servo2"] = servo2;
    obj["servo3"] = servo3;
    obj["servo4"] = servo4;
    obj["servo5"] = servo5;
    obj["servo6"] = servo6;
    obj["terminal"] = terminal;
    obj["feedback"] = feedback;
    return obj;
}

QString CRFirmwareAndXmlVersion::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void CRFirmwareAndXmlVersion::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void CRFirmwareAndXmlVersion::fromJson(const QJsonObject &obj)
{
    unio = obj.value("unio").toString();
    safeio = obj.value("safeio").toString();
    servo1 = obj.value("servo1").toString();
    servo2 = obj.value("servo2").toString();
    servo3 = obj.value("servo3").toString();
    servo4 = obj.value("servo4").toString();
    servo5 = obj.value("servo5").toString();
    servo6 = obj.value("servo6").toString();
    terminal = obj.value("terminal").toString();
    feedback = obj.value("feedback").toString();
}

QJsonObject CRFirmwareAndXmlData::toJsonObject() const
{
    QJsonObject obj;
    obj["operationType"] = operationType;
    obj["currentVersion"] = currentVersion.toJsonObject();
    obj["cabType"] = cabType;
    obj["cabVersion"] = cabVersion;

    return obj;
}

QString CRFirmwareAndXmlData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void CRFirmwareAndXmlData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void CRFirmwareAndXmlData::fromJson(const QJsonObject &obj)
{
    operationType = obj.value("operationType").toString();
    currentVersion.fromJson(obj.value("currentVersion").toObject());
    cabType = obj.value("cabType").toString();
    cabVersion = obj.value("cabVersion").toString();
}
