#include "FaultRepairData.h"
#include <QJsonDocument>

QJsonObject FaultRepairData::toJsonObject() const
{
    QJsonObject obj;
    obj["operationType"] = operationType;
    obj["macAddress"] = macAddress;
    obj["date"] = date;
    obj["time"] = time;
    return obj;
}

QString FaultRepairData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void FaultRepairData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void FaultRepairData::fromJson(const QJsonObject &obj)
{
    operationType = obj.value("operationType").toString();
    macAddress = obj.value("macAddress").toString();
    date = obj.value("date").toString();
    time = obj.value("time").toString();
}
