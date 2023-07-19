#ifndef SERVOPRMREADDATA_H
#define SERVOPRMREADDATA_H

#include <QJsonObject>
#include <QList>

struct ServoPrmReadDataBean
{
    QString servoNum;
    QString key;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

struct ServoPrmReadData
{
    QList<ServoPrmReadDataBean> body;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

#endif // SERVOPRMREADDATA_H
