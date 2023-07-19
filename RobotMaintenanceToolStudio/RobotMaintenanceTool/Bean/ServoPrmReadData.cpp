#include "ServoPrmReadData.h"

#include <QJsonArray>
#include <QJsonDocument>

QJsonObject ServoPrmReadDataBean::toJsonObject() const
{
    QJsonObject obj;
    obj["servoNum"] = servoNum;
    obj["key"] = key;
    return obj;
}

QString ServoPrmReadDataBean::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmReadDataBean::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmReadDataBean::fromJson(const QJsonObject &obj)
{
    servoNum = obj.value("servoNum").toString();
    key = obj.value("key").toString();
}

QJsonObject ServoPrmReadData::toJsonObject() const
{
    QJsonObject obj;
    QJsonArray arr;
    for(auto itr=body.begin(); itr!=body.end(); ++itr)
    {
        arr.append(itr->toJsonObject());
    }
    obj["body"] = arr;
    return obj;
}

QString ServoPrmReadData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmReadData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmReadData::fromJson(const QJsonObject &obj)
{
    body.clear();
    QJsonArray arr = obj.value("body").toArray();
    for (auto itr=arr.begin(); itr!=arr.end(); ++itr)
    {
        ServoPrmReadDataBean bean;
        bean.fromJson(itr->toObject());
        body.append(bean);
    }
}
