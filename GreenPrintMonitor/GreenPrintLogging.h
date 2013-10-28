#include <windows.h>

#pragma once

class IGreenPrintLogging
{
public:
	virtual VOID Log( LPWSTR Text ) = 0;
};

class CGreenPrintWindowLogging : public IGreenPrintLogging
{
public:
	CGreenPrintWindowLogging(HWND hWndEdit) : m_hWndEdit(hWndEdit) { }
	virtual ~CGreenPrintWindowLogging(void) { }

	virtual VOID Log( LPWSTR Text );

private:
	HWND m_hWndEdit;
};
