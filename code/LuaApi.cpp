#include "LuaApi.h"
#include "lua/lua.hpp"
#include "Define.h"
#include <QtConcurrent>
#include <atomic>
#include <unordered_map>
#include <QMutex>
#include <QMutexLocker>
#include <QEventLoop>

//在LuaRunner.cpp中
extern int luaRunner(int argc, const char **argv, lua_State*& pL);
extern void luaStopper(lua_State*& pL);
/************************************************************************************************
 * ************************************************************************************************
**************************************************************************************************/
struct CLuaApiPrivate
{
    CLuaApi* ptr{};
    lua_State* pLuaEnv{nullptr};
    std::string strLuaFile{};
    std::string strExec{};
    bool bIsRun{false};

    bool run()
    {
        if (!QFile::exists(QString::fromStdString(strLuaFile))) return false;
        if (bIsRun) return true;
        bIsRun = true;
        int argc = 2;
        const char* pszExec = strExec.c_str();
        const char* pszFile = strLuaFile.c_str();
        const char* argv[]={pszExec,pszFile};
        luaRunner(argc, argv, pLuaEnv);
        return true;
    }
    void stop()
    {
        luaStopper(pLuaEnv);
        pLuaEnv = nullptr;
        bIsRun = false;
    }
};

CLuaApi::CLuaApi(QObject* p):QObject(p)
{
    m_ptr = new CLuaApiPrivate;
    m_ptr->ptr = this;
    m_ptr->strExec = qApp->applicationFilePath().toStdString();
    m_ptr->strLuaFile = QDir::toNativeSeparators(getUpdateFirmwareLuaFile()).toStdString();
}

CLuaApi::~CLuaApi()
{
    delete m_ptr;
}

CLuaApi *CLuaApi::getInstance()
{
    static CLuaApi obj;
    return &obj;
}

void CLuaApi::startLua()
{
    m_ptr->run();
}

void CLuaApi::stopLua()
{
    m_ptr->stop();
}

bool CLuaApi::isLuaRun()
{
    return m_ptr->bIsRun;
}

QString CLuaApi::getVersion()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getVersion");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return QString(lua_tostring(m_ptr->pLuaEnv, -1));
        }
    }
    return "";
}

COptional<bool> CLuaApi::isShowSearchMenuItems(const QString &strGroupVersion, const QFileInfoList& fileInfos)
{
    if (!m_ptr->pLuaEnv) return {};

    std::string strV = strGroupVersion.toStdString();
    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isShowSearchMenuItems");
    lua_pushstring(m_ptr->pLuaEnv, strV.c_str());

    char** pData = new char*[fileInfos.size()];

    lua_newtable(m_ptr->pLuaEnv); //栈顶创建table
    for (int i=0; i<fileInfos.size(); ++i)
    {
        auto fi = fileInfos[i];
        QByteArray strFile = fi.fileName().toUtf8();
        pData[i] = new char[strFile.size()+1];
        memcpy(pData[i], strFile.data(),strFile.size()+1);

        lua_newtable(m_ptr->pLuaEnv); // 创建内层table
        lua_pushstring(m_ptr->pLuaEnv, "name");
        lua_pushstring(m_ptr->pLuaEnv, pData[i]);
        lua_settable(m_ptr->pLuaEnv, -3);
        lua_pushstring(m_ptr->pLuaEnv, "isfile");
        lua_pushboolean(m_ptr->pLuaEnv, fi.isFile());
        lua_settable(m_ptr->pLuaEnv, -3);

        lua_seti(m_ptr->pLuaEnv, -2, i+1);
    }

    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 2, 1, 0))
    {
        if (lua_isboolean(m_ptr->pLuaEnv, -1))
        {
            for (int i=0; i<fileInfos.size(); ++i)
            {
                delete[] pData[i];
            }
            delete[] pData;
            return {lua_toboolean(m_ptr->pLuaEnv, -1)?true:false};
        }
    }
    for (int i=0; i<fileInfos.size(); ++i)
    {
        delete[] pData[i];
    }
    delete[] pData;
    return {};
}

COptional<bool> CLuaApi::isShowSearchSubMenuItems(const QString &strPackageVersion)
{
    if (!m_ptr->pLuaEnv) return {};

    std::string strV = strPackageVersion.toStdString();
    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isShowSearchSubMenuItems");
    lua_pushstring(m_ptr->pLuaEnv, strV.c_str());
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 1, 1, 0))
    {
        if (lua_isboolean(m_ptr->pLuaEnv, -1))
        {
            return {lua_toboolean(m_ptr->pLuaEnv, -1)?true:false};
        }
    }
    return {};
}

COptional<QStringList> CLuaApi::getSingleUpdrageFiles(const QString &strPackageVersion,
                                                      const QStringList &lstFile,
                                                      const QString &strUpgradeType,
                                                      const QHash<QString,QHash<QString,QString>>& allLuaVal)
{
    if (!m_ptr->pLuaEnv) return {};

    std::vector<std::string> allFile;
    allFile.reserve(lstFile.size());
    for (int i=0; i<lstFile.size(); ++i)
    {
        allFile.push_back(lstFile.at(i).toStdString());
    }

    std::unordered_map<std::string,std::unordered_map<std::string,std::string> > allIni;
    for (auto itr=allLuaVal.begin(); itr!=allLuaVal.end(); ++itr)
    {
        auto strGN = itr.key().toStdString();
        auto& allv = itr.value();
        std::unordered_map<std::string,std::string> tmpcpp;
        for (auto i=allv.begin(); i!=allv.end(); ++i){
            tmpcpp.insert({i.key().toStdString(), i.value().toStdString()});
        }
        allIni.insert({strGN, tmpcpp});
    }

    std::string strV = strPackageVersion.toStdString();
    std::string strType = strUpgradeType.toStdString();
    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getSingleUpdrageFiles");
    lua_pushstring(m_ptr->pLuaEnv, strV.c_str());
    lua_newtable(m_ptr->pLuaEnv); //栈顶创建table
    for (size_t i=0; i<allFile.size(); ++i)
    {
        lua_pushstring(m_ptr->pLuaEnv, allFile.at(i).c_str());
        lua_seti(m_ptr->pLuaEnv, -2, i+1);
    }
    lua_pushstring(m_ptr->pLuaEnv, strType.c_str());

    lua_newtable(m_ptr->pLuaEnv); //栈顶创建table
    for (auto itrK=allIni.begin(); itrK!=allIni.end(); ++itrK){
        std::string strGN = itrK->first;
        auto& mapKV = itrK->second;
        lua_pushstring(m_ptr->pLuaEnv, strGN.c_str());
        lua_newtable(m_ptr->pLuaEnv);
        for (auto itrV=mapKV.begin(); itrV!=mapKV.end(); ++itrV){
            lua_pushstring(m_ptr->pLuaEnv, itrV->first.c_str());
            lua_pushstring(m_ptr->pLuaEnv, itrV->second.c_str());
            lua_settable(m_ptr->pLuaEnv, -3);
        }
        lua_settable(m_ptr->pLuaEnv, -3);
    }

    QStringList retFiles;
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 4, LUA_MULTRET, 0))
    {
        int nCount = lua_gettop(m_ptr->pLuaEnv);
        for (int i=0; i<nCount; ++i)
        {
            if (lua_isstring(m_ptr->pLuaEnv, -1))
            {
                retFiles.append(QString(lua_tostring(m_ptr->pLuaEnv, -1)));
            }
            lua_pop(m_ptr->pLuaEnv, 1);
        }
    }
    if (!retFiles.isEmpty()) return {retFiles};
    return {};
}

COptional<bool> CLuaApi::isCanUseTool(QString& strReasonMsg)
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isCanUseTool");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 2, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            strReasonMsg = lua_tostring(m_ptr->pLuaEnv, -1);
        }
        if (lua_isboolean(m_ptr->pLuaEnv, -2))
        {
            return {lua_toboolean(m_ptr->pLuaEnv, -2)?true:false};
        }
    }
    return {};
}

COptional<bool> CLuaApi::isCanUpgrade(QString& strReasonMsg)
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isCanUpgrade");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 2, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            strReasonMsg = lua_tostring(m_ptr->pLuaEnv, -1);
        }
        if (lua_isboolean(m_ptr->pLuaEnv, -2))
        {
            return {lua_toboolean(m_ptr->pLuaEnv, -2)?true:false};
        }
    }
    return {};
}

COptional<bool> CLuaApi::isFileNeedCopyToController(const QString &strFileName)
{
    if (!m_ptr->pLuaEnv) return {};

    std::string strV = strFileName.toStdString();
    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isFileNeedCopyToController");
    lua_pushstring(m_ptr->pLuaEnv, strV.c_str());
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 1, 1, 0))
    {
        if (lua_isboolean(m_ptr->pLuaEnv, -1))
        {
            return {lua_toboolean(m_ptr->pLuaEnv, -1)?true:false};
        }
    }
    return {};
}

COptional<QStringList> CLuaApi::getControllerUpdateFiles()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getControllerUpdateFiles");

    QStringList retFiles;
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, LUA_MULTRET, 0))
    {
        int nCount = lua_gettop(m_ptr->pLuaEnv);
        for (int i=0; i<nCount; ++i)
        {
            if (lua_isstring(m_ptr->pLuaEnv, -1))
            {
                retFiles.append(QString(lua_tostring(m_ptr->pLuaEnv, -1)));
            }
            lua_pop(m_ptr->pLuaEnv, 1);
        }
    }
    if (!retFiles.isEmpty()) return {retFiles};
    return {};
}

COptional<QPair<bool,QString>> CLuaApi::isOnekeyServoFuncCanUse()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isOnekeyServoFuncCanUse");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 2, 0))
    {
        int nCount = lua_gettop(m_ptr->pLuaEnv);
        if (1==nCount)
        {
            if (lua_isboolean(m_ptr->pLuaEnv, -1))
            {
                return {QPair<bool,QString>(lua_toboolean(m_ptr->pLuaEnv, -1)?true:false,QString())};
            }
        }
        else if (2==nCount)
        {
            bool b = false;
            QString str;
            if (lua_isboolean(m_ptr->pLuaEnv, -2) &&
                lua_isstring(m_ptr->pLuaEnv, -1))
            {
                b = lua_toboolean(m_ptr->pLuaEnv, -2)?true:false;
                str = lua_tostring(m_ptr->pLuaEnv, -1);
                return {QPair<bool,QString>(b,str)};
            }
        }
    }
    return {};
}

COptional<QString> CLuaApi::getServoUpgradeFolderName()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getServoUpgradeFolderName");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return {QString(lua_tostring(m_ptr->pLuaEnv, -1))};
        }
    }
    return {};
}

COptional<QString> CLuaApi::getServoParamTemplateFile()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getServoParamTemplateFile");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return {QString(lua_tostring(m_ptr->pLuaEnv, -1))};
        }
    }
    return {};
}

COptional<QString> CLuaApi::getServoJointTableFile()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getServoJointTableFile");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return {QString(lua_tostring(m_ptr->pLuaEnv, -1))};
        }
    }
    return {};
}

COptional<bool> CLuaApi::isSNCodeCanUse(QString& strReasonMsg)
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isSNCodeCanUse");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 2, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            strReasonMsg = lua_tostring(m_ptr->pLuaEnv, -1);
        }
        if (lua_isboolean(m_ptr->pLuaEnv, -2))
        {
            return {lua_toboolean(m_ptr->pLuaEnv, -2)?true:false};
        }
    }
    return {};
}

COptional<long long> CLuaApi::getSNCodeWriteType()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getSNCodeWriteType");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isinteger(m_ptr->pLuaEnv, -1))
        {
            return {lua_tointeger(m_ptr->pLuaEnv, -1)};
        }
    }
    return {};
}

COptional<QString> CLuaApi::getCabVersion()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getCabVersion");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return {QString(lua_tostring(m_ptr->pLuaEnv, -1))};
        }
    }
    return {};
}

COptional<QString> CLuaApi::getCabType()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getCabType");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return {QString(lua_tostring(m_ptr->pLuaEnv, -1))};
        }
    }
    return {};
}

COptional<QString> CLuaApi::getCabVersionKernel()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getCabVersionKernel");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return {QString(lua_tostring(m_ptr->pLuaEnv, -1))};
        }
    }
    return {};
}

COptional<QHash<QString,QString>> CLuaApi::getUpgradeIniVersion(const QHash<QString,QHash<QString,QString>>& allLuaVal)
{
    if (!m_ptr->pLuaEnv) return {};

    std::unordered_map<std::string,std::unordered_map<std::string,std::string> > allIni;
    for (auto itr=allLuaVal.begin(); itr!=allLuaVal.end(); ++itr)
    {
        auto strGN = itr.key().toStdString();
        auto& allv = itr.value();
        std::unordered_map<std::string,std::string> tmpcpp;
        for (auto i=allv.begin(); i!=allv.end(); ++i){
            tmpcpp.insert({i.key().toStdString(), i.value().toStdString()});
        }
        allIni.insert({strGN, tmpcpp});
    }

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getUpgradeIniVersion");
    lua_newtable(m_ptr->pLuaEnv); //栈顶创建table
    for (auto itrK=allIni.begin(); itrK!=allIni.end(); ++itrK){
        std::string strGN = itrK->first;
        auto& mapKV = itrK->second;
        lua_pushstring(m_ptr->pLuaEnv, strGN.c_str());
        lua_newtable(m_ptr->pLuaEnv);
        for (auto itrV=mapKV.begin(); itrV!=mapKV.end(); ++itrV){
            lua_pushstring(m_ptr->pLuaEnv, itrV->first.c_str());
            lua_pushstring(m_ptr->pLuaEnv, itrV->second.c_str());
            lua_settable(m_ptr->pLuaEnv, -3);
        }
        lua_settable(m_ptr->pLuaEnv, -3);
    }

    QHash<QString,QString> retFiles;
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 1, 1, 0))
    {
        if (lua_istable(m_ptr->pLuaEnv, -1))
        {
            lua_pushnil(m_ptr->pLuaEnv);
            while (lua_next(m_ptr->pLuaEnv, -2)!=0){
                if (lua_isstring(m_ptr->pLuaEnv, -1)){
                    QString strK = lua_tostring(m_ptr->pLuaEnv, -2);
                    QString strV = lua_tostring(m_ptr->pLuaEnv, -1);
                    retFiles.insert(strK,strV);
                }
                lua_pop(m_ptr->pLuaEnv, 1);
            }
            return {retFiles};
        }
    }
    return {};
}

COptional<int> CLuaApi::getServoParamReadTimeoutMs()
{
    int iVal = 400;
    if (!m_ptr->pLuaEnv) return {iVal};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getServoParamReadTimeoutMs");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isinteger(m_ptr->pLuaEnv, -1))
        {
            iVal = lua_tointeger(m_ptr->pLuaEnv, -1);
        }
    }
    return {iVal};
}

COptional<int> CLuaApi::getServoParamWriteTimeoutMs()
{
    int iVal = 400;
    if (!m_ptr->pLuaEnv) return {iVal};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getServoParamWriteTimeoutMs");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isinteger(m_ptr->pLuaEnv, -1))
        {
            iVal = lua_tointeger(m_ptr->pLuaEnv, -1);
        }
    }
    return {iVal};
}

COptional<bool> CLuaApi::isCanImportPackage(const QString& strPackageVersion, const QHash<QString,QHash<QString,QString>>& allLuaVal)
{
    if (!m_ptr->pLuaEnv) return {};

    std::string strV = strPackageVersion.toStdString();
    std::unordered_map<std::string,std::unordered_map<std::string,std::string> > allIni;
    for (auto itr=allLuaVal.begin(); itr!=allLuaVal.end(); ++itr)
    {
        auto strGN = itr.key().toStdString();
        auto& allv = itr.value();
        std::unordered_map<std::string,std::string> tmpcpp;
        for (auto i=allv.begin(); i!=allv.end(); ++i){
            tmpcpp.insert({i.key().toStdString(), i.value().toStdString()});
        }
        allIni.insert({strGN, tmpcpp});
    }

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isCanImportPackage");
    lua_pushstring(m_ptr->pLuaEnv, strV.c_str());
    lua_newtable(m_ptr->pLuaEnv); //栈顶创建table
    for (auto itrK=allIni.begin(); itrK!=allIni.end(); ++itrK){
        std::string strGN = itrK->first;
        auto& mapKV = itrK->second;
        lua_pushstring(m_ptr->pLuaEnv, strGN.c_str());
        lua_newtable(m_ptr->pLuaEnv);
        for (auto itrV=mapKV.begin(); itrV!=mapKV.end(); ++itrV){
            lua_pushstring(m_ptr->pLuaEnv, itrV->first.c_str());
            lua_pushstring(m_ptr->pLuaEnv, itrV->second.c_str());
            lua_settable(m_ptr->pLuaEnv, -3);
        }
        lua_settable(m_ptr->pLuaEnv, -3);
    }

    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 2, 1, 0))
    {
        if (lua_isboolean(m_ptr->pLuaEnv, -1))
        {
            return {lua_toboolean(m_ptr->pLuaEnv, -1)?true:false};
        }
    }
    return {};
}

COptional<QHash<QString,bool>> CLuaApi::getShowFirmAdvanceUI()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getShowFirmAdvanceUI");

    QHash<QString,bool> retFiles;
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_istable(m_ptr->pLuaEnv, -1))
        {
            lua_pushnil(m_ptr->pLuaEnv);
            while (lua_next(m_ptr->pLuaEnv, -2)!=0){
                if (lua_isboolean(m_ptr->pLuaEnv, -1)){
                    QString strK = lua_tostring(m_ptr->pLuaEnv, -2);
                    bool strV = !!lua_toboolean(m_ptr->pLuaEnv, -1);
                    retFiles.insert(strK,strV);
                }
                lua_pop(m_ptr->pLuaEnv, 1);
            }
            return {retFiles};
        }
    }
    return {};
}

COptional<QPair<bool,QString>> CLuaApi::isSingleUpdrageFilesValid(const QString& strUpgradeType, QStringList allUpdateFile)
{
    if (!m_ptr->pLuaEnv) return {};

    std::string strType = strUpgradeType.toStdString();
    std::vector<std::string> allstr;
    if (allUpdateFile.size()==0){
        allstr = {"",""};
    }else if (allUpdateFile.size()==1){
        allstr = {allUpdateFile[0].toStdString(), ""};
    }else{
        allstr.reserve(allUpdateFile.size());
        for (int i=0; i<allUpdateFile.size(); ++i){
            allstr.insert(allstr.end(), allUpdateFile.at(i).toStdString());
        }
    }

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "isSingleUpdrageFilesValid");
    lua_pushstring(m_ptr->pLuaEnv, strType.c_str());
    for (int i=0; i<allstr.size(); ++i){
        lua_pushstring(m_ptr->pLuaEnv, allstr[i].c_str());
    }
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 1+allstr.size(), LUA_MULTRET, 0))
    {
        int nCount = lua_gettop(m_ptr->pLuaEnv);
        if (1==nCount)
        {
            if (lua_isboolean(m_ptr->pLuaEnv, -1))
            {
                return {QPair<bool,QString>(lua_toboolean(m_ptr->pLuaEnv, -1)?true:false,QString())};
            }
        }
        else if (2==nCount)
        {
            bool b = false;
            QString str;
            if (lua_isboolean(m_ptr->pLuaEnv, -2) &&
                lua_isstring(m_ptr->pLuaEnv, -1))
            {
                b = lua_toboolean(m_ptr->pLuaEnv, -2)?true:false;
                str = lua_tostring(m_ptr->pLuaEnv, -1);
                return {QPair<bool,QString>(b,str)};
            }
        }
    }
    return {};
}

COptional<int> CLuaApi::getSingleUpdrageImportCount(const QString& strUpgradeType)
{
    if (!m_ptr->pLuaEnv) return {};

    std::string strType = strUpgradeType.toStdString();
    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getSingleUpdrageImportCount");
    lua_pushstring(m_ptr->pLuaEnv, strType.c_str());

    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 1, 1, 0))
    {
        if (lua_isinteger(m_ptr->pLuaEnv, -1))
        {
            int iVal = lua_tointeger(m_ptr->pLuaEnv, -1);
            if (iVal > 0) return {iVal};
        }
    }
    return {};
}

QString CLuaApi::getRobotArchitecture()
{
    if (!m_ptr->pLuaEnv) return "";

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getRobotArchitecture");
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_isstring(m_ptr->pLuaEnv, -1))
        {
            return QString(lua_tostring(m_ptr->pLuaEnv, -1));
        }
    }
    return "";
}

COptional<QHash<QString,int>> CLuaApi::getUpdateSequerence()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getUpdateSequerence");

    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_istable(m_ptr->pLuaEnv, -1))
        {
            QHash<QString,int> retFiles;
            lua_pushnil(m_ptr->pLuaEnv);
            while (lua_next(m_ptr->pLuaEnv, -2)!=0){
                if (lua_isnumber(m_ptr->pLuaEnv, -1)){
                    QString strK = lua_tostring(m_ptr->pLuaEnv, -2);
                    int strV = lua_tointeger(m_ptr->pLuaEnv, -1);
                    retFiles.insert(strK,strV);
                }
                lua_pop(m_ptr->pLuaEnv, 1);
            }
            static QStringList all = {"unio","control","feedback","safeio","servo","terminal"};
            if (all.size() != retFiles.size()) return {};
            foreach(QString str, all)
            {
                if(!retFiles.contains(str)){
                    return {};
                }
            }
            return {retFiles};
        }
    }
    return {};
}

COptional<QHash<QString,bool>> CLuaApi::getShowUpdateProcessUI()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getShowUpdateProcessUI");

    QHash<QString,bool> retFiles;
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_istable(m_ptr->pLuaEnv, -1))
        {
            lua_pushnil(m_ptr->pLuaEnv);
            while (lua_next(m_ptr->pLuaEnv, -2)!=0){
                if (lua_isboolean(m_ptr->pLuaEnv, -1)){
                    QString strK = lua_tostring(m_ptr->pLuaEnv, -2);
                    bool strV = !!lua_toboolean(m_ptr->pLuaEnv, -1);
                    retFiles.insert(strK,strV);
                }
                lua_pop(m_ptr->pLuaEnv, 1);
            }
            return {retFiles};
        }
    }
    return {};
}

COptional<QHash<QString,int>> CLuaApi::getHardwareSlaveId()
{
    if (!m_ptr->pLuaEnv) return {};

    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "getHardwareSlaveId");

    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 0, 1, 0))
    {
        if (lua_istable(m_ptr->pLuaEnv, -1))
        {
            QHash<QString,int> retFiles;
            lua_pushnil(m_ptr->pLuaEnv);
            while (lua_next(m_ptr->pLuaEnv, -2)!=0){
                if (lua_isinteger(m_ptr->pLuaEnv, -1)){
                    QString strK = lua_tostring(m_ptr->pLuaEnv, -2);
                    int strV = lua_tointeger(m_ptr->pLuaEnv, -1);
                    retFiles.insert(strK,strV);
                }
                lua_pop(m_ptr->pLuaEnv, 1);
            }
            static QStringList all = {"control","feedback","safeio","ccboxio","unio","j1","j2","j3","j4","j5","j6","terminal"};
            if (all.size() != retFiles.size()) return {};
            foreach(QString str, all)
            {
                if(!retFiles.contains(str)){
                    return {};
                }
            }
            return {retFiles};
        }
    }
    return {};
}

QPair<bool,QString> CLuaApi::execRecoveryFactory(const QString& strFile)
{
    if (!m_ptr->pLuaEnv) return {false,""};

    std::string strV = strFile.toStdString();
    lua_settop(m_ptr->pLuaEnv, 0);
    lua_getglobal(m_ptr->pLuaEnv, "execRecoveryFactory");
    lua_pushstring(m_ptr->pLuaEnv, strV.c_str());
    if (LUA_OK == lua_pcall(m_ptr->pLuaEnv, 1, LUA_MULTRET, 0))
    {
        int nCount = lua_gettop(m_ptr->pLuaEnv);
        if (1==nCount)
        {
            if (lua_isboolean(m_ptr->pLuaEnv, -1))
            {
                return {QPair<bool,QString>(lua_toboolean(m_ptr->pLuaEnv, -1)?true:false,QString())};
            }
        }
        else if (2==nCount)
        {
            bool b = false;
            QString str;
            if (lua_isboolean(m_ptr->pLuaEnv, -2) &&
                lua_isstring(m_ptr->pLuaEnv, -1))
            {
                b = lua_toboolean(m_ptr->pLuaEnv, -2)?true:false;
                str = lua_tostring(m_ptr->pLuaEnv, -1);
                return {QPair<bool,QString>(b,str)};
            }
        }
    }
    return {false,""};
}
