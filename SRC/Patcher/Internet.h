#pragma once

#include <WinInet.h>
#pragma comment(lib, "wininet.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

typedef void (*ONWORKBEGIN)(DWORD dwWorkMax);
typedef bool (*ONWORK)(DWORD dwWork, DWORD dwWorkTotal, DWORD dwWorkMax, float Speed, void *pData);

class CInternet
{
public:
	CInternet(void);
	~CInternet(void);

	int Connect(const wchar_t *lpszUrl);
	int Download(const wchar_t *lpszFileName, BOOL Patch = TRUE);
	void Disconnect();
	void CallBack(ONWORKBEGIN OnWorkBegin = NULL, ONWORK OnWork = NULL, void *pData = NULL);

	DWORD m_dwFileSize;
	BOOL IsConnected;

private:
	URL_COMPONENTS m_URLC;
	wchar_t *m_URL;
	HANDLE m_hFile;

	HINTERNET m_hOpen,
			  m_hConnect,
			  m_hRequest;

	DWORD m_dwReadSize,
		  m_dwLastError;

	ONWORKBEGIN m_OnWorkBegin;
	ONWORK m_OnWork;
	PVOID m_pData;
};
