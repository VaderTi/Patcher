#include "StdAfx.h"
#include "ImgButton.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CImgButton, CButton)

CImgButton::CImgButton()
{
	m_Disabled = FALSE;
	m_Tracking = FALSE;
	m_Hovering = FALSE;

	m_imgStandart = nullptr;
	m_imgHovered = nullptr;
	m_imgPressed = nullptr;
	m_imgDisabled = nullptr;
}

CImgButton::~CImgButton()
{
}

BEGIN_MESSAGE_MAP(CImgButton, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CImgButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW, 0);
	CButton::PreSubclassWindow();
}

BOOL CImgButton::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}

HBRUSH CImgButton::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)::GetStockObject(NULL_BRUSH);
}

BOOL CImgButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CImgButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	Graphics gx(dc);

	Image *Img = m_imgStandart;
	UINT state = lpDrawItemStruct->itemState;

	if ((state & ODS_SELECTED) && m_imgPressed != NULL) Img = m_imgPressed;
	else if (m_Disabled && m_imgDisabled != NULL) Img = m_imgDisabled;
	else if (m_Hovering && m_imgHovered != NULL) Img = m_imgHovered;

	Rect rc(0, 0, Img->GetWidth(), Img->GetHeight());

	gx.DrawImage(Img, rc);

	dc.Detach();
}

void CImgButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	InvalidateParent();

	CButton::OnLButtonDown(nFlags, point);
}

void CImgButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	InvalidateParent();

	CButton::OnLButtonUp(nFlags, point);
}

void CImgButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	if (!m_Tracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_Tracking = _TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}

LRESULT CImgButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Hovering = TRUE;
	InvalidateParent();

	return NULL;
}

LRESULT CImgButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Tracking = FALSE;
	m_Hovering = FALSE;
	InvalidateParent();

	return NULL;
}

void CImgButton::InvalidateParent()
{
	CRect rect;
	GetWindowRect(rect);
	CWnd* pParent = GetParent();
	if(::IsWindow(pParent->GetSafeHwnd()))
	{
		pParent->ScreenToClient(rect);
		pParent->InvalidateRect(rect, FALSE);
	}
}

void CImgButton::Disable(BOOL Disabled /* = FALSE */)
{
	m_Disabled = Disabled;
	if (!m_Disabled)
	{
		ShowWindow(SW_SHOW);
		UpdateWindow();
		OnMouseLeave(NULL, NULL);
	}
	else
	{
		ShowWindow(SW_HIDE);
		UpdateWindow();
		Invalidate();
	}
}

BOOL CImgButton::Disabled()
{
	return m_Disabled;
}

BOOL CImgButton::LoadImages(UINT nIDStandart, UINT nIDHovered /* = 0 */, UINT nIDPressed /* = 0 */, UINT nIDDisabled /* = 0 */)
{
	delete m_imgStandart; m_imgStandart = nullptr;
	delete m_imgHovered; m_imgHovered = nullptr;
	delete m_imgPressed; m_imgPressed = nullptr;
	delete m_imgDisabled; m_imgDisabled = nullptr;

	return TRUE;
}

BOOL CImgButton::LoadImages(LPCTSTR lpszStandart, LPCTSTR lpszHovered /* = NULL */, LPCTSTR lpszPressed /* = NULL */, LPCTSTR lpszDisabled /* = NULL */)
{
	delete m_imgStandart; m_imgStandart = nullptr;
	delete m_imgHovered; m_imgHovered = nullptr;
	delete m_imgPressed; m_imgPressed = nullptr;
	delete m_imgDisabled; m_imgDisabled = nullptr;

	m_imgStandart = Gdiplus::Image::FromFile(lpszStandart);
	if (m_imgStandart->GetLastStatus() != Gdiplus::Status::Ok)
	{
		return FALSE;
	}

	if (lpszHovered != nullptr)
	{
		m_imgHovered = Gdiplus::Image::FromFile(lpszHovered);
		if (m_imgHovered->GetLastStatus() != Gdiplus::Status::Ok)
		{
			return FALSE;
		}
	}

	if (lpszPressed != nullptr)
	{
		m_imgPressed = Gdiplus::Image::FromFile(lpszPressed);
		if (m_imgPressed->GetLastStatus() != Gdiplus::Status::Ok)
		{
			return FALSE;
		}
	}

	if (lpszDisabled != nullptr)
	{
		m_imgDisabled = Gdiplus::Image::FromFile(lpszDisabled);
		if (m_imgDisabled->GetLastStatus() != Gdiplus::Status::Ok)
		{
			return FALSE;
		}
	}

	SetWindowPos(NULL, -1, -1, m_imgStandart->GetWidth(), m_imgStandart->GetHeight(),
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);

	return TRUE;
}