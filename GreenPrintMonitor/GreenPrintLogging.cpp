#include "GreenPrintLogging.h"
#include <strsafe.h>

// ---------------------------------------------
// Append the text to the logging window
// ---------------------------------------------
VOID CGreenPrintWindowLogging::Log( LPWSTR Text )
{
	DWORD ErrorCode = GetLastError();

	if ( NULL != Text && NULL != m_hWndEdit )
	{
		SYSTEMTIME CurrentTimeUTC;
		GetSystemTime( &CurrentTimeUTC );

		SYSTEMTIME CurrentTimeLocal;
		SystemTimeToTzSpecificLocalTime( NULL, &CurrentTimeUTC, &CurrentTimeLocal );

		WCHAR TimeText[ 128 ] = { 0 };
		StringCchPrintf( TimeText, _countof(TimeText), L"%02d/%02d/%d  %02d:%02d:%02d: ",
			CurrentTimeLocal.wMonth, CurrentTimeLocal.wDay, CurrentTimeLocal.wYear,
			CurrentTimeLocal.wHour, CurrentTimeLocal.wMinute, CurrentTimeLocal.wSecond );
		
		WCHAR ErrorText[ 64 ] = { 0 };
		if ( NO_ERROR != ErrorCode )
		{
			StringCchPrintf( ErrorText, _countof(ErrorText), L" (%d)", ErrorCode );
		}

		WCHAR *NewLine = L"\r\n";

		DWORD cchNewText = GetWindowTextLength( m_hWndEdit ) + wcslen(TimeText) + wcslen(Text) + wcslen(ErrorText) + wcslen(NewLine) + 1;

		WCHAR *NewText = (WCHAR*)LocalAlloc( LMEM_ZEROINIT, cchNewText * sizeof(WCHAR) );

		if ( NULL != NewText )
		{
			GetWindowText( m_hWndEdit, NewText, cchNewText );

			if ( SUCCEEDED( StringCchCat( NewText, cchNewText, TimeText ) ) )
			{
				if ( SUCCEEDED( StringCchCat( NewText, cchNewText, Text ) ) )
				{
					if ( SUCCEEDED( StringCchCat( NewText, cchNewText, ErrorText ) ) )
					{
						if ( SUCCEEDED( StringCchCat( NewText, cchNewText, NewLine ) ) )
						{
							SetWindowText( m_hWndEdit, NewText );
						}
					}
				}
			}

			LocalFree( NewText );
			NewText = NULL;
		}
	}
}