#pragma once

class CPatcher : public CWinApp
{
	DECLARE_DYNCREATE(CPatcher)

public:
	CPatcher();
	virtual ~CPatcher();
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CSettings		m_Settings;
	CSkin			m_Skin;
	CLanguage		m_Language;
	CPatcherForm	m_MainForm;

	bool			m_bWorking,
					m_bExiting;

	CString			m_AppPath;
	CString			m_TmpPath;

protected:
	DECLARE_MESSAGE_MAP()

private:
	HANDLE m_hConsole;
	void CreateConsole(void);


};

extern CPatcher Patcher;

