#pragma once

#include "unrar.hpp"
#ifdef _DEBUG
#pragma comment(lib, "unrar_d")
#else
#pragma comment(lib, "unrar")
#endif

CString GetFileExt(const CString FileName);

void ShowStatus(CString Msg, ...);
void ShowInfo(CString Msg, ...);

void CProgressPos(int Pos, BOOL Add = FALSE);
void CProgressMax(int Max);

void TProgressPos(int Pos);
void TProgressMax(int Max);

void OnDownloadBegin(DWORD dwWorkMax);
bool OnDownload(DWORD dwWork, DWORD dwWorkTotal, DWORD dwWorkMax, FLOAT Speed, LPVOID pData);

bool OnGRF(const char *FileName, unsigned int Current, unsigned int Total, void *pData);

void DeleteFiles(CString Delete);

void AutoUpdate();

void RunClient(CString ExeName, CString ExeParam, CString Login = CString(), CString Password = CString());
void RunCmd(CString FirstParam, CString SecondParam);

uint32_t UnRAR(CString RarFile);

COLORREF HexToRGB(CString Hex);
CString RGBToHex(COLORREF Rgb);
