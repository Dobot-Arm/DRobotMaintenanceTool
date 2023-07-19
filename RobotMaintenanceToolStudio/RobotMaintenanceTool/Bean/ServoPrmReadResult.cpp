#include "ServoPrmReadResult.h"

#include <QJsonArray>
#include <QJsonDocument>

QJsonObject ServoPrmReadResultBean::toJsonObject() const
{
    QJsonObject obj;
    obj["servoNum"] = servoNum;
    obj["key"] = key;
    obj["value"] = value;
    obj["status"] = status;
    return obj;
}

QString ServoPrmReadResultBean::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmReadResultBean::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmReadResultBean::fromJson(const QJsonObject &obj)
{
    servoNum = obj.value("servoNum").toString();
    key = obj.value("key").toString();
    value = obj.value("value").toDouble();
    status = obj.value("status").toBool();
}

QJsonObject ServoPrmReadResult::toJsonObject() const
{
    QJsonObject obj;
    obj["status"] = status;

    QJsonArray arr;
    for(auto itr=body.begin(); itr!=body.end(); ++itr)
    {
        arr.append(itr->toJsonObject());
    }
    obj["body"] = arr;
    return obj;
}

QString ServoPrmReadResult::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmReadResult::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmReadResult::fromJson(const QJsonObject &obj)
{
    body.clear();
    status = obj.value("status").toBool();
    QJsonArray arr = obj.value("body").toArray();
    for (auto itr=arr.begin(); itr!=arr.end(); ++itr)
    {
        ServoPrmReadResultBean bean;
        bean.fromJson(itr->toObject());
        body.append(bean);
    }
}
