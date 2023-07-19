#ifndef SERVOPRMMODIFYDATA_H
#define SERVOPRMMODIFYDATA_H

#include <QJsonObject>
#include <QList>

struct ServoPrmModifyDataBean
{
    QString servoNum;
    QString key;
    double value;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

struct ServoPrmModifyData
{
    QString src;//固定值为httpClient
    QList<ServoPrmModifyDataBean> body;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

#endif // SERVOPRMMODIFYDATA_H
