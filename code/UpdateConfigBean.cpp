#include "UpdateConfigBean.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

QString UpdateConfigBean::toJson()
{
    QJsonObject json;
    json["version"] = version;
    json["filemd5"] = filemd5;
    json["url"] = url;
    QJsonDocument doc(json);
    return QString(doc.toJson());
}

QJsonObject UpdateConfigBean::toJsonObject()
{
    QJsonObject json;
    json["version"] = version;
    json["filemd5"] = filemd5;
    json["url"] = url;
    QJsonDocument doc(json);
    return doc.object();
}

void UpdateConfigBean::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    QJsonObject obj = doc.object();
    version = obj.value("version").toInt(0);
    filemd5 = obj.value("filemd5").toString();
    url = obj.value("url").toString();
}

UpdateConfigObject::UpdateConfigObject(QString strJsonFile)
{
    QFile file(strJsonFile);
    if (file.open(QFile::ReadOnly))
    {
        fromJson(file.readAll());
        file.close();
    }
}

QString UpdateConfigObject::toJson()
{
    QJsonObject json;
    json["updatefirmware"] = updatefirmware.toJsonObject();;
    json["startServer"] = startServer.toJsonObject();
    QJsonDocument doc(json);
    return QString(doc.toJson());
}

void UpdateConfigObject::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    QJsonObject obj = doc.object();
    QString firm = QJsonDocument(obj.value("updatefirmware").toObject()).toJson();
    QString server = QJsonDocument(obj.value("startServer").toObject()).toJson();

    updatefirmware.fromJson(firm);
    startServer.fromJson(server);
}

void UpdateConfigObject::fromJsonFile(const QString &strJsonFile)
{
    QFile file(strJsonFile);
    if (file.open(QFile::ReadOnly))
    {
        fromJson(file.readAll());
        file.close();
    }
}
