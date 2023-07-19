#ifndef SERVOPRMMODIFYRESULT_H
#define SERVOPRMMODIFYRESULT_H

#include <QJsonObject>
#include <QList>

struct ServoPrmModifyResultBean
{
    QString servoNum;
    QString key;
    bool status;//true表示成功，false表示获取失败

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

struct ServoPrmModifyResult
{
    bool status; //true表示成功，false表示获取失败
    QList<ServoPrmModifyResultBean> body;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

#endif // SERVOPRMMODIFYRESULT_H
