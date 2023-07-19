#ifndef CRFIRMWAREANDXMLDATA_H
#define CRFIRMWAREANDXMLDATA_H

#include <QJsonObject>

struct CRFirmwareAndXmlVersion
{
    QString unio;
    QString safeio;
    QString servo1;
    QString servo2;
    QString servo3;
    QString servo4;
    QString servo5;
    QString servo6;
    QString terminal;
    QString feedback;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

struct CRFirmwareAndXmlData
{
    QString operationType; //start表示开始，stop表示停止
    QString cabType; //big表示大型控制柜，small表示小型控制柜
    QString cabVersion;//V1表示1代柜，V2表示2代柜
    CRFirmwareAndXmlVersion currentVersion;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

#endif // CRFIRMWAREANDXMLDATA_H
