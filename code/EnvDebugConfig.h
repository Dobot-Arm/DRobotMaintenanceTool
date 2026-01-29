#ifndef ENVDEBUGCONFIG_H
#define ENVDEBUGCONFIG_H

#include <QString>

class EnvDebugConfig
{
public:
    EnvDebugConfig();

    //是否为测试环境
    static bool isDebugEnv();

    //获取机器码
    static QString GetComputerCode();
    //检查机器码与注册码是否相等
    static bool checkRegComputeCode(QString strComputerCode, QString strRegCode);
    //切换为release环境
    static void change2Release();
    //切换为debug环境，成功返回true
    static bool change2Debug(QString strComputerCode, QString strRegCode);
};

#endif // ENVDEBUGCONFIG_H
