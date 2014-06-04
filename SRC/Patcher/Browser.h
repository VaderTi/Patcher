#pragma once

#ifdef _WIN32_WCE
#error "CHtmlView не поддерживается в Windows CE."
#endif 

// HTML-представление CBrowser

class CBrowser : public CHtmlView
{
	DECLARE_DYNCREATE(CBrowser)

protected:
	CBrowser();           // защищенный конструктор, используемый при динамическом создании
	virtual ~CBrowser();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
};


