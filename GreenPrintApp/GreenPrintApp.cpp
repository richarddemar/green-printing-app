#include <windows.h>
#include "resource.h"

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

WCHAR Message[ 256 ] = { 0 };

INT APIENTRY wWinMain( HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LPWSTR pPageTotal = NULL;

	INT cchArgs = 0;
	LPWSTR *Args = CommandLineToArgvW( GetCommandLine(), &cchArgs );

	if ( NULL != Args )
	{
		for(int i = 0; i < cchArgs; i++)
		{
			pPageTotal = wcsstr( Args[i], L"/pagecount:" );
			
			if ( NULL != pPageTotal )
			{
				pPageTotal += 11; // "/pagecount:"
			}
		}
	}

	if ( NULL != pPageTotal )
	{
		wcscat_s( Message, _countof(Message), L"You are about to print " );
		wcscat_s( Message, _countof(Message), pPageTotal );
		wcscat_s( Message, _countof(Message), L" pages.\r\n\r\n" );
		wcscat_s( Message, _countof(Message), L"Users are responsible for paying for all their printed pages.\r\nPlease see a staff person for assistance with printing.\r\n\r\nClick OK to print." );
	}
	else
	{
		wcscat_s( Message, _countof(Message), L"Users are responsible for paying for all their printed pages.\r\nPlease see a staff person for assistance with printing.\r\n\r\nClick OK to print." );
	}

	if ( NULL != Args )
	{
		LocalFree( Args );
		Args = NULL;
	}

	if ( IDOK == DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_PROMPT), NULL, DialogProc) )
	{
		return 1;
	}

	return 0;
}

// ---------------------------------------------
// Message handler for dialog box.
// ---------------------------------------------
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowText( GetDlgItem( hDlg, IDC_STATIC_PROMPT ), Message );
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
