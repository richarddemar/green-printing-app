
#include "GreenPrintMonitor.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;
HWND g_hWndEdit = NULL;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

VOID RegisterMainWindowClass(HINSTANCE hInstance);
BOOL CreateMainWindow(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL AddMainWindowNotifyIcon( HWND hWnd );
BOOL RemoveMainWindowNotifyIcon( HWND hWnd );
VOID ShowNotifyContextMenu(HWND hWnd, POINT pt);

// ---------------------------------------------
// Main entry point for the application
// ---------------------------------------------
INT APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ---------------------------------------------
	// Make sure there is only one instance of the monitor running
	// ---------------------------------------------
	HANDLE hEvent = CreateEvent( NULL, FALSE, FALSE, L"GreenPrintMonitorMutex" );

	if ( ERROR_ALREADY_EXISTS == GetLastError() )
	{
		return 0;
	}

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GREENPRINTMONITOR, szWindowClass, MAX_LOADSTRING);
	
	RegisterMainWindowClass(hInstance);

	if ( !CreateMainWindow(hInstance, nCmdShow) )
	{
		return FALSE;
	}

	CGreenPrintWindowLogging logging( g_hWndEdit );

	CGreenPrintMonitorThread monitorThread( &logging );
	monitorThread.Start();

	MSG msg;
	while ( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	monitorThread.Stop();

	if ( NULL != hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}

	return (int) msg.wParam;
}

// ---------------------------------------------
// Register the main / status window
// ---------------------------------------------
VOID RegisterMainWindowClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	ZeroMemory( &wcex, sizeof(wcex) );

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GREENPRINTMONITOR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GREENPRINTMONITOR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);
}

// ---------------------------------------------
// Create the main / status window hidden at first
// ---------------------------------------------
BOOL CreateMainWindow( HINSTANCE hInstance, INT nCmdShow )
{
	g_hInst = hInstance;

	g_hWnd = CreateWindow(
		szWindowClass, 
		szTitle, 
		(WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ),
		CW_USEDEFAULT, 0, 
		400, 600,
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	if ( NULL == g_hWnd )
	{
		return FALSE;
	}

	HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, L"Tahoma" );

	if ( NULL != hFont )
	{
		SendMessage( g_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage( g_hWndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
	}

	ShowWindow( g_hWnd, SW_HIDE );
	UpdateWindow( g_hWnd );

	return TRUE;
}

// ---------------------------------------------
// Message handler for the status / main window.
// ---------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		{
			AddMainWindowNotifyIcon( hWnd );

			g_hWndEdit = CreateWindowEx(
				0, 
				L"EDIT",
				NULL,
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
				0, 0, 0, 0,
				hWnd,
				(HMENU)IDC_STATUS,
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
				NULL );
		}
		break;
	case WM_SETFOCUS: 
		SetFocus(g_hWndEdit);
		return 0; 

	case WM_SIZE:
		{
			// ---------------------------------------------
			// Make the edit control the size of the window's client area.
			// ---------------------------------------------
			MoveWindow(g_hWndEdit, 
				0, 0,
				LOWORD(lParam),
				HIWORD(lParam),
				TRUE);
		}
		return 0; 
	case WMAPP_NOTIFYCALLBACK:
		switch (LOWORD(lParam))
		{
		case WM_CONTEXTMENU:
			{
				POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
				ShowNotifyContextMenu( hWnd, pt );
			}
			break;
		}
		break;
	case WM_COMMAND:
		{
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_STATUS:
				ShowWindow(g_hWnd, SW_SHOW);
				break;
			case IDM_CLOSE:
				ShowWindow(g_hWnd, SW_HIDE);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		RemoveMainWindowNotifyIcon(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ---------------------------------------------
// Message handler for about box.
// ---------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
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

// ---------------------------------------------
// Add the icon / button to the windows tray area
// ---------------------------------------------
BOOL AddMainWindowNotifyIcon( HWND hWnd )
{
	NOTIFYICONDATA nid;
	ZeroMemory( &nid, sizeof(nid) );

	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
	nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
	//LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDI_GREENPRINTMONITOR), LIM_SMALL, &nid.hIcon);
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_GREENPRINTMONITOR));
	LoadString(g_hInst, IDS_APP_TITLE, nid.szTip, ARRAYSIZE(nid.szTip));
	Shell_NotifyIcon(NIM_ADD, &nid);

	// NOTIFYICON_VERSION_4 is prefered if running on Vista or later
	nid.uVersion = NOTIFYICON_VERSION_4;
	return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

// ---------------------------------------------
// Remove the icon / button from the windows tray area
// ---------------------------------------------
BOOL RemoveMainWindowNotifyIcon( HWND hWnd )
{
	NOTIFYICONDATA nid = {sizeof(nid)};
	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = 1;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

// ---------------------------------------------
// Show the context menu in the windows tray area
// ---------------------------------------------
VOID ShowNotifyContextMenu(HWND hWnd, POINT pt)
{
	HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));

	if (hMenu)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, 0);

		if (hSubMenu)
		{
			// ---------------------------------------------
			// The window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
			// ---------------------------------------------
			SetForegroundWindow( hWnd );

			// ---------------------------------------------
			// Respect the menu drop alignment
			// ---------------------------------------------
			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				uFlags |= TPM_RIGHTALIGN;
			}
			else
			{
				uFlags |= TPM_LEFTALIGN;
			}

			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hWnd, NULL);
		}

		DestroyMenu(hMenu);
	}
}
