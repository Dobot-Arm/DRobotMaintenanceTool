#ifndef CHTTPUSERAPI_H
#define CHTTPUSERAPI_H

#include "Bean/CRFirmwareAndXmlData.h"
#include "Bean/CRFirmwareAndXmlResult.h"
#include "Bean/CRSingleFirmwareXmlData.h"
#include "Bean/FaultRepairData.h"
#include "Bean/ServoPrmModifyData.h"
#include "Bean/ServoPrmModifyResult.h"
#include "Bean/ServoPrmReadData.h"
#include "Bean/ServoPrmReadResult.h"

struct CHttpUserAPIPrivate;

class CHttpUserAPI : public QObject
{
    Q_OBJECT

private:
    CHttpUserAPIPrivate* m_ptr;
protected:
    explicit CHttpUserAPI(QObject *parent = nullptr);

public:
    CHttpUserAPI(const CHttpUserAPI&)=delete;
    CHttpUserAPI(CHttpUserAPI&&)=delete;
    CHttpUserAPI& operator=(const CHttpUserAPI&)=delete;
    CHttpUserAPI& operator=(CHttpUserAPI&&)=delete;
    ~CHttpUserAPI();

    static CHttpUserAPI& getInstance();

    /*
     *功  能：通过检查特定磁盘是否存在来判断是否适合升级
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true表示适合升级，false表示不适合升级
     */
    bool checkSpecialDiskCanUpdate(const QString& strIP,
                               int iTimeoutMilliseconds=3000);

    /*
     *功  能：CR的一键升级
     *参  数：strIP-ip地址，
              data-升级参数，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool updateCRFirmwareAndXml(const QString& strIP, 
                              const CRFirmwareAndXmlData& data,
                              int iTimeoutMilliseconds=3000);

    /*
     *功  能：获取CR的一键升级结果状态
     *参  数：strIP-ip地址，
              result-返回参数表示结果值，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool getUpdateCRFirmwareAndXmlStatus(const QString& strIP, 
                                      CRFirmwareAndXmlResult& result,
                                      int iTimeoutMilliseconds=3000);

    /*
     *功  能：CR的单个升级
     *参  数：strIP-ip地址，
              data-升级参数，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool updateCRSingleFirmwareAndXml(const QString& strIP, 
                              const CRSingleFirmwareXmlData& data,
                              int iTimeoutMilliseconds=3000);

	/*
     *功  能：获取单个升级结果状态
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：返回状态结果
     */
    UpdateState getUpdateSingleFirmwareAndXmlStatus(const QString& strIP, 
                                      int iTimeoutMilliseconds=3000);

    /*
     *功  能：故障修复
     *参  数：strIP-ip地址，
              data-修复参数，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool faultRepair(const QString& strIP, 
                 const FaultRepairData& data,
                 int iTimeoutMilliseconds=3000);

    /*
     *功  能：获取固件版本号
     *参  数：strIP-ip地址，
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    QString getToolVersion(const QString& strIP, 
                     int iTimeoutMilliseconds=3000);

	/*
     *功  能：修改伺服参数
     *参  数：strIP-ip地址，
              data-修改参数，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool modifyServoParameters(const QString& strIP, 
                            const ServoPrmModifyData& data,
                            int iTimeoutMilliseconds=3000);

	/*
     *功  能：获取修改伺服参数结果状态
     *参  数：strIP-ip地址，
              result-返回参数表示结果值，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool getModifyServoParametersStatus(const QString& strIP, 
                                      ServoPrmModifyResult& result,
                                      int iTimeoutMilliseconds=3000);

	/*
     *功  能：读取伺服参数
     *参  数：strIP-ip地址，
              data-修改参数，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool readServoParameters(const QString& strIP, 
                          const ServoPrmReadData& data,
                          int iTimeoutMilliseconds=3000);

	/*
     *功  能：获取读取伺服参数结果状态
     *参  数：strIP-ip地址，
              result-返回参数表示结果值，具体查看后续章节
              iTimeoutMilliseconds-毫秒超时时间
     *返回值：true/false表示成功或失败
     */
    bool getReadServoParametersStatus(const QString& strIP, 
                                      ServoPrmReadResult& result,
                                      int iTimeoutMilliseconds=3000);

private:
    QString httpGet(const QString& strUrl, int iTimeoutMilliseconds);
    QString httpPost(const QString& strUrl, const QString& strPostData, int iTimeoutMilliseconds);
};

#endif // CHTTPUSERAPI_H
