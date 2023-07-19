#ifndef SERVOPRMREADRESULT_H
#define SERVOPRMREADRESULT_H

#include <QJsonObject>
#include <QList>

struct ServoPrmReadResultBean
{
    QString servoNum;
    QString key;
    double value;
    bool status;//true表示成功，false表示获取失败

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

struct ServoPrmReadResult
{
    bool status; //true表示成功，false表示获取失败
    QList<ServoPrmReadResultBean> body;

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

#endif // SERVOPRMREADRESULT_H
