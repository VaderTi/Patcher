#include "stdafx.h"
#include "Res.h"

CRes::CRes(void)
{
}


CRes::~CRes(void)
{
}

#if 0
BOOL CRes::PackFiles(CStringArray& FilesList, CString PackName /* = _T */)
{
	RESHEADER Header; ZeroMemory(&Header, RHSize);
	CFile File, tmpFile;
	DWORD dwFilesCount = FilesList.GetCount();
	DWORD dwTableSize = dwFilesCount * RESize, dwFreeOffset = RHSize;
	unique_ptr<BYTE[]> upTable(new BYTE[dwTableSize]);
	auto pTable = upTable.get();

	memmove_s(Header.Header, 3, "PRF", 3);
	Header.FilesCount = dwFilesCount;
	Header.TableSize = dwTableSize;
	File.Open(PackName, CFile::modeCreate|CFile::modeReadWrite);
	File.Write(&Header, RHSize);

	for (DWORD i = 0; i < dwFilesCount; i++)
	{
		CString FileName;
		unique_ptr<BYTE> upBuffer, upzBuffer;
		RESENTRY Table; ZeroMemory(&Table, RESize);

		tmpFile.Open(FilesList[i], CFile::modeRead);
		DWORD dwFileSize = tmpFile.GetLength();
		upBuffer = unique_ptr<BYTE>(new BYTE[dwFileSize+1]);
		auto pBuffer = upBuffer.get();

		tmpFile.Read(pBuffer, dwFileSize);
		DWORD dwzFileSize = compressBound(dwFileSize);
		upzBuffer = unique_ptr<BYTE>(new BYTE[dwzFileSize]);
		auto pzBuffer = upzBuffer.get();
		compress(pzBuffer, &dwzFileSize, pBuffer, dwFileSize);
		File.Write(pzBuffer, dwzFileSize);
		tmpFile.Close();

		FileName = GetName(FilesList[i]);
		size_t iRet = 0;
		wcstombs_s(&iRet, Table.ResName, 32, FileName, FileName.GetLength());
		Table.ResOffset = dwFreeOffset;
		Table.ResSize = dwFileSize;
		Table.zResSize = dwzFileSize;
		dwFreeOffset += dwzFileSize;
		memmove_s(pTable, dwTableSize, &Table, RESize);
		pTable += RESize;
	}

	Header.TableOffset = dwFreeOffset;
	File.Write(upTable.get(), dwTableSize);
	File.SeekToBegin();
	File.Write(&Header, RHSize);
	File.Close();
	return TRUE;
}

BOOL CRes::InsertResource(CString ExeName, CString FileName, CString ResName)
{
	CFile File;
	File.Open(FileName, CFile::modeRead);
	DWORD dwResSize = File.GetLength();
	unique_ptr<BYTE[]> upBuffer(new BYTE[dwResSize]);
	auto pBuffer = upBuffer.get();

	File.Read(pBuffer, dwResSize);
	File.Close();

	auto hResHandle = BeginUpdateResource(ExeName, false);
	if (!hResHandle)
	{
		MessageBox(NULL, _T("Cannt load exe to embed!"), _T("Error!"), MB_ICONERROR);
		return FALSE;
	}
	auto bRes = UpdateResource(hResHandle, RT_RCDATA, ResName,
		MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA),
		pBuffer, dwResSize);
	if (!bRes)
	{
		MessageBox(NULL, _T("Cannt insert data exe!"), _T("Error!"), MB_ICONERROR);
		return FALSE;
	}
	bRes = EndUpdateResource(hResHandle, false);
	if (!bRes)
	{
		MessageBox(NULL, _T("Cannt update exe!"), _T("Error!"), MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}
#else
LPVOID CRes::SaveResource(const CString ResName, const CString SaveName /* = _T */, DWORD *dwResSize /* = nullptr */)
{
	auto hHandle = AfxGetInstanceHandle();

	auto hRes = FindResource(hHandle, ResName, RT_RCDATA); // ищешь там нужный тебе ресурс
	if (!hRes)
	{
		MessageBox(NULL, _T("Cannt find data!"), _T("KPatcher Error!"), MB_ICONERROR);
		return nullptr;
	}
	auto dwSize = SizeofResource(hHandle, hRes); // размер ресурса
	if(!dwSize)
	{
		MessageBox(NULL, _T("Cannt get data size!"), _T("KPatcher Error!"), MB_ICONERROR);
		return nullptr;
	}
	auto hResData = LoadResource(hHandle, hRes); // получаешь хендл на нужный ресурс
	if (!hResData)
	{
		MessageBox(NULL, _T("Cannt load data!"), _T("KPatcher Error!"), MB_ICONERROR);
		return nullptr;
	}
	auto pData = LockResource(hResData); // получаешь указатель на буфер
	if (!pData)
	{
		MessageBox(NULL, _T("Cannt lock data!"), _T("KPatcher Error!"), MB_ICONERROR);
		return nullptr;
	}

	if (dwResSize == nullptr)
	{
		CFile File;
		File.Open(SaveName, CFile::modeCreate|CFile::modeReadWrite);
		File.Write(pData, dwSize);
	}
	else
	{
		*dwResSize = dwSize;
	}

	FreeResource(hResData);
	return pData;
}

BOOL CRes::ExtractResource(CString ResName, CString *SaveFolder /* = nullptr */)
{
	CFile File, TmpFile;
	RESHEADER Header; ZeroMemory(&Header, RHSize);

	File.Open(ResName, CFile::modeReadWrite);
	File.Read(&Header, RHSize);

	if (memcmp(Header.Header, "PRF", 3) != 0) return FALSE;

	unique_ptr<BYTE[]> spTable(new BYTE[Header.TableSize]);
	auto pTable = spTable.get();

	File.Seek(Header.TableOffset, CFile::begin);
	File.Read(pTable, Header.TableSize);

	for (DWORD i = 0; i < Header.FilesCount; ++i)
	{
		RESENTRY Entry; ZeroMemory(&Entry, RESize);
		memcpy(&Entry, pTable, RESize);

		std::vector<uint8_t> Buffer, zBuffer;
		Buffer.resize(Entry.ResSize), zBuffer.resize(Entry.zResSize);

		File.Seek(Entry.ResOffset, CFile::begin);
		File.Read(&zBuffer[0], Entry.zResSize);
		uncompress(&Buffer[0], reinterpret_cast<uLongf*>(&Entry.ResSize), &zBuffer[0], Entry.zResSize);

		CString SaveName;
		if (SaveFolder != nullptr) SaveName = *SaveFolder;
		SaveName += Entry.ResName;

		TmpFile.Open(SaveName, CFile::modeCreate|CFile::modeReadWrite);
		TmpFile.Write(&Buffer[0], Entry.ResSize);
		TmpFile.Close();

		pTable += RESize;
	}
	return TRUE;
}
#endif
#ifndef PATCHER
const wchar_t* CRes::GetName(CString &FileName)
{
	return wcsrchr(FileName.GetString(), _T('/'))+1;
}
#endif
