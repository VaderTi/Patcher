#include "StdAfx.h"
#include "GrfLib.h"
#ifdef _DEBUG
#pragma comment(lib, "grflib_d")
#else
#pragma comment(lib, "grflib")
#endif
#include "PatchThread.h"

CPatchThread::CPatchThread(void)
{
	m_LastPatchID = 0;
	m_PatchesCount = 0;
	m_IsMain = FALSE;
}


CPatchThread::~CPatchThread(void)
{
	Clear();
}

void CPatchThread::Clear()
{
	m_LastPatchID = 0;
	m_PatchesCount = 0;
	m_IsMain = FALSE;
	m_PatchList.clear();
}

void CPatchThread::LoadSettings(_SERVERINFO& Server, BOOL IsMain)
{
	DbgMsg(_T("Loading patch server settings:\t"));
	m_IsMain = IsMain;
	m_Server = Server;
	CFile File;

	BOOL IsExist = File.Open(m_Server.GRFNAME, CFile::modeReadWrite);
	if (IsExist) File.Close();

	if (m_IsMain)
	{
		if (!IsExist)
		{
			GrfFile Grf = GrfCreate(CStringA(m_Server.GRFNAME));
			GrfFree(Grf);
		}
		CIni Ini;
		Ini.LoadFile(m_Server.PATCHINF);

		Ini.SetSection(Patcher.m_Settings.m_rSettings.SERVER_UID);
		m_LastPatchID = Ini.GetInt(_T("LastPatchID"), 0);
	}
	else
	{

		if (!IsExist)
		{
			CString Error;
			Error.Format(Patcher.m_Language.FileNotFound, m_Server.GRFNAME);
			throw Error;
		}
		if (File.Open(Server.PATCHINF, CFile::modeRead))
			File.Read(&m_LastPatchID, 4), File.Close();
	}
	DbgMsg(_T("Completed.\n"));
}

BOOL CPatchThread::GetPatchList()
{
	if (Patcher.m_bExiting) return FALSE;

	CInternet Inet;
	ShowStatus(Patcher.m_Language.Connecting, m_Server.PATCHLISTURL);
	ShowInfo(_T(""));

	DbgMsg(_T("Connecting to \"%s\":\t"), m_Server.PATCHLISTURL);
	Inet.Connect(m_Server.PATCHLISTURL);
	DbgMsg(_T("Connected.\n"));

	DbgMsg(_T("Downloading patchlist \"%s\":\t"), m_Server.PATHLISTNAME);
	auto iSize = Inet.Download(m_Server.PATHLISTNAME, FALSE);
	DbgMsg(_T("Downloaded %d bytes.\n"), iSize);

	return TRUE;
}

BOOL CPatchThread::ParsePatchList()
{
	if (Patcher.m_bExiting) return FALSE;

	wfstream PatchList;
	int iRet = 0, iPatchNum = 0;
	CString Str;

	PatchList.open(m_Server.PATHLISTNAME, wfstream::in);
	if (!PatchList.is_open()) return FALSE;

	unique_ptr<wchar_t[]>	pszLine(new wchar_t[512]),
							pszPatchNum(new wchar_t[8]),
							pszPatchName(new wchar_t[128]),
							pszPatchType(new wchar_t[32]),
							pszPatchGrf(new wchar_t[32]);

	_PATCH Patch;
	DbgMsg(_T("Parsing patchlist...\t"));
	for (PatchList.getline(pszLine.get(), 512); !PatchList.eof(); PatchList.getline(pszLine.get(), 512))
	{
		if (Patcher.m_bExiting) return FALSE;

		/// Пропускаем комментарии, пробелы и прочие символы
		if (pszLine.get()[0] == _T('')||
			(pszLine.get()[0] == _T('/') && pszLine.get()[1] == _T('/'))||
			iswspace(pszLine.get()[0]) || pszLine.get()[0] == _T(';')||
			pszLine.get()[0] == _T('#') || pszLine.get()[0] == _T('\n')) continue;

		if (m_IsMain) iRet = swscanf_s(pszLine.get(), _T("%8[^:]:%32[^:]:%32[^:\r\n]:%32[^\r\n]s"),
			pszPatchNum.get(), 8, pszPatchType.get(), 32, pszPatchName.get(), 128, pszPatchGrf.get(), 32);
		else iRet = swscanf_s(pszLine.get(), _T("%8s %128[^\r\n]s"), pszPatchNum.get(), 8, pszPatchName.get(), 128);

		if (iRet == EOF) break;

		swscanf_s(pszPatchNum.get(), _T("%d"), &iPatchNum);

		if (iPatchNum <= m_LastPatchID) continue;

		switch(iRet)
		{
		case 4:
			Patch.iPatchID = iPatchNum;
			Patch.PatchType = pszPatchType.get();
			Patch.PatchName = pszPatchName.get();
			Patch.PatchGrf = pszPatchGrf.get();
			break;

		case 3:
			Patch.iPatchID = iPatchNum;
			Patch.PatchType = pszPatchType.get();
			Patch.PatchName = pszPatchName.get();
			Patch.PatchGrf = m_Server.GRFNAME;
			break;

		case 2:
			Patch.iPatchID = iPatchNum;
			Str = szUpdates; Str += pszPatchName.get();
			Patch.PatchType = GetFileExt(pszPatchName.get());
			Patch.PatchName = pszPatchName.get();
			Patch.PatchGrf = m_Server.GRFNAME;
			break;

		default:
			break;
		}

		if (!Patch.PatchType.CompareNoCase(_T("grf")) || !Patch.PatchType.CompareNoCase(_T("gpf")))
			Patch.iPatchType = GPF;
		else if (!Patch.PatchType.CompareNoCase(_T("rgz")))
			Patch.iPatchType = RGZ;
		else if (!Patch.PatchType.CompareNoCase(_T("rar")))
			Patch.iPatchType = RAR;
		else if (!Patch.PatchType.CompareNoCase(_T("gdf")))
			Patch.iPatchType = GDF;
		else if (!Patch.PatchType.CompareNoCase(_T("cdf")))
			Patch.iPatchType = CDF;
		else
		{
			CString Error;
			Error.Format(_T("Unknown patch type %s"), Patch.PatchType);
			throw Error;
			return FALSE;
		}

		m_PatchList.push_back(Patch);
	}
	PatchList.close();
	DeleteFile(m_Server.PATHLISTNAME);
	m_PatchesCount = m_PatchList.size();
	DbgMsg(_T("Parsed %d entrie(s).\n"), m_PatchesCount);

	return TRUE;
}

BOOL CPatchThread::DownloadPatches()
{
	if (Patcher.m_bExiting) return FALSE;
	if (!m_PatchesCount) return TRUE;

	CInternet Inet;
	if (!Inet.Connect(m_Server.PATCHSERVERURL)) return FALSE;
	Inet.CallBack(OnDownloadBegin, OnDownload);

	TProgressPos(0);
	TProgressMax(m_PatchesCount);

	auto Idx = m_PatchList.begin();
	auto End = m_PatchList.end();

	DbgMsg(_T("Downloading patches...\t\t"));
	int i = 1;
	for (; Idx != End; ++Idx, ++i)
	{
		if (Patcher.m_bExiting) return FALSE;

		TProgressPos(i);

		switch (Idx->iPatchType)
		{
		case GPF:
		case RGZ:
		case RAR:
			ShowStatus(Patcher.m_Language.DownloadFile, Idx->PatchName, i, m_PatchesCount);
			Inet.Download(Idx->PatchName);
			Sleep(100);
			break;
		}
	}
	DbgMsg(_T("Downloaded %d file(s).\n"), i-1);

	return TRUE;
}

void CPatchThread::SavePatchID(int& PatchID)
{
	if (m_IsMain)
	{
		//m_pIni->SetInt(_T("LastPatchID"), PatchID);
		//m_pIni->Save();

	}
	else
	{
		m_pFile->SeekToBegin();
		m_pFile->Write(&PatchID, 4);
	}
}

int CPatchThread::ProcessPatches()
{
	if (Patcher.m_bExiting) return 0;
	if (!m_PatchesCount) return m_PatchesCount;

	BOOL bGrf = FALSE;
	CString PatchLoc, PatchName, Err;

	DbgMsg(_T("Processing patches...\t"));
	if (m_IsMain)
	{
		m_pIni = new CIni;
		m_pIni->LoadFile(m_Server.PATCHINF);
		m_pIni->SetSection(Patcher.m_Settings.m_rSettings.SERVER_UID);
	}
	else
	{
		m_pFile = new CFile;
		m_pFile->Open(m_Server.PATCHINF, CFile::modeReadWrite|CFile::modeCreate);
	}


	GrfFile Grf = nullptr;

	ShowStatus(Patcher.m_Language.ApplyingPatches);

	TProgressPos(0);
	TProgressMax(m_PatchesCount);

	auto Patch = m_PatchList.begin();
	auto NPatch = Patch+1;
	auto End = m_PatchList.end();
	CString PrevGrf, NextGrf;
	int i = 1;
	for (; Patch != End; ++Patch, ++i)
	{
		if (Patcher.m_bExiting) break;
		TProgressPos(i);
		switch(Patch->iPatchType)
		{
		case GPF:
			ShowInfo(Patcher.m_Language.ApplyingProcess, Patch->PatchName, i, m_PatchesCount);
			if (PrevGrf != Patch->PatchGrf)
			{
				Grf = GrfOpen(CStringA(Patch->PatchGrf));
			}
			PatchLoc = szUpdates; PatchLoc += Patch->PatchName;
			GrfSetCallback(Grf, OnGRF);
			GrfMerge(Grf, CStringA(PatchLoc));
			PrevGrf = Patch->PatchGrf;
			if (NPatch != End)
				NextGrf = NPatch->PatchGrf;
			if (NPatch == End || NextGrf != Patch->PatchGrf)
				GrfFree(Grf);
			bGrf = TRUE;
			SavePatchID(Patch->iPatchID);
			break;

		case RGZ:
			ShowInfo(Patcher.m_Language.ApplyingProcess, Patch->PatchName, i, m_PatchesCount);
			PatchLoc = szUpdates; PatchLoc += Patch->PatchName;
			PatchName = _T("./"); PatchName += Patch->PatchName;
			CopyFile(PatchLoc, PatchName, FALSE);
			ExtractRGZ(CStringA(PatchName));
			DeleteFile(PatchName);
			SavePatchID(Patch->iPatchID);
			break;

		case RAR:
			ShowInfo(Patcher.m_Language.ApplyingProcess, Patch->PatchName, i, m_PatchesCount);
			PatchLoc = szUpdates; PatchLoc += Patch->PatchName;
			PatchName = _T("./"); PatchName += Patch->PatchName;
			CopyFile(PatchLoc, PatchName, FALSE);
			UnRAR(PatchName);
			DeleteFile(PatchName);
			SavePatchID(Patch->iPatchID);
			break;

		case GDF:
			ShowInfo(Patcher.m_Language.ApplyingProcess, Patch->PatchName, i, m_PatchesCount);
			Grf = GrfOpen(CStringA(Patch->PatchGrf));
			GrfDelete(Grf, CStringA(Patch->PatchName));
			GrfFree(Grf);
			bGrf = TRUE;
			SavePatchID(Patch->iPatchID);
			break;

		case CDF:
			DeleteFiles(Patcher.m_AppPath + Patch->PatchName);
			SavePatchID(Patch->iPatchID);
			break;
		}
		if (NPatch != End)
			++NPatch;
	}
	if (m_IsMain)
		delete m_pIni;
	else
	{
		m_pFile->Close();
		delete m_pFile;
	}
	DbgMsg(_T("Processed %d file(s)\n"), i-1);

	//if (!Patcher.m_bExiting && bGrf)
	//{
	//	ShowInfo(_T(""));
	//	unsigned FreeSpace = 0, MaxFreeSpace = 0;
	//	CString PrevGRF;
	//	auto Patch = m_PatchList.begin();
	//	DbgMsg(_T("GRF file defragmentation..."));
	//	for (; Patch != m_PatchList.end(); ++Patch)
	//	{
	//		if (Patcher.m_bExiting) break;
	//		if (PrevGRF == Patch->PatchGrf) continue;
	//		if (Patch->iPatchType != GPF && Patch->iPatchType != GDF) continue;

	//		PrevGRF = Patch->PatchGrf;
	//		ShowStatus(Patcher.m_Language.Defragmentation, PrevGRF);

	//		//Grf->Open(PrevGRF);

	//		MaxFreeSpace = static_cast<int>(Grf->GetSize()*(Patcher.m_Settings->DefragPercent/100.00));
	//		if (Grf->WastedSpace() >= MaxFreeSpace)
	//		{
	//			//Grf->SetCallback(OnGRF);
	//			//Grf->Repack();
	//		}
	//		//Grf->Close();
	//	}
	//	DbgMsg(_T("Done. Defragmented."));
	//}
	return m_PatchesCount;
}
