#include "EnvDebugConfig.h"
#include "Logger.h"
#include <QSettings>
#include <QRandomGenerator>

#include <windows.h>
#include "md5.h"
static bool getDiskInfo(QString& strSerialNumber)
{
    HANDLE hDevice = CreateFile(
        L"\\\\.\\PhysicalDrive0",  // 硬盘设备路径，这里是物理驱动器0
        0,                         // 不需要任何访问权限
        FILE_SHARE_READ | FILE_SHARE_WRITE, // 允许其他进程读写共享
        NULL,                     // 无安全属性
        OPEN_EXISTING,            // 打开已存在的设备
        0,                        // 不需要任何特殊属性
        NULL                      // 无模板
    );
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        qDebug() << "CreateFile failed with error: " << GetLastError();
        return false;
    }

    STORAGE_PROPERTY_QUERY query = {};
    query.PropertyId = StorageDeviceProperty;//StorageDeviceIdProperty;
    query.QueryType = PropertyStandardQuery;

    unsigned long nSize = sizeof(STORAGE_DEVICE_DESCRIPTOR);
    char* pDeviceInfo = (char*)malloc(nSize);
    STORAGE_DEVICE_DESCRIPTOR* pDevice = (STORAGE_DEVICE_DESCRIPTOR*)pDeviceInfo;
    pDevice->Size = nSize;

    DWORD bytesReturned = 0;
    BOOL bResult = DeviceIoControl(
        hDevice,                        // 设备句柄
        IOCTL_STORAGE_QUERY_PROPERTY,   // 执行查询属性操作的控制码
        &query,                         // 查询结构体
        sizeof(query),                  // 查询结构体大小
        pDevice,                    // 设备描述符结构体
        nSize,             // 设备描述符结构体大小
        &bytesReturned,                  // 返回的输出大小
        NULL                            // 不需要重叠结构
    );
    if (!bResult)
    {
        qDebug() << "DeviceIoControl failed with error: " << GetLastError();
        CloseHandle(hDevice);
        return false;
    }
    //成功后会得到device.Size的新的大小
    if (pDevice->Size > nSize)
    {//需要分配内存重新获取大小
        nSize = pDevice->Size;
        pDeviceInfo = (char*)realloc(pDeviceInfo, nSize);
        memset(pDeviceInfo, 0, nSize);
        pDevice = (STORAGE_DEVICE_DESCRIPTOR*)pDeviceInfo;
        pDevice->Size = nSize;
        bResult = DeviceIoControl(
            hDevice,                        // 设备句柄
            IOCTL_STORAGE_QUERY_PROPERTY,   // 执行查询属性操作的控制码
            &query,                         // 查询结构体
            sizeof(query),                  // 查询结构体大小
            pDevice,                    // 设备描述符结构体
            nSize,             // 设备描述符结构体大小
            &bytesReturned,                  // 返回的输出大小
            NULL                            // 不需要重叠结构
        );
        if (!bResult)
        {
            qDebug() << "DeviceIoControl failed with error: " << GetLastError();
            CloseHandle(hDevice);
            return false;
        }
    }

    char* data = nullptr;
    if (pDevice->VendorIdOffset > 0)
    {
        data = pDeviceInfo + pDevice->VendorIdOffset;
        qDebug() << "VendorId:" << data << '\n';
    }
    if (pDevice->ProductIdOffset > 0)
    {
        data = pDeviceInfo + pDevice->ProductIdOffset;
        qDebug() << "ProductId:" << data << '\n';
    }
    if (pDevice->ProductRevisionOffset > 0)
    {
        data = pDeviceInfo + pDevice->ProductRevisionOffset;
        qDebug() << "ProductRevision:" << data << '\n';
    }
    if (pDevice->SerialNumberOffset > 0)
    {
        data = pDeviceInfo + pDevice->SerialNumberOffset;
        strSerialNumber = QString::fromUtf8(data);
        qDebug() << "SerialNumber:" << data << '\n';
    }
    CloseHandle(hDevice);
    return 0;
}


EnvDebugConfig::EnvDebugConfig()
{
}

bool EnvDebugConfig::isDebugEnv()
{
    QSettings setting("config/config.ini",QSettings::IniFormat);
    QString strEnv = setting.value("CFG/env").toString();
    QString strRegCode = setting.value("CFG/regcode").toString();
    QString strRandomCode = setting.value("CFG/randomcode").toString();
    if ("debug"==strEnv && !strRegCode.isEmpty() && !strRandomCode.isEmpty())
    {//是测试环境则需要校验相关信息是否正确
        //二者相符合则说明是测试环境，否则不是测试环境
        //return checkRegComputeCode(GetComputerCode(),strRegCode);
        return checkRegComputeCode(strRandomCode,strRegCode);
    }
    return false;
}

QString EnvDebugConfig::GetComputerCode()
{
    /*QStorageInfo info("C:\\");
    QString strDevice(info.device());
    QString strSerialNumber;
    getDiskInfo(strSerialNumber);
    QString str = QString("dobot={\"device\":%1,\"sn\":%1}").arg(strDevice).arg(strSerialNumber);
    CMd5 md5;
    return QString::fromStdString(md5.GetMd5(str.toStdString()));*/
    return QString::number(QRandomGenerator::global()->bounded(1000,9999));
}

/*
校验 机器码与注册码是否相符合，符合返回true，否则返回false
*/
bool EnvDebugConfig::checkRegComputeCode(QString strComputerCode, QString strRegCode)
{
    if (strComputerCode.isEmpty() || strRegCode.isEmpty()) return false;
    /*QString strCode = strComputerCode+"[key=dobot20240805205135]";

    CMd5 md5;
    QString md5Code = QString::fromStdString(md5.GetMd5(strCode.toStdString()));
    if (md5Code == strRegCode)
    {//符合
        return true;
    }
    else
    {
        return false;
    }*/
    int ccode = strComputerCode.toInt();
    int rcode = strRegCode.toInt();
    ccode = ccode | 4353;
    return ccode==rcode;
}

void EnvDebugConfig::change2Release()
{
    QSettings setting("config/config.ini",QSettings::IniFormat);
    setting.setValue("CFG/env","release");
    setting.setValue("CFG/regcode","");
    setting.setValue("CFG/randomcode","");
    setting.sync();
}

bool EnvDebugConfig::change2Debug(QString strComputerCode, QString strRegCode)
{
    if (checkRegComputeCode(strComputerCode, strRegCode))
    {
        QSettings setting("config/config.ini",QSettings::IniFormat);
        setting.setValue("CFG/env","debug");
        setting.setValue("CFG/regcode",strRegCode);
        setting.setValue("CFG/randomcode",strComputerCode);
        setting.sync();

        return true;
    }
    return false;
}
