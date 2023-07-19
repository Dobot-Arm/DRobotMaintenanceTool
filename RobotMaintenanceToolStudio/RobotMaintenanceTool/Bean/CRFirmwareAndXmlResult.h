#ifndef CRFIRMWAREANDXMLRESULT_H
#define CRFIRMWAREANDXMLRESULT_H

#include <QJsonObject>
#include <QJsonArray>
#include "UpdateState.h"

struct FwUpdateResult
{
    UpdateState safeio;
    UpdateState feedback;
    UpdateState unio;
    UpdateState servo1;
    UpdateState servo2;
    UpdateState servo3;
    UpdateState servo4;
    UpdateState servo5;
    UpdateState servo6;
    UpdateState terminal;

    QJsonArray toJsonArray() const;
    QString toJsonString() const;
    void fromJson(const QString& strJsonArray);
    void fromJson(const QJsonArray& arr);
};
struct XmlUpdateResult
{
    UpdateState safeio;
    UpdateState unio;
    UpdateState servo1;
    UpdateState servo2;
    UpdateState servo3;
    UpdateState servo4;
    UpdateState servo5;
    UpdateState servo6;
    UpdateState terminal;

    QJsonArray toJsonArray() const;
    QString toJsonString() const;
    void fromJson(const QString& strJsonArray);
    void fromJson(const QJsonArray& arr);
};

struct CRFirmwareAndXmlResult
{
    FwUpdateResult fwUpdateResult;
    XmlUpdateResult xmlUpdateResult;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

#endif // CRFIRMWAREANDXMLRESULT_H
