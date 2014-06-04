#include "StdAfx.h"
#include "Language.h"

void CLanguage::Load(Languages Lng /* = Default */)
{
	LANGID LngID = 0;
	if (!Lng) LngID = GetUserDefaultLangID();
	else LngID = Lng;

	auto hInstance = AfxGetInstanceHandle();

	switch (LngID)
	{
	case RU:
	case EN:
	case FR:
		break;
	default:
		LngID = EN;
		break;
	}

	Connecting.LoadString(hInstance, CONNECTING, LngID);
	DownloadFile.LoadString(hInstance, DOWNLOAD_FILE, LngID);
	DownloadProcess.LoadString(hInstance, DOWNLOAD_PROCESS, LngID);
	ApplyingPatches.LoadString(hInstance, APPLYING_PATCHES, LngID);
	ApplyingProcess.LoadString(hInstance, APPLYING_PROCESS, LngID);
	Defragmentation.LoadString(hInstance, DEFRAGMENTATION, LngID);
	PatchComplete.LoadString(hInstance, PATCH_COMPLETE, LngID);
	NoPatches.LoadString(hInstance, NO_PATCHES, LngID);

	FileNotFound.LoadString(hInstance, FILE_NOT_FOUND, LngID);
	PatchInProgress.LoadString(hInstance, PATCH_IN_PROGRESS, LngID);

	KByte.LoadString(hInstance, KBYTE, LngID);
	MByte.LoadString(hInstance, MBYTE, LngID);
	KBPS.LoadString(hInstance, KBYTE_SEC, LngID);
	MBPS.LoadString(hInstance, MBYTE_SEC, LngID);
}
