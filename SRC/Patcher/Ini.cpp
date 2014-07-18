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

bool CIni::SetSection(const CString Section, bool Create)
{
	m_CurrentSection = Section;
	if ((m_iSection = m_Sections.find(m_CurrentSection)) != end(m_Sections))
		return true;
	return false;

}


CString CIni::GetSection(void)
{
	return m_CurrentSection;
}


CString CIni::GetString(const CString Key, const CString Val)
{
	if (m_iSection == m_Sections.end() || !m_iSection->second.size()) return Val;
	
	auto Idx = begin(m_iSection->second);
	auto End = end(m_iSection->second);
	for (; Idx != End; ++Idx)
	{
		if (!Idx->first.CompareNoCase(Key))
			return Idx->second;
	}

	return Val;
}


int CIni::GetInt(const CString Key, int Val)
{
	CString Tmp; Tmp.Format(_T("%d"), Val);
	return StrToInt(GetString(Key, Tmp));
}

void CIni::SetString(const CString Key, const CString Val)
{
	if (SetSection(m_CurrentSection) && m_iSection->second.size())
	{
		for (auto Idx = m_iSection->second.begin(); Idx != m_iSection->second.end(); ++Idx)
		{
			if (!Idx->first.CompareNoCase(Key))
			{
				Idx->second = Val;
				return;
			}
		}
	}

	_KEY _Key;
	_Key.first.Append(Key);
	_Key.second.Append(Val);
	m_Sections[m_CurrentSection].push_back(_Key);
}

void CIni::SetInt(const CString Key, int Val /* = 0 */)
{
	CString S; S.Format(_T("%d"), Val);
	SetString(Key, S);
}

void CIni::Save()
{
	wfstream Stream;
	Stream.open(m_FileName, wfstream::out | wfstream::trunc);

	for (auto Section = m_Sections.begin(); Section != m_Sections.end(); ++Section)
	{
		CString Buffer; Buffer.Format(_T("[%s]\n"), Section->first);
		Stream.write(Buffer, Buffer.GetLength());
		for (auto Pair = Section->second.begin(); Pair != Section->second.end(); ++Pair)
		{
			CString Data; Data.Format(_T("%s=%s\n"), Pair->first, Pair->second);
			Stream.write(Data, Data.GetLength());
		}
	}
	Stream.flush(); Stream.close();
}
