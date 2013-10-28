#include <windows.h>
#include "GreenPrintLogging.h"

#pragma once

class CGreenPrintMonitorThread
{
public:
    CGreenPrintMonitorThread(IGreenPrintLogging *pLogging);
    virtual ~CGreenPrintMonitorThread(void);

    BOOL Start(void);
    BOOL Stop(void);

private:	
	DWORD OnSpoolMonitorThread(void);
	static DWORD WINAPI SpoolMonitorThread( LPVOID pContext );
	VOID GetJobInfoFromPrinterNotifyInfo( PPRINTER_NOTIFY_INFO pPrinterNotifyInfo, DWORD *pJobId, DWORD *pJobPages );
	DWORD JobPagesFromJobId( HANDLE hPrinter, DWORD JobId );
	BOOL ShouldResumePrintJob( DWORD JobPageTotal );

private:
	HANDLE m_hSpoolMonitorThread;
	HANDLE m_hExitSpoolMonitorThread;
	IGreenPrintLogging *m_pLogging;
};
