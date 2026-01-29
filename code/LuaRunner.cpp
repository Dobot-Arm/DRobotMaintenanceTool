/*
** $Id: lua.c $
** Lua stand-alone interpreter
** See Copyright Notice in lua.h
*/


#include "lua/lua.hpp"
#include "CommonData.h"
#include "LuaApi.h"
#include "Logger.h"
#include <functional>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QEventLoop>

extern pfnPrintLog g_pfnPrint;

static void printLog(const char* pszLog, int iLength, int itype)
{
    QtMsgType type;
    if (0==itype){
        qInfo()<<"lua print:"<<pszLog;
        type = QtMsgType::QtDebugMsg;
    }
    else{
        qCritical()<<"lua print error:"<<pszLog;
        type = QtMsgType::QtCriticalMsg;
    }
    QString strLog(pszLog);
    emit CLuaApi::getInstance()->signalLuaLog(strLog, type);
}

static const char *progname="";

static QNetworkAccessManager g_network;
static bool g_bInitNetwork = false;
static QNetworkAccessManager* getHttpClient()
{
    if (!g_bInitNetwork)
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::NoProxy);
        g_network.setProxy(proxy);
    }
    if (g_network.networkAccessible() != QNetworkAccessManager::Accessible)
    {
        g_network.setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    return &g_network;
}

/*
 * 提供给lua脚本调用：获取当前控制器的一些信息
*/
static int exportDobotGetCurrentVersionInfo(lua_State *L)
{
    DobotType::SettingsVersion info = CommonData::getCurrentSettingsVersion();

    const std::string algs{info.algs.toStdString()};
    const std::string codesys{info.codesys.toStdString()};
    const std::string control{info.control.toStdString()};
    const std::string feedback{info.feedback.toStdString()};
    const std::string safeio{info.safeio.toStdString()};
    const std::string servo1{info.servo1.toStdString()};
    const std::string servo2{info.servo2.toStdString()};
    const std::string servo3{info.servo3.toStdString()};
    const std::string servo4{info.servo4.toStdString()};
    const std::string servo5{info.servo5.toStdString()};
    const std::string servo6{info.servo6.toStdString()};
    const std::string system{info.system.toStdString()};
    const std::string terminal{info.terminal.toStdString()};
    const std::string unio{info.unio.toStdString()};

    lua_newtable(L); //栈顶创建table

    const char* data[]={"algs",algs.c_str(),
                        "codesys",codesys.c_str(),
                        "control",control.c_str(),
                        "feedback",feedback.c_str(),
                        "safeio",safeio.c_str(),
                        "servo1",servo1.c_str(),
                        "servo2",servo2.c_str(),
                        "servo3",servo3.c_str(),
                        "servo4",servo4.c_str(),
                        "servo5",servo5.c_str(),
                        "servo6",servo6.c_str(),
                        "system",system.c_str(),
                        "terminal",terminal.c_str(),
                        "unio",unio.c_str(),
                        NULL,NULL};
    for (int i=0;;++i,++i)
    {
        if (NULL!=data[i])
        {
            lua_pushstring(L, data[i]);
            lua_pushstring(L, data[i+1]);
            lua_settable(L, -3);
        }
        else
        {
            break;
        }
    }
    return 1;
}

/*
 * 提供给lua脚本调用：获取升级包的一些信息
*/
static int exportDobotGetUpgradeVersionInfo(lua_State *L)
{
    DobotType::SettingsVersion info = CommonData::getUpgradeSettingsVersion();

    const std::string algs{info.algs.toStdString()};
    const std::string codesys{info.codesys.toStdString()};
    const std::string control{info.control.toStdString()};
    const std::string feedback{info.feedback.toStdString()};
    const std::string safeio{info.safeio.toStdString()};
    const std::string servo1{info.servo1.toStdString()};
    const std::string servo2{info.servo2.toStdString()};
    const std::string servo3{info.servo3.toStdString()};
    const std::string servo4{info.servo4.toStdString()};
    const std::string servo5{info.servo5.toStdString()};
    const std::string servo6{info.servo6.toStdString()};
    const std::string system{info.system.toStdString()};
    const std::string terminal{info.terminal.toStdString()};
    const std::string unio{info.unio.toStdString()};

    lua_newtable(L); //栈顶创建table

    const char* data[]={"algs",algs.c_str(),
                        "codesys",codesys.c_str(),
                        "control",control.c_str(),
                        "feedback",feedback.c_str(),
                        "safeio",safeio.c_str(),
                        "servo1",servo1.c_str(),
                        "servo2",servo2.c_str(),
                        "servo3",servo3.c_str(),
                        "servo4",servo4.c_str(),
                        "servo5",servo5.c_str(),
                        "servo6",servo6.c_str(),
                        "system",system.c_str(),
                        "terminal",terminal.c_str(),
                        "unio",unio.c_str(),
                        NULL,NULL};
    for (int i=0;;++i,++i)
    {
        if (NULL!=data[i])
        {
            lua_pushstring(L, data[i]);
            lua_pushstring(L, data[i+1]);
            lua_settable(L, -3);
        }
        else
        {
            break;
        }
    }
    return 1;
}

/*
 * 提供给lua脚本调用：获取控制器名称
*/
static int exportDobotGetControllerTypeName(lua_State *L)
{
    QString str = CommonData::getControllerType().originName;
    lua_pushstring(L, str.toStdString().c_str());
    return 1;
}

/*
 * 提供给lua脚本调用：获取控制器扩展名称,返回值是string，例如：CR10A-IS、CR5A-IS
*/
static int exportDobotGetControllerExtName(lua_State *L)
{
    QString str = CommonData::getControllerType().nameExt;
    lua_pushstring(L, str.toStdString().c_str());
    return 1;
}

/*
 * 提供给lua脚本调用：获取控制柜名称
*/
static int exportDobotGetCabinetTypeName(lua_State *L)
{
    QString str = CommonData::getStrPropertiesCabinetType().name;
    lua_pushstring(L, str.toStdString().c_str());
    return 1;
}
static int exportDobotGetCabinetTypePower(lua_State *L)
{
    QString str = CommonData::getStrPropertiesCabinetType().power;
    lua_pushstring(L, str.toStdString().c_str());
    return 1;
}
static int exportDobotGetConnectedIP(lua_State *L)
{
    QString str = g_strCurrentIP;
    lua_pushstring(L, str.toStdString().c_str());
    return 1;
}

/*
 * 提供给lua脚本调用：判断当前语言是否为英文
*/
static int exportDobotLangIsEnglish(lua_State* L)
{
    lua_pushboolean(L, g_isEnglish?1:0);
    return 1;
}

/*
 * 提供给lua脚本调用：获取维护工具版本号
*/
static int exportDobotGetAppVersionId(lua_State* L)
{
    lua_pushinteger(L, 230);
    return 1;
}

static int exportDobotHttpGet(lua_State* L)
{
    const char* pszUrl = nullptr;
    int iTimeoutMilliseconds = 0;
    int iParams = lua_gettop(L);
    if (iParams>0){
        int idx = -iParams;
        if (lua_type(L, idx) != LUA_TSTRING){
            lua_pushinteger(L,-1);
            lua_pushstring(L, "the first parameter must be a string");
            return 2;
        }
        pszUrl = lua_tostring(L, idx);
        if (NULL==pszUrl || strlen(pszUrl)==0){
            lua_pushinteger(L,-1);
            lua_pushstring(L, "the first parameter must not be a empty string");
            return 2;
        }
    }

    --iParams;
    if (iParams>0){
        int idx = -iParams;
        if (lua_type(L, idx) != LUA_TNUMBER || !lua_isinteger(L, idx)){
            lua_pushinteger(L,-1);
            lua_pushstring(L, "the second parameter must be a integer and greater then 0");
            return 2;
        }
        iTimeoutMilliseconds = lua_tointeger(L, -1);
    }

    if (nullptr==pszUrl){
        lua_pushinteger(L,-1);
        lua_pushstring(L, "the first parameter must be a string");
        return 2;
    }
    if (iTimeoutMilliseconds<=0) iTimeoutMilliseconds = 3000;

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl(QString(pszUrl)));
    QNetworkReply* pReply = getHttpClient()->get(request);

    QTimer* pTimer = new QTimer(pReply);
    pTimer->setSingleShot(true);
    pTimer->setInterval(iTimeoutMilliseconds);

    QEventLoop loop;
    QObject::connect(pTimer, &QTimer::timeout, [pReply,&loop]{
        loop.setProperty("timeout",QVariant(true));
        pReply->abort();
        loop.quit();
    });
    QObject::connect(pReply, &QNetworkReply::finished, [pReply, pTimer, &loop]{
        pTimer->stop();
        if (pReply->error()==QNetworkReply::NoError)
        {
            loop.setProperty("msg",QVariant(QString::fromUtf8(pReply->readAll())));
        }
        else
        {
            loop.setProperty("errmsg",QVariant(pReply->errorString()));
        }
        loop.quit();
    });
    pTimer->start();
    loop.exec();
    pReply->deleteLater();

    int iRetCode = 0;
    std::string strMsg;
    if (loop.property("timeout").isNull())
    {
        if (loop.property("msg").isNull())
        {
            iRetCode = -1;
            strMsg = loop.property("errmsg").toString().toStdString();
        }
        else
        {
            iRetCode = 0;
            strMsg = loop.property("msg").toString().toStdString();
        }
    }
    else
    {
        iRetCode = -1;
        strMsg = "the http get request timeout";
    }
    lua_pushinteger(L,iRetCode);
    lua_pushstring(L, strMsg.c_str());
    return 2;
}

static int exportDobotHttpPost(lua_State* L)
{
    const char* pszUrl = nullptr;
    const char* pszBody = nullptr;
    int iTimeoutMilliseconds = 0;
    int iParams = lua_gettop(L);
    if (iParams>0){
        int idx = -iParams;
        if (lua_type(L, idx) != LUA_TSTRING){
            lua_pushinteger(L,-1);
            lua_pushstring(L, "the first parameter must be a string");
            return 2;
        }
        pszUrl = lua_tostring(L, idx);
        if (NULL==pszUrl || strlen(pszUrl)==0){
            lua_pushinteger(L,-1);
            lua_pushstring(L, "the first parameter must not be a empty string");
            return 2;
        }
    }

    --iParams;
    if (iParams>0){
        int idx = -iParams;
        if (lua_type(L, idx) != LUA_TSTRING){
            lua_pushinteger(L,-1);
            lua_pushstring(L, "the second parameter must be a string");
            return 2;
        }
        pszBody = lua_tostring(L, idx);
    }

    --iParams;
    if (iParams>0){
        int idx = -iParams;
        if (lua_type(L, idx) != LUA_TNUMBER || !lua_isinteger(L, idx)){
            lua_pushinteger(L,-1);
            lua_pushstring(L, "the second parameter must be a integer and greater then 0");
            return 2;
        }
        iTimeoutMilliseconds = lua_tointeger(L, -1);
    }
    if (nullptr==pszUrl||nullptr==pszBody){
        lua_pushinteger(L,-1);
        lua_pushstring(L, "the first/second parameter must be a string");
        return 2;
    }
    if (iTimeoutMilliseconds<=0) iTimeoutMilliseconds = 3000;

    QByteArray postData(pszBody);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, postData.size());
    request.setUrl(QUrl(QString(pszUrl)));
    QNetworkReply* pReply = getHttpClient()->post(request, postData);

    QTimer* pTimer = new QTimer(pReply);
    pTimer->setSingleShot(true);
    pTimer->setInterval(iTimeoutMilliseconds);

    QEventLoop loop;
    QObject::connect(pTimer, &QTimer::timeout, [pReply,&loop]{
        loop.setProperty("timeout",QVariant(true));
        pReply->abort();
        loop.quit();
    });
    QObject::connect(pReply, &QNetworkReply::finished, [pReply, pTimer, &loop]{
        pTimer->stop();
        if (pReply->error()==QNetworkReply::NoError)
        {
            loop.setProperty("msg",QVariant(QString::fromUtf8(pReply->readAll())));
        }
        else
        {
            loop.setProperty("errmsg",QVariant(pReply->errorString()));
        }
        loop.quit();
    });
    pTimer->start();
    loop.exec();
    pReply->deleteLater();

    int iRetCode = 0;
    std::string strMsg;
    if (loop.property("timeout").isNull())
    {
        if (loop.property("msg").isNull())
        {
            iRetCode = -1;
            strMsg = loop.property("errmsg").toString().toStdString();
        }
        else
        {
            iRetCode = 0;
            strMsg = loop.property("msg").toString().toStdString();
        }
    }
    else
    {
        iRetCode = -1;
        strMsg = "the http post request timeout";
    }
    lua_pushinteger(L,iRetCode);
    lua_pushstring(L, strMsg.c_str());
    return 2;
}

static const luaL_Reg exportLib[] = {
    {"DobotGetCurrentVersionInfo",exportDobotGetCurrentVersionInfo},
    {"DobotGetUpgradeVersionInfo",exportDobotGetUpgradeVersionInfo},
    {"DobotGetControllerTypeName",exportDobotGetControllerTypeName},
    {"DobotGetControllerExtName",exportDobotGetControllerExtName},
    {"DobotGetCabinetTypeName",exportDobotGetCabinetTypeName},
    {"DobotGetCabinetTypePower",exportDobotGetCabinetTypePower},
    {"DobotGetConnectedIP",exportDobotGetConnectedIP},
    {"DobotLangIsEnglish",exportDobotLangIsEnglish},
    {"DobotGetAppVersionId",exportDobotGetAppVersionId},
    {"DobotHttpGet",exportDobotHttpGet},
    {"DobotHttpPost",exportDobotHttpPost},
    {NULL, NULL}
};

static void register_dobot(lua_State *L)
{
    const luaL_Reg* lib;
    for (lib=exportLib; lib->func; ++lib)
    {
        lua_register(L, lib->name, lib->func);
    }
}

/*
** Prints an error message, adding the program name in front of it
** (if present)
*/
static void l_message (const char *pname, const char *msg) {
    if (pname) lua_writestringerror("%s: ", pname);
    lua_writestringerror("%s\n", msg);
}


/*
** Check whether 'status' is not OK and, if so, prints the error
** message on the top of the stack. It assumes that the error object
** is a string, as it was either generated by Lua or by 'msghandler'.
*/
static int report (lua_State *L, int status) {
    if (status != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        l_message(progname, msg);
        lua_pop(L, 1);  /* remove message */
    }
    return status;
}


/*
** Message handler used to run all chunks
*/
static int msghandler (lua_State *L) {
    const char *msg = lua_tostring(L, 1);
    if (msg == NULL) {  /* is error object not a string? */
        if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
                lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
            return 1;  /* that is the message */
        else
            msg = lua_pushfstring(L, "(error object is a %s value)",
                                  luaL_typename(L, 1));
    }
    luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
    return 1;  /* return the traceback */
}


/*
** Interface to 'lua_pcall', which sets appropriate message function
** and C-signal handler. Used to run all chunks.
*/
static int docall (lua_State *L, int narg, int nres) {
    int status;
    int base = lua_gettop(L) - narg;  /* function index */
    lua_pushcfunction(L, msghandler);  /* push message handler */
    lua_insert(L, base);  /* put it under function and args */
    status = lua_pcall(L, narg, nres, base);
    lua_remove(L, base);  /* remove message handler from the stack */
    return status;
}


/*
** Create the 'arg' table, which stores all arguments from the
** command line ('argv'). It should be aligned so that, at index 0,
** it has 'argv[script]', which is the script name. The arguments
** to the script (everything after 'script') go to positive indices;
** other arguments (before the script name) go to negative indices.
** If there is no script name, assume interpreter's name as base.
*/
static void createargtable (lua_State *L, char **argv, int argc, int script) {
    int i, narg;
    if (script == argc) script = 0;  /* no script name? */
    narg = argc - (script + 1);  /* number of positive indices */
    lua_createtable(L, narg, script + 1);
    for (i = 0; i < argc; i++) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i - script);
    }
    lua_setglobal(L, "arg");
}

/*
** Push on the stack the contents of table 'arg' from 1 to #arg
*/
static int pushargs (lua_State *L) {
    int i, n;
    if (lua_getglobal(L, "arg") != LUA_TTABLE)
        luaL_error(L, "'arg' is not a table");
    n = (int)luaL_len(L, -1);
    luaL_checkstack(L, n + 3, "too many arguments to script");
    for (i = 1; i <= n; i++)
        lua_rawgeti(L, -i, i);
    lua_remove(L, -i);  /* remove table from the stack */
    return n;
}

static int handle_script_Old (lua_State *L, char **argv) {
    int status;
    const char *fname = argv[0];
    status = luaL_loadfile(L, fname);
    if (status == LUA_OK) {
        int n = pushargs(L);  /* push arguments to script */
        status = docall(L, n, LUA_MULTRET);
    }
    return report(L, status);
}

static int handle_script (lua_State *L, char **argv) {
    int status;
    QByteArray data;
    QFile innerFile(":/luaJson.lua");
    if (innerFile.open(QFile::ReadOnly))
    {
        data += innerFile.readAll();
        innerFile.close();
    }
    QFileInfo info(argv[0]);
    QString strPath = QString("\r\npackage.path=\"%1\\?.lua;\"..package.path\r\n").arg(info.absolutePath());
    strPath.replace('\\','/');
    QString strRequire = QString("require(\"%1\")\r\n").arg(info.baseName());
    data += strPath;
    data += strRequire;
    //status = luaL_loadbufferx(L, data.data(), data.size(),argv[0], NULL);
    status = luaL_loadbufferx(L, data.data(), data.size(),NULL, NULL);
    if (status == LUA_OK) {
        int n = pushargs(L);  /* push arguments to script */
        status = docall(L, n, LUA_MULTRET);
    }
    return report(L, status);
}

/*
** Main body of stand-alone interpreter (to be called in protected mode).
** Reads the options and handles them all.
*/
static int pmain (lua_State *L) {
    int argc = (int)lua_tointeger(L, 1);
    char **argv = (char **)lua_touserdata(L, 2);
    int script = 1;
    luaL_checkversion(L);  /* check that interpreter has correct version */
    if (argv[0] && argv[0][0]) progname = argv[0];

    luaL_openlibs(L);  /* open standard libraries */
    register_dobot(L);


    createargtable(L, argv, argc, script);  /* create table 'arg' */
    lua_gc(L, LUA_GCGEN, 0, 0);  /* GC in generational mode */

    if (script < argc &&  /* execute main script (if there is one) */
            handle_script(L, argv + script) != LUA_OK)
        return 0;
    lua_pushboolean(L, 1);  /* signal no errors */
    return 1;
}

bool luaRunner(int argc, const char **argv, lua_State*& pL)
{
    printLog("lua script is running....\n",1, 0);
    g_pfnPrint = printLog;
    int status, result;
    lua_State *L = luaL_newstate();  /* create state */
    pL = L;
    if (L == NULL) {
        l_message(argv[0], "cannot create state: not enough memory");
        return false;
    }

    lua_pushcfunction(L, &pmain);  /* to call 'pmain' in protected mode */
    lua_pushinteger(L, argc);  /* 1st argument */
    lua_pushlightuserdata(L, argv); /* 2nd argument */
    status = lua_pcall(L, 2, 1, 0);  /* do the call */
    result = lua_toboolean(L, -1);  /* get result */
    report(L, status);
    //return (result && status == LUA_OK) ? 0 : -1;
    return true;
}

void luaStopper(lua_State*& pL)
{
    if (pL) lua_close(pL);
    printLog("lua script has stop\n\n",1, 0);
}
