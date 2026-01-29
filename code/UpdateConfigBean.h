#ifndef UPDATECONFIGBEAN_H
#define UPDATECONFIGBEAN_H

#include <QString>
#include <QJsonObject>

struct UpdateConfigBean
{
    int version = 0;
    QString filemd5;
    QString url;

    QString toJson();
    QJsonObject toJsonObject();
    void fromJson(const QString& strJson);
};

class UpdateConfigObject
{
public:
    UpdateConfigObject(){};
    UpdateConfigObject(QString strJsonFile);

    UpdateConfigBean updatefirmware;
    UpdateConfigBean startServer;

    QString toJson();
    void fromJson(const QString& strJson);
    void fromJsonFile(const QString& strJsonFile);
};

#endif // UPDATECONFIGBEAN_H
