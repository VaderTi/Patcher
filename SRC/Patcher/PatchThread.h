#pragma once

static const wchar_t szUpdates[] = _T("./updates/");

struct _PATCH
{
	int		iPatchID,
			iPatchType;
	CString PatchType,
			PatchName,
			PatchGrf;
};
typedef std::vector<struct _PATCH> _PatchList;

class CPatchThread
{
public:
	CPatchThread(void);
	~CPatchThread(void);
	void Clear();
	void LoadSettings(_SERVERINFO& Server, BOOL IsMain = TRUE);
	BOOL GetPatchList(void);
	BOOL ParsePatchList(void);
	BOOL DownloadPatches(void);
	int ProcessPatches(void);

private:
	enum _PATCHTYPE
	{
		GPF = 1,
		RGZ,
		RAR,
		GDF,
		CDF
	};
	int m_LastPatchID,
		m_PatchesCount;
	BOOL m_IsMain;
	_SERVERINFO m_Server;
	_PatchList m_PatchList;

	CFile *m_pFile;
	CIni *m_pIni;

	void SavePatchID(int& PatchID);
};

