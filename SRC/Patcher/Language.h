#pragma once

class CLanguage
{
public:
	enum Languages
	{
		Default = 0,
		RU = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT),
		EN = MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		FR = MAKELANGID(LANG_FRENCH, SUBLANG_DEFAULT)
	};
public:
	CLanguage(void){};
	~CLanguage(void){};

	void Load(Languages Lng = Default);

	CString	Connecting,
			DownloadFile,
			DownloadProcess,
			ApplyingPatches,
			ApplyingProcess,
			Defragmentation,
			PatchComplete,
			NoPatches;

	CString	FileNotFound,
			PatchInProgress;

	CString	KByte,
			MByte,
			KBPS,
			MBPS;
};
