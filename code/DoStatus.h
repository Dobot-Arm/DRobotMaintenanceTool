#ifndef DOSTATUS_H
#define DOSTATUS_H

enum DoStatus
{
    E_DO_FAILED = -1, //处理失败
    E_DO_READY = 0, //
    E_DO_UPDATING = 1, //处理中
    E_DO_SUCCESS = 2, //处理成功
    E_DO_WARNSUCCESS = 3, //处理成功(显示警告)
    E_DO_MACSUCCESS = 4  //MAC地址处理成功
};

enum IPRecoveryStatus
{
    IP_RECOVERY_INIT = 0, //IP界面初始化
    IP_RECOVERY_DOING = 1, //IP恢复中
    IP_RECOVERY_SUCCESS = 2, //IP恢复成功
    IP_RECOVERY_FAILED = -1 //IP恢复失败
};

enum FileCheckStatus
{
    FILE_CHECK_DOING = 1, //检测中
    FILE_CHECK_SUCCESS = 2, //检测无缺失
    FILE_CHECK_FAILED = -1 //检测有缺失
};

enum ProgressBarStatus
{
    Progress_LOADING = 1, //加载中
    Progress_SUCCESS = 2, //加载成功
    Progress_FAILED = -1 //加载失败
};


enum UpgradeStatus
{
    E_UPGRADE_OUTAGE = -3,//中断升级
    E_UPGRADE_BEFORE_FAILED = -2,//升级固件前失败
    E_UPGRADE_FAILED = -1, //升级失败
    E_UPGRADE_READY = 0, //准备升级
    E_UPGRADE_UPDATING = 1, //升级中
    E_UPGRADE_SUCCESS = 2 //升级成功
};

#endif // DOSTATUS_H
