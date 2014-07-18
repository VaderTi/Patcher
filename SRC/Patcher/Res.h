#pragma once

#include "zlib.h"
#ifdef _DEBUG
#pragma comment(lib, "zlib_d")
#else
#pragma comment(lib, "zlib")
#endif

#pragma pack(push,1)
struct RESHEADER
{
	char	Header[3];
	DWORD	FilesCount,
			TableOffset,
			TableSize;
};
#pragma pack(pop)
static const auto RHSize = sizeof(RESHEADER);

#pragma pack(push,1)
struct RESENTRY
{
	char	ResName[32];
	DWORD	ResOffset,
			ResSize,
			zResSize;
};
#pragma pack(pop)
static const auto RESize = sizeof(RESENTRY);

class CRes
{
public:
	CRes(void){};
	~CRes(void){};

#if 0
	bool PackFiles(CStringArray& FilesList, CString PackName = _T("default.res"));
	bool InsertResource(CString ExeName, CString FileName, CString ResName = _T("RES"));
#else //
	LPVOID SaveResource(const CString ResName, CString SaveName = CString("default.res"), DWORD *dwResSize = nullptr);
	bool ExtractResource(const CString ResName, CString* SaveFolder = nullptr);
#endif //

private:
#ifndef PATCHER
	const wchar_t* GetName(CString &FileName);
#endif
};

