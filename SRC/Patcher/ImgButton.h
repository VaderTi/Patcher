#pragma once

class CImgButton : public CButton
{
	DECLARE_DYNAMIC(CImgButton)

public:
	CImgButton();
	virtual ~CImgButton();

private:
	void InvalidateParent(void);

	BOOL m_Disabled;
	BOOL m_Tracking;
	BOOL m_Hovering;

	Gdiplus::Image	*m_imgStandart,
					*m_imgHovered,
					*m_imgPressed,
					*m_imgDisabled;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	void Disable(BOOL Disabled = FALSE);
	BOOL Disabled();
	BOOL LoadImages(UINT nIDStandart, UINT nIDHovered = 0, UINT nIDPressed = 0, UINT nIDDisabled = 0);
	BOOL LoadImages(LPCTSTR lpszStandart, LPCTSTR lpszHovered = NULL, LPCTSTR lpszPressed = NULL, LPCTSTR lpszDisabled = NULL);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
};

