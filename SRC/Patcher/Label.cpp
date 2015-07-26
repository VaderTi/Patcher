#include "StdAfx.h"
#include "Label.h"

IMPLEMENT_DYNAMIC(CLabel, CStatic)

CLabel::CLabel()
{
	m_crText = RGB(0, 0, 0);
	m_Font.CreateFont(15, 5, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, _T("MS Shell Dlg"));
	m_Font.GetLogFont(&m_LF);
}

CLabel::~CLabel()
{
}

BEGIN_MESSAGE_MAP(CLabel, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETTEXT,OnSetText)
END_MESSAGE_MAP()

HBRUSH CLabel::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetTextColor(m_crText);
	pDC->SelectObject(&m_Font);
	pDC->SetBkMode(TRANSPARENT);
	return static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
}

BOOL CLabel::OnEraseBkgnd(CDC* pDC)
{
	if (m_Bmp.GetSafeHandle() == nullptr)
	{
		CRect Rect;
		GetWindowRect(&Rect);
		auto pParent = GetParent();
		ASSERT(pParent);
		pParent->ScreenToClient(&Rect);  //convert our corrdinates to our parents
		//copy what's on the parents at this point
		auto DC = pParent->GetDC();
		CDC MemDC;
		MemDC.CreateCompatibleDC(DC);
		m_Bmp.CreateCompatibleBitmap(DC, Rect.Width(), Rect.Height());
		auto pOldBmp = MemDC.SelectObject(&m_Bmp);
		MemDC.BitBlt(0,0,Rect.Width(), Rect.Height(),DC,Rect.left,Rect.top,SRCCOPY);
		MemDC.SelectObject(pOldBmp);
		pParent->ReleaseDC(DC);
	}
	else //copy what we copied off the parent the first time back onto the parent
	{
		CRect Rect;
		GetClientRect(Rect);
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		auto pOldBmp = MemDC.SelectObject(&m_Bmp);
		pDC->BitBlt(0,0,Rect.Width(),Rect.Height(),&MemDC,0,0,SRCCOPY);
		MemDC.SelectObject(pOldBmp);
	}

	return TRUE;
}

LRESULT CLabel::OnSetText(WPARAM wParam,LPARAM lParam)
{
	auto Result = Default();
	Invalidate();
	UpdateWindow();
	return Result;
}

void CLabel::ReconstructFont()
{
	m_Font.DeleteObject();
	auto bCreated = m_Font.CreateFontIndirect(&m_LF);

	ASSERT(bCreated);
}

void CLabel::SetText(const CString& strText)
{
	SetWindowText(strText);
}

void CLabel::SetTextColor(COLORREF crText)
{
	m_crText = crText;
}

void CLabel::SetFontBold(BOOL bBold)
{
	m_LF.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
}

void CLabel::SetFontSize(int nSize)
{
	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(nSize * 10, m_LF.lfFaceName);
	cf.GetLogFont(&lf);

	m_LF.lfHeight = lf.lfHeight;
	m_LF.lfWidth  = lf.lfWidth;

	ReconstructFont();
}
