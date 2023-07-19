#ifndef CRSINGLEFIRMWAREXMLDATA_H
#define CRSINGLEFIRMWAREXMLDATA_H

#include <QJsonObject>

struct CRSingleFirmwareXmlData
{
    QString operationType; //start表示开始，stop表示停止
    QString cabType; //big表示大型控制柜，small表示小型控制柜
    QString cabVersion;//V1表示1代柜，V2表示2代柜
    QString updateType; //xml表示xml升级，firmware表示固件升级
    QString updateFile; //放到/dobot/userdata/project/update/file路径下
    QString updateFile2;//可选参数，仅用于二代大控制柜安全io，放到/dobot/userdata/project/update/file路径下
    int slaveId; //详细描述见下文

    QJsonObject toJsonObject() const;
    QString toJsonString() const;
    void fromJson(const QString& strJson);
    void fromJson(const QJsonObject& obj);
};
/*
关于slaveId说明如下：
当为CR一代大控制柜时slaveId的含义如下：
   值为1表示安全io（1块）和馈能板；
   值为2表示通用io；
   值为3~8表示伺服1~6；
   值为9表示末端io
当为CR一代小控制柜时slaveId的含义如下：
   值为1表示mcu；
   值为2~7表示伺服1~6；
   值为8表示末端io
当为CR二代大控制柜时slaveId的含义如下：
   值为1表示安全io（2块）和馈能板；
   值为2表示通用io；
   值为3~8表示伺服1~6；
   值为9表示末端io
*/
#endif // CRSINGLEFIRMWAREXMLDATA_H
