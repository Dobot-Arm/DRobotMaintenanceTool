#include "Define.h"
#include <QJsonDocument>
#include "MainWidget2.h"
#include "EnvDebugConfig.h"
#include "LuaApi.h"

QString g_controlModeDisable = "disable";
QString g_controlModeEnable = "enable";

QString g_strCurrentIP = "";
bool g_isEnglish = false;
bool g_isRestartServer = true;
QString g_strPropertiesCabinetTypeCCBOX = "CCBOX";
QString g_strPropertiesCabinetTypeCC162 = "CC162";
QString g_strPropertiesCabinetTypeCC262 = "CC262";
QString g_strPropertiesCabinetTypeCC26X = "CC26";
QString g_strPropertiesCabinetTypeMG400 = "MG400";
QString g_strPropertiesCabinetTypeMagicianE6 = "Magician E6";
QString g_strPropertiesCabinetTypeCC263 = "CC263";


QString g_strSpinServoParamsFailStyleSheet = "color: rgb(55, 71, 95);\nbackground-color: rgba(206,73,73,1);\nborder: 0.5px solid #C6C9D7;\nborder-radius: 4px;";
QString g_strSpinServoParamsSuccessStyleSheet = "color: rgb(55, 71, 95);\nborder: 0.5px solid #C6C9D7;\nborder-radius: 4px;";

QString g_strFontFailedStyleSheet = "border:none;\ncolor: rgba(206,73,73,1);";
QString g_strFontSuccessStyleSheet = "border:none;\ncolor: rgba(0,57,200,1);\n";

QString g_strServoParamsPassword = "Dobot123";

QString g_strServoParamsColValueTrue = "TRUE";
QString g_strServoParamsColValueFalse = "FALSE";

QString g_strSuccess = "success";

QString g_strAddr200118 = "addr200118";
QString g_strAddr200101 = "addr200101";
QString g_strAddr200102 = "addr200102";

//"http://cdn.debug.dobot.cc/dobotUpgradeToolFile";
QString g_strHttpDobotPrefixDebug = "http://cdn.release.dobot.cc/RobotMaintenanceToolDebug";
//"http://cdn.release.dobot.cc/dobotUpgradeToolFile";
QString g_strHttpDobotPrefixRealse = "http://cdn.release.dobot.cc/RobotMaintenanceToolRelease";
QString g_strAllVersion = "allVersion.ini";
QString g_strHttpDobotPrefix = g_strHttpDobotPrefixRealse;

int CompareVersion(const QString& strV1, const QString& strV2)
{
    if (strV1==strV2) return 0;

    QStringList v1 = strV1.split(".", QString::SkipEmptyParts, Qt::CaseInsensitive);
    QStringList v2 = strV2.split(".", QString::SkipEmptyParts, Qt::CaseInsensitive);
    std::vector<int> vInt1,vInt2;
    foreach(QString s, v1)
    {
        vInt1.push_back(s.toInt());
    }
    foreach(QString s, v2)
    {
        vInt2.push_back(s.toInt());
    }
    for(unsigned int i=0; i<vInt1.size()&&i<vInt2.size(); ++i)
    {
        if (vInt1[i]>vInt2[1]) return 1;
        else if (vInt1[i]<vInt2[1]) return -1;
    }
    return 0;
}
bool isCurrentFWLargerTargetFWVersion(QString currentFW, QString targertFW)
{
    QStringList currentFWs = currentFW.split('.');
    QStringList targertFWs = targertFW.split('.');
    for(int i = 0; i < currentFWs.size(); i++)
    {
        if(currentFWs[i].toInt()>targertFWs[i].toInt())
        {
            return true;
        }
        if(currentFWs[i].toInt()==targertFWs[i].toInt())
        {
            continue;
        }
        if(currentFWs[i].toInt()<targertFWs[i].toInt())
        {
            return false;
        }
    }
    return true;

}

bool isCurrentControlLagerTargetControl(QString currentControl, QString targetControl)
{
    if(currentControl.at(0)=='V'||currentControl.at(0)=='v')
    {
        currentControl = currentControl.mid(1);
    }
    QStringList currentControlSubVersion = currentControl.split('.');
    currentControlSubVersion[3] = currentControlSubVersion[3].mid(0,1);
    QStringList targertControlSubVersion = targetControl.split('.');
    for(int i = 0; i < targertControlSubVersion.size(); i++)
    {
        if(currentControlSubVersion[i].toInt()>targertControlSubVersion[i].toInt())
        {
            return true;
        }
        if(currentControlSubVersion[i].toInt()==targertControlSubVersion[i].toInt())
        {
            continue;
        }
        if(currentControlSubVersion[i].toInt()<targertControlSubVersion[i].toInt())
        {
            return false;
        }
    }
    return true;
}

bool g_bReStartApp = false;
static MainWidget2* g_pMainWnd = nullptr;
void PublicSetMainWindow(MainWidget2 * p)
{
    if (nullptr==g_pMainWnd)
    {
        g_pMainWnd = p;
    }
}
MainWidget2* PublicgGetMainWindow()
{
    if (!g_pMainWnd)
    {
        foreach(QWidget*w, qApp->topLevelWidgets())
        {
            MainWidget2* pTmp = qobject_cast<MainWidget2*>(w);
            if(pTmp)
            {
                g_pMainWnd = pTmp;
                return g_pMainWnd;
            }
        }
    }
    return g_pMainWnd;
}

void PublicSleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/*csv格式是以逗号分隔列的。
 * 每行内容是以换行符作为一行的。
 * 如果内容中包含换行符，则整个内容要以双引号括起来。
 * 如果内容中包含逗号，则整个内容要以双引号括起来。
 * 如果内容中又有双引号，则会是2个双引号连在一起表示一个双引号。
 * 每个字段的左右2边的空白字符可以忽略。
 * 还有其他复杂的情况，暂时不考虑
===================================================
 *针对以上csv的复杂多变的格式，我们这里只考虑以下几种情况吧：
 * 1. 逗号分隔列。
 * 2. 某个列的内容可能多行显示。
 * 3. 某个列可能存在逗号、双引号、换行。
*/
QList<QStringList> parseCSVFile(const QString &strFile)
{
    QList<QStringList> allRecordSets;
    QFile file(strFile);
    if (!file.open(QFile::ReadOnly)) return allRecordSets;
    QTextStream ins(&file);
    ins.setCodec(checkFileTextCode(&file));

    bool bInQuote = false;
    const QChar chQuote('\"');
    const QChar chEnter('\n');
    const QChar chComma(',');

    int iFileLineCount = 0;
    QStringList currentRecordset;
    QString strFieldData;
    while (!ins.atEnd())
    {
        ++iFileLineCount;
        const QString strLine = ins.readLine()+'\n'; //QTextStream::readLine()去掉了换行符
        for (int i=0; i<strLine.size(); ++i)
        {
            const QChar c = strLine.at(i);
            if (chComma == c)
            {
                if (bInQuote)
                {
                    strFieldData.append(c);
                }
                else
                {
                    strFieldData = strFieldData.trimmed(); //去掉左右两边的空白字符
                    currentRecordset.append(strFieldData);
                    strFieldData = "";
                }
            }
            else if (chEnter == c)
            {
                if (bInQuote)
                {
                    strFieldData.append(c);
                }
                else
                {
                    strFieldData = strFieldData.trimmed(); //去掉左右两边的空白字符
                    currentRecordset.append(strFieldData);
                    allRecordSets.append(currentRecordset);
                    strFieldData = "";
                    currentRecordset.clear();
                }
            }
            else if (chQuote == c)
            {
                //由于strLine的最后一个字符一定是'\n',所以strLine.at(i+1)肯定不会越界,所以无需判断越界。
                if (bInQuote)
                {//当前字符是双引号，且最开始已经有双引号，则当前字符可能是字段内容，也可能是字段结束位置
                    QChar next = strLine.at(i+1);
                    if (chQuote == next)
                    {//当前字符的下一个字符是双引号，说明是字段内容的一部分
                        strFieldData.append(c);
                        ++i;
                    }
                    else if (chEnter == next)
                    {//当前字符的下一个字符是换行，说明当前的一条记录解析完毕
                        bInQuote = false;
                    }
                    else if (chComma == next)
                    {//当前字符的下一个字符是逗号，说明当前字段解析完毕
                        bInQuote = false;
                    }
                    else if (next.isSpace())
                    {//双引号结束的情况下，如果当前字符的下一个字符是空白字符，则忽略
                        for(++i;i<strLine.size();)
                        {
                            next = strLine.at(i);
                            if (next.isSpace() && next!=chEnter)
                            {//是空白字符但不是换行
                                ++i;
                            }
                            else
                            {
                                --i; //便于下一个解析，所以指针回退。
                                break;
                            }
                        }
                        bInQuote = false;
                    }
                    else
                    {//说明格式有问题
                        qDebug().noquote().nospace()<<QString("the csv file(%1)(line=%2) has format error").arg(strFile).arg(iFileLineCount);
                        strFieldData.append(c);
                    }
                }
                else
                {//表示字段的开始
                    bInQuote = true;
                }
            }
            else
            {
                strFieldData.append(c);
            }
        }
    }
    return allRecordSets;
}

//检查文件的编码格式
const char* checkFileTextCode(QFile* pFile)
{
    const char* strCodeName = "GB2312";
    qint64 pos = pFile->pos();
    QByteArray arr = pFile->readAll();
    QTextCodec::ConverterState state;
    QTextCodec* pCode = QTextCodec::codecForName(strCodeName);
    pCode->toUnicode(arr.constData(), arr.size(), &state);
    if (state.invalidChars>0)
    {//无效字符个数大于0说明可能不是gb2312编码
        strCodeName = "UTF-8";
    }
    pFile->seek(pos);
    return strCodeName;
}

QJsonObject ControllerSnData::toJsonObject() const
{
    QJsonObject obj;
    obj["ControllCabinetSNCode"] = ControllCabinetSNCode;
    return obj;
}

QString ControllerSnData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void ControllerSnData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void ControllerSnData::fromJson(const QJsonObject &obj)
{
    ControllCabinetSNCode = obj.value("ControllCabinetSNCode").toString();
}
QJsonObject V3ControllerSnData::toJsonObject() const
{
    QJsonObject obj;
    obj["SNcode"] = SNcode;
    return obj;
}

QString V3ControllerSnData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void V3ControllerSnData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void V3ControllerSnData::fromJson(const QJsonObject &obj)
{
    SNcode = obj.value("SNcode").toString();
}

QJsonObject RobotArmSnData::toJsonObject() const
{
    QJsonObject obj;
    obj["RobotArmSNCode"] = RobotArmSNCode;
    return obj;
}

QString RobotArmSnData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void RobotArmSnData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void RobotArmSnData::fromJson(const QJsonObject &obj)
{
    RobotArmSNCode = obj.value("RobotArmSNCode").toString();
}

QJsonObject FaultCheckData::toJsonObject() const
{
    QJsonObject obj;
    obj["operationType"] = operationType;
    obj["control"] = control;
    return obj;
}

QString FaultCheckData::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void FaultCheckData::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void FaultCheckData::fromJson(const QJsonObject &obj)
{
    operationType = obj.value("operationType").toString();
    control = obj.value("control").toString();
}

QJsonObject FaultCheckResult::toJsonObject() const
{
    QJsonObject obj;
    obj["result"] = result;
    QJsonArray arr;
    for(QString file:missingFiles)
    {
        arr.append(file);
    }
    obj["missingFiles"] = arr;
    return obj;
}

QString FaultCheckResult::toJsonString() const
{
    QJsonDocument doc(toJsonObject());
    return doc.toJson();
}

void FaultCheckResult::fromJson(const QString &strJson)
{
    QJsonDocument doc = QJsonDocument::fromJson(strJson.toUtf8());
    fromJson(doc.object());
}

void FaultCheckResult::fromJson(const QJsonObject &obj)
{
    result = obj.value("result").toString();
    for(QJsonValue jsonValue:obj.value("missingFiles").toArray())
    {
        missingFiles.append(jsonValue.toString());
    }
}

QString getUpdateConfigJsonFile()
{
    QString str = QCoreApplication::applicationDirPath()+"/config/";
    if (EnvDebugConfig::isDebugEnv())
    {
        str += "debugtmp/";
    }
    else
    {
        str += "releasetmp/";
    }
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    str += "updateconfig.json";
    return str;
}

QString getUpdateConfigJsonFileUrl()
{
    return g_strHttpDobotPrefix+"/updateconfig.json";
}

QString getUpdateFirmwareLuaFile()
{
    QString str = QCoreApplication::applicationDirPath()+"/config/";
    if (EnvDebugConfig::isDebugEnv())
    {
        str += "debugtmp/";
    }
    else
    {
        str += "releasetmp/";
    }
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    str += "updatefirmware.lua";
    return str;
}

QString getStartServerZipFile()
{
    QString str = QCoreApplication::applicationDirPath()+"/tool/";
    if (EnvDebugConfig::isDebugEnv())
    {
        str += "debugtmp/";
    }
    else
    {
        str += "releasetmp/";
    }
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    str += "startServer.zip";
    return str;
}

QString getStartServerZipFileDir()
{
    QString str = QCoreApplication::applicationDirPath()+"/tool/";
    if (EnvDebugConfig::isDebugEnv())
    {
        str += "debugtmp/";
    }
    else
    {
        str += "releasetmp/";
    }
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    return str;
}

QString getServoParamTemplateDir()
{
    QString str = QCoreApplication::applicationDirPath()+"/upgradeFiles/servoParamTemplate/";
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    return str;
}

QString getServoTemplateJointFileZIP()
{
    QString str = getServoParamTemplateDir();
    str += "ServoTemplateJointConfig.zip";
    return str;
}

QString getServoTemplateJointFileUrl()
{
    return g_strHttpDobotPrefixRealse+"/ServoTemplateJointConfig.zip";
}

static QString getServoJointTableFileDir()
{
    QString str = getServoParamTemplateDir();
    str += "ServoJointConfig/";
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    return str;
}

QString getServoJointTableFile()
{
    QString str = getServoJointTableFileDir();
    auto strtmplateCsv = CLuaApi::getInstance()->getServoJointTableFile();
    if (strtmplateCsv.hasValue())
    {
        str.append(strtmplateCsv.value());
    }
    else
    {
        const QString strName = CommonData::getControllerType().name;
        if (strName.contains("CR"))
        {
            if(strName.contains("V") && !strName.contains("V2")) str += "CRV_prefix.csv";
            else if(strName.contains("A-IS")) str += "CRA-IS_prefix.csv";
            else str += "CR_prefix.csv";
        }
        else if(strName.contains("Nova"))
        {
            str += "Nova_prefix.csv";
        }
        else if(strName.contains("E6"))
        {
            str += "E6_prefix.csv";
        }
    }
    qDebug()<<"getServoJointTableFile is:"<<str;
    return str;
}

QString getKeyParameterCSVFile()
{
    QString str = QCoreApplication::applicationDirPath()+"/upgradeFiles/KeyParameter.csv";
    return str;
}

QString getServoTemplateFile()
{
    QString str = getServoParamTemplateDir();
    auto strtmplateCsv = CLuaApi::getInstance()->getServoParamTemplateFile();
    if (strtmplateCsv.hasValue())
    {
        str.append(strtmplateCsv.value());
    }
    else
    {
        const QString strName = CommonData::getControllerType().name;
        if (strName.contains("CR"))
        {
            if(strName.contains("V") && !strName.contains("V2")) str += "CRV_Param.csv";
            else if(strName.contains("A-IS")) str += "CRA-IS_Param.csv";
            else str += "CR_Param.csv";
        }
        else if(strName.contains("Nova"))
        {
            str += "Nova_Param.csv";
        }
        else if(strName.contains("E6"))
        {
            str += "E6_Param.csv";
        }
    }
    qDebug()<<"getServoTemplateFile is:"<<str;
    return str;
}

QString getServoCSVSerialTypeName()
{
    QString strFolderName = "";
    auto servoFoldName = CLuaApi::getInstance()->getServoUpgradeFolderName();
    if (servoFoldName.hasValue())
    {
        strFolderName = servoFoldName.value();
    }
    else
    {
        if(CommonData::getControllerType().name.contains("CR"))
        {
            const QString strName = CommonData::getControllerType().name;
            if(strName.contains("A") && !strName.contains("A-IS")) strFolderName += "CRA";
            else if(strName.contains("A") && strName.contains("A-IS")) strFolderName += "CRAIS";
            else if(strName.contains("V") && !strName.contains("V2")) strFolderName += "CRV";
            else if(strName.contains("V2")) strFolderName += "CRV2";
            else strFolderName += "CR";
        }
        else if(CommonData::getControllerType().name.contains("Nova"))
        {
            strFolderName += "Nova";
        }
        else if(CommonData::getControllerType().name.contains("E6"))
        {
            strFolderName += "E6";
        }
    }
    return strFolderName;
}

CRUpdateInfo getMatchCRUpdateInfo(const QString strCRUpdateName)
{
    CRUpdateInfo info;
    info.valid = false;
    info.index = -1;
    info.length = 0;

    QString str1 = "CR_update_";
    QString str2 = "CR_x86_64_update_";
    int idx = strCRUpdateName.indexOf(str1);
    if (idx >= 0)
    {
        info.valid = true;
        info.index = idx;
        info.length = str1.length();
        return info;
    }
    idx = strCRUpdateName.indexOf(str2);
    if (idx >= 0)
    {
        info.valid = true;
        info.index = idx;
        info.length = str2.length();
        return info;
    }
    return info;
}

QString getControllerScriptDir()
{
    QString str = QCoreApplication::applicationDirPath()+"/tool/robotfile/";
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    return str;
}
QString getControllerScriptFileZIP()
{
    QString str = getControllerScriptDir();
    str += "ControllerScriptFile.zip";
    return str;
}
QString getControllerScriptFileUrl()
{
    return g_strHttpDobotPrefixRealse+"/ControllerScriptFile.zip";
}

QString getFactoryRecoveryDir()
{
    QString str = QCoreApplication::applicationDirPath()+"/tool/tmpfiles/";
    if (!QFileInfo(str).isDir()) {
        QDir dir;
        dir.mkpath(str);
    }
    return str;
}
QString getFactoryRecoveryFile()
{
    QString str = getFactoryRecoveryDir();
    str += "tmpfactoryrecovery.json";
    return str;
}
