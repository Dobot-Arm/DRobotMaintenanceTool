#include "ServoPrmModifyResult.h"

#include <QJsonArray>
#include <QJsonDocument>

QJsonObject ServoPrmModifyResultBean::toJsonObject() const
{
    QJsonObject obj;
    obj["servoNum"] = servoNum;
    obj["key"] = key;
    obj["status"] = status;
    return obj;
}

QString ServoPrmModifyResultBean::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmModifyResultBean::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmModifyResultBean::fromJson(const QJsonObject &obj)
{
    servoNum = obj.value("servoNum").toString();
    key = obj.value("key").toString();
    status = obj.value("status").toBool();
}

QJsonObject ServoPrmModifyResult::toJsonObject() const
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

QString ServoPrmModifyResult::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmModifyResult::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmModifyResult::fromJson(const QJsonObject &obj)
{
    body.clear();
    status = obj.value("status").toBool();
    QJsonArray arr = obj.value("body").toArray();
    for (auto itr=arr.begin(); itr!=arr.end(); ++itr)
    {
        ServoPrmModifyResultBean bean;
        bean.fromJson(itr->toObject());
        body.append(bean);
    }
}
