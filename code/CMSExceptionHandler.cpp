#include "CMSExceptionHandler.h"

#include <tchar.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#pragma warning(push)
#pragma warning(disable : 4091)
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment(lib, "Dbghelp.lib")

static CMSExceptionHandler g_MSExceptionHandler;

LPTOP_LEVEL_EXCEPTION_FILTER CMSExceptionHandler::m_previousFilter;

static void GetNowTime(struct tm& nowTime)
{
    memset(&nowTime, 0, sizeof(struct tm));
    time_t t = time(NULL);
    struct tm* pTime = localtime(&t);
    if (pTime)
    {
        nowTime.tm_sec = pTime->tm_sec;
        nowTime.tm_min = pTime->tm_min;
        nowTime.tm_hour = pTime->tm_hour;
        nowTime.tm_mday = pTime->tm_mday;
        nowTime.tm_mon = pTime->tm_mon;
        nowTime.tm_year = pTime->tm_year;
        nowTime.tm_wday = pTime->tm_wday;
        nowTime.tm_yday = pTime->tm_yday;
        nowTime.tm_isdst = pTime->tm_isdst;
    }
}

static char* TrimString(char* psz)
{
    char szTmp[512] = { 0 };
    char* pszDot = strrchr(psz, '\\');
    if (pszDot)
    {
        pszDot++;   // Advance past the '\\'
        strcpy(szTmp, pszDot);
        ZeroMemory(psz, strlen(psz));
        strcpy(psz, szTmp);
    }
    return psz;
}

CMSExceptionHandler::CMSExceptionHandler()
{
    m_hReportFile = NULL;

    m_previousFilter = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionFilter);

    // Get machine type
    m_dwMachineType = 0;
    TCHAR wszProcessor[256] = { 0 };
    _tcscpy(wszProcessor, ::_tgetenv(_T("PROCESSOR_ARCHITECTURE")));
    if (_tcslen(wszProcessor)>0)
    {
        if ((!_tcsicmp(_T("EM64T"), wszProcessor)) || !_tcsicmp(_T("AMD64"), wszProcessor))
        {
            m_dwMachineType = IMAGE_FILE_MACHINE_AMD64;
        }
        else if (!_tcsicmp(_T("x86"), wszProcessor))
        {
            m_dwMachineType = IMAGE_FILE_MACHINE_I386;
        }
    }

    // Figure out what the report file will be named, and store it away
    GetModuleFileName(0, m_szLogFileName, MAX_PATH);
    TCHAR szLogFile[MAX_PATH] = _T("");
    // Look for the '.' before the "EXE" extension.  Replace the extension
    // with "RPT"
    PTSTR pszDot = _tcsrchr(m_szLogFileName, _T('\\'));
    if (pszDot)
    {
        pszDot++;   // Advance past the '\\'
        *pszDot = 0;
        _tcscpy(szLogFile, m_szLogFileName);
    }
    TCHAR szTime[125] = _T("debug.rpt");
    struct tm nowTime;
    GetNowTime(nowTime);
    _stprintf(szTime, _T("debug-%04d%02d%02d%02d%02d%02d.RPT"), nowTime.tm_year + 1900,
        nowTime.tm_mon + 1, nowTime.tm_mday, nowTime.tm_hour,
        nowTime.tm_min, nowTime.tm_sec);
    _tcscat(szLogFile, szTime);
    _tcscpy(m_szLogFileName, szLogFile);
}

CMSExceptionHandler::~CMSExceptionHandler()
{
    SetUnhandledExceptionFilter(m_previousFilter);
}

LONG CMSExceptionHandler::ExceptionFilter(LPEXCEPTION_POINTERS e)
{
    return g_MSExceptionHandler.HandleException(e);
}

LONG CMSExceptionHandler::HandleException(LPEXCEPTION_POINTERS pExceptionInfo)
{
    HANDLE hProcess = INVALID_HANDLE_VALUE;

    // Initializes the symbol handler
    if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
    {
        SymCleanup(hProcess);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    m_hReportFile = CreateFile(m_szLogFileName,
        GENERIC_WRITE,
        0,
        0,
        OPEN_ALWAYS,
        FILE_FLAG_WRITE_THROUGH,
        0);

    if (m_hReportFile != INVALID_HANDLE_VALUE && NULL != m_hReportFile)
    {
        SetFilePointer(m_hReportFile, 0, 0, FILE_END);

        GenerateExceptionReport(pExceptionInfo);

        // Work through the call stack upwards.
        TraceCallStack(pExceptionInfo->ContextRecord);

        CloseHandle(m_hReportFile);
        m_hReportFile = 0;
    }

    SymCleanup(hProcess);

    return(EXCEPTION_EXECUTE_HANDLER);

    /*if (m_previousFilter)
    return m_previousFilter(pExceptionInfo);
    else
    return EXCEPTION_CONTINUE_SEARCH;*/
}

void CMSExceptionHandler::GenerateExceptionReport(LPEXCEPTION_POINTERS pExceptionInfo)
{
    // Start out with a banner
    PrintTraceLog("//=====================================================\n");

    struct tm nowTime;
    GetNowTime(nowTime);
    PrintTraceLog("Crash Last Time: %04d-%02d-%02d %02d:%02d:%02d\n", nowTime.tm_year + 1900,
        nowTime.tm_mon + 1, nowTime.tm_mday, nowTime.tm_hour,
        nowTime.tm_min, nowTime.tm_sec);

    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

    // First print information about the type of fault
    PrintTraceLog("Exception code: %08X %s\n",
        pExceptionRecord->ExceptionCode,
        GetExceptionString(pExceptionRecord->ExceptionCode));
#if defined(_WIN64)
    PrintTraceLog("Fault address: %016llX\n",
        pExceptionRecord->ExceptionAddress);
#else
    PrintTraceLog("Fault address: %08X\n",
        pExceptionRecord->ExceptionAddress);
#endif
}

const char* CMSExceptionHandler::GetExceptionString(DWORD dwCode)
{
#define EXCEPTION( x ) case EXCEPTION_##x: return (#x);

    switch (dwCode)
    {
        EXCEPTION(ACCESS_VIOLATION)
            EXCEPTION(DATATYPE_MISALIGNMENT)
            EXCEPTION(BREAKPOINT)
            EXCEPTION(SINGLE_STEP)
            EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
            EXCEPTION(FLT_DENORMAL_OPERAND)
            EXCEPTION(FLT_DIVIDE_BY_ZERO)
            EXCEPTION(FLT_INEXACT_RESULT)
            EXCEPTION(FLT_INVALID_OPERATION)
            EXCEPTION(FLT_OVERFLOW)
            EXCEPTION(FLT_STACK_CHECK)
            EXCEPTION(FLT_UNDERFLOW)
            EXCEPTION(INT_DIVIDE_BY_ZERO)
            EXCEPTION(INT_OVERFLOW)
            EXCEPTION(PRIV_INSTRUCTION)
            EXCEPTION(IN_PAGE_ERROR)
            EXCEPTION(ILLEGAL_INSTRUCTION)
            EXCEPTION(NONCONTINUABLE_EXCEPTION)
            EXCEPTION(STACK_OVERFLOW)
            EXCEPTION(INVALID_DISPOSITION)
            EXCEPTION(GUARD_PAGE)
            EXCEPTION(INVALID_HANDLE)
    }

    // If not one of the "known" exceptions, try to get the string
    // from NTDLL.DLL's message table.

    static char szBuffer[512] = { 0 };

    FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
        GetModuleHandle(_T("NTDLL.DLL")),
        dwCode, 0, szBuffer, sizeof(szBuffer), 0);

    return szBuffer;
}

// Work through the stack to get the entire call stack
void CMSExceptionHandler::TraceCallStack(CONTEXT* pContext)
{
    // Initialize stack frame
    STACKFRAME64 sf;
    memset(&sf, 0, sizeof(STACKFRAME));

#if defined(_WIN64)
    sf.AddrPC.Offset = pContext->Rip;
    sf.AddrStack.Offset = pContext->Rsp;
    sf.AddrFrame.Offset = pContext->Rbp;
#elif defined(WIN32)
    sf.AddrPC.Offset = pContext->Eip;
    sf.AddrStack.Offset = pContext->Esp;
    sf.AddrFrame.Offset = pContext->Ebp;
#endif
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Mode = AddrModeFlat;


    PrintTraceLog("\nRegisters:\n");

#if defined(_WIN64)
    PrintTraceLog("EAX:%016llX\nEBX:%016llX\nECX:%016llX\nEDX:%016llX\nESI:%016llX\nEDI:%016llX\n",
        pContext->Rax, pContext->Rbx, pContext->Rcx, pContext->Rdx, pContext->Rsi, pContext->Rdi);
    PrintTraceLog("CS:EIP:%04X:%016llX\n", pContext->SegCs, sf.AddrPC.Offset);
    PrintTraceLog("SS:ESP:%04X:%016llX  EBP:%016llX\n",
        pContext->SegSs, sf.AddrStack.Offset, sf.AddrFrame.Offset);
#else
    PrintTraceLog("EAX:%08X\nEBX:%08X\nECX:%08X\nEDX:%08X\nESI:%08X\nEDI:%08X\n",
        pContext->Eax, pContext->Ebx, pContext->Ecx, pContext->Edx, pContext->Esi, pContext->Edi);
    PrintTraceLog("CS:EIP:%04X:%08llX\n", pContext->SegCs, sf.AddrPC.Offset);
    PrintTraceLog("SS:ESP:%04X:%08llX  EBP:%08llX\n",
        pContext->SegSs, sf.AddrStack.Offset, sf.AddrFrame.Offset);
#endif
    PrintTraceLog("DS:%04X  ES:%04X  FS:%04X  GS:%04X\n",
        pContext->SegDs, pContext->SegEs, pContext->SegFs, pContext->SegGs);
    PrintTraceLog("Flags:%08X\n", pContext->EFlags);

    if (0 == m_dwMachineType)
        return;

    PrintTraceLog("\nCall stack:\n");

#if defined(_WIN64)
    PrintTraceLog("Address           Line    Function    File    Module\n");
#else
    PrintTraceLog("Address   Line    Function    File    Module\n");
#endif

    // Walk through the stack frames.
    int CALLSTACK_DEPTH = 0;
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();
    while (StackWalk64(m_dwMachineType, hProcess, hThread, &sf, pContext, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0))
    {
        if (sf.AddrFrame.Offset == 0 || CALLSTACK_DEPTH >= 10)
            break;
        CALLSTACK_DEPTH++;

        // 1. Get function name at the address
        const int nBuffSize = (sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64);
        ULONG64 symbolBuffer[nBuffSize];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;

        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD64 dwAddress = sf.AddrPC.Offset;
        DWORD dwLineNumber = 0;
        char szFuncName[260] = { 0 };
        char szFileName[260] = { 0 };
        char szModuleName[260] = { 0 };

        DWORD64 moduleBase = SymGetModuleBase64(hProcess, sf.AddrPC.Offset);
        if (moduleBase && GetModuleFileNameA((HINSTANCE)moduleBase, szModuleName, 260))
        {
            TrimString(szModuleName);
        }
        if (strlen(szModuleName) <= 0) strcpy(szModuleName, "Unknow");

        DWORD64 dwSymDisplacement = 0;
        if (SymFromAddr(hProcess, sf.AddrPC.Offset, &dwSymDisplacement, pSymbol))
        {
            std::string str(pSymbol->Name);
            strcpy(szFuncName, str.c_str());
        }
        if (strlen(szFuncName) <= 0) strcpy(szFuncName, "Unknow");

        //2. get line and file name at the address
        IMAGEHLP_LINE64 lineInfo = { sizeof(IMAGEHLP_LINE64) };
        DWORD dwLineDisplacement = 0;

        if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
        {
            std::string str(lineInfo.FileName);
            strcpy(szFileName, str.c_str());
            TrimString(szFileName);
            dwLineNumber = lineInfo.LineNumber;
        }
        if (strlen(szFileName) <= 0) strcpy(szFileName, "Unknow");

        // Call stack stored
#if defined(_WIN64)
        PrintTraceLog("%016llX  %-8ld%s    %s    %s\n",
            dwAddress, dwLineNumber, szFuncName, szFileName, szModuleName);
#else
        PrintTraceLog("%08llX  %-8d%s    %s    %s\n",
            dwAddress, dwLineNumber, szFuncName, szFileName, szModuleName);
#endif
    }
}

int CMSExceptionHandler::PrintTraceLog(const char * format, ...)
{
    char szBuff[1024] = "";
    int retValue;
    DWORD cbWritten;
    va_list argptr;

    va_start(argptr, format);
    retValue = vsprintf(szBuff, format, argptr);
    va_end(argptr);

    WriteFile(m_hReportFile, szBuff, retValue * sizeof(char), &cbWritten, 0);

    return retValue;
}
