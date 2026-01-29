#include "MainWidget2.h"
#include "ui_MainWidget2.h"
#include "RobotStudio/WidgetToast.h"
#include "MaskDialogContainer.h"
#include "SystemToolSNWriteWidget.h"
#include "SNWriteConfirmWidget.h"
#include "SNWriteStatusWidget.h"
#include "SNMatchFailedWidget.h"
#include "SNWriteDoingWidget.h"
#include "SystemToolSNRepairWidget.h"
#include "LuaApi.h"
#include "LoadingUI.h"
#include <QSet>
#include <QScopeGuard>
#include <QElapsedTimer>

bool MainWidget2::isCanUseSN()
{
    QString strReason;
    auto bCanUseSN = CLuaApi::getInstance()->isSNCodeCanUse(strReason);
    if (bCanUseSN.hasValue())
    {
        if (bCanUseSN.value())
        {
            return true;
        }
        if (strReason.isEmpty()) strReason = tr("当前机型无法使用此功能");
        auto* messageWidget = new MessageWidget(this);
        messageWidget->setMessage("warn",strReason);
        messageWidget->show();
        return false;
    }
    int isUseCurrentFunc = -1;//-1:机型不支持，-2:控制器版本不支持，-3:伺服版本不支持，-4:控制器、伺服都不支持，0:支持
    if(CommonData::getControllerType().name.contains("CR")&&CommonData::getControllerType().name.contains("A"))
    {
        if(isCurrentControlLagerTargetControl(CommonData::getCurrentSettingsVersion().control,"4.5.0.0"))
        {
            if(isCurrentFWLargerTargetFWVersion(CommonData::getCurrentSettingsVersion().servo1,"6.1.4.0"))
            {
                isUseCurrentFunc = 0;
            }
            else
            {
                isUseCurrentFunc = -3;
            }
        }
        else
        {
            isUseCurrentFunc = -2;
            if(!isCurrentFWLargerTargetFWVersion(CommonData::getCurrentSettingsVersion().servo1,"6.1.4.0"))
            {
                isUseCurrentFunc = -4;
            }

        }
    }

    if(CommonData::getControllerType().name==g_strPropertiesCabinetTypeMagicianE6)
    {
        if(isCurrentControlLagerTargetControl(CommonData::getCurrentSettingsVersion().control,"4.5.0.0"))
        {
            isUseCurrentFunc = 0;
        }
        else
        {
            isUseCurrentFunc = -2;
        }
    }

    if ((CommonData::getControllerType().name.contains("CR")&&!CommonData::getControllerType().name.contains("A"))
            ||CommonData::getControllerType().name.contains("Nova",Qt::CaseInsensitive))
    {
        if(isCurrentControlLagerTargetControl(CommonData::getCurrentSettingsVersion().control,"3.5.7.0"))
        {
            isUseCurrentFunc = 0;
        }
        else
        {
            isUseCurrentFunc = -5;
        }
    }

    if(isUseCurrentFunc == -1)
    {
        auto* messageWidget = new MessageWidget(this);
        messageWidget->setMessage("warn",tr("当前机型无法使用此功能"));
        messageWidget->show();
        return false;
    }
    if(isUseCurrentFunc == -2)
    {
        auto* messageWidget = new MessageWidget(this);
        messageWidget->setMessage("warn",tr("%1仅支持控制器V4.5.0及以上版本使用此功能").arg(CommonData::getControllerType().name));
        messageWidget->show();
        return false;
    }

    if(isUseCurrentFunc == -3)
    {
        auto* messageWidget = new MessageWidget(this);
        messageWidget->setMessage("warn",tr("%1仅支持伺服6.1.4.0及以上版本使用此功能").arg(CommonData::getControllerType().name));
        messageWidget->show();
        return false;
    }
    if(isUseCurrentFunc == -4)
    {
        auto* messageWidget = new MessageWidget(this);
        messageWidget->setMessage("warn",tr("%1仅支持控制器V4.5.0及以上版本,伺服6.1.4.0及以上版本使用此功能")
                                  .arg(CommonData::getControllerType().name));
        messageWidget->show();
        return false;
    }

    if(isUseCurrentFunc == -5)
    {
        auto* messageWidget = new MessageWidget(this);
        messageWidget->setMessage("warn",tr("%1仅支持控制器V3.5.7及以上版本使用此功能").arg(CommonData::getControllerType().name));
        messageWidget->show();
        return false;
    }
    return true;
}

void MainWidget2::writeE6SN()
{
    auto* systemToolSNWriteWidget = new SystemToolSNWriteWidget(this);
    auto* snWriteStatusWidget = new SNWriteStatusWidget(this);

    while (!systemToolSNWriteWidget->isClose())
    {
        systemToolSNWriteWidget->exec();
        if (systemToolSNWriteWidget->isClose()) break;
        //确认写进去
        ControllerSnData data;
        data.ControllCabinetSNCode = systemToolSNWriteWidget->getControllCabinetSN();
        bool isConrtolWriteSuccess = m_httpProtocol->postSettingsProductInfoControllerSn(g_strCurrentIP,data);
        if(!isConrtolWriteSuccess)
        {
            snWriteStatusWidget->setWriteStatus(false);
            snWriteStatusWidget->setWriteFailedMsg(tr("控制柜SN码写入失败"));
            snWriteStatusWidget->exec();
        }
        else
        {
            CommonData::setSettingsProductInfoHardwareInfo(m_httpProtocol->getSettingsProductInfoHardwareInfo(g_strCurrentIP));
            snWriteStatusWidget->setWriteStatus(true);
            snWriteStatusWidget->exec();
        }
        if(snWriteStatusWidget->isClose())
        {
            break;
        }
    }
    systemToolSNWriteWidget->deleteLater();
    snWriteStatusWidget->deleteLater();
}

void MainWidget2::writeCC162OrCCBOXSN()
{
    auto* systemToolSNWriteWidget = new SystemToolSNWriteWidget(this);
    auto* snWriteStatusWidget = new SNWriteStatusWidget(this);

    while (!systemToolSNWriteWidget->isClose())
    {
        systemToolSNWriteWidget->exec();
        if (systemToolSNWriteWidget->isClose()) break;
        //确认写进去
        V3ControllerSnData data;
        data.SNcode = systemToolSNWriteWidget->getControllCabinetSN();
        bool isConrtolWriteSuccess = m_httpProtocol->postV3SettingsProductInfoControllerSn(g_strCurrentIP,data);
        if(!isConrtolWriteSuccess)
        {
            snWriteStatusWidget->setWriteStatus(false);
            snWriteStatusWidget->setWriteFailedMsg(tr("控制柜SN码写入失败"));
            snWriteStatusWidget->exec();
        }
        else
        {
            DobotType::SettingsProductInfoHardwareInfo settingsProductInfoHardwareInfo;
            settingsProductInfoHardwareInfo.ControllCabinetSNCode = m_httpProtocol->getSettingsProductInfoControllerSn(g_strCurrentIP);
            CommonData::setSettingsProductInfoHardwareInfo(settingsProductInfoHardwareInfo);
            snWriteStatusWidget->setWriteStatus(true);
            snWriteStatusWidget->exec();
        }

        if(snWriteStatusWidget->isClose())
        {
            break;
        }

    }
    systemToolSNWriteWidget->deleteLater();
    snWriteStatusWidget->deleteLater();
}

static bool g_bNeedCC26XCheckValid = true;
void MainWidget2::writeCC26XSN()
{
    auto* systemToolSNWriteWidget = new SystemToolSNWriteWidget(this);
    auto* snWriteConfirmWidget = new SNWriteConfirmWidget(this);
    auto* snMatchFailedWidget = new SNMatchFailedWidget(this);
    auto* snWriteStatusWidget = new SNWriteStatusWidget(this);
    while (!systemToolSNWriteWidget->isClose())
    {
        systemToolSNWriteWidget->exec();
        if (systemToolSNWriteWidget->isClose()) break;
        //确认写进去
        bool bSNMatchSuccess = systemToolSNWriteWidget->isSNMacthSuccess();
        if (!g_bNeedCC26XCheckValid){
            bSNMatchSuccess = true; //不需要检测则默认为true
        }
        if(bSNMatchSuccess)
        {
            snWriteConfirmWidget->exec();
        }
        else
        {
            snMatchFailedWidget->exec();
        }

        if (bSNMatchSuccess&&snWriteConfirmWidget->isConfirmWrite())
        {
            auto* snWriteDoingWidget = new SNWriteDoingWidget(this);
            snWriteDoingWidget->setControllCabinetSN(systemToolSNWriteWidget->getControllCabinetSN());
            snWriteDoingWidget->setRobotArmSN(systemToolSNWriteWidget->getRobotArmSN());
            snWriteDoingWidget->exec();
            bool isConrtolWriteSuccess = snWriteDoingWidget->isConrtolWriteSuccess();
            bool isRobotArmWriteSuccess = snWriteDoingWidget->isRobotArmWriteSuccess();
            snWriteDoingWidget->deleteLater();
            if(!isRobotArmWriteSuccess&&!isConrtolWriteSuccess)
            {
                snWriteStatusWidget->setWriteStatus(false);
                snWriteStatusWidget->setWriteFailedMsg(tr("全部写入失败"));
                snWriteStatusWidget->exec();
            }
            else if(!isRobotArmWriteSuccess)
            {
                snWriteStatusWidget->setWriteStatus(false);
                snWriteStatusWidget->setWriteFailedMsg(tr("机械臂SN码写入失败"));
                snWriteStatusWidget->exec();
            }
            else if(!isConrtolWriteSuccess)
            {
                snWriteStatusWidget->setWriteStatus(false);
                snWriteStatusWidget->setWriteFailedMsg(tr("控制柜SN码写入失败"));
                snWriteStatusWidget->exec();
            }
            else
            {
                snWriteStatusWidget->setWriteStatus(true);
                snWriteStatusWidget->exec();
            }
            snWriteDoingWidget->deleteLater();
        }
        if(snWriteStatusWidget->isClose())
        {
            break;
        }
    }

    systemToolSNWriteWidget->deleteLater();
    snWriteConfirmWidget->deleteLater();
    snMatchFailedWidget->deleteLater();
    snWriteStatusWidget->deleteLater();
}
void MainWidget2::slotSystemSNWrite(bool bNeedCheckValid/*=true*/)
{
    g_bNeedCC26XCheckValid = bNeedCheckValid;
    if (bNeedCheckValid)
    {
        if(!isCanUseSN()) return;
    }

    m_searchDeviceAndIsOnlineTimer->stop();

    auto type = CLuaApi::getInstance()->getSNCodeWriteType();
    if (type.hasValue())
    {
        if (1 == type.value())
        {
            writeE6SN();
        }
        else if (2 == type.value())
        {
            writeCC162OrCCBOXSN();
        }
        else if (3 == type.value())
        {
            writeCC26XSN();
        }
    }
    else
    {
        if(CommonData::getControllerType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
        {
            writeE6SN();
        }
        else if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                            ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
        {
            writeCC162OrCCBOXSN();
        }
        else
        {//CC26X的写入
            writeCC26XSN();
        }
    }
    m_searchDeviceAndIsOnlineTimer->start(2500);
}

void MainWidget2::slotSystemSNRepair()
{
    if(!isCanUseSN())
    {
        return;
    }

    m_searchDeviceAndIsOnlineTimer->stop();

    auto* systemToolSNRepairWidget = new SystemToolSNRepairWidget(this);
    auto* snWriteConfirmWidget = new SNWriteConfirmWidget(this);
    auto* snMatchFailedWidget = new SNMatchFailedWidget(this);
    auto* snWriteStatusWidget = new SNWriteStatusWidget(this);
    while (!systemToolSNRepairWidget->isClose())
    {
        systemToolSNRepairWidget->exec();
        if (systemToolSNRepairWidget->isClose()) break;

        if(systemToolSNRepairWidget->isSNMacthSuccess())
        {
            snWriteConfirmWidget->exec();
        }
        else
        {
            snMatchFailedWidget->exec();
        }
        if (systemToolSNRepairWidget->isSNMacthSuccess()&&snWriteConfirmWidget->isConfirmWrite())
        {
            //确认写进去
            auto* snWriteDoingWidget = new SNWriteDoingWidget(this);
            snWriteDoingWidget->setControllCabinetSN(systemToolSNRepairWidget->getControllCabinetSN());
            snWriteDoingWidget->setRobotArmSN(systemToolSNRepairWidget->getRobotArmSN());
            snWriteDoingWidget->exec();
            bool isConrtolWriteSuccess = snWriteDoingWidget->isConrtolWriteSuccess();
            bool isRobotArmWriteSuccess = snWriteDoingWidget->isRobotArmWriteSuccess();
            bool isAlarmClearSuccess = snWriteDoingWidget->isAlarmClearSuccess();
            snWriteDoingWidget->deleteLater();
            if(!isRobotArmWriteSuccess&&!isConrtolWriteSuccess)
            {
                snWriteStatusWidget->setWriteStatus(false);
                snWriteStatusWidget->setWriteFailedMsg(tr("全部写入失败"));
                snWriteStatusWidget->exec();
            }
            else if(!isRobotArmWriteSuccess)
            {
                snWriteStatusWidget->setWriteStatus(false);
                snWriteStatusWidget->setWriteFailedMsg(tr("机械臂SN码写入失败"));
                snWriteStatusWidget->exec();
            }
            else if(!isConrtolWriteSuccess)
            {
                snWriteStatusWidget->setWriteStatus(false);
                snWriteStatusWidget->setWriteFailedMsg(tr("控制柜SN码写入失败"));
                snWriteStatusWidget->exec();
            }
            else
            {
                if(isAlarmClearSuccess)
                {
                    if (m_pSNAlarmBubbleTipWidget)
                    {
                        m_pSNAlarmBubbleTipWidget->hide();
                    }
                    snWriteStatusWidget->setWriteStatus(true);
                    snWriteStatusWidget->exec();
                }
                else
                {
                    snWriteStatusWidget->setWriteStatus(true);
                    snWriteStatusWidget->setWriteOkMsg(tr("写入成功，需重启控制柜方可生效!"));
                    snWriteStatusWidget->exec();
                }
            }
        }
        if(snWriteStatusWidget->isClose())
        {
            break;
        }
    }
    systemToolSNRepairWidget->deleteLater();
    snWriteConfirmWidget->deleteLater();
    snMatchFailedWidget->deleteLater();
    snWriteStatusWidget->deleteLater();

    m_searchDeviceAndIsOnlineTimer->start(2500);
}

bool MainWidget2::checkSveroParameters(bool& bNeedUpdate, QList<int>& bJointNotEqual, QString arrJointCSVFile[6], const bool bShowTips/*=false*/)
{
    m_bIsCheckingServoParam = true;
    auto scopeGuard = qScopeGuard([this]{m_bIsCheckingServoParam = false;});

    if(m_upgradeVersion.isEmpty())
    {//请选择升级包
        qDebug()<<"伺服参数校验对比失败，请选择升级包";
        if (bShowTips){
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("请选择升级包"));
            pWndMsg->show();
        }
        return false;
    }
    qDebug()<<"准备进行伺服参数校验对比...checkSveroParameters,ParentVersion="<<m_upgradeParentVersion<<",m_upgradeVersion="<<m_upgradeVersion;
    /*伺服说，只要这张表里面地址对应的数据与本体对应地址的数据相等，就认为是最新的伺服参数*/
    const QString strKeyParameterCSV = getKeyParameterCSVFile();
    if(!QFile::exists(strKeyParameterCSV))
    {//没有找到模板表格
        qDebug()<<"没有找到模板表格 file not exist:"<<strKeyParameterCSV;
        if (bShowTips){
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",QString("%1:%2").arg(tr("没有找到模板表格")).arg(QDir::toNativeSeparators(strKeyParameterCSV)));
            pWndMsg->show();
        }
        return false;
    }

    QString servoParameterTemplatePath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"
            +m_upgradeParentVersion+"/"+m_upgradeVersion+"/servoCSV";
    {
        QString str = getServoCSVSerialTypeName();
        servoParameterTemplatePath.append('/');
        if (!str.isEmpty()){
            servoParameterTemplatePath.append(str);
            servoParameterTemplatePath.append('/');
        }
    }
    m_strServoParameterPath = servoParameterTemplatePath;
    const DobotType::ControllerType controllerType = CommonData::getControllerType();
    servoParameterTemplatePath += controllerType.name;
    servoParameterTemplatePath += "/";

    //获取6个伺服电机的厂家型号和电机型号，特殊处理
    QHash<QString, DobotType::StructServoValue> servoJointTab = m_httpProtocol->getReadServoValueDianjixinghao(g_strCurrentIP);
    if (servoJointTab.isEmpty()){
        qDebug()<<"伺服参数校验对比失败,因为无法读取6个关节的伺服电机型号";
        if (bShowTips){
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("读伺服电机型号参数数据失败"));
            pWndMsg->show();
        }
        return false;
    }

    for (int i=1;i<=6;++i)
    {
        QString jointFileName;
        auto itr = servoJointTab.find(QString::asprintf("J%d",i));
        if (itr != servoJointTab.end()){
            QPair<bool,QString> rr = getServoJointTable(itr.value().changjia, itr.value().dianji);
            if (rr.first){
                if (rr.second.isEmpty()) jointFileName = QString("J%1.csv").arg(i);
                else jointFileName = QString("%1-J%2.csv").arg(rr.second).arg(i);
            }else{
                qDebug()<<"伺服参数对比失败，因为伺服参数对照表中没有找到该型号的厂家:"<<itr.value().changjia<<",和电机:"<<itr.value().dianji;
                if (bShowTips){
                    auto* pWndMsg = new MessageWidget(this);
                    pWndMsg->setMessage("warn",tr("伺服参数对照表中没有找到该电机型号(%1)和厂家(%2)的配置").arg(itr.value().dianji).arg(itr.value().changjia));
                    pWndMsg->show();
                }
                return false;
            }
        }else{
            qDebug()<<"warn the servo joint table no J"<<i<<" parameter name";
            jointFileName = QString("J%1.csv").arg(i);
        }

        QString servoParamPath = servoParameterTemplatePath+jointFileName;
        if (!QFile::exists(servoParamPath))
        {//升级包中没有伺服文件
            qDebug()<<"该升级包中，没有找到伺服关节参数文件："<<servoParamPath;
            if (bShowTips){
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("%1版本升级包中未找到%2的伺服参数文件%3，请重新下载该版本升级包")
                                    .arg(CommonData::getUpgradeSettingsVersion().control)
                                    .arg(controllerType.name).arg(jointFileName));
                pWndMsg->show();
            }
            return false;
        }
        arrJointCSVFile[i-1] = jointFileName;
    }

    //解析csv文件
    QList<QStringList> csvKeyParameter = parseCSVFile(strKeyParameterCSV);
    QList<QStringList> csvJ[6];
    csvJ[0] = parseCSVFile(servoParameterTemplatePath+arrJointCSVFile[0]);
    csvJ[1] = parseCSVFile(servoParameterTemplatePath+arrJointCSVFile[1]);
    csvJ[2] = parseCSVFile(servoParameterTemplatePath+arrJointCSVFile[2]);
    csvJ[3] = parseCSVFile(servoParameterTemplatePath+arrJointCSVFile[3]);
    csvJ[4] = parseCSVFile(servoParameterTemplatePath+arrJointCSVFile[4]);
    csvJ[5] = parseCSVFile(servoParameterTemplatePath+arrJointCSVFile[5]);

    QList<DobotType::StructServoParam> servoParamListJ[6];
    QHash<QString,int> dotDividedBitsHash;
    if (csvKeyParameter.size()<3) {
        qDebug()<<"文件缺少数据，无法对比伺服参数,"<<strKeyParameterCSV;
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",QString("%1:%2").arg(tr("文件缺少数据，无法对比伺服参数"),strKeyParameterCSV));
        pWndMsg->show();
        return false;
    }
    for (int i=2; i<csvKeyParameter.size();++i)
    {
        QString paramIndex = csvKeyParameter.at(i)[0];
        QString address = "addr"+paramIndex2Address(paramIndex);
        dotDividedBitsHash.insert(address,csvKeyParameter.at(i)[5].toInt());

        for (int j=0; j<6; ++j){
            DobotType::StructServoParam servoParam;
            servoParam.servoNum = QString("J%1").arg(j+1);
            servoParam.key = address;
            servoParamListJ[j].append(servoParam);
        }
    }
    const int iSingleWaitTime = CLuaApi::getInstance()->getServoParamReadTimeoutMs().value();
    for (int i=0; i<6; ++i)
    {
        QHash<QString,double> servoParamsCSVHash;
        if (csvJ[i].size()<3) {
            qDebug()<<"关节csv文件缺少数据，无法对比伺服参数";
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",QString("%1:%2").arg(tr("文件缺少数据，无法对比伺服参数")).arg(arrJointCSVFile[i]));
            pWndMsg->show();
            return false;
        }
        for (int row=2; row<csvJ[i].size();++row)
        {
            QString paramIndex = csvJ[i].at(row)[0];
            QString address = "addr"+paramIndex2Address(paramIndex);
            double value = csvJ[i].at(row)[3].split(":")[0].toDouble();
            servoParamsCSVHash.insert(address,value);
        }

        DobotType::StructSettingsServoParams settingsServoParams;
        settingsServoParams.src = "";
        settingsServoParams.servoParams = servoParamListJ[i];

        if (!m_httpProtocol->postSettingsReadServoParams(g_strCurrentIP,settingsServoParams))
        {//请求读伺服参数数据
            qDebug()<<" http post接口请求读伺服参数数据失败:J"<<(i+1);
            if (bShowTips){
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("请求读伺服参数数据失败"));
                pWndMsg->show();
            }
            return false;
        }

        {
            const int iTotalSleepTimes = servoParamListJ[i].size()*iSingleWaitTime;
            QElapsedTimer elapsed;
            elapsed.start();
            while(!elapsed.hasExpired(iTotalSleepTimes)){
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                if (m_upgradeVersion.isEmpty()){ //在检测过程中，掉线了，或者断开连接，则需要提前结束
                    qDebug()<<"读伺服参数数据失败，可能是因为断开了连接";
                    if (bShowTips){
                        auto* pWndMsg = new MessageWidget(this);
                        pWndMsg->setMessage("warn",tr("读伺服参数数据失败，可能是因为断开了连接"));
                        pWndMsg->show();
                    }
                    return false;
                }
            }
        }

        DobotType::StructSettingsServoParamsResult readServoParams  = m_httpProtocol->getSettingsReadServoParams(g_strCurrentIP);
        if (!readServoParams.status)
        {//读取伺服参数数据失败
            qDebug()<<" http get接口请求读伺服参数数据失败:J"<<(i+1);
            if (bShowTips){
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("获取伺服参数数据失败"));
                pWndMsg->show();
            }
            return false;
        }
        QList<DobotType::StructServoParam> servoParamList = readServoParams.servoParams;
        for(DobotType::StructServoParam servoParam: servoParamList)
        {
            auto dotV = dotDividedBitsHash.value(servoParam.key);
            auto csvV = servoParamsCSVHash.value(servoParam.key);
            int servoParamsCSVValue = dotDividedBitsValue(false,dotV,csvV)+0.0005;
            int intServoParamValue = servoParam.value+0.0005;
            qDebug()<<QString("read adddress=%1, asrcValue=%2, servoParamsCSVValue=%3 ").arg(servoParam.key).arg(servoParam.value).arg(csvV);
            if(intServoParamValue != servoParamsCSVValue)
            {//伺服参数不一样则需要升级
                qDebug()<<"  intServoParamValue  --- "<<intServoParamValue <<" servoParamsCSVValue ---   "<<servoParamsCSVValue;
                bNeedUpdate = true;
                //return true; 不需要返回，因为需要告诉大家到底是哪个关节伺服参数不一致。
                int jointId = i+1;
                if (!bJointNotEqual.contains(jointId)){
                    bJointNotEqual.append(jointId);
                }
                break;
            }
        }
    }
    qDebug()<<"伺服校验对比成功了.....";
    return true;
}

void MainWidget2::slotCheckServoParam()
{
    if (m_bIsCheckingServoParam)
    {
        WidgetToast::warn(tr("请等待伺服参数对比完成"), 2000);
        return ;
    }
    bool bNeedUpdate = false;
    QList<int> bJointNotEqual; //检测到不相等的伺服关节
    QString arrJointCSVFile[6];
    auto pLoading = new LoadingUI(this);
    pLoading->setGeometry(0,0,width(),height());
    pLoading->setText("<font color='red'>"+tr("对比过程中请勿断开机器或者关闭窗口")+"</font>");
    pLoading->setBigText(tr("伺服参数对比"));
    pLoading->show();

    ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant());
    qDebug()<<"手动点击伺服参数校验对比.......slotCheckServoParam";
    if (checkSveroParameters(bNeedUpdate, bJointNotEqual, arrJointCSVFile, true))
    {
        QString str;
        for(auto j : bJointNotEqual){
            str += QString::asprintf("J%d,",j);
        }
        if (!str.isEmpty()){//不为空说明存在关节伺服参数不一致的情况
            str = QString(tr("机器本体的%1伺服参数与升级包的不一致")).arg(str);
        }
        ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(bNeedUpdate),false,str);
        auto ptr = new FormServoTips(this);
        if (bNeedUpdate) {
            if (str.isEmpty())
                ptr->setText(tr("伺服参数非最新版本，请更新！"));
            else
                ptr->setText(str+"\r\n"+tr("伺服参数非最新版本，请更新！"));
        }
        else ptr->setText(tr("伺服参数为最新版本，无需更新！"));
        ptr->show();
    }
    else
    {
        if (bNeedUpdate){//这种情况，就是其中某几个关节检测到需要更新，另外几个关节检测失败了
            QString str;
            for(auto j : bJointNotEqual){
                str += QString::asprintf("J%d,",j);
            }
            str = QString(tr("机器本体的%1伺服参数与升级包的不一致")).arg(str);
            ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(),true,str);
        }else{
            ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(),true);
        }
    }
    pLoading->deleteLater();
}

bool MainWidget2::servoParamsAllWrite(QString arrJointCSVFile[6])
{
    qDebug()<<"伺服参数覆盖准备写入............";
    auto pServoUpgradeWidget = new FormServoWriteProgress(this);
    pServoUpgradeWidget->setUpgrading(0);
    pServoUpgradeWidget->setGeometry(0,0,width(),height());
    pServoUpgradeWidget->raise();
    pServoUpgradeWidget->show();

    const QString servoParameterTemplatePath = m_strServoParameterPath;

    //伺服说用通用的模板文件作为参考标准，而不是param.csv
    QString servoParameterTemplateFile = getServoTemplateFile();//servoParameterTemplatePath+"param.csv";
    QHash<QString,  //地址，例如addr200102
            QPair<int, //倍率系数，就是dotDivided
                QString //ReadOnly的字段内容 TRUE、FALSE
        >> dotDividedBitsHash;
    qDebug()<<"伺服参数覆盖准备写入 servoParameterTemplateFile   "<<servoParameterTemplateFile;
    if(!QFile::exists(servoParameterTemplateFile))
    {//没有找到模板表格
        qDebug()<<"没有找到模板表格:"<<servoParameterTemplateFile;
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",QString("%1:%2").arg(tr("没有找到模板表格")).arg(QDir::toNativeSeparators(servoParameterTemplateFile)));
        pWndMsg->show();
        pServoUpgradeWidget->close();
        return false;
    }
    QList<QStringList> csvParameter = parseCSVFile(servoParameterTemplateFile);
    for (int row = 2; row < csvParameter.size(); row++)
    {
        QStringList split = csvParameter.at(row);/*列数据*/
        if(split[0].contains("TRUE")||split[0].contains("FALSE"))
        {
            QString address = "addr"+paramIndex2Address(split[3]);
            int dotDivided = split[11].toInt();
            QString strReadOnly = split[12];
            qDebug()<<"ok++++++++++++++++++++++address="<<address<<",dotDivided="<<dotDivided<<",ReadOnly="<<strReadOnly;
            dotDividedBitsHash.insert(address,qMakePair(dotDivided,strReadOnly));
        }
    }
    if (dotDividedBitsHash.isEmpty()) {
        qDebug()<<"文件缺少数据，无法对比伺服参数,"<<servoParameterTemplateFile;
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",QString("%1:%2").arg(tr("文件缺少数据，无法对比伺服参数")).arg(QDir::toNativeSeparators(servoParameterTemplateFile)));
        pWndMsg->show();
        pServoUpgradeWidget->close();
        return false;
    }

    //读取伺服参数跳过文件，查看哪些参数可以跳过
    QSet<QString> allFilter;
    QFile file(QCoreApplication::applicationDirPath()+"/upgradeFiles/CRContinueParam.json");
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument jDoc = QJsonDocument::fromJson(file.readAll());
        file.close();
        QJsonObject obj = jDoc.object();
        QJsonValue value = obj.value(CommonData::getControllerType().name);
        QJsonArray arr = value.toArray();
        for(QJsonValue v : arr)
        {
            if (!v.toString().isEmpty()) allFilter.insert(v.toString());
        }
    }
    //读取伺服文件内容
    int iServoParamCounts = 0;
    const DobotType::ControllerType controllerType = CommonData::getControllerType();
    QList<DobotType::StructServoParam> servoParamsList[6];
    for (int i=0;i<6;++i)
    {
        const QString strCSVFile = arrJointCSVFile[i]; //QString("J%1.csv").arg(i+1);
        const QString servoParamPath = servoParameterTemplatePath+controllerType.name+"/"+strCSVFile;
        qDebug()<<"伺服参数覆盖准备写入,关节 csv file:"<<servoParamPath;
        if (!QFile::exists(servoParamPath))
        {//升级包中没有伺服文件
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("%1版本升级包中未找到%2的伺服参数文件%3，请重新下载该版本升级包")
                                .arg(CommonData::getUpgradeSettingsVersion().control)
                                .arg(controllerType.name).arg(strCSVFile));
            pWndMsg->show();
            pServoUpgradeWidget->close();
            return false;
        }
        const QList<QStringList> csvJoint = parseCSVFile(servoParamPath);
        if (csvJoint.size()<3){
            qDebug()<<"文件缺少数据，无法对比伺服参数,"<<servoParamPath;
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",QString("%1:%2").arg(tr("文件缺少数据，无法对比伺服参数"),strCSVFile));
            pWndMsg->show();
            pServoUpgradeWidget->close();
            return false;
        }
        QStringList csvJointHeaderList = csvJoint.at(1);
        if(csvJointHeaderList.size()>=5 && csvJointHeaderList[0] == "ParaIndStr")
        {
            for(int row = 2; row < csvJoint.size();row++)
            {
                QStringList servoParamsLines=csvJoint.at(row);/*行数据*/
                QString address = "addr"+paramIndex2Address(servoParamsLines[0]);
                if(allFilter.contains(address))
                {
                    continue;
                }
                int dotDividedTmp = 0;
                if (dotDividedBitsHash.contains(address)){
                    dotDividedTmp = dotDividedBitsHash.value(address).first;
                    if (dotDividedBitsHash.value(address).second == "TRUE"){ //伺服模板中对应地址的ReadOnly为TRUE则不做写入操作
                        continue;
                    }
                }else{
                    if(servoParamsLines[4] == "TRUE"){
                        continue;
                    }
                }
                DobotType::StructServoParam servoParam;
                servoParam.key = address;
                double srcValue = servoParamsLines[3].split(":")[0].toDouble();
                servoParam.value = dotDividedBitsValue(false,dotDividedTmp,srcValue);
                servoParam.servoNum = QString("J%1").arg(i+1);
                qDebug()<<QString("adddress=%1, asrcValue=%2, newValue=%3").arg(address).arg(srcValue).arg(servoParam.value);
                servoParamsList[i].append(servoParam);
            }
        }
        iServoParamCounts += servoParamsList[i].size();
    }
    const int iSingleWaitTime = CLuaApi::getInstance()->getServoParamWriteTimeoutMs().value();
    int iServoCurrentCounts = 0;
    QString errorParamsAddress;
    for(int i = 0; i < 6;i++)
    {
        qDebug()<<"伺服参数覆盖准备写入 ##servoParamsList.size="<<servoParamsList[i].size();
        if (servoParamsList[i].isEmpty()){
            qDebug()<<"伺服参数覆盖准备写入 ##servoParamsList data is empyt, do not update";
            continue;
        }
        while (servoParamsList[i].size()>0){
            DobotType::StructSettingsServoParams settingsServoParams;
            settingsServoParams.src = "httpClient";
            for (int m=0; m<servoParamsList[i].size() && m<20; ++m){//接口每次最大只能支持20条，所以分批次
                settingsServoParams.servoParams.append(servoParamsList[i].takeFirst());
            }
            bool isPostSuccess = m_httpProtocol->postSettingsModifyServoParams(g_strCurrentIP,settingsServoParams);
            if(!isPostSuccess)
            {
                qDebug()<<"伺服参数覆盖准备写入，发送请求写入数据过程失败servoParamsAllWrite-postSettingsModifyServoParams返回fail:J"<<i+1;
                pServoUpgradeWidget->showFail(errorParamsAddress);
                return false;
            }
            //请求后要等待这么长时间才能拿到数据
            //为了让进度条动态效果更好，将原来得sleep分解成以下这种方式。
            for (int itime=0; itime<settingsServoParams.servoParams.size();++itime)
            {
                QElapsedTimer elapsed;
                elapsed.start();
                while(!elapsed.hasExpired(iSingleWaitTime))
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                ++iServoCurrentCounts;
                pServoUpgradeWidget->setUpgrading(static_cast<int>(iServoCurrentCounts*100.0f/iServoParamCounts));
            }
            DobotType::StructSettingsServoParamsResult settingsServoParamsResult = m_httpProtocol->getSettingsModifyServoParams(g_strCurrentIP);
            if(!settingsServoParamsResult.status)
            {
                qDebug()<<"伺服参数覆盖准备写入，获取写入结果失败servoParamsAllWrite-getSettingsModifyServoParams return status=fail:J"<<i+1;
                pServoUpgradeWidget->showFail(errorParamsAddress);
                return false;
            }
            for(DobotType::StructServoParam servoParam : settingsServoParamsResult.servoParams)
            {
                if(!servoParam.status)
                {
                    errorParamsAddress.append(QString("J%1 : ").arg(i+1)+address2ParamIndex(servoParam.key)+"\n");
                }
            }
        }
    }
    if(errorParamsAddress.isEmpty())
    {
        pServoUpgradeWidget->showSuccess();
        return true;
    }
    else
    {
        pServoUpgradeWidget->showFail(errorParamsAddress);
        return false;
    }
}
