#include "StdAfx.h"
#include "Thread.h"

UINT __cdecl PatchThread(LPVOID pParam)
{
	Patcher.m_bWorking = TRUE;
	auto Idx = Patcher.m_Skin.Buttons.begin();
	auto End = Patcher.m_Skin.Buttons.end();
	for (; Idx != End; ++Idx)
		if (Idx->second.Button && Idx->second.Action != APP_RUN && Idx->second.Action != URL_OPEN && Idx->second.Action != KP_CLOSE)
			Idx->second.Button->Disable(TRUE);

	auto PatchType = reinterpret_cast<int>(pParam);
	auto PatchCount = 0;

	CPatchThread Patch;

	CreateDirectory(szUpdates, nullptr);

	if (PatchType&1)
	{
		//AutoUpdate();
		Patch.LoadSettings(Patcher.m_Settings.m_rSettings.SERVERINFO[SERVERTYPE::SERVER_MAIN]);
		Patch.GetPatchList();
		Patch.ParsePatchList();
		Patch.DownloadPatches();
		PatchCount += Patch.ProcessPatches();
		Patch.Clear();
	}
	ShowStatus(_T(""));
	ShowInfo(_T(""));
	//if (PatchType&2 && Patcher.m_Settings.m_rSettings.SERVERINFO[SERVERTYPE::SERVER_KRO].STARTTYPE)
	//{
	//	Patch.LoadSettings(Patcher.m_Settings.m_rSettings.SERVERINFO[SERVERTYPE::SERVER_KRO], FALSE);
	//	Patch.GetPatchList();
	//	Patch.ParsePatchList();
	//	Patch.DownloadPatches();
	//	PatchCount += Patch.ProcessPatches();
	//	Patch.Clear();
	//}
	ShowStatus(_T(""));
	ShowInfo(_T(""));
	//if (PatchType&4 && Patcher.Settings->PatchServer[KRORE_SERVER].StartType)
	//{
	//	Patch.LoadSettings(Patcher.Settings->PatchServer[KRORE_SERVER], FALSE);
	//	Patch.GetPatchList();
	//	Patch.ParsePatchList();
	//	Patch.DownloadPatches();
	//	PatchCount += Patch.ProcessPatches();
	//	Patch.Clear();
	//}

	if (Patcher.m_bExiting)
	{
		Patcher.m_MainForm.EndDialog(IDCANCEL);
		return 0;
	}

	Patcher.m_bWorking = FALSE;

	Idx = Patcher.m_Skin.Buttons.begin();
	End = Patcher.m_Skin.Buttons.end();
	for (; Idx != End; ++Idx)
		if (Idx->second.Button && Idx->second.Action != APP_RUN && Idx->second.Action != URL_OPEN && Idx->second.Action != KP_CLOSE)
			Idx->second.Button->Disable(FALSE);

	CProgressMax(1), CProgressPos(1);
	TProgressMax(1), TProgressPos(1);
	if (PatchCount)
	{
		ShowStatus(Patcher.m_Language.PatchComplete);
		ShowInfo(_T(""));
	}
	else
	{
		ShowStatus(Patcher.m_Language.NoPatches);
		ShowInfo(_T(""));
	}
	DbgMsg(_T("Cleanup:\t\t\t\t"))
	if (!Patcher.m_bExiting)
	{
		CString Str = szUpdates; Str += _T("*.*");
		DeleteFiles(Str);
		RemoveDirectory(szUpdates);
	}

	DbgMsg(_T("Completed.\n"));
	return 1;
}

void StartThread(int PatchType /* = 1 */)
{
	auto pThread = AfxBeginThread(PatchThread, reinterpret_cast<LPVOID>(PatchType), THREAD_PRIORITY_BELOW_NORMAL, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = TRUE;
	pThread->ResumeThread();
}