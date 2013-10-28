#include "GreenPrintMonitorThread.h"

#include <shlwapi.h>
#include <strsafe.h>

#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "shlwapi.lib")

#define UI_COMMAND_APP_NAME       L"GreenPrintApp.exe"
#define MAX_SPOOLING_TIME_IN_MINS 5

CGreenPrintMonitorThread::CGreenPrintMonitorThread(IGreenPrintLogging *pLogging) : 
	m_pLogging(pLogging),
	m_hSpoolMonitorThread(NULL),
	m_hExitSpoolMonitorThread(NULL)
{
}

CGreenPrintMonitorThread::~CGreenPrintMonitorThread(void)
{
	Stop();
}

// ---------------------------------------------
// Start the print spool monitor thread
// ---------------------------------------------
BOOL CGreenPrintMonitorThread::Start(void)
{
	BOOL fStartSpoolMonitorThread = FALSE;

	m_pLogging->Log( L"Green Print Monitor Started" );

	// ---------------------------------------------
	// Create an event to single when the monitor thread should exit
	// ---------------------------------------------
	m_hExitSpoolMonitorThread = CreateEvent( NULL, FALSE, FALSE, NULL );

	if ( NULL != m_hExitSpoolMonitorThread )
	{
		// ---------------------------------------------
		// Start the print spool monitor thread
		// ---------------------------------------------
		m_hSpoolMonitorThread = CreateThread( NULL, 0, SpoolMonitorThread, this, 0, NULL );

		if ( NULL != m_hSpoolMonitorThread )
		{
			fStartSpoolMonitorThread = NULL;
		}
		else
		{
			m_pLogging->Log( L"Failed to create the Spool Monitor Thread." );
		}
	}
	else
	{
		m_pLogging->Log( L"Failed to create the Spool Monitor Thread exit event." );
	}

	return fStartSpoolMonitorThread;
}

// ---------------------------------------------
// Stop the print spool monitor thread
// ---------------------------------------------
BOOL CGreenPrintMonitorThread::Stop(void)
{
	BOOL fStopSpoolMonitorThread = FALSE;

	m_pLogging->Log( L"Green Print Monitor Stop" );

	if ( NULL != m_hExitSpoolMonitorThread )
	{
		// ---------------------------------------------
		// Signal the monitor thread should exit
		// ---------------------------------------------
		SetEvent( m_hExitSpoolMonitorThread );

		if ( NULL != m_hSpoolMonitorThread )
		{
			// ---------------------------------------------
			// Wait a bit to for the thread to exit cleanly, if not log it and continue. 
			// A clean exit here isn't critial, but good to know about if it happens
			// ---------------------------------------------
			DWORD WaitReason = WaitForSingleObject( m_hSpoolMonitorThread, 10 * 1000 );

			if ( WAIT_OBJECT_0 == WaitReason )
			{
				fStopSpoolMonitorThread = TRUE;
			}
			else
			{
				m_pLogging->Log( L"Timeout waiting for Spool Monitor Thread to exit." );
			}
		}

		CloseHandle( m_hExitSpoolMonitorThread );
		m_hExitSpoolMonitorThread = NULL;
	}

	return fStopSpoolMonitorThread;
}

DWORD WINAPI CGreenPrintMonitorThread::SpoolMonitorThread( LPVOID pContext )
{
	CGreenPrintMonitorThread *pGreenPrintMonitorThread = static_cast<CGreenPrintMonitorThread*>( pContext );

	if ( NULL == pGreenPrintMonitorThread )
	{
		return ERROR_INVALID_DATA;
	}

	return pGreenPrintMonitorThread->OnSpoolMonitorThread();
}

// ---------------------------------------------
// Monitor the print spool and prompt users to confirm print jobs
// ---------------------------------------------
DWORD CGreenPrintMonitorThread::OnSpoolMonitorThread(void)
{
	DWORD cchPrinterName = 0;

	// ---------------------------------------------
	// Get the buffer size needed to hold the printer name
	// ---------------------------------------------
	GetDefaultPrinter( NULL, &cchPrinterName );

	if ( cchPrinterName > 0 )
	{
		WCHAR *PrinterName = (WCHAR*)LocalAlloc( LMEM_ZEROINIT, cchPrinterName * sizeof(WCHAR) );

		if ( NULL != PrinterName )
		{
			// ---------------------------------------------
			// Get the name of the print
			// ---------------------------------------------
			if ( GetDefaultPrinter( PrinterName, &cchPrinterName ) )
			{
				HANDLE hPrinter = NULL;

				if ( OpenPrinter( PrinterName, &hPrinter, NULL ) )
				{
					HANDLE hChangeNotification = NULL;

					// ---------------------------------------------
					// Setup the structures needed to register for print spool notifications
					// ---------------------------------------------

					PRINTER_NOTIFY_INFO_DATA NotifyInfoData[] = {
						JOB_NOTIFY_TYPE,
						JOB_NOTIFY_FIELD_TOTAL_PAGES,
						0, // Reserved
						JOB_NOTIFY_TYPE,
						0
					};

					PRINTER_NOTIFY_OPTIONS_TYPE NotifyOptionsType[] = {
						JOB_NOTIFY_TYPE,
						0, // Reserved 0
						0, // Reserved 1
						0, // Reserved 2
						_countof(NotifyInfoData),
						(PWORD)NotifyInfoData
					};

					PRINTER_NOTIFY_OPTIONS NotifyOptions;
					ZeroMemory( &NotifyOptions, sizeof(NotifyOptions) );
					NotifyOptions.Version = 2;
					NotifyOptions.Count = _countof(NotifyOptionsType);
					NotifyOptions.pTypes = NotifyOptionsType;

					// ---------------------------------------------
					// Request a notification handle that will be signaled when a new print job is queued
					// ---------------------------------------------
					hChangeNotification = 
						FindFirstPrinterChangeNotification( 
						hPrinter, 
						PRINTER_CHANGE_ADD_JOB, 
						0, // Reserved 
						&NotifyOptions
						);

					if ( INVALID_HANDLE_VALUE != hChangeNotification )
					{
						do
						{
							// ---------------------------------------------
							// The order of these handles is important to the code below that checks which event was signaled
							// ---------------------------------------------
							HANDLE HandleList[] = {
								hChangeNotification, 
								m_hExitSpoolMonitorThread
							};

							// ---------------------------------------------
							// Wait for the print job notification handle or the exit handle to be signaled
							// ---------------------------------------------
							DWORD WaitReason = WaitForMultipleObjects( 
								_countof(HandleList), 
								HandleList, 
								FALSE, 
								INFINITE );

							if ( WAIT_OBJECT_0 == WaitReason )
							{
								PRINTER_NOTIFY_INFO *pPrinterNotifyInfo = NULL;

								// ---------------------------------------------
								// The notification handle was signaled so get the info about the print job
								// ---------------------------------------------
								if ( FindNextPrinterChangeNotification( 
										hChangeNotification,
										NULL,
										&NotifyOptions,
										(LPVOID*)&pPrinterNotifyInfo ) )
								{
									if ( NULL != pPrinterNotifyInfo )
									{
										// ---------------------------------------------
										// Required code to make the FindNextPrinterChangeNotification happy (see MSDN documentation)
										// ---------------------------------------------
										if ( PRINTER_NOTIFY_INFO_DISCARDED & pPrinterNotifyInfo->Flags )
										{
											FreePrinterNotifyInfo( pPrinterNotifyInfo );
											pPrinterNotifyInfo = NULL;

											DWORD Flags = NotifyOptions.Flags;

											NotifyOptions.Flags = PRINTER_NOTIFY_OPTIONS_REFRESH;

											FindNextPrinterChangeNotification( 
													hChangeNotification,
													NULL,
													&NotifyOptions,
													(LPVOID*)&pPrinterNotifyInfo );

											NotifyOptions.Flags = Flags;
										}
									}

									// ---------------------------------------------
									// Get the needed information from the job
									// ---------------------------------------------
									DWORD JobId = 0;
									DWORD JobPages = 0;

									if ( NULL != pPrinterNotifyInfo )
									{
										GetJobInfoFromPrinterNotifyInfo( pPrinterNotifyInfo, &JobId, &JobPages );

										FreePrinterNotifyInfo( pPrinterNotifyInfo );
										pPrinterNotifyInfo = NULL;
									}

									// ---------------------------------------------
									// Pause the print job 
									// ---------------------------------------------
									if ( SetJob( hPrinter,
											JobId,
											0,
											NULL,
											JOB_CONTROL_PAUSE ) )
									{
										// ---------------------------------------------
										// If we weren't able to get the number of pages already
										// then get the count from the job itself
										// ---------------------------------------------
										if ( 0 == JobPages )
										{
											JobPages = JobPagesFromJobId( hPrinter, JobId );
										}

										// ---------------------------------------------
										// Interact with the user to determine if they would really like to print
										// ---------------------------------------------
										BOOL ResumeJob = ShouldResumePrintJob( JobPages );
										
										if ( ResumeJob )
										{
											if ( !SetJob( hPrinter,
												JobId,
												0,
												NULL,
												JOB_CONTROL_RESUME ) )
											{
												m_pLogging->Log( L"Failed resuming print job." );
											}
										}
										else
										{
											if ( !SetJob( hPrinter,
												JobId,
												0,
												NULL,
												JOB_CONTROL_DELETE ) )
											{
												m_pLogging->Log( L"Failed deleting print job." );
											}											
										}
									}
								}
								else
								{
									m_pLogging->Log( L"Failed getting information concerning a print job." );
								}
							}
							else if ( ( WAIT_OBJECT_0 + 1 ) == WaitReason ) 
							{
								// ---------------------------------------------
								// The exit handle was signaled. Stop checking for print jobs.
								// ---------------------------------------------
								break;
							}
							else
							{
								m_pLogging->Log( L"Unexpected notification while waiting for print job." );
							}
						}
						while( TRUE );
					}
					else
					{
						m_pLogging->Log( L"Failed trying to get print job notification handle." );
					}

					if ( INVALID_HANDLE_VALUE != hChangeNotification )
					{
						FindClosePrinterChangeNotification( hChangeNotification );
						hChangeNotification = NULL;
					}

					if ( NULL != hPrinter )
					{
						ClosePrinter( hPrinter );
						hPrinter = NULL;
					}
				}
			}

			if ( NULL != PrinterName )
			{
				cchPrinterName = 0;

				LocalFree( PrinterName );
				PrinterName = NULL;
			}
		}
	}

	return NO_ERROR;
}

// ---------------------------------------------
// Extract information about the print job from the notify info structures if possible
// ---------------------------------------------
VOID CGreenPrintMonitorThread::GetJobInfoFromPrinterNotifyInfo( PPRINTER_NOTIFY_INFO pPrinterNotifyInfo, DWORD *pJobId, DWORD *pJobPages )
{
	if ( NULL != pPrinterNotifyInfo && NULL != pJobId && NULL != pJobPages )
	{
		*pJobId = 0;
		*pJobPages = 0;

		for ( DWORD i=0; i < pPrinterNotifyInfo->Count; i++ )
		{
			PRINTER_NOTIFY_INFO_DATA *pNotifyInfoData = &pPrinterNotifyInfo->aData[ i ];

			if ( NULL != pNotifyInfoData )
			{
				if ( JOB_NOTIFY_TYPE == pNotifyInfoData->Type )
				{
					*pJobId = pNotifyInfoData->Id;

					//
					// NOTE: This does not provide a reliable page count value
					//
					//if ( JOB_NOTIFY_FIELD_TOTAL_PAGES == pNotifyInfoData->Field )
					//{
					//	*pJobPages = pNotifyInfoData->NotifyData.adwData[0];
					//}

					break;
				}
			}
		}
	}
}

// ---------------------------------------------
// Extract information about the print job directly from the job
// ---------------------------------------------
DWORD CGreenPrintMonitorThread::JobPagesFromJobId( HANDLE hPrinter, DWORD JobId )
{
	DWORD JobPages = 0;

	BOOL IsSpooling = FALSE;
	DWORD SpoolingTime = 0;

	do
	{
		DWORD cbNeeded = 0;

		if ( !GetJob(
				hPrinter,
				JobId,
				1,
				NULL,
				0,
				&cbNeeded ) )
		{
			JOB_INFO_1 *pJobInfo = (JOB_INFO_1 *)LocalAlloc( LMEM_ZEROINIT, cbNeeded );

			if ( NULL != pJobInfo )
			{
				if ( GetJob(
						hPrinter,
						JobId,
						1,
						(LPBYTE)pJobInfo,
						cbNeeded,
						&cbNeeded ) )
				{
					JobPages = pJobInfo->TotalPages;

					// ---------------------------------------------
					// Note: depending on the size of the document it can make minutes to entirely spool, 
					// so we check the state before returning so we get an accurate page count
					// ---------------------------------------------
					IsSpooling = pJobInfo->Status & JOB_STATUS_SPOOLING;
				}

				LocalFree( pJobInfo );
				pJobInfo = NULL;
			}
		}

		// ---------------------------------------------
		// We won't wait forever for a document to spool since we can't trust windows
		// ---------------------------------------------
		if ( IsSpooling )
		{
			Sleep( 500 );
			SpoolingTime += 500;
		}
	}
	while ( IsSpooling && SpoolingTime < ( MAX_SPOOLING_TIME_IN_MINS * ( 60 * 1000 ) ) );

	return JobPages;
}

// ---------------------------------------------
// Start an external user interface application with the page count on the command line that 
// asks the user if they would really like to print. If the user interface application's exit 
// code is 0 they do not want to print the job, anything else we assume we should resume.
// ---------------------------------------------
BOOL CGreenPrintMonitorThread::ShouldResumePrintJob( DWORD JobPages )
{
	BOOL ResumeJob = TRUE;

	WCHAR UICommand[ MAX_PATH + 1 ];
	ZeroMemory( UICommand, sizeof(UICommand) );

	if ( 0 != GetModuleFileName( NULL, UICommand, _countof(UICommand) ) )
	{
		if ( PathRemoveFileSpec( UICommand ) )
		{
			if ( PathAppend( UICommand, UI_COMMAND_APP_NAME ) )
			{
				WCHAR UICommandLine[ MAX_PATH ];
				ZeroMemory( &UICommandLine, sizeof(UICommandLine) );

				if ( SUCCEEDED( StringCchPrintf( UICommandLine, _countof(UICommandLine), L" /pagecount:%d", JobPages ) ) )
				{
					STARTUPINFO si = { 0 };
					si.cb = sizeof(si);

					PROCESS_INFORMATION pi = { 0 };

					if ( CreateProcess( UICommand, 
						UICommandLine,
						NULL,
						NULL,
						FALSE,
						0,
						NULL,
						NULL,
						&si,
						&pi ) )
					{
						WaitForSingleObject( pi.hProcess, INFINITE );

						DWORD ExitCode = 0;
						if ( GetExitCodeProcess( pi.hProcess, &ExitCode ) )
						{
							if ( 0 == ExitCode )
							{
								ResumeJob = FALSE;
							}
						}

						CloseHandle( pi.hProcess );
						CloseHandle( pi.hThread );
					}
				}
			}
		}
	}

	return ResumeJob;
}
