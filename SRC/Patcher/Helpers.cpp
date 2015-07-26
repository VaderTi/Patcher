#include "StdAfx.h"
#include "Patcher.h"
#include "Helpers.h"

#include "unrar.hpp"
#ifdef _DEBUG
#pragma comment(lib, "unrar_d")
#else
#pragma comment(lib, "unrar")
#endif

CString GetFileExt(const CString FileName)
{
	return FileName.Right(FileName.GetLength()-(FileName.ReverseFind('.')+1));
}

COLORREF HexToRGB(const CString Hex)
{
	CString R, G, B;
	DWORD iR = 0, iG = 0, iB = 0;

	for (int i = 0; i < 2; i++)
	{
		R += Hex[i];
		G += Hex[i+2];
		B += Hex[i+4];
	}

	swscanf_s(R, _T("%x"), &iR);
	swscanf_s(G, _T("%x"), &iG);
	swscanf_s(B, _T("%x"), &iB);

	return RGB(iR, iG, iB);
}

double MB_GB(double fSize)
{
	if (fSize < 1048576.00) return fSize/1024.00;
	return fSize/1048576.00;
}

const wchar_t *MB_GB_S(double fSize)
{
	if (fSize < 1048576.00) return Patcher.m_Language.KByte;
	return Patcher.m_Language.MByte;
}

const wchar_t *MB_GB_SEC_S(double fSpeed)
{
	if (fSpeed < 1048576.00) return Patcher.m_Language.KBPS;
	return Patcher.m_Language.MBPS;
}

void ShowStatus(CString Msg, ...)
{
	if (Patcher.m_bExiting) return;
	CString str;
	va_list Args;
	va_start(Args, Msg);
	str.FormatV(Msg, Args);
	va_end(Args);
	Patcher.m_Skin.Labels[CONTROL::STATUS].Label->SetText(str);
}

void ShowInfo(CString Msg, ...)
{
	if (Patcher.m_bExiting) return;
	CString str;
	va_list Args;
	va_start(Args, Msg);
	str.FormatV(Msg, Args);
	va_end(Args);
	Patcher.m_Skin.Labels[CONTROL::INFO].Label->SetText(str);
}

void CProgressPos(int Pos, BOOL Add /* = FALSE */)
{
	if (!Patcher.m_Skin.PBars[CONTROL::CPROGRESS].ProgressBar) return;
	auto iPos = Pos;
	if (Add) iPos = Patcher.m_Skin.PBars[CONTROL::CPROGRESS].ProgressBar->GetPos() + Pos;
	Patcher.m_Skin.PBars[CONTROL::CPROGRESS].ProgressBar->SetPos(iPos);
}
void CProgressMax(int Max)
{
	if (!Patcher.m_Skin.PBars[CONTROL::CPROGRESS].ProgressBar) return;
	Patcher.m_Skin.PBars[CONTROL::CPROGRESS].ProgressBar->SetRange32(0, Max);
}
void TProgressPos(int Pos)
{
	if (!Patcher.m_Skin.PBars[CONTROL::TPROGRESS].ProgressBar) return;
	Patcher.m_Skin.PBars[CONTROL::TPROGRESS].ProgressBar->SetPos(Pos);
}
void TProgressMax(int Max)
{
	if (!Patcher.m_Skin.PBars[CONTROL::TPROGRESS].ProgressBar) return;
	Patcher.m_Skin.PBars[CONTROL::TPROGRESS].ProgressBar->SetRange32(0, Max);
}

void OnDownloadBegin(DWORD dwWorkMax)
{
	ShowInfo(Patcher.m_Language.DownloadProcess, MB_GB(0.), MB_GB_S(0.),
		MB_GB(dwWorkMax), MB_GB_S(dwWorkMax), MB_GB(0.), MB_GB_SEC_S(0.));
	CProgressPos(0);
	CProgressMax(dwWorkMax);
}

bool OnDownload(DWORD dwWork, DWORD dwWorkTotal, DWORD dwWorkMax, FLOAT Speed, LPVOID pData)
{
	if (Patcher.m_bExiting) return false;

	ShowInfo(Patcher.m_Language.DownloadProcess, MB_GB(dwWorkTotal), MB_GB_S(dwWorkTotal),
		MB_GB(dwWorkMax), MB_GB_S(dwWorkMax), MB_GB(Speed), MB_GB_SEC_S(Speed));
	CProgressPos(dwWorkTotal);
	return true;
}

bool OnGRF(const char *FileName, unsigned int Current, unsigned int Total, void *pData)
{
	if (Patcher.m_bExiting) return false;

	CProgressMax(Total);
	CProgressPos(Current);
	return true;
}

void DeleteFiles(CString Delete)
{
	CString FindData, Dir, Expression, Tmp;
	int iDelim = 0;
	if ((iDelim = Delete.ReverseFind(_T('/'))) == -1)
		iDelim = Delete.ReverseFind(_T('\\'));
	if (iDelim != -1)
	{
		iDelim += 1;
		Dir.Append(Delete, iDelim);
		Expression.Append(Delete.GetBuffer()+iDelim, Delete.GetLength() - iDelim);
	}

	WIN32_FIND_DATA FindFileData;

	BOOL bRet = TRUE;
	FindData += Delete;
	HANDLE hFind = FindFirstFile(FindData, &FindFileData);

	while (hFind != INVALID_HANDLE_VALUE && bRet)
	{
		if (( !_wcsicmp(FindFileData.cFileName, _T(".")) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )||
			( !_wcsicmp(FindFileData.cFileName, _T("..")) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ))
		{
			bRet = FindNextFile(hFind, &FindFileData);
			continue;
		}

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			Tmp = Dir; Tmp += FindFileData.cFileName; Tmp += _T("\\"); Tmp += Expression;
			DeleteFiles(Tmp);
			Tmp = Patcher.m_AppPath; Tmp += Dir; Tmp += FindFileData.cFileName; Tmp += _T("\\");
			RemoveDirectory(Tmp);
		}
		else
		{
			Tmp.Format(_T("%s%s%s"), Patcher.m_AppPath, Dir, FindFileData.cFileName);
			DeleteFile(Tmp);
		}
		bRet = FindNextFile(hFind, &FindFileData);
	}
	FindClose(hFind);
}

//void SelfUpdate()
//{
//	CIni Ini;
//	CInternet Inet;
//	CStringArray Array;
//	DWORD dwHeaderSum = 0, dwCheckSum = 0;//, dwFileSize = 0;
//	DWORD CRC = 0;
//	CString File, oFile, oldPE;
//	BOOL Restart = FALSE;
//
//	if (!Ini.LoadFile(Patcher.Settings->RemoteUpdate))
//	{
//		DeleteFile(Patcher.Settings->RemoteUpdate);
//		return;
//	}
//	Ini.GetSections(Array);
//	DeleteFile(Patcher.Settings->RemoteUpdate);
//
//	TProgressMax(Array.GetCount());
//	TProgressPos(0);
//
//	oldPE = Patcher.m_pszAppName; oldPE += _T(".old");
//	DeleteFile(oldPE);
//	Inet.Connect(Patcher.Settings->PatchServer[MAIN_SERVER].FileServerURL);
//	Inet.CallBack(OnDownloadBegin, OnDownload);
//	for (int i = 0; i < Array.GetCount(); i++)
//	{
//		TProgressPos(i+1);
//		Restart = FALSE;
//		Ini.SetSection(Array[i]);
//		CRC = Ini.GetInt(_T("CRC"), 0);
//		oFile = Ini.GetString(_T("File"), _T(""));
//		if (!CRC || oFile.GetLength() < 4) continue;
//
//		if (Array[i] == _T("Patcher"))
//		{
//			Restart = TRUE;
//			File = Patcher.m_pszExeName, File += _T(".exe");
//		}
//		else if (Array[i] == _T("Client"))
//			File = Patcher.Settings->ExeName;
//		else
//			File = Array[i];
//
//		MapFileAndCheckSum(File, &dwHeaderSum, &dwCheckSum);
//		ShowStatus(Patcher.m_Language.DownloadFile, oFile, i+1, Array.GetCount());
//		if ( CRC != dwCheckSum )
//		{
//			if (Restart)
//				CFile::Rename(File, oldPE);
//
//			CProgressPos(0);
//			Inet.Download(oFile, FALSE);
//			ExtractRAR(oFile);
//			DeleteFile(oFile);
//
//			if (Restart) break;
//		}
//	}
//	Inet.Disconnect();
//
//	if (Restart)
//	{
//		STARTUPINFOW si = {0};
//		PROCESS_INFORMATION pi = {0};
//		si.cb = sizeof(STARTUPINFOW);
//		si.dwFlags = STARTF_USESHOWWINDOW;
//		si.wShowWindow = SW_SHOWNORMAL;
//
//		CreateProcess(NULL, (wchar_t *)File.GetString(), NULL, NULL, false, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi);
//		Patcher.MainFrm->EndDialog(IDOK);
//	}
//
//	CProgressMax(1); TProgressMax(1);
//	CProgressPos(1); TProgressPos(1);
//}

void RunClient(const CString ExeName, const CString ExeParam, const CString Login /* = _T */, const CString Password /* = _T */)
{
	CString Start;
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOW));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;

	Start = ExeName;
	if (!Login.IsEmpty() && !Password.IsEmpty())
		Start += _T(" -t:") + Password + _T(" ") + Login;
	Start += _T(" ") + ExeParam;

	CreateProcess(nullptr, const_cast<LPTSTR>(Start.GetString()), nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &si, &pi);
	Sleep(500);
	Patcher.m_MainForm.EndDialog(IDOK);
}

void RunCmd(const CString FirstParam, const CString SecondParam)
{
	if (!FirstParam.IsEmpty())
	{
		ShellExecute(nullptr, _T("open"), FirstParam, SecondParam, nullptr, SW_SHOW);
	}
}

int CALLBACK Callback(UINT msg, LPARAM UserData, LPARAM P1, LPARAM P2)
{
	if (msg == UCM_PROCESSDATA)
		CProgressPos(P2, TRUE);
	return 0;
}

uint32_t UnRAR(CString RarFile)
{
	auto pszName = RarFile.GetBuffer();

	HANDLE hData;
	RARHeaderData				HeaderData;
	ZeroMemory(&HeaderData, sizeof(HeaderData));
	RAROpenArchiveDataEx		OpenArchiveData;
	ZeroMemory(&OpenArchiveData, sizeof(OpenArchiveData));

	OpenArchiveData.ArcNameW = pszName;
	OpenArchiveData.OpenMode = RAR_OM_LIST;
	auto iTotalUnpackSize = 0;

	CProgressPos(0);

	if ((hData = RAROpenArchiveEx(&OpenArchiveData)) == nullptr)
		return  0;

	while (!RARReadHeader(hData, &HeaderData))
	{
		iTotalUnpackSize += HeaderData.UnpSize;
		if (RARProcessFile(hData, RAR_SKIP, nullptr, nullptr))
		{
			RARCloseArchive(hData);
			return 0;
		}
	}
	RARCloseArchive(hData);

	CProgressMax(iTotalUnpackSize);

	ZeroMemory(&HeaderData, sizeof(HeaderData));
	ZeroMemory(&OpenArchiveData, sizeof(OpenArchiveData));

	OpenArchiveData.ArcNameW = pszName;
	OpenArchiveData.OpenMode = RAR_OM_EXTRACT;

	if ((hData = RAROpenArchiveEx(&OpenArchiveData)) == nullptr)
		return 0;

	RARSetCallback(hData, Callback, NULL);
	while (!RARReadHeader(hData, &HeaderData))
	{
		if (RARProcessFile(hData, RAR_EXTRACT, nullptr, nullptr))
		{
			RARCloseArchive(hData);
			return 0;
		}
	}
	RARCloseArchive(hData);
	return 1;
}
