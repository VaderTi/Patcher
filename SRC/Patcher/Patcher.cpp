#include "StdAfx.h"
#include "Patcher.h"

IMPLEMENT_DYNCREATE(CPatcher, CWinApp)

CPatcher::CPatcher()
{
	m_bExiting = false;
#ifdef _DEBUG
	if (!AllocConsole())
	{
		AfxMessageBox(_T("Ошибка создания окна отладки!"));
	}
	else
	{
		FILE* cd = nullptr;
		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		freopen_s(&cd, "CONOUT$", "wt", stdout);
	}
#endif // _DEBUG
}

CPatcher::~CPatcher()
{
#ifdef _DEBUG
	FreeConsole();
#endif // _DEBUG
}

CPatcher Patcher;

BOOL CPatcher::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	auto ret = InitCommonControlsEx(&InitCtrls);
	m_pMainWnd = &m_MainForm;

	auto dwRet = GetCurrentDirectory(MAX_PATH, m_AppPath.GetBufferSetLength(MAX_PATH + 1));
	m_AppPath.GetBuffer()[dwRet] = _T('\\'); m_AppPath.GetBuffer()[dwRet + 1] = _T('\0');
	GetTempPath(MAX_PATH, m_TmpPath.GetBufferSetLength(MAX_PATH + 1));
	
	m_Settings.LoadLocal(_T("./local.ini"));
	m_Settings.LoadRemote(/*_T("./settings.ini")*/);

	m_Language.Load();

	CString SkinPath;
	SkinPath.Format(_T("%s%s\\"), m_AppPath, _T("Default.skin"));
	m_Skin.LoadSkin(SkinPath);

	m_pMainWnd = &m_MainForm;
	auto nResponse = m_MainForm.DoModal();

	return TRUE;
}

int CPatcher::ExitInstance()
{
	return CWinApp::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPatcher, CWinApp)
END_MESSAGE_MAP()
