#pragma once

#ifdef _WIN32_WCE
#error "CHtmlView �� �������������� � Windows CE."
#endif 

// HTML-������������� CBrowser

class CBrowser : public CHtmlView
{
	DECLARE_DYNCREATE(CBrowser)

protected:
	CBrowser();           // ���������� �����������, ������������ ��� ������������ ��������
	virtual ~CBrowser();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
};


