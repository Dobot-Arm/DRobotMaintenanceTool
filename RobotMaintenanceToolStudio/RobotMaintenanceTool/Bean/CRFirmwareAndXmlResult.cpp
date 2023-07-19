#include "CRFirmwareAndXmlResult.h"
#include <QJsonDocument>

QJsonArray FwUpdateResult::toJsonArray() const
{
    QJsonArray arr;
    arr.append((int)safeio);
    arr.append((int)feedback);
    arr.append((int)unio);
    arr.append((int)servo1);
    arr.append((int)servo2);
    arr.append((int)servo3);
    arr.append((int)servo4);
    arr.append((int)servo5);
    arr.append((int)servo6);
    arr.append((int)terminal);
    return arr;
}

QString FwUpdateResult::toJsonString() const
{
    QJsonDocument doc(toJsonArray());
    return doc.toJson();
}

void FwUpdateResult::fromJson(const QString &strJsonArray)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJsonArray.toUtf8());
    fromJson(doc.array());
}

void FwUpdateResult::fromJson(const QJsonArray &arr)
{
    int idx = 0;
    if (arr.size()>idx) safeio = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) feedback = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) unio = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo1 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo2 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo3 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo4 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo5 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo6 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) terminal = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
}

QJsonArray XmlUpdateResult::toJsonArray() const
{
    QJsonArray arr;
    arr.append((int)safeio);
    arr.append((int)unio);
    arr.append((int)servo1);
    arr.append((int)servo2);
    arr.append((int)servo3);
    arr.append((int)servo4);
    arr.append((int)servo5);
    arr.append((int)servo6);
    arr.append((int)terminal);
    return arr;
}

QString XmlUpdateResult::toJsonString() const
{
    QJsonDocument doc(toJsonArray());
    return doc.toJson();
}

void XmlUpdateResult::fromJson(const QString &strJsonArray)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJsonArray.toUtf8());
    fromJson(doc.array());
}

void XmlUpdateResult::fromJson(const QJsonArray &arr)
{
    int idx = 0;
    if (arr.size()>idx) safeio = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) unio = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo1 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo2 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo3 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo4 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo5 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) servo6 = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
    if (arr.size()>idx) terminal = static_cast<UpdateState>(arr[idx].toInt(static_cast<int>(UpdateState::E_FAILED)));
    idx++;
}

QJsonObject CRFirmwareAndXmlResult::toJsonObject() const
{
    QJsonObject obj;
    obj["fwUpdateResult"] = fwUpdateResult.toJsonArray();
    obj["xmlUpdateResult"] = xmlUpdateResult.toJsonArray();
    return obj;
}

QString CRFirmwareAndXmlResult::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void CRFirmwareAndXmlResult::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void CRFirmwareAndXmlResult::fromJson(const QJsonObject &obj)
{
    fwUpdateResult.fromJson(obj["fwUpdateResult"].toArray());
    xmlUpdateResult.fromJson(obj["xmlUpdateResult"].toArray());
}
