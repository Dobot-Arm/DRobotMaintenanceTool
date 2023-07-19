#ifndef FAULTREPAIRDATA_H
#define FAULTREPAIRDATA_H

#include <QJsonObject>

struct FaultRepairData
{
    QString operationType; //分别是这些值dns/mac/overTurn/defaultIp/synTime，表示不同功能
    QString macAddress; //可选参数，修复mac的使用用到
    QString date; //yyyy-mm-dd,可选参数，修复synTime的使用用到
    QString time; //HH:MM:ss,可选参数，修复synTime的使用用到

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};

#endif // FAULTREPAIRDATA_H
