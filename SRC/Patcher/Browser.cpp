#include "StdAfx.h"
#include "Patcher.h"
#include "Browser.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

// CBrowser

IMPLEMENT_DYNCREATE(CBrowser, CHtmlView)

CBrowser::CBrowser()
{

}

CBrowser::~CBrowser()
{
}

void CBrowser::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBrowser, CHtmlView)
END_MESSAGE_MAP()


// диагностика CBrowser

#ifdef _DEBUG
void CBrowser::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CBrowser::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// обработчики сообщений CBrowser


void CBrowser::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel)
{
	if (Patcher.m_Settings.m_rSettings.NOTICE_URL.CompareNoCase(lpszURL) != 0)
	{
		ShellExecute(NULL, _T("open"), lpszURL, nullptr, nullptr, SW_SHOW);
		return;
	}

	CHtmlView::OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}
