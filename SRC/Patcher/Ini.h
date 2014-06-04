#pragma once

typedef pair<CString, CString> _KEY;
typedef vector<_KEY> _KEYS;

class CIni
{
public:
	CIni(void);
	~CIni(void);
private:
	CString m_FileName,
		m_CurrentSection;
	int m_SectionsCount;
	map<CString, _KEYS> m_Sections;
	map<CString, _KEYS>::iterator m_iSection;
public:
	bool LoadFile(const CString FileName);
	bool SetSection(const CString Section);
	CString GetSection(void);
	CString GetString(const CString Key, const CString Default = CString());
	int GetInt(const CString Key, int Default = 0);
};

