#include "StdAfx.h"
#include "Ini.h"


CIni::CIni(void)
{
	m_FileName = _T("");
	m_SectionsCount = 0;
	m_CurrentSection = _T("");
}


CIni::~CIni(void)
{
}


bool CIni::LoadFile(const CString FileName)
{
	m_FileName = FileName;
	unique_ptr<wchar_t[]> pSections(new wchar_t[1024]);
	auto pszSections = pSections.get();

	if (!GetPrivateProfileSectionNames(pszSections, 1024, m_FileName))
		return false;

	auto pSection = pszSections;
	while (*pSection != _T('\0'))
	{
		unique_ptr<wchar_t[]> pPairs(new wchar_t[1024]);
		auto pszPairs = pPairs.get();

		m_Sections[pSection];

		GetPrivateProfileSection(pSection, pszPairs, 1024, m_FileName);

		auto pPair = pszPairs;
		while (*pPair != _T('\0'))
		{
			_KEY Key;
			unique_ptr<wchar_t[]> pszKey(new wchar_t[1024]);
			unique_ptr<wchar_t[]> pszValue(new wchar_t[1024]);
			swscanf_s(pPair, _T("%1024[^=]=%1024[^\r\n]s"), pszKey.get(), 1024, pszValue.get(), 1024);
			Key.first = pszKey.get();
			Key.second = pszValue.get();
			m_Sections[pSection].push_back(Key);
			pPair += wcslen(pPair) + 1;
		}

		pSection += wcslen(pSection) + 1;
		m_SectionsCount++;
	}
	return true;
}

bool CIni::SetSection(const CString Section)
{
	if ((m_iSection = m_Sections.find(Section)) != end(m_Sections))
	{
		m_CurrentSection = Section;
		return true;
	}
	return false;

}


CString CIni::GetSection(void)
{
	return m_CurrentSection;
}


CString CIni::GetString(const CString Key, const CString Default)
{
	if (m_iSection == m_Sections.end() || !m_iSection->second.size()) return Default;
	
	auto Idx = begin(m_iSection->second);
	auto End = end(m_iSection->second);
	for (; Idx != End; ++Idx)
	{
		if (!Idx->first.CompareNoCase(Key))
			return Idx->second;
	}

	return Default;
}


int CIni::GetInt(const CString Key, int Default)
{
	CString Tmp; Tmp.Format(_T("%d"), Default);
	return StrToInt(GetString(Key, Tmp));
}
