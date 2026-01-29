#ifndef CLUAAPI_H
#define CLUAAPI_H

/*
 * 调用lua脚本中的接口，并返回对应的值
*/
#include <QObject>
#include <QString>
#include <QPair>
#include <QFileInfo>

template<typename T>
class COptional
{
    QPair<bool,T> m_value;
public:
    COptional():m_value(qMakePair<bool,T>(false,{})){}
    COptional(T v):m_value(qMakePair<bool,T>(true,v)){}
    COptional(const COptional& other)
    {
        if (this != &other)
        {
            m_value = other.m_value;
        }
    }
    COptional(COptional&& other)
    {
        m_value = other.m_value;
        other.m_value = qMakePair<bool,T>(false,{});
    }
    COptional& operator=(const COptional& other)
    {
        if (this != &other)
        {
            m_value = other.m_value;
        }
        return *this;
    }
    COptional& operator=(COptional&& other)
    {
        if (this != &other)
        {
            m_value = other.m_value;
            other.m_value = qMakePair<bool,T>(false,{});
        }
        return *this;
    }

    constexpr explicit operator bool() const noexcept
    {
        return m_value.first;
    }

    constexpr bool hasValue() const noexcept
    {
        return m_value.first;
    }

    constexpr const T& value() const
    {
        return m_value.second;
    }
};

struct CLuaApiPrivate;
class CLuaApi : public QObject
{
    Q_OBJECT
private:
    CLuaApi(QObject* p=nullptr);

public:
    CLuaApi(const CLuaApi&)=delete;
    CLuaApi(CLuaApi&&)=delete;
    CLuaApi& operator=(const CLuaApi&)=delete;
    CLuaApi& operator=(CLuaApi&&)=delete;
    ~CLuaApi();

    static CLuaApi *getInstance();

    void startLua();
    void stopLua();
    bool isLuaRun();

    /*
     * 功能：返回维护工具的版本号
     * 参数：无
     * 返回值：版本号。
    */
    QString getVersion();

    /*
    ** 功能：是否将该版本添加到组菜单上，也就是1级菜单
    ** 参数：strGroupVersion-表示这个需要判断的分组大版本号，例如：3.5.8。
    **      lstDirInfo-表示strGroupVersion这个分组目录下的所有文件和文件夹，例如：{{name="a.txt",isfile=true},{name="crc",isfile=false}}
    ** 返回值：true表示允许，false表示不允许，nil表示走c++代码逻辑判断。
    */
    COptional<bool> isShowSearchMenuItems(const QString& strGroupVersion, const QFileInfoList& fileInfos);

    /*
    ** 功能：是否将该版本的升级包添加到子级菜单上，也就是2级菜单
    ** 参数：strPackageVersion-表示这个需要判断的升级包版本号，例如：3.5.8.1。
    ** 返回值：true表示允许，false表示不允许，nil表示走c++代码逻辑判断。
    */
    COptional<bool> isShowSearchSubMenuItems(const QString& strPackageVersion);

    /*
    ** 功能：返回高级功能中单个升级的文件
    ** 参数：strPackageVersion-表示选中的升级包版本号（也就是二级菜单名称），例如：3.5.8.1。
    **      lstFile-表示strPackageVersion这个分组目录下的所有文件，例如：{"a.txt","crc"}
    **      strUpgradeType-表示选中的升级的类型，固定值如下：
                    CCBOX,CCBOXXML 分别表示选中了“IO板卡固件”的FOE和XML
                    SafeIO,SafeIOXML 分别表示选中了“安全IO固件”的FOE和XML
                    FeedBack 表示选中了“馈能板固件”
                    UniIO,UniIOXML 分别表示选中了“通用IO固件”的FOE和XML
                    Terminal,TerminalXML 分别表示选中了“末端IO”的FOE和XML
                    ServoJALL,ServoJALLXML 分别表示选中了“伺服固件”的FOE和XML
                    ServoJ1,ServoJ1XML 分别表示选中了“伺服J1”的FOE和XML
                    ServoJ2,ServoJ2XML 分别表示选中了“伺服J2”的FOE和XML
                    ServoJ3,ServoJ3XML 分别表示选中了“伺服J3”的FOE和XML
                    ServoJ4,ServoJ4XML 分别表示选中了“伺服J4”的FOE和XML
                    ServoJ5,ServoJ5XML 分别表示选中了“伺服J5”的FOE和XML
                    ServoJ6,ServoJ6XML 分别表示选中了“伺服J6”的FOE和XML
            allLuaVal-ini配置文件的内容，与`getUpgradeIniVersion`接口参数完全相同
    ** 返回值：当返回nil表示走c++代码逻辑来选择文件。
            否则可以有1个或2个或更多返回值，分别是：文件名1,文件名2,...-表示要选择这文件升级。
            如果不想走c++逻辑，又不允许选择文件，那么返回空字符串即可。
    */
    COptional<QStringList> getSingleUpdrageFiles(const QString& strPackageVersion,
                                                 const QStringList& lstFile,
                                                 const QString& strUpgradeType,
                                                 const QHash<QString,QHash<QString,QString>>& allLuaVal);

    /*
     * 功能：是否允许使用该维护升级工具
     * 参数：strReasonMsg,返回参数，不允许的原因
     * 返回值：true表示允许，false表示不允许，nil表示走c++代码逻辑判断。
    */
    COptional<bool> isCanUseTool(QString& strReasonMsg);

    /*
     * 功能：是否允许升级
     * 参数：strReasonMsg,返回参数，不允许的原因
     * 返回值：true表示允许，false表示不允许，nil表示走c++代码逻辑判断。
    */
    COptional<bool> isCanUpgrade(QString& strReasonMsg);

    /*
    ** 功能：升级文件是否允许传输到控制器
    ** 参数：strFileName需要判断的文件名,例如：CCBOX_IO_FOE_V2.2.2.0.efw
    ** 返回值：true表示允许，false表示不允许，nil表示走c++代码逻辑判断。
    */
    COptional<bool> isFileNeedCopyToController(const QString& strFileName);

    /*
    ** 功能：获取升级包中控制器的升级文件，主要是升级包中的CR_update_XXX/src目录下的文件。
    ** 参数：无
    ** 返回值：当返回nil表示走c++代码逻辑来选择文件。
            否则可以有多个返回值，分别是：文件名1,文件名2,...文件名n-表示要选择这些文件升级。
            如果不想走c++逻辑，又不允许选择文件，那么返回空字符串即可。
    */
    COptional<QStringList> getControllerUpdateFiles();

    /*
     * 功能：一键升级功能的伺服升级功能是否允许使用
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑判断。
            true,”” 第一个表示允许，第二个表示原因，
            false,”不支持” 第一个表示不允许，第二个表示原因
    */
    COptional<QPair<bool,QString>> isOnekeyServoFuncCanUse();

    /*
     * 功能：返回伺服升级选择的param.csv和关节J1.csv~J6.csv文件夹目录名称。
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑来选择目录名称。
            其他返回值，比如："CR","CRA","CRV2","Nova","E6"，则表示在对应的目录选择csv文件。
    */
    COptional<QString> getServoUpgradeFolderName();

    /*
     * 功能：返回伺服参数模板文件名，例如：CR_Param.csv
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑来选择目录名称。
            其他返回值，比如："CR_Param.csv","E6_Param.csv","Nova_Param.csv"，则表示在对应的csv文件。
    */
    COptional<QString> getServoParamTemplateFile();

    /*
     * 功能：返回“伺服关节电机厂家配置对照表”文件名，例如：CR_prefix.csv
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑来选择目录名称。
            其他返回值，比如："CR_prefix.csv","CRA-IS_prefix.csv","CRV_prefix.csv","E6_prefix.csv","Nova_prefix.csv"，则表示在对应的csv文件。
    */
    COptional<QString> getServoJointTableFile();

    /*
     * 功能：SN码功能写入、修复是否允许使用
     * 参数：strReasonMsg,返回参数，不允许的原因
     * 返回值：true表示允许，false表示不允许，nil表示走c++代码逻辑判断。
    */
    COptional<bool> isSNCodeCanUse(QString& strReasonMsg);

    /*
     * 功能：选择哪种方式写入SN码。
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑判断。
            1-按照E6的方式写入SN码。
            2-按照CC162和CCBOX的方式写入SN码。
            3-按照CC26X的方式写入SN码。
    */
    COptional<long long> getSNCodeWriteType();

    /*
     * 功能：返回控制柜版本，主要是用于升级时cabVersion字段的参数值
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑判断。
              其他返回值，比如："V1"、"V2"
    */
    COptional<QString> getCabVersion();

    /*
     * 功能：返回控制柜类型，主要是用于升级时cabType字段的参数值
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑判断。
              其他返回值，比如："big"、"small"
    */
    COptional<QString> getCabType();

    /*
     * 功能：返回内核升级时的控制柜版本
     * 参数：无
     * 返回值：当返回nil表示走c++代码逻辑判断。
              其他返回值，比如："V1"、"V2"
    */
    COptional<QString> getCabVersionKernel();

    /*
    * 功能：返回升级包中配置文件xxxxxupgrade.ini的配置版本信息
    * 参数：table值，内容为ini配置文件的内容，格式如下，
          假如ini配置文件内容为：
            [version]
            safeIO=1.2.2.2
            uniIO=3.0.3.2

            [Section2]
            k1=v1
            k2=v2
          则入参的table如下：
            {
                version={safeIO="1.2.2.2", uniIO="3.0.3.2"}
                Section2={k1="v1",k2="v2"}
            }
    * 返回值：当返回nil表示走c++代码逻辑读取，否则返回内容为table，如下格式：
            {
                control="4.6.0.0-rc3", --控制器/主控板显示版本
                safeio="1.4.1.1", --安全IO固件
                terminal="6.3.3.3", --末端IO固件
                unio="3.0.3.4", --通用IO固件
                ccboxio="3.0.3.4", --IO板固件
                feedback="2.3.5.0", --馈能板固件
                servo="6.1.9.0" --伺服显示版本
            }
    * 【特别注意】：因为读取的INI配置文件最终会转为lua的table，因此配置文件的每个section和key命名都要符合lua的标准
    */
    COptional<QHash<QString,QString>> getUpgradeIniVersion(const QHash<QString,QHash<QString,QString>>& allLuaVal);

    /*
     * 功能：获取单个伺服参数读/写的毫秒超时时间
     * 参数：无
     * 返回值：毫秒时间，默认值是400
    */
    COptional<int> getServoParamReadTimeoutMs();
    COptional<int> getServoParamWriteTimeoutMs();

    /*
     * 功能：是否允许导入升级包
     * 参数：strPackageVersion-表示选中的升级包版本号（也就是二级菜单名称），例如：3.5.8.1。
     *      allLuaVal-ini配置文件的内容，与`getUpgradeIniVersion`接口参数完全相同
     * 返回值：true表示允许，false表示不允许，nil表示走c++代码逻辑判断。
    */
    COptional<bool> isCanImportPackage(const QString& strPackageVersion, const QHash<QString,QHash<QString,QString>>& allLuaVal);

    /*
    * 功能：获取一键升级/高级功能页面中需要显示的元素
    * 参数：无
    * 返回值：当返回nil表示走c++代码逻辑读取，否则返回内容为table，如下格式：
            true表示显示，false表示隐藏，如果字段不给，默认就隐藏
            {
                control=true, --控制器/主控板显示版本
                ccboxio=false, --IO板固件
                safeio=true, --安全IO固件
                feedback=true, --馈能板固件
                unio=true, --通用IO固件
                servo=true, --伺服固件
                terminal=true --末端IO固件
            }
    */
    COptional<QHash<QString,bool>> getShowFirmAdvanceUI();

    /*
     * 功能：高级功能单个升级前，对选择的单个文件做命名规则检查，检查合格的允许进行单个升级，否则不允许。
    参数：strUpgradeType-字符串，表示选中的升级的类型,具体参考getSingleUpdrageFiles接口
         allUpdateFile-所有文件
    返回值：当返回nil表示走c++代码逻辑判断。
        true,”” 第一个表示允许，第二个表示原因，
        false,”不支持” 第一个表示不允许，第二个表示原因
    */
    COptional<QPair<bool,QString>> isSingleUpdrageFilesValid(const QString& strUpgradeType, QStringList allUpdateFile);

    /*
     * 功能：高级功能单个升级，获取手动导入的个数。
    参数：strUpgradeType-字符串，表示选中的升级的类型,具体参考getSingleUpdrageFiles接口
    返回值：当返回nil表示走c++代码逻辑判断个数，否则返回实际导入的个数(至少1个,否则按照nil处理)
    */
    COptional<int> getSingleUpdrageImportCount(const QString& strUpgradeType);

    /*
     * 功能：获取机器人的系统架构
     * 参数：无
     * 返回值：字符串
    */
    QString getRobotArchitecture();

    /*
    功能：获取一键升级的UI显示顺序
    参数：无
    返回值：返回nil表示按照默认现有的顺序显示，返回table则按照table的显示
    说明：http接口返回的状态值顺序保持不变，含义也保持不变，只是UI的顺序根据lua接口变更。
    key表示对应的UI，value显示显示顺序
    {
        unio = 1, --通用IO固件、IO板固件
        control = 2, --控制器/主控板显示版本
        feedback = 3, --馈能板固件
        safeio = 4, --安全IO固件
        servo = 5, --伺服固件
        terminal = 6 --末端IO固件
    }
    */
    COptional<QHash<QString,int>> getUpdateSequerence();

    /*
    * 功能：获取一键升级进度页面中需要显示的元素
    * 参数：无
    * 返回值：当返回nil表示走c++代码逻辑读取，否则返回内容为table，如下格式：
            true表示显示，false表示隐藏，如果字段不给，默认就隐藏
            {
                unio=true, --通用IO固件
                ccboxio=false, --IO板固件
                control=true, --控制器/主控板显示版本
                feedback=true, --馈能板固件
                safeio=true, --安全IO固件
                servo=true, --伺服固件
                terminal=true --末端IO固件
            }
    */
    COptional<QHash<QString,bool>> getShowUpdateProcessUI();

    /*
    * 功能：获取单个升级的硬件从站id
    * 参数：无
    * 返回值：当返回nil表示走c++代码逻辑读取，否则返回内容为table，如下格式：
            key表示对应的硬件，value表示该硬件的从站ID值
            以下的字段一个都不能少，否则在c++层面认为是返回nil
            {
                control = 0, --控制器/主控板显示版本
                feedback = 1, --馈能板
                safeio = 1, --安全IO板
                ccboxio = 1, --IO板固件
                unio = 2, --通用IO固件
                j1 = 3, --伺服固件
                j2 = 4, --伺服固件
                j3 = 5, --伺服固件
                j4 = 6, --伺服固件
                j5 = 7, --伺服固件
                j6 = 8, --伺服固件
                terminal = 9 --末端IO固件
            }
    */
    COptional<QHash<QString,int>> getHardwareSlaveId();

    /*
     * 功能：执行一键恢复出厂设置。
    参数：strFile-字符串，表示恢复的配置文件
    返回值：true,”成功” 第一个表示成功，第二个表示提示语，
           false,”失败” 第一个表示失败，第二个表示失败原因
           如果第二个参数不存在或为空字符串则使用默认提示语
    */
    QPair<bool,QString> execRecoveryFactory(const QString& strFile);

signals:
    void signalStopRun();
    void signalLuaLog(QString strLog, QtMsgType type);

private:
    CLuaApiPrivate* m_ptr;
};

#endif // CLUAAPI_H
