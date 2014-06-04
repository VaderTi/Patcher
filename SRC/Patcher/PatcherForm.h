#pragma once

class CPatcherForm : public CDialog
{
	DECLARE_DYNAMIC(CPatcherForm)

public:
	CPatcherForm(CWnd* pParent = NULL);
	virtual ~CPatcherForm();

	enum { IDD = IDD_PATCHERFORM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
private:

	HICON m_hIcon;
	CImage *m_pSkin;

	static const int CLR_TRANCPERENT = RGB(255,0,255);

	void DrawForm(void);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnButtonDown(UINT nID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
