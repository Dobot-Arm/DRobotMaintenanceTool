#include "CRSingleFirmwareXmlData.h"
#include <QJsonDocument>

QJsonObject CRSingleFirmwareXmlData::toJsonObject() const
{
    QJsonObject obj;
    obj["operationType"] = operationType;
    obj["cabType"] = cabType;
    obj["cabVersion"] = cabVersion;
    obj["updateType"] = updateType;
    obj["updateFile"] = updateFile;
    obj["updateFile2"] = updateFile2;
    obj["slaveId"] = slaveId;
    return obj;
}

QString CRSingleFirmwareXmlData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void CRSingleFirmwareXmlData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void CRSingleFirmwareXmlData::fromJson(const QJsonObject &obj)
{
    operationType = obj.value("operationType").toString();
    cabType = obj.value("cabType").toString();
    cabVersion = obj.value("cabVersion").toString();
    updateType = obj.value("updateType").toString();
    updateFile = obj.value("updateFile").toString();
    updateFile2 = obj.value("updateFile2").toString();
    slaveId = obj.value("slaveId").toInt();
}
