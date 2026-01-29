#ifndef CMSEXCEPTIONHANDLER_H
#define CMSEXCEPTIONHANDLER_H

#include <Windows.h>
#include <string>

class CMSExceptionHandler
{
public:
    CMSExceptionHandler();
    ~CMSExceptionHandler();

private:
    static LONG ExceptionFilter(LPEXCEPTION_POINTERS e);

    // The main function to handle exception
    LONG HandleException(LPEXCEPTION_POINTERS pExceptionInfo);

    void GenerateExceptionReport(LPEXCEPTION_POINTERS pExceptionInfo);

    const char* GetExceptionString(DWORD dwCode);

    // Work through the stack upwards to get the entire call stack
    void TraceCallStack(CONTEXT* pContext);

    int PrintTraceLog(const char * format, ...);

private:
    static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;

    // Machine type matters when trace the call stack (StackWalk64)
    DWORD m_dwMachineType;
    HANDLE m_hReportFile;
    TCHAR m_szLogFileName[MAX_PATH];
};

#endif // CMSEXCEPTIONHANDLER_H
