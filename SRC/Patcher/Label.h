#pragma once

class CLabel : public CStatic
{
	DECLARE_DYNAMIC(CLabel)

public:
	CLabel();
	virtual ~CLabel();

	void SetText(const CString& strText);
	void SetTextColor(COLORREF crText);
	void SetFontBold(BOOL bBold);
	void SetFontSize(int nSize);

private:
	LOGFONT m_LF;
	CFont m_Font;
	COLORREF m_crText;
	CBitmap m_Bmp;
	void ReconstructFont();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetText(WPARAM,LPARAM);
};

