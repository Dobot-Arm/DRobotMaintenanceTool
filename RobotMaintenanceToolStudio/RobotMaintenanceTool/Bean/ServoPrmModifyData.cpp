#include "ServoPrmModifyData.h"

#include <QJsonArray>
#include <QJsonDocument>

QJsonObject ServoPrmModifyDataBean::toJsonObject() const
{
    QJsonObject obj;
    obj["servoNum"] = servoNum;
    obj["key"] = key;
    obj["value"] = value;
    return obj;
}

QString ServoPrmModifyDataBean::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmModifyDataBean::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmModifyDataBean::fromJson(const QJsonObject &obj)
{
    servoNum = obj.value("servoNum").toString();
    key = obj.value("key").toString();
    value = obj.value("value").toDouble();
}

QJsonObject ServoPrmModifyData::toJsonObject() const
{
    QJsonObject obj;
    obj["src"] = src;

    QJsonArray arr;
    for(auto itr=body.begin(); itr!=body.end(); ++itr)
    {
        arr.append(itr->toJsonObject());
    }
    obj["body"] = arr;
    return obj;
}

QString ServoPrmModifyData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ServoPrmModifyData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ServoPrmModifyData::fromJson(const QJsonObject &obj)
{
    body.clear();
    src = obj.value("src").toString();
    QJsonArray arr = obj.value("body").toArray();
    for (auto itr=arr.begin(); itr!=arr.end(); ++itr)
    {
        ServoPrmModifyDataBean bean;
        bean.fromJson(itr->toObject());
        body.append(bean);
    }
}
