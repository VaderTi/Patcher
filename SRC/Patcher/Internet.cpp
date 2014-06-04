#include "StdAfx.h"
#include "Internet.h"

CInternet::CInternet(void)
{
	ZeroMemory(&m_URLC, sizeof(URL_COMPONENTS));

	m_URLC.dwStructSize = sizeof(URL_COMPONENTS);
	m_URLC.dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;
	m_URLC.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
	m_URLC.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
	m_URLC.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
	m_URLC.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	m_URLC.dwExtraInfoLength = INTERNET_MAX_PATH_LENGTH;

	IsConnected = FALSE;

	m_URL = NULL;
	m_hFile = NULL;
	m_hOpen = NULL;
	m_hConnect = NULL;
	m_hRequest = NULL;
	m_OnWorkBegin = NULL;
	m_OnWork = NULL;
	m_pData = NULL;

	m_dwReadSize = 16*1024;
	m_dwLastError = 0;
	m_dwFileSize = 0;
}

CInternet::~CInternet(void)
{
	Disconnect();
}

int CInternet::Connect(LPCTSTR lpszUrl)
{
	CString HostUrl, Login, Password, Err;

	InternetCrackUrl(lpszUrl, _tcslen(lpszUrl), 0, &m_URLC);

	HostUrl.Append(m_URLC.lpszHostName, m_URLC.dwHostNameLength);
	if (m_URLC.dwUserNameLength)
		 Login.Append(m_URLC.lpszUserName, m_URLC.dwUserNameLength);
	if (m_URLC.dwPasswordLength)
		 Password.Append(m_URLC.lpszPassword, m_URLC.dwPasswordLength);

	if (!(m_hOpen = InternetOpen(_T("KPatcher"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)))
	{
		Err.Format(_T("Err:WinInet:InternetOpen. Error - %u"), GetLastError());
		throw Err;
	}

	if (!(m_hConnect = InternetConnect(m_hOpen, HostUrl, m_URLC.nPort, Login,
										Password, m_URLC.nScheme, INTERNET_FLAG_PASSIVE, NULL )))
	{
		Err.Format(_T("Err:WinInet:InternetConnect. Address:Port - %s:%d .Error ID - %u."),
			HostUrl, m_URLC.nPort, GetLastError());
		throw Err;
	}

	if (m_URLC.nScheme == INTERNET_SCHEME_FTP)
		FtpSetCurrentDirectory(m_hConnect, m_URLC.lpszUrlPath);

	IsConnected = TRUE;

	return 1;
}

void CInternet::Disconnect()
{
	if (m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}
	if (m_hConnect)
	{
		InternetCloseHandle(m_hConnect);
		m_hConnect = NULL;
	}
	if (m_hOpen)
	{
		InternetCloseHandle(m_hOpen);
		m_hOpen = NULL;
	}

	ZeroMemory(&m_URLC, sizeof(URL_COMPONENTS));

	m_URLC.dwStructSize = sizeof(URL_COMPONENTS);
	m_URLC.dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;
	m_URLC.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
	m_URLC.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
	m_URLC.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
	m_URLC.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	m_URLC.dwExtraInfoLength = INTERNET_MAX_PATH_LENGTH;

	IsConnected = FALSE;

	m_dwLastError = 0;
	m_dwFileSize = 0;
}

int CInternet::Download(LPCTSTR lpszFileName, BOOL Patch /* = TRUE */)
{
	CString Url, SName, Header = _T("Accept: */*\r\n"), Err, Command;
	if (Patch)
	{
		SName = _T("./updates/");
		SName += lpszFileName;
	}
	else SName = lpszFileName;
	DWORD dwCT = 0,
		  dwFileSize = 0,
		  dwBytesReaded = 1,
		  dwBytesWritten = 0,
		  dwBytesReadedTotal = 0,
		  dwLastSpeedTick = 0,
		  dwLastUpdateTick = 0,
		  dwDiffSpeedTick = 0,
		  dwDiffUpdateTick = 0,
		  dwSpeedRate = 0,
		  dwErrCount = 0,
		  dwSize = 4;

	HINTERNET hFind = nullptr;
	WIN32_FIND_DATA Data;

	LARGE_INTEGER liSize = {0}, liNewPos = {0};

	float Speed = 0.;

	BOOL Ret = FALSE;

	PVOID pData = nullptr;

	if ((GetFileAttributes(SName) != INVALID_FILE_ATTRIBUTES) && Patch)
	{
		m_hFile = CreateFile(SName, GENERIC_WRITE, FILE_SHARE_READ,
							NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		GetFileSizeEx(m_hFile, &liSize);
		SetFilePointerEx(m_hFile, liSize, &liNewPos, FILE_BEGIN);
		dwBytesReadedTotal = liSize.LowPart;
	}
	else
	{
		m_hFile = CreateFile(SName, GENERIC_WRITE, FILE_SHARE_READ,
							NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_dwLastError = GetLastError();
		Err.Format(_T("Err:Kernel:CreateFile. File - '%s'. Error - %u."),
			SName, m_dwLastError);
		throw Err;
	}

	if (m_URLC.nScheme == INTERNET_SCHEME_FTP)
	{
		Command.Format(_T("REST %u"), dwBytesReadedTotal);
	}

	switch (m_URLC.nScheme)
	{
	case INTERNET_SCHEME_HTTP:
		Url = m_URLC.lpszUrlPath;
		Url += lpszFileName;
		m_hRequest = HttpOpenRequest(m_hConnect, NULL, Url,
									 NULL, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE|
									 INTERNET_FLAG_PRAGMA_NOCACHE, NULL);
		if (!m_hRequest)
		{
			Err.Format(_T("Err:WinInet:HttpOpenRequest. Request - '%s'. Error - %u."),
				Url, GetLastError());
			throw Err;
		}

		Ret = HttpSendRequest(m_hRequest, Header, Header.GetLength(), NULL, 0);
		if (!Ret)
		{
			Err.Format(_T("Err:WinInet:HttpSendRequest. Request - '%s'. Error - %u."),
				Url, GetLastError());
			throw Err;
		}

		Ret = HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH|
							 HTTP_QUERY_FLAG_NUMBER, &dwFileSize, &dwSize, 0);
		if (!Ret)
		{
			Err.Format(_T("Err:WinInet:HttpQueryInfo. Request - '%s'. Error - %u."),
				Url, GetLastError());
			throw Err;
		}
		break;

	case INTERNET_SCHEME_FTP:
Repeat:
		hFind = FtpFindFirstFile(m_hConnect, lpszFileName, &Data, NULL, NULL );
		if (!hFind)
		{
			InternetCloseHandle(hFind);
			if (GetLastError() == ERROR_INTERNET_EXTENDED_ERROR)
			{
				if (dwErrCount < 3)
				{
					Sleep(250);
					dwErrCount++;
					goto Repeat;
				}
			}
			Err.Format(_T("Err:WinInet:FtpFindFirstFile. FileName - '%s'. Error - %u."),
				lpszFileName, GetLastError());
			throw Err;
		}
		InternetCloseHandle(hFind);
		dwFileSize = Data.nFileSizeLow;
		FtpCommand(m_hConnect, FALSE, FTP_TRANSFER_TYPE_BINARY, Command, NULL, NULL);
		m_hRequest = FtpOpenFile(m_hConnect, lpszFileName, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
		break;

	default:
		;
	}

	m_dwFileSize = dwFileSize;
	if (m_OnWork)
		if (dwFileSize == liSize.LowPart)
			goto Return;

	pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_dwReadSize);
	if (!pData)
	{
		m_dwLastError = GetLastError();
		wchar_t szError[MAX_PATH] = _T("");
		swprintf_s(szError, MAX_PATH, _T("Err:Kernel:HeapAlloc. Allocation Error!. Error - %u."), m_dwLastError);
		throw szError;
	}

	if (m_OnWorkBegin) m_OnWorkBegin(dwFileSize);

	dwLastUpdateTick = GetTickCount();
	dwLastSpeedTick = dwLastUpdateTick - 1000;
	while(dwBytesReaded)
	{
		if (InternetReadFile(m_hRequest, pData, m_dwReadSize, &dwBytesReaded))
		{
			if (!WriteFile(m_hFile, pData, dwBytesReaded, &dwBytesWritten, 0))
			{
				HeapFree(GetProcessHeap(), 0, pData);
				CloseHandle(m_hFile);
				m_dwLastError = GetLastError();
				wchar_t szError[MAX_PATH] = _T("");
				swprintf_s(szError, MAX_PATH, _T("Err:Kernel:WriteFile. Error - %u."), m_dwLastError);
				throw szError;
			}
		}
		if (!m_OnWork) continue;
		dwBytesReadedTotal += dwBytesReaded;
		dwSpeedRate += dwBytesReaded;

		dwCT = GetTickCount();
		dwDiffSpeedTick = dwCT - dwLastSpeedTick;
		dwDiffUpdateTick = dwCT - dwLastUpdateTick;
		//if (dwDiffTick < 1000)
		//{
		//	if( m_OnWork )
		//		if ( !m_OnWork(dwBytesReaded, dwBytesReadedTotal, dwFileSize, Speed, m_pData) )
		//			goto Return;
		//}
		//else
		if (dwDiffUpdateTick > 250)
		{
			if (dwDiffSpeedTick > 1000)
			{
				Speed = static_cast<float>(dwSpeedRate / (dwDiffSpeedTick/1000));
				dwSpeedRate = 0;
				dwLastSpeedTick = GetTickCount();
			}
			if( m_OnWork )
			{
				if ( !m_OnWork(dwBytesReaded, dwBytesReadedTotal, dwFileSize, Speed, m_pData) )
				{
					goto Return;
				}
			}
			dwLastUpdateTick = GetTickCount();
		}
	}
	Return:
	HeapFree(GetProcessHeap(), 0, pData);
	CloseHandle(m_hFile);
	InternetCloseHandle(m_hRequest);

	return dwFileSize;
}

void CInternet::CallBack(ONWORKBEGIN OnWorkBegin /* = NULL */, ONWORK OnWork /* = NULL */, void *pData /* = NULL */)
{
	m_OnWorkBegin = OnWorkBegin;
	m_OnWork = OnWork;
	m_pData = pData;
}
